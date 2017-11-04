#ifndef UAS_UTILITY_H
#define UAS_UTILITY_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QList>
#include <vector>
#include <math.h>
#include <atomic>
#include "../Mavlink/ardupilotmega/mavlink.h"
#include "modules/mavlink_relay/mavlink_relay_tcp.hpp"
#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/request_message.hpp"
#include "modules/uas_message/uas_message_serial_framer.hpp"

namespace Utility
{
    /*!
     * \brief calcHorizontal returns a string command indicating horizontal movement required to point at the drone.
     * The calculation is based on positional data form the drone.
     * \return command in the form of a string
     */
    float calcHorizontal(std::shared_ptr<mavlink_global_position_int_t> gpsData, float yawIMU, float latBase,
                         float lonBase, float headingBase);

    /*!
     * \brief calcVertical returns a string command indicating vertical movement required to point at the drone.
     * The calculation is based on positional data form the drone.
     * \return command in the form of a string
     */
    float calcVertical(std::shared_ptr<mavlink_global_position_int_t> gpsData, float pitchIMU, float latBase,
                       float lonBase, float elevationBase);

}

#endif // UAS_UTILITY_H
