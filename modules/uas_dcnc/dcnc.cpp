//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QDebug>
// GCOM Includes
#include "dcnc.hpp"

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

bool DCNC::startServer(int port)
{
    if (serverStatus != DCNCStatus::OFFLINE)
        stopServer();

    bool startStatus = server->listen(QHostAddress::Any, port);
    if (startStatus == true)
        serverStatus = DCNCStatus::SEARCHING;

    return startStatus;
}

void DCNC::stopServer()
{
    // If there any client connections
    if(clientConnection != nullptr)
    {
        // Handle teardown of the socket
        disconnect(clientConnection, SIGNAL(readyRead()),
                   this, SLOT(handleClientData()));
        disconnect(clientConnection, SIGNAL(disconnected()),
                   this, SLOT(handleClientDisconnected()));
        clientConnection->close();
        clientConnection->deleteLater();
        clientConnection = nullptr;
    }
    // Stop listning on the selected interfaces and update state
    server->close();
    serverStatus = DCNCStatus::OFFLINE;
}

void DCNC::cancelConnection()
{
    if(clientConnection != nullptr)
        clientConnection->close();
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
    connectionData.setDevice(clientConnection);
    // Connect the connection slot's signals
    connect(clientConnection, SIGNAL(readyRead()),
            this, SLOT(handleClientData()));
    connect(clientConnection, SIGNAL(disconnected()),
            this, SLOT(handleClientDisconnected()));
    // Update the DCNC's state and notify listners
    serverStatus = DCNCStatus::CONNECTED;
    emit receivedConnection();
}

void DCNC::handleClientDisconnected()
{
    // Set the internal state
    serverStatus = DCNCStatus::SEARCHING;
    // Handle teardown of the socket
    disconnect(clientConnection, SIGNAL(readyRead()),
               this, SLOT(handleClientData()));
    disconnect(clientConnection, SIGNAL(disconnected()),
               this, SLOT(handleClientDisconnected()));
    clientConnection->deleteLater();
    clientConnection = nullptr;
    // Set the DCNC back into searching state
    server->resumeAccepting();
    // Inform any lisners of the dropped connection
    emit droppedConnection();
}

void DCNC::handleClientData()
{
    /*//attempts to generate the message
    while(!messageFramer.status()){
        connectionData.startTransaction();
        connectionData >> messageFramer;
        //check the status of dataIn and the framer
        if(!messageFramer.status() || connectionData.status() == QDataStream::ReadPastEnd ||
                connectionData.status() == QDataStream::ReadCorruptData){
            connectionData.rollbackTransaction();
            messageFramer.clearMessage();
        }
    }
    message = messageFramer.generateMessage();
    connectionData.commitTransaction();

    /*lets send the message to the listening slots now
    switch(message.MessageID){
        case //enumerate and emit right signals
    }*/
    //call close connection and set isConnected to false when you no longer need the client
    //emit reconnection or new connection signal*/
}
