#include "dcnc.hpp"

DCNC::DCNC()
{
    server = new QTcpServer();
    connect(server, SIGNAL(newConnection()),
            this, SLOT(handleConection(QTCPSocket)));
}

DCNC::~DCNC()
{
    server->close();
    delete server;
}

//make startServer connect slots with signals
int DCNC::startServer(int port, std::string addr)
{
    this->port = port;
    this->address = addr;
    server->listen(this->address,this->port);
}


//slots // send smart pointers though slots
void DCNC::handleConection(QTCPSocket socket)
{
    client_connection = server->nextPendingConnection();
    connect(client_connection, SIGNAL(readyRead()),
            this, SLOT(handleMessage()));
    data_in =
}

void DCNC::handleMessage()
{
    //get the message type and do something
}
