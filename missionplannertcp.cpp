#include <QNetworkSession>
#include "missionplannertcp.hpp"
#include "Mavlink/common/mavlink.h"
#include <QtCore>

MissionPlannerSocket::MissionPlannerSocket()
{

}

void MissionPlannerSocket::run()
{
    socket = new QTcpSocket(this);

    connect(socket,SIGNAL(connected()), this, SLOT(connected()));
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket,SIGNAL(readyRead()), this, SLOT(readBytes()));
    connect(socket,SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    qDebug() << "Connecting...";

    socket->connectToHost("127.0.0.1",14550);

    if(!socket->waitForConnected(1000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void MissionPlannerSocket::connected()
{
     qDebug() << "Connected!";

}

void MissionPlannerSocket::disconnected()
{
    qDebug() << "Disconnected!";
}

void MissionPlannerSocket::bytesWritten(qint64 bytes)
{
    qDebug() << "Wrote: " << bytes;
}

void MissionPlannerSocket::readBytes()
{
    mavlink_message_t message;
    mavlink_status_t status;
    uint8_t msgReceived = false;
    QByteArray ct;
   // qDebug() << "Reading...";

    ct = socket->readAll();

    //Convert QByteArray to a vector of uint_8
    std::vector<unsigned char> cp(
        ct.begin(), ct.end());

    for(int i = 0; i < cp.size(); i++){
        msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp[i], &message, &status);
    }



    switch (message.msgid) {
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        mavlink_global_position_int_t packet;
        mavlink_msg_global_position_int_decode(&message, &packet);
        //qDebug() << packet.time_boot_ms << packet.lat << packet.lon << packet.hdg;
        break;
    case MAVLINK_MSG_ID_HIGHRES_IMU:
        qDebug() << "Hi";
        break;
    case MAVLINK_MSG_ID_CAMERA_TRIGGER_CRC:
        mavlink_camera_trigger_t camerapacket;
        mavlink_msg_camera_trigger_decode(&message, &camerapacket);

        qDebug() << "Camera Triggered  " << camerapacket.time_usec << "   " << camerapacket.seq;
        break;
    case MAVLINK_MSG_ID_CAMERA_TRIGGER:
        mavlink_camera_trigger_t camerapacket2;
        mavlink_msg_camera_trigger_decode(&message, &camerapacket2);
        qDebug() << "Camera Triggered  " << camerapacket2.time_usec << "   " << camerapacket2.seq;
    default:
        break;
    }
}
