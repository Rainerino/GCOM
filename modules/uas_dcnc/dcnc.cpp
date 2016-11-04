#include "dcnc.hpp"

DCNC::DCNC()
{
    server = new QTcpServer();
    connect(server, SIGNAL(newConnection()),
            this, SLOT(handleConection()));
    isConnected = false;
    port = 6969;
    clientConnection = nullptr;
}

DCNC::~DCNC()
{
    //close the clientSocket if it is open check if the ptr is null
    server->close();
}

//make startServer connect slots with signals
void DCNC::startServer(int port, std::string addr)
{
    this->port = port;
    this->address = QString::fromStdString(addr);
    QHostAddress hostAddress = QHostAddress(address);
    server->listen(hostAddress,port);
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
    
    //replace this garbage with the QTCPSocket signal to trigger handle Data as a socket
    while(clientConnection->bytesAvailable() <= 0){}
    //do appropriate opperations with message
    handleData(clientConnection);
    clientConnection->close();
}

//should be a slot that responds to QTCPSignal
void DCNC::handleData(QTcpSocket* socket)
{
    //initialize new gremlin connection and prepare message
    dataIn.setDevice(socket);               //should be done at the handle connection level
    std::unique_ptr<UASMessage> message;
    UASMessageTCPFramer messageFramer;

    //attempts to generate the message
    while(!messageFramer.status()){
        dataIn.startTransaction();
        dataIn >> messageFramer;
        //check the status of dataIn and the framer
        message = messageFramer.generateMessage();
        if(!messageFramer.status()){
            dataIn.rollbackTransaction();
            messageFramer.clearMessage();
        }
    }
    dataIn.commitTransaction();

    /*lets send the message to the listening slots now
    switch(message.MessageID){
        case //enumerate and emit right signals
    }*/
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




