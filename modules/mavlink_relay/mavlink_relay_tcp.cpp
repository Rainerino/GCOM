//===================================================================
// Includes
//===================================================================
// System Includes
#include <QtCore>
#include <QString>
#include <memory>
#include <qdebug>
// GCOM Includes
#include "mavlink_relay_tcp.hpp"
#include "../Mavlink/ardupilotmega/mavlink.h"

//===================================================================
// Class Definitions
//===================================================================
MAVLinkRelay::MAVLinkRelay()
{
    ipaddress = "127.0.0.1";
    port = 14550;
    missionplannerSocket = nullptr;
}

bool MAVLinkRelay::stop()
{
    if (missionplannerSocket != nullptr)
    {
        missionplannerSocket->disconnectFromHost();
        disconnect(missionplannerSocket,SIGNAL(connected()), this, SLOT(connected()));
        disconnect(missionplannerSocket,SIGNAL(disconnected()), this, SLOT(disconnected()));
        disconnect(missionplannerSocket,SIGNAL(readyRead()), this, SLOT(readBytes()));
        delete missionplannerSocket;
        missionplannerSocket = nullptr;

        return true;
    }

    return false;
}

void MAVLinkRelay::setup(QString ipAddress, qint16 port)
{
    // If setup was called twice  withougt calling stop in between then we call
    // stop
    if (missionplannerSocket != nullptr)
        stop();

    // Save connection Parameters
    this->ipaddress = ipAddress;
    this->port = port;

    // Build the sockets and connect the signals/slots
    missionplannerSocket = new QTcpSocket(this);
    connect(missionplannerSocket,SIGNAL(connected()), this, SLOT(connected()));
    connect(missionplannerSocket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(missionplannerSocket,SIGNAL(readyRead()), this, SLOT(readBytes()));
}

bool MAVLinkRelay::start(int timeout)
{
    if(missionplannerSocket == nullptr)
        return false;

    missionplannerSocket->connectToHost(ipaddress, port);
    if(!missionplannerSocket->waitForConnected(timeout))
        return false;

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

    // Read all available data from TCP Socket
    bufferByteArray = missionplannerSocket->readAll();
    // Loop through every read byte and pass it through the decode function
    for(auto messageCurrentByte = bufferByteArray.begin();
        messageCurrentByte != bufferByteArray.end(); ++messageCurrentByte)
    {
        // Note, mavlink_parse_char keeps an internal buffer which you pass each
        // byte to, when a message is fully decoded, msgReceived is returned as
        // true. If a message has not fully been decoded, or a corrupt message is
        // received then False is returned.

        // The implementation of mavlink_parse_char is not thread safe when
        // decoding the same comm channel!!
        msgReceived = mavlink_parse_char(MAVLINK_COMM_1,
                                         static_cast<uint8_t>(*messageCurrentByte),
                                         &message, &status);
        if(msgReceived)
        {
            switch (message.msgid)
            {

                case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                {
                    std::shared_ptr<mavlink_global_position_int_t> gpsPacketPointer(
                                new mavlink_global_position_int_t);
                    mavlink_msg_global_position_int_decode(&message,gpsPacketPointer.get());
                    emit mavrelayGPSInfo(gpsPacketPointer);
                    break;
                }

                case MAVLINK_MSG_ID_CAMERA_FEEDBACK:
                {
                    std::shared_ptr<mavlink_camera_feedback_t> cameraPacketPointer(
                                new mavlink_camera_feedback_t);
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
