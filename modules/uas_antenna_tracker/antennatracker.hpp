#ifndef ANTENNATRACKER_H
#define ANTENNATRACKER_H

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
    enum class AntennaTrackerConnectionState
    {
        UNDEFINED_STATE,
        SUCCESS,
        ARDUINO_UNINITIALIZED,
        ZABER_UNITIALIZED,
        ARDUINO_NOT_OPEN,
        ZABER_NOT_OPEN,
        RELAY_NOT_OPEN,
        FAILED
    };

    enum class AntennaTrackerSerialDevice
    {
        UNDEFINED_DEVICE,
        ARDUINO,
        ZABER
    };

    // TODO: Make static
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
    bool setupDevice(QString port, QSerialPort::BaudRate baud, AntennaTrackerSerialDevice devType);
    AntennaTrackerConnectionState getDeviceStatus(AntennaTrackerSerialDevice device);
    void disconnectArduino();
    void disconnectZaber();

    AntennaTrackerConnectionState startTracking(MAVLinkRelay * const relay);
    void stopTracking();

    /*!
     * \brief setStationPos overrides the GPS coordinates of the antenna tracker station with the inputted parameters.
     * \param lon is the longitude
     * \param lat is the latitude
     * \return whether the position has been set
     */
    bool setStationPos(float lon, float lat);

    /*!
     * \brief overrideGPSToggle sets overrideGPSToggle to the given state
     */
    void setOverrideGPSToggle(bool toggled);

public slots:
    /*!
     * \brief receiveHandler handles getting serial data from the arduino once a set of
     * GPS data has been received
     * \param gps_data is the mavlink GPS data received
     */
    void receiveHandler(std::shared_ptr<mavlink_global_position_int_t> gpsData);

signals:
    void antennaTrackerDeviceDisconnected(AntennaTrackerSerialDevice device);
    void antennaTrackerDisconnected();

private:
    /*!
     * \brief calcHorizontal returns a string command indicating the horizontal movement required to point at the drone.
     * The calculation is based on positional data form the drone.
     * \return command in the form of a string
     */
    QString calcHorizontal(std::shared_ptr<mavlink_global_position_int_t> gpsData, float yawIMU);

    /*!
     * \brief calcVertical returns a string command indicating the vertical movement required to point at the drone.
     * The calculation is based on positional data form the drone.
     * \return command in the form of a string
     */
    QString calcVertical(std::shared_ptr<mavlink_global_position_int_t> gpsData, float pitchIMU);

    /*!
     * \brief retrieveStationPos sets the actual GPS coordinates of the antenna tracker station.
     * \return false if the antenna tracker connection state has failed
     */
    bool retrieveStationPos();

    // ================
    // Member Variables
    // ================
    QSerialPort *arduinoSerial;
    QSerialPort *zaberSerial;
    QDataStream *arduinoDataStream;
    UASMessageSerialFramer *arduinoFramer;

    // Base station GPS
    float latBase;
    float lonBase;

    // State Variables
    bool antennaTrackerConnected;
    bool overrideGPSToggle;

    std::atomic<bool> sentRequest;

private slots:
    void zaberControllerDisconnected(QSerialPort::SerialPortError error);
    void arduinoDisconnected(QSerialPort::SerialPortError);
};

#endif // ANTENNATRACKER_H
