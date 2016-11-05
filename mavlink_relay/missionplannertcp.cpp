#include <QNetworkSession>
#include "missionplannertcp.hpp"
#include "../Mavlink/common/mavlink.h"
#include <QtCore>
#include <QSharedPointer>
#include <string>
#include <QString>
#include <memory>

MissionPlannerSocket::MissionPlannerSocket()
{

}

void MissionPlannerSocket::exit(){
    disconnect(socket,SIGNAL(connected()), this, SLOT(connected()));
    disconnect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    disconnect(socket,SIGNAL(readyRead()), this, SLOT(readBytes()));
}

void MissionPlannerSocket::run(const std::string &ipaddress, qint16 port, int timeout)
{
    socket = new QTcpSocket(this);

    connect(socket,SIGNAL(connected()), this, SLOT(connected()));// Runs Connected when TCP socket is connected
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected())); //Runs Disconnected when TCP socket is disconnected
    connect(socket,SIGNAL(readyRead()), this, SLOT(readBytes())); //reads bytes when there are bytes availible to read

    //qDebug() << "Connecting...";
    socket->connectToHost(QString::fromStdString(ipaddress),port);



    if(!socket->waitForConnected(timeout))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void MissionPlannerSocket::connected()
{
     //qDebug() << "Connected!";
     emit connectedtomavlink();

}


void MissionPlannerSocket::disconnected()
{
    //qDebug() << "Disconnected!";
    emit disconnectedfrommavlink();
}




void MissionPlannerSocket::readBytes()
{
    mavlink_message_t message;
    mavlink_status_t status;
    uint8_t msgReceived = false;
    QByteArray bufferqbytearray;


    std::shared_ptr<mavlink_global_position_int_t> gpspacketpointer(new mavlink_global_position_int_t);
    std::shared_ptr<mavlink_camera_trigger_t> camerapacketpointer(new mavlink_camera_trigger_t);

   // qDebug() << "Reading...";

    //Read a QByteArray from TCP Socket
    bufferqbytearray = socket->readAll();
    //Convert QByteArray to a vector of uint_8
    std::vector<unsigned char> buffervector(
        bufferqbytearray.begin(), bufferqbytearray.end());


    //Todo: impletemt testing for message errors and incomplete messages

    //Decode the vector into a message
    for(int i = 0; i < buffervector.size(); i++){
        mavlink_parse_char(MAVLINK_COMM_1, buffervector[i], &message, &status);
    }

    //do message recieved boolean
    //if(msgReceived){

        switch (message.msgid) {

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            mavlink_msg_global_position_int_decode(&message, gpspacketpointer.get());            
            emit mavlinkgpsinfo(gpspacketpointer);
            //qDebug() << gpspacketpointer->time_boot_ms << gpspacketpointer->lat << gpspacketpointer->lon;
            break;

        case MAVLINK_MSG_ID_CAMERA_TRIGGER_CRC:
            mavlink_msg_camera_trigger_decode(&message, camerapacketpointer.get());            
            emit mavlinkcamerainfo(camerapacketpointer);
            //qDebug() << "Camera Triggered  " << camerapacketpointer->time_usec << "   " << camerapacketpointer->seq;
            break;

        default:
            break;
        }
    //}
}
//push on to new branch
//when everything is done do pull on master
