#include "dcnc.hpp"

DCNC::DCNC()
{
    server = new QTcpServer();
    connect(server, SIGNAL(newConnection()),
            this, SLOT(handleConection()));
    connect(this, SIGNAL(receivedCompleteMessage(UASMessage*)),
                this, SLOT(handleMessage(UASMessage*)));

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
    server->listen(this->address,this->port);
}


//slots // send smart pointers though slots
void DCNC::handleConection(QTCPSocket socket)
{
    //initialize new gremlin connection and prepare message
    client_connection = server->nextPendingConnection();
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

    data_in.commitTransaction();
    emit receivedCompleteMessage(message);

}

void DCNC::handleMessage(UASMessage* message)
{
    switch(message->type()){
        //execute appropriately
    }
}
