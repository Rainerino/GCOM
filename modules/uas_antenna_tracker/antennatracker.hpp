#ifndef ANTENNATRACKER_H
#define ANTENNATRACKER_H

/*
 * currently runs off main wind
 * SHOULD BE CHANGED TO RUN OFF A THREAD LATER ON!!!!
 */
#include <QMainWindow>
#include <string>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <Qtcore/QDebug>
#include <QList>

#include <vector>
#include <math.h>

#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/uas_message_tcp_framer.hpp"
#include "Mavlink/common/mavlink.h"
#include "modules/mavlink_relay/missionplannertcp.hpp"

/*!
 * \brief The AntennaTracker class
 */
class AntennaTracker : QObject
{
    Q_OBJECT

public:
    /*!
     * \brief AntennaTracker is the constructor where pointers are declared as null
     */
    AntennaTracker();
    ~AntennaTracker();

    /*!
     * \brief The DEVICE_STAT enum indicates current status of all devices connected.
     */
    enum class deviceConnectionStat
    {
        SUCCESS,
        ARDUINO_UNINITIALIZED,
        ZABER_UNITIALIZED,
        ARDUINO_NOT_OPEN,
        ZABER_NOT_OPEN
    };

    //SHOULD BE STATIC
    /*!
     * \brief getArduinoList returns a list of all available serial devices that can be identified as an Arduino
     * \return QList of port names
     */
    static QList<QString> getArduinoList();
    /*!
     * \brief getZaberList returns a list of all available serial devices that can be identified as a Zaber Controller
     * \return QList of port names
     */
    static QList<QString> getZaberList();

    /*!
     * \brief setupDevice initializes devices indicated by arduino_port and zaber_port, and initializes the devices
     * \param arduino_port is the serial port for the arduino
     * \param zaber_port is the serial port for the zaber controller
     */
    void setupDevice(QString arduinoPort, QString zaberPort, QSerialPort::BaudRate arduinoBaud, QSerialPort::BaudRate zaberBaud);
    deviceConnectionStat startDevice(MissionPlannerSocket * const relay);

    /*!
     * \brief setStationPos sets the GPS coordinates of the antenna tracker station
     * \param lon is the longitude
     * \param lat is the latitude
     * \return whether the position has been set
     */
    bool setStationPos(QString lon, QString lat);

public slots:
    /*!
     * \brief receiveHandler handles getting serial data from the arduino once a set of
     * GPS data has been received
     * \param gps_data is the mavlink GPS data received
     */
    void receiveHandler(std::shared_ptr<mavlink_global_position_int_t> gpsData);

private:
    QString arduinoPort;
    QString zaberPort;
    QSerialPort *arduinoSerial;
    QSerialPort *zaberSerial;

    mavlink_global_position_int_t gpsData;

    float degToRad;
    float radToDeg;
    float radiusEarth;

    float prevYawIMU;
    float droneAngle;
    float trackerAngle;
    float angleDiff;

    uint32_t baseLat;
    uint32_t baseLon;

    QString arduinoPortName;
    QString zaberPortName;
    //std::string arduinoRequest = "GCOM_ready";

    /*!
     * \brief calcMovement returns a string command constrcuted based on GPS data and positional data form
     * the arduino.
     * \return command in the form of a string
     */
    QString calcMovement(std::shared_ptr<mavlink_global_position_int_t> gpsData, float yawIMU, float pitchIMU);
};

#endif // ANTENNATRACKER_H
