//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QFile>
#include <QSignalSpy>
#include <QtTest/QtTest>
// GCOM Includes
#include "test_image_tagged_message.hpp"

const QString IP_ADDRESS = "127.0.0.1";
const int PORT = 4206;

const int TEST_START_INDEX = 0;
const int TEST_END_INDEX = 6;
const QString SEQ_TEST[] = {"0", "1", "34", "100", "178", "201", "255"};
const QString LAT_TEST[] = {"0", "1", "-1", "700001", "12345678", "-900000000", "900000000"};
const QString LON_TEST[] = {"0", "1", "-1", "129999", "87654321", "-1800000000", "1800000000"};
const QString ALT_ABS_TEST[] = {"0", "1", "-1", "401", "29383", "-500000", "500000"};
const QString ALT_REL_TEST[] = {"0", "1", "-1", "399", "19273", "-500000", "500000"};
const QString HDG_TEST[] = {"0", "1", "101", "2345", "10492", "32111", "36000"};
const QString IMG_PATH_TEST[] = {
                    IMAGE_PATH + QString("connected.png"), IMAGE_PATH + QString("kingfisher.jpg"),
                    IMAGE_PATH + QString("flower.jpeg"), IMAGE_PATH + QString("marbles.bmp"),
                    IMAGE_PATH + QString("walle.jpg"), IMAGE_PATH + QString("planet.jpg"),
                    IMAGE_PATH + QString("nebula.tif")};

// In milliseconds
const int SOCKET_TIMEOUT_DURATION = 5000;
const int SIGNAL_TIMEOUT_DURATION = 180000;

QTEST_MAIN(TestImageTaggedMessage)

void TestImageTaggedMessage::initTestCase()
{
    // Register shared ptr to allow being used in signal
    qRegisterMetaType<std::shared_ptr<ImageTaggedMessage>>();

    dcnc = new DCNC();
    dcnc->startServer(IP_ADDRESS, PORT);

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(dcnc, SIGNAL(receivedImageTaggedData(std::shared_ptr<ImageTaggedMessage>)),
            this, SLOT(compareImageTaggedData(std::shared_ptr<ImageTaggedMessage>)));

    socket->connectToHost(IP_ADDRESS, PORT);
    // Verify socket connects
    QVERIFY(socket->waitForConnected(SOCKET_TIMEOUT_DURATION));

}

void TestImageTaggedMessage::cleanupTestCase()
{
    socket->disconnectFromHost();
    dcnc->stopServer();

    disconnect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    disconnect(dcnc, SIGNAL(receivedImageTaggedData(std::shared_ptr<ImageTaggedMessage>)),
           this, SLOT(compareImageTaggedData(std::shared_ptr<ImageTaggedMessage>)));

    delete socket;
    delete dcnc;
    delete sentImageData;
}

void TestImageTaggedMessage::socketConnected()
{
    qInfo() << "Socket Connected";
    connectionDataStream.resetStatus();
    connectionDataStream.setDevice(socket);
}

void TestImageTaggedMessage::socketDisconnected()
{
    qInfo() << "Socket Disconnected";
    connectionDataStream.resetStatus();
    connectionDataStream.unsetDevice();
}

void TestImageTaggedMessage::sendTaggedImage_data() {
    // Create a data table with each test as a row

    // Add headings
    QTest::addColumn<QString>("sequence");
    QTest::addColumn<QString>("latitude");
    QTest::addColumn<QString>("longitude");
    QTest::addColumn<QString>("altitudeAbs");
    QTest::addColumn<QString>("altitudeRel");
    QTest::addColumn<QString>("heading");
    QTest::addColumn<QString>("imagePath");

    // Add rows
    for (int i = TEST_START_INDEX; i <= TEST_END_INDEX; i++) {
        QTest::newRow(qPrintable(QString::number(i)))
                << SEQ_TEST[i] << LAT_TEST[i] << LON_TEST[i] << ALT_ABS_TEST[i]
                << ALT_REL_TEST[i] << HDG_TEST[i] << IMG_PATH_TEST[i];
    }
}

void TestImageTaggedMessage::sendTaggedImage()
{
    // Retrieve all the data from a row in the data table to test
    QFETCH(QString, sequence);
    QFETCH(QString, latitude);
    QFETCH(QString, longitude);
    QFETCH(QString, altitudeAbs);
    QFETCH(QString, altitudeRel);
    QFETCH(QString, heading);
    QFETCH(QString, imagePath);

    seq = sequence.toInt();
    lat = latitude.toInt();
    lon = longitude.toInt();
    altAbs = altitudeAbs.toInt();
    altRel = altitudeRel.toInt();
    hdg = heading.toInt();

    QFile image(imagePath);
    // Verify image opened correctly
    QVERIFY(image.open(QIODevice::ReadOnly));

    imageSize = image.size();

    sentImageData = new uint8_t[imageSize];

    // Extract all bytes into byte array and copy it to sentImageData
    QByteArray imageByteArray = image.readAll();
    std::copy(imageByteArray.begin(), imageByteArray.end(), sentImageData);

    ImageTaggedMessage outgoingMessage(seq, lat, lon, altAbs, altRel, hdg, sentImageData, imageSize);
    messageFramer.frameMessage(outgoingMessage);
    connectionDataStream << messageFramer;

    image.close();

    // Create a signal spy to keep track of the signal
    QSignalSpy taggedImageMessageSpy(dcnc,
               SIGNAL(receivedImageTaggedData(std::shared_ptr<ImageTaggedMessage>)));
    QVERIFY(taggedImageMessageSpy.isValid());

    // Create an event loop that waits until signal is emitted
    // If the signal has not been emitted by the time SIGNAL_TIMEOUT_DURATION ends, wait returns false
    QVERIFY(taggedImageMessageSpy.wait(SIGNAL_TIMEOUT_DURATION));
}

void TestImageTaggedMessage::compareImageTaggedData(std::shared_ptr<ImageTaggedMessage> image)
{
    // Compare all values before sending and after sending
    QCOMPARE(image->sequenceNumber, seq);
    QCOMPARE(image->latitudeRaw, lat);
    QCOMPARE(image->longitudeRaw, lon);
    QCOMPARE(image->altitudeAbsRaw, altAbs);
    QCOMPARE(image->altitudeRelRaw, altRel);
    QCOMPARE(image->headingRaw, hdg);

    // Compare the images byte by byte
    unsigned int numBytes;
    for (numBytes = 0; numBytes < image->imageData.size(); numBytes++) {
        QCOMPARE(image->imageData[numBytes], *(sentImageData + numBytes));
    }
    QCOMPARE(image->imageData.size(), imageSize);
    qInfo() << "Number of bytes checked: " << numBytes;
}
