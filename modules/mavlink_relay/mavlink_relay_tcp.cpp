#include "mavlink_relay_tcp.hpp"
#include "../Mavlink/ardupilotmega/mavlink.h"
#include <QtCore>
#include <QString>
#include <memory>
#include <qdebug>

//===================================================================
// Class Definitions
//===================================================================
MAVLinkRelay::MAVLinkRelay()
{
    ipaddress = "127.0.0.1";
    port = 14550;
    timeout = 1000;
    missionplannerSocket = nullptr;
}

bool MAVLinkRelay::stop()
{
    if (missionplannerSocket != nullptr)
    {

        disconnect(missionplannerSocket,SIGNAL(connected()), this, SLOT(connected()));
        disconnect(missionplannerSocket,SIGNAL(disconnected()), this, SLOT(disconnected()));
        disconnect(missionplannerSocket,SIGNAL(readyRead()), this, SLOT(readBytes()));
        delete missionplannerSocket;

        return true;
    }

    return false;

}

void MAVLinkRelay::setup(QString ipaddress, qint16 port, int timeout)
{
    // Save connection Parameters
    this->ipaddress = ipaddress;
    this->port = port;
    this->timeout = timeout;
    // Build the sockets and connect the signals/slots
    missionplannerSocket = new QTcpSocket(this);
    connect(missionplannerSocket,SIGNAL(connected()), this, SLOT(connected()));
    connect(missionplannerSocket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(missionplannerSocket,SIGNAL(readyRead()), this, SLOT(readBytes()));
}

bool MAVLinkRelay::start()
{
    if(missionplannerSocket == nullptr)
    {
        return false;
    }

    missionplannerSocket->connectToHost(ipaddress, port);
    if(!missionplannerSocket->waitForConnected(timeout))
    {
        return false;
    }

    return true;
}

void MAVLinkRelay::connected()
{
     emit mavrelayConnected();
}

void MAVLinkRelay::disconnected()
{
    emit mavrelayDisconnected();
}

void MAVLinkRelay::readBytes()
{
    mavlink_message_t message;
    mavlink_status_t status;
    uint8_t msgReceived = false;
    QByteArray bufferByteArray;

    // Read from TCP Socket
    bufferByteArray = missionplannerSocket->readAll();
    // Decode and emit the mavlink message
    for(auto messageCurrentByte = bufferByteArray.begin(); messageCurrentByte != bufferByteArray.end(); ++messageCurrentByte)
    {
        msgReceived = mavlink_parse_char(MAVLINK_COMM_1, static_cast<uint8_t>(*messageCurrentByte), &message, &status);
        if(msgReceived)
        {
            switch (message.msgid)
            {

                case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                {
                    std::shared_ptr<mavlink_global_position_int_t> gpsPacketPointer(new mavlink_global_position_int_t);
                    qDebug() << "GOT IT";
                    mavlink_msg_global_position_int_decode(&message, gpsPacketPointer.get());
                    emit mavrelayGPSInfo(gpsPacketPointer);
                    break;
                }

                case MAVLINK_MSG_ID_CAMERA_FEEDBACK:
                {
                    qDebug() << "GOT CAM WITH MESSAGE: " << msgReceived;
                    std::shared_ptr<mavlink_camera_feedback_t> cameraPacketPointer(new mavlink_camera_feedback_t);
                    mavlink_msg_camera_feedback_decode(&message, cameraPacketPointer.get());
                    emit mavrelayCameraInfo(cameraPacketPointer);
                    break;
                }

                default:
                    break;
            }
        }
    }
}

