#include "dcnc.hpp"

DCNC::DCNC()
{
    server = new QTcpServer();
    isConnected = false;
    previouslyDropped = false;
    port = 6969;
    clientConnection = nullptr;

    connect(server, SIGNAL(newConnection()),this, SLOT(handleConection()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(handleData()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(droppedConnection()));
}

DCNC::~DCNC()
{
    //close the clientSocket if it is open check if the ptr is null
    if(!clientConnection && clientConnection->isOpen())
        clientConnection->close();
    server->close();
}

//make startServer connect slots with signals
void DCNC::startServer(int port, std::string addr)
{
    this->port = port;
    this->address = QString::fromStdString(addr);
    hostAddress = QHostAddress(address);
    server->listen(hostAddress,port);
}

void DCNC::startServer(int port)
{
    this->port = port;
    server->listen(hostAddress, port);
}

void DCNC::cancelConnection()
{
    if(!clientConnection && clientConnection->isOpen())
        clientConnection->close();
    isConnected = false;
}

void DCNC::handleConection()
{
    if(isConnected){
        QDebug("we are already connected \n");
        return;
    }

    //otherwise let's accept a new connection
    isConnected = true;
    clientConnection = server->nextPendingConnection();
    dataIn.setDevice(clientConnection);
}

void DCNC::handleData()
{
    //attempts to generate the message
    while(!messageFramer.status()){
        dataIn.startTransaction();
        dataIn >> messageFramer;
        //check the status of dataIn and the framer
        if(!messageFramer.status() || dataIn.status() == QDataStream::ReadPastEnd ||
                dataIn.status() == QDataStream::ReadCorruptData){
            dataIn.rollbackTransaction();
            messageFramer.clearMessage();
        }
    }
    message = messageFramer.generateMessage();
    dataIn.commitTransaction();

    /*lets send the message to the listening slots now
    switch(message.MessageID){
        case //enumerate and emit right signals
    }*/
    //call close connection and set isConnected to false when you no longer need the client
    //emit reconnection or new connection signal
}

void DCNC::droppedConnection()
{
    isConnected = false;
    previouslyDropped = true;
    emit droppedClient();
}

//create a slot to handle dropped connection - set is connected to false
//check ID message if client is dropped flag
//1. send a signal if a client is reconnection from dropout
//2. send client a message to acknowledge that client dropped before

//create new connection signal for UI if you get a new connection after a drop
//create connection dropped signal

//create method to force drop connection
//if i get a dirty message tell the gremlin to reset => gaurantees always get an ID message after it
//otherwise its a new connection and treat normally
//




