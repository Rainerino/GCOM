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
        std::cout << "we are already connected" << std::endl;
        return;
    }
    //otherwise let's accept a new connection
    isConnected = true;
    clientConnection = server->nextPendingConnection();
    while(clientConnection->bytesAvailable() <= 0){}
    //do appropriate opperations with message
    handleData(clientConnection);
    clientConnection->close();
}

void DCNC::handleData(QTcpSocket* socket)
{
    //initialize new gremlin connection and prepare message
    dataIn.setDevice(socket);
    std::unique_ptr<UASMessage> message;
    UASMessageTCPFramer messageFramer;

    //attempts to generate the message
    while(!messageFramer.status()){
        dataIn.startTransaction();
        dataIn >> messageFramer;
        message = messageFramer.generateMessage();
        messageFramer.frameMessage(*message.get());
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

