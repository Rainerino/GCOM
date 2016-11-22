#include <QNetworkSession>
#include "missionplannertcp.hpp"
#include "../Mavlink/ardupilotmega/ardupilotmega.h"
#include <QtCore>
#include <QString>
#include <memory>


MissionPlannerSocket::MissionPlannerSocket()
{
    ipaddress = "127.0.0.1"; // Set IP Address to default value used by mission planner
    port = 14550; // Set port to default port used by mission planner
    timeout = 1000;

    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(connected()), this, SLOT(connected()));// Runs Connected when TCP socket is connected
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected())); // Runs Disconnected when TCP socket is disconnected
    connect(socket,SIGNAL(readyRead()), this, SLOT(readBytes())); // Reads bytes when there are bytes availible to read
}



void MissionPlannerSocket::end(){
    qDebug() << "Thread Ended";
    disconnect(socket,SIGNAL(connected()), this, SLOT(connected()));
    disconnect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    disconnect(socket,SIGNAL(readyRead()), this, SLOT(readBytes()));
    quit();
}

void MissionPlannerSocket::setup(QString s_ipaddress, qint16 s_port, int s_timeout){
    //todo: atomic boolean(thread safe) keep track of connection or thread.isalive
    ipaddress = s_ipaddress;
    port = s_port;
    timeout = s_timeout;
}

void MissionPlannerSocket::run()
{
    socket->connectToHost(ipaddress,port);
    if(!socket->waitForConnected(timeout))
    {
        qDebug() << "Error: " << socket->errorString();
    }

    //exec();
}

void MissionPlannerSocket::connected()
{
     emit connectedtomavlink();
}

void MissionPlannerSocket::disconnected()
{
    emit disconnectedfrommavlink();
    exit();
}

void MissionPlannerSocket::readBytes()
{
    mavlink_message_t message;
    mavlink_status_t status;
    uint8_t msgReceived = false;
    QByteArray bufferByteArray;

    bufferByteArray = socket->readAll(); // Read a QByteArray from TCP Socket
    //char readChar = 'a';
    //socket->read(&readChar, sizeof(char));
    std::vector<unsigned char> bufferVector(bufferByteArray.begin(),
                                            bufferByteArray.end());// Convert QByteArray to a vector of uint_8

    // Decode the vector into a message
    for(int i = 0; i < bufferVector.size(); i++){
        msgReceived = mavlink_parse_char(MAVLINK_COMM_1, bufferVector[i], &message, &status);
        }
        //if(msgReceived)
        // msgRecievied boolean blocks the MAVLINK_MSG_ID_CAMERA_FEEDBACK
        // so it has been temporaily disabled.
        // this will be investigated
        //{
            switch (message.msgid) {

            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            {
                std::shared_ptr<mavlink_global_position_int_t> gpsPacketPointer(new mavlink_global_position_int_t);

                mavlink_msg_global_position_int_decode(&message, gpsPacketPointer.get());
                emit mavlinkgpsinfo(gpsPacketPointer);
                break;
            }

            case MAVLINK_MSG_ID_CAMERA_FEEDBACK:
            {
                std::shared_ptr<mavlink_camera_feedback_t> cameraPacketPointer(new mavlink_camera_feedback_t);
                mavlink_msg_camera_feedback_decode(&message, cameraPacketPointer.get());
                emit mavlinkcamerainfo(cameraPacketPointer);
                break;
            }



            default:
                break;
            }
        //}
    //}

}

