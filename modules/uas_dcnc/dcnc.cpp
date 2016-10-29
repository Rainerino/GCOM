#include "dcnc.hpp"

DCNC::DCNC()
{
    server = new QTcpServer();
    connect(server, SIGNAL(newConnection()),
            this, SLOT(handleConection()));
}

DCNC::~DCNC()
{
    server->close();
}

//make startServer connect slots with signals
void DCNC::startServer(int port, std::string addr)
{
    this->port = port;
    this->address = addr;
    server->listen(addr,port);
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
    if (clientConnection->bytesAvailable != 0){
        handleData(clientConnection);
    }

    clientConnection.close();
}

void DCNC::handleData(QTCPSocket* socket)
{
    //initialize new gremlin connection and prepare message
    dataIn = new QDataStream(*socket);
    unique_ptr<UASMessage> message;
    UASMessageTCPFramer message_framer;
    message_framer.mStatus = false;

    //attempts to generate the message
    while(!message_framer.status()){
        message_framer >> data_in.startTransaction();
        message = message_framer.generateMessage();
        message_framer.frameMessage(message);
        if(!message_framer.status()){
            data_in.rollbackTransaction();
            message_framer.clearMessage();
        }
    }

    //we gucci with the transaction now
    data_in.commitTransaction();

    //lets send the message to the listening slots now
    switch(message.MessageID){
        case //enumerate and emit right signals
    }

}

