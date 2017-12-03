//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QSignalSpy>
#include <QtTest/QtTest>
// GCOM Includes
#include "test_dcnc.hpp"
#include "modules/uas_message/request_message.hpp"

const QString IP_ADDRESS = "127.0.0.1";
const int PORT = 4206;

const int SOCKET_TIMEOUT_DURATION = 5000;

const int TEST_START_INDEX = 0;
const int TEST_STOP_INDEX = 2;

const CapabilitiesMessage::Capabilities CAPABILITIES_TEST[] =
{
    CapabilitiesMessage::Capabilities::IMAGE_RELAY,
    CapabilitiesMessage::Capabilities::CAMERA_TAGGED,
    CapabilitiesMessage::Capabilities::CAMERA_UNTAGGED
};

QTEST_MAIN(TestDCNC)

void TestDCNC::initTestCase()
{
    // Register types for use in QSignalSpy
    qRegisterMetaType<UASMessage::MessageID>();
    qRegisterMetaType<CapabilitiesMessage::Capabilities>();
    qRegisterMetaType<CommandMessage::Commands>();

    // Setup DCNC
    dcnc = new DCNC();
    QVERIFY(dcnc->startServer(IP_ADDRESS, PORT));
    QCOMPARE(dcnc->status(), DCNC::DCNCStatus::SEARCHING);

    // Setup socket
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    socket->connectToHost(IP_ADDRESS, PORT);

    // Verify socket connects
    QVERIFY(socket->waitForConnected(SOCKET_TIMEOUT_DURATION));

    connect(socket, SIGNAL(readyRead()), this, SLOT(handleClientData()));

    // Verify connection is received on DCNC end
    QSignalSpy receivedConnectionSpy(dcnc, SIGNAL(receivedConnection()));
    QVERIFY(receivedConnectionSpy.isValid());
    QVERIFY(receivedConnectionSpy.wait());
    QCOMPARE(dcnc->status(), DCNC::DCNCStatus::CONNECTED);
    QCOMPARE(receivedConnectionSpy.count(), 1);

    // Verify the DATA_SYSTEM_INFO request in DCNC's handleClientConnection sends correctly
    connect(this, SIGNAL(receivedRequest(UASMessage::MessageID)),
            this, SLOT(compareRequestMessage(UASMessage::MessageID)));
    QSignalSpy receivedRequestSpy(this, SIGNAL(receivedRequest(UASMessage::MessageID)));
    QVERIFY(receivedRequestSpy.isValid());
    QVERIFY(receivedRequestSpy.wait());
}

void TestDCNC::cleanupTestCase()
{
    socket->disconnectFromHost();

    dcnc->stopServer();
    QCOMPARE(dcnc->status(), DCNC::DCNCStatus::OFFLINE);

    disconnect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

    delete socket;
    delete dcnc;
}

void TestDCNC::socketConnected()
{
    qInfo() << "Socket Connected";
    connectionDataStream.resetStatus();
    connectionDataStream.setDevice(socket);
}

void TestDCNC::socketDisconnected()
{
    qInfo() << "Socket Disconnected";
    connectionDataStream.resetStatus();
    connectionDataStream.unsetDevice();
}

void TestDCNC::handleClientData()
{
    messageFramer.clearMessage();
    while (messageFramer.status() != UASMessageTCPFramer::TCPFramerStatus::INCOMPLETE_MESSAGE)
    {
        connectionDataStream.startTransaction();
        connectionDataStream >> messageFramer;
        if (messageFramer.status() == UASMessageTCPFramer::TCPFramerStatus::SUCCESS)
        {
            handleClientMessage(messageFramer.generateMessage());
            connectionDataStream.commitTransaction();
        }
        else if (messageFramer.status() == UASMessageTCPFramer::TCPFramerStatus::INCOMPLETE_MESSAGE)
        {
            connectionDataStream.rollbackTransaction();
        }
        else
        {
            connectionDataStream.abortTransaction();
        }
    }
}

void TestDCNC::handleClientMessage(std::shared_ptr<UASMessage> message)
{
    UASMessage *outgoingMessage = nullptr;
    switch (message->type())
    {
        case UASMessage::MessageID::REQUEST:
        {
            std::shared_ptr<RequestMessage> request =
                    std::static_pointer_cast<RequestMessage>(message);
            emit receivedRequest(request->requestedMessage);
            break;
        }
        case UASMessage::MessageID::COMMAND:
        {
            std::shared_ptr<CommandMessage> command =
                    std::static_pointer_cast<CommandMessage>(message);
            emit receivedCommand(command->command);
            break;
        }
        case UASMessage::MessageID::DATA_CAPABILITIES:
        {
            std::shared_ptr<CapabilitiesMessage> systemCapabilities =
                    std::static_pointer_cast<CapabilitiesMessage>(message);
            emit receivedGremlinCapabilities(systemCapabilities->capabilities);
            break;
        }

        default:
            outgoingMessage = nullptr;
        break;
    }

    if (outgoingMessage == nullptr)
        return;

    messageFramer.frameMessage(*outgoingMessage);
    // TODO: that the message is successfully sent
    connectionDataStream << messageFramer;
    qDebug() << ((int)messageFramer.status());
    delete outgoingMessage;
}

void TestDCNC::compareRequestMessage(UASMessage::MessageID request)
{
    QCOMPARE(request, UASMessage::MessageID::DATA_SYSTEM_INFO);
    disconnect(this, SIGNAL(receivedRequest(UASMessage::MessageID)),
               this, SLOT(compareRequestMessage(UASMessage::MessageID)));
}

void TestDCNC::testSendUASMessage_data()
{
    QTest::addColumn<CapabilitiesMessage::Capabilities>("capabilities");

    for (int i = TEST_START_INDEX; i <= TEST_STOP_INDEX; i++)
    {
        QTest::newRow(qPrintable(QString::number(i))) << CAPABILITIES_TEST[i];
    }
}

void TestDCNC::testSendUASMessage()
{
    QFETCH(CapabilitiesMessage::Capabilities, capabilities);
    sentCapabilities = capabilities;

    connect(this, SIGNAL(receivedGremlinCapabilities(CapabilitiesMessage::Capabilities)),
            this, SLOT(compareSendUASMessage_capabilities(CapabilitiesMessage::Capabilities)));

    CapabilitiesMessage message(sentCapabilities);
    QVERIFY(dcnc->sendUASMessage(static_cast<std::shared_ptr<CapabilitiesMessage>>(&message)));

    QSignalSpy capabilitiesSpy(this, SIGNAL(receivedGremlinCapabilities(CapabilitiesMessage::Capabilities)));
    QVERIFY(capabilitiesSpy.isValid());
    QVERIFY(capabilitiesSpy.wait());
    QCOMPARE(capabilitiesSpy.count(), 1);
}

void TestDCNC::compareSendUASMessage_capabilities(CapabilitiesMessage::Capabilities capabilities)
{
    QCOMPARE(capabilities, sentCapabilities);
    disconnect(this, SIGNAL(receivedGremlinCapabilities(CapabilitiesMessage::Capabilities)),
               this, SLOT(compareSendUASMessage_capabilities(CapabilitiesMessage::Capabilities)));
}

void TestDCNC::testStartImageRelay()
{
    connect(this, SIGNAL(receivedCommand(CommandMessage::Commands)),
            this, SLOT(compareStartImageRelay(CommandMessage::Commands)));

    QSignalSpy commandSpy(this, SIGNAL(receivedCommand(CommandMessage::Commands)));
    QVERIFY(commandSpy.isValid());

    dcnc->startImageRelay();

    QVERIFY(commandSpy.wait());
    QCOMPARE(commandSpy.count(), 1);
}

void TestDCNC::compareStartImageRelay(CommandMessage::Commands command)
{
    QCOMPARE(command, CommandMessage::Commands::IMAGE_RELAY_START);
    disconnect(this, SIGNAL(receivedCommand(CommandMessage::Commands)),
               this, SLOT(compareStartImageRelay(CommandMessage::Commands)));
}

void TestDCNC::testStopImageRelay()
{
    connect(this, SIGNAL(receivedCommand(CommandMessage::Commands)),
            this, SLOT(compareStopImageRelay(CommandMessage::Commands)));

    QSignalSpy commandSpy(this, SIGNAL(receivedCommand(CommandMessage::Commands)));
    QVERIFY(commandSpy.isValid());

    dcnc->stopImageRelay();

    QVERIFY(commandSpy.wait());
    QCOMPARE(commandSpy.count(), 1);
}

void TestDCNC::compareStopImageRelay(CommandMessage::Commands command)
{
    QCOMPARE(command, CommandMessage::Commands::IMAGE_RELAY_STOP);
    disconnect(this, SIGNAL(receivedCommand(CommandMessage::Commands)),
               this, SLOT(compareStopImageRelay(CommandMessage::Commands)));
}

void TestDCNC::testCancelConnection()
{
    QSignalSpy droppedConnectionSpy(dcnc, SIGNAL(droppedConnection()));
    QVERIFY(droppedConnectionSpy.isValid());

    dcnc->cancelConnection();

    QCOMPARE(dcnc->status(), DCNC::DCNCStatus::SEARCHING);
    QCOMPARE(droppedConnectionSpy.count(), 1);
}
