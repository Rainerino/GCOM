//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QDebug>
// GCOM Includes
#include "dcnc.hpp"
#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/request_message.hpp"
#include "modules/uas_message/system_info_message.hpp"
#include "modules/uas_message/capabilities_message.hpp"
#include "modules/uas_message/command_message.hpp"
//===================================================================
// Public Class Declaration
//===================================================================
DCNC::DCNC()
{
    // Set default values
    server = new QTcpServer();
    server->setMaxPendingConnections(1);
    port = 42069;
    clientConnection = nullptr;
    serverStatus = DCNCStatus::OFFLINE;

    // Connect Signals to Slots
    connect(server, SIGNAL(newConnection()),
            this, SLOT(handleClientConection()));
}

DCNC::~DCNC()
{
    stopServer();
    delete(server);
}

bool DCNC::startServer(QString address, int port)
{
    // If the DCNC is currently running then stop it
    if (serverStatus != DCNCStatus::OFFLINE)
        stopServer();

    this->port = port;
    this->address = address;
    hostAddress = QHostAddress(address);
    bool startStatus = server->listen(hostAddress, port);
    if (startStatus == true)
        serverStatus = DCNCStatus::SEARCHING;

    return startStatus;
}

void DCNC::stopServer()
{
    cancelConnection();
    // Stop listning on the selected interfaces and update state
    server->close();
    serverStatus = DCNCStatus::OFFLINE;
}

void DCNC::cancelConnection()
{
    if(clientConnection != nullptr)
    {
        clientConnection->close();
        disconnect(clientConnection, SIGNAL(readyRead()),
                   this, SLOT(handleClientData()));
        disconnect(clientConnection, SIGNAL(disconnected()),
                   this, SLOT(handleClientDisconnected()));
        clientConnection->deleteLater();
        clientConnection = nullptr;
    }
    server->resumeAccepting();
}

DCNC::DCNCStatus DCNC::status()
{
    return serverStatus;
}

void DCNC::handleClientConection()
{
    // While this connection is established stop accepting more connections
    server->pauseAccepting();
    // Setup the connection socket
    clientConnection = server->nextPendingConnection();
    connectionDataStream.setDevice(clientConnection);
    // Connect the connection slot's signals
    connect(clientConnection, SIGNAL(readyRead()),
            this, SLOT(handleClientData()));
    connect(clientConnection, SIGNAL(disconnected()),
            this, SLOT(handleClientDisconnected()));

    // Update the DCNC's state and notify listners
    serverStatus = DCNCStatus::CONNECTED;

    // Send system info request
    // TODO: that the message is successfully sent
    RequestMessage request(UASMessage::MessageID::SYSTEM_INFO);
    messageFramer.frameMessage(request);
    connectionDataStream << messageFramer;

    emit receivedConnection();
}

void DCNC::handleClientDisconnected()
{
    serverStatus = DCNCStatus::SEARCHING;
    cancelConnection();
    emit droppedConnection();
}

void DCNC::handleClientData()
{
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

//===================================================================
// Outgoing message methods
//===================================================================
void DCNC::startImageRelay()
{
    CommandMessage outgoingMessage = CommandMessage(CommandMessage::Commands::IMAGE_RELAY_START);
    messageFramer.frameMessage(outgoingMessage);
    connectionDataStream << messageFramer;
}

void DCNC::stopImageRelay()
{
    CommandMessage outgoingMessage = CommandMessage(CommandMessage::Commands::IMAGE_RELAY_STOP);
    messageFramer.frameMessage(outgoingMessage);
    connectionDataStream << messageFramer;
}

//===================================================================
// Receive Handler
//===================================================================
void DCNC::handleClientMessage(std::shared_ptr<UASMessage> message)
{
    UASMessage *outgoingMessage = nullptr;
    switch (message->type())
    {
        case (UASMessage::MessageID::SYSTEM_INFO):
        {
            std::shared_ptr<SystemInfoMessage> systemInfo = std::static_pointer_cast<SystemInfoMessage>(message);
            emit receivedGremlinInfo(QString(systemInfo->systemId.c_str()),
                                     systemInfo->versionNumber,
                                     systemInfo->dropped);
            // Send system capabilities request
            // TODO: that the message is successfully sent
            outgoingMessage = new RequestMessage(UASMessage::MessageID::MESG_CAPABILITIES);


            break;
        }

        case (UASMessage::MessageID::MESG_CAPABILITIES):
        {
            std::shared_ptr<CapabilitiesMessage> systemCapabilities =
                    std::static_pointer_cast<CapabilitiesMessage>(message);
            emit receivedGremlinCapabilities(systemCapabilities->getCapabilities());
            break;
        }

        case (UASMessage::MessageID::RESPONSE):
        {
            std::shared_ptr<ResponseMessage> response =
                    std::static_pointer_cast<ResponseMessage>(message);
            emit receivedGremlinResponse(response->command(), response->responseCode());
            break;
        }

    }

    if (outgoingMessage == nullptr)
        return;

    messageFramer.frameMessage(*outgoingMessage);
    connectionDataStream << messageFramer;
    delete outgoingMessage;
}
