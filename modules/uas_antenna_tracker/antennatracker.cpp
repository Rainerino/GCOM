//===================================================================
// Includes
//===================================================================
// GCOM Includes
#include "antennatracker.hpp"
#include "modules/mavlink_relay/mavlink_relay_tcp.hpp"
#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/request_message.hpp"
#include "modules/uas_message/gps_message.hpp"
#include "modules/uas_message/imu_message.hpp"
#include "modules/uas_message/uas_message_serial_framer.hpp"
#include "../Mavlink/ardupilotmega/mavlink.h"
// QT Includes
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <Qtcore/QDebug>
#include <QList>
#include <QtMath>
#include <QElapsedTimer>
#include <QDebug>
// System Includes
#include <vector>
#include <math.h>

//===================================================================
// Defines
//===================================================================
#define DEGREE_TO_MICROSTEPS 1599.722
#define ANGLE_TO_MICROSTEPS(x) (x * DEGREE_TO_MICROSTEPS)
#define UNPACK_LAT_LON(x)  (((float) x)/10000000)

//===================================================================
// Constants
//===================================================================
// Strings
const QString ZABER_DEVICE_NAME = "Microsoft";
const QString TEXT_ZABER_NO_CONTROLLER = "NO ZABER CONTROLLER CONNECTED";
const QString ARDUINO_DEVICE_NAME = "Arduino";
const QString TEXT_ARDUINO_NOT_CONNECTED = "NO ARDUINO CONNECTED";
// Zaber Command Templates
const QString zaberStopCommandTemplate= "/1 %1 stop\n";
const QString zaberMoveCommandTemplate= "/1 %1 move rel %2\n";
// Constants
const float RADIUS_EARTH = 6378137;

//===================================================================
// Class Definitions
//===================================================================
AntennaTracker::AntennaTracker()
{
    // Default Values
    arduinoSerial = nullptr;
    zaberSerial = nullptr;
    arduinoFramer = new UASMessageSerialFramer();

    // Reset the state varaibles
    antennaTrackerConnected = false;

    //initialize base coordinates with arbitrary garbage value
    latBase = 0;
    lonBase = 0;

    prevYawIMU = 0;
    droneAngle = 0;
    trackerAngle = 0;
    angleDiff = 0;
    sentRequest = false;
}

AntennaTracker::~AntennaTracker()
{
    //close serial connection to arduino if it's open
    if(arduinoSerial != nullptr && arduinoSerial->isOpen())
    {
        arduinoSerial->close();
        delete zaberSerial;
    }

    //close serial connection to zaber if it's open
    if(zaberSerial != nullptr && zaberSerial->isOpen())
    {
        zaberSerial->close();
        delete zaberSerial;
    }
}

bool AntennaTracker::setupDevice(QString port, QSerialPort::BaudRate baud,
                                 AntennaTrackerSerialDevice devType)
{
    if (antennaTrackerConnected)
        stopTracking();

    if(devType == AntennaTrackerSerialDevice::ARDUINO)
    {
        // Create the object if its not already initialized
        if (arduinoSerial == nullptr)
            arduinoSerial = new QSerialPort(port);

        // Close the port if its open
        if (arduinoSerial->isOpen())
            disconnectArduino();



        if(!arduinoSerial->open(QIODevice::ReadWrite))
            return false;

        // Initialize arduino serial port
        arduinoSerial->setBaudRate(QSerialPort::BaudRate::Baud9600);
        arduinoSerial->setDataBits(QSerialPort::Data8);
        arduinoSerial->setParity(QSerialPort::NoParity);
        arduinoSerial->setStopBits(QSerialPort::OneStop);
        arduinoSerial->setFlowControl(QSerialPort::NoFlowControl);

        connect(arduinoSerial,
                SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this,
                SLOT(arduinoDisconnected(QSerialPort::SerialPortError)));
    }
    else if(devType == AntennaTrackerSerialDevice::ZABER)
    {
        if (zaberSerial == nullptr)
            zaberSerial = new QSerialPort(port);

        if (zaberSerial->isOpen())
            disconnectZaber();

        // Init the  zaber serial port
        zaberSerial = new QSerialPort(port);
        zaberSerial->setBaudRate(baud);
        zaberSerial->setDataBits(QSerialPort::Data8);
        zaberSerial->setParity(QSerialPort::NoParity);
        zaberSerial->setStopBits(QSerialPort::OneStop);
        zaberSerial->setFlowControl(QSerialPort::NoFlowControl);
        if (!zaberSerial->open(QIODevice::ReadWrite))
            return false;

        connect(zaberSerial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
                this, SLOT(zaberControllerDisconnected(QSerialPort::SerialPortError)));
    }

    return true;
}

AntennaTracker::AntennaTrackerConnectionState AntennaTracker::startTracking(MAVLinkRelay * const relay)
{
    QElapsedTimer timer;
    timer.start();
    // Double check that the conditions required for the connection is correct
    if (arduinoSerial == nullptr)
        return AntennaTrackerConnectionState::ARDUINO_UNINITIALIZED;

    if (zaberSerial == nullptr)
        return AntennaTrackerConnectionState::ZABER_UNITIALIZED;

    if (!arduinoSerial->isOpen())
        return AntennaTrackerConnectionState::ARDUINO_NOT_OPEN;

    if (!zaberSerial->isOpen())
        return AntennaTrackerConnectionState::ZABER_NOT_OPEN;

    // Create the datastreams and framer
    arduinoDataStream = new QDataStream(arduinoSerial);

    // Retrieve the base's GPS Corrdinates
    RequestMessage gpsRequest = RequestMessage(UASMessage::MessageID::DATA_GPS);
    arduinoFramer->frameMessage(gpsRequest);
    (*arduinoDataStream) << (*arduinoFramer);

    // Verify the connection
    if (!arduinoSerial->waitForReadyRead(3000))
        return AntennaTrackerConnectionState::FAILED;

    arduinoFramer->clearMessage();

    // Attempt to read the GPS Message from the antenna tracker
    // Note: This is a bad busy loop! We are only allowed to do this because the drone will not be
    // in flight at this time!
    while (true)
    {
        arduinoDataStream->startTransaction();
        (*arduinoDataStream) >> (*arduinoFramer);
        if (arduinoFramer->status() == UASMessageSerialFramer::SerialFramerStatus::SUCCESS)
            break;
        else if (arduinoFramer->status() == UASMessageSerialFramer::SerialFramerStatus::INCOMPLETE_MESSAGE)
            arduinoDataStream->rollbackTransaction();
        else
            return AntennaTrackerConnectionState::FAILED;
        arduinoSerial->waitForReadyRead(10);
    }
    arduinoDataStream->commitTransaction();
    qDebug() << timer.elapsed();

    // Process the GPS Coordinates of the base station!
    std::shared_ptr<UASMessage> gpsMessage = arduinoFramer->generateMessage();
    if ((gpsMessage == nullptr) || (gpsMessage->type() != UASMessage::MessageID::DATA_GPS))
        return AntennaTrackerConnectionState::FAILED;

    lonBase = qDegreesToRadians(std::static_pointer_cast<GPSMessage>(gpsMessage)->lon * -1);
    latBase = qDegreesToRadians(std::static_pointer_cast<GPSMessage>(gpsMessage)->lat);

    // Connect the Mavlink Relay
    if(relay->status() != MAVLinkRelay::MAVLinkRelayStatus::CONNECTED)
        return AntennaTrackerConnectionState::RELAY_NOT_OPEN;

    connect(relay,
            SIGNAL(mavlinkRelayGPSInfo(std::shared_ptr<mavlink_global_position_int_t>)),
            this, SLOT(receiveHandler(std::shared_ptr<mavlink_global_position_int_t>)));

    antennaTrackerConnected = true;
    return AntennaTrackerConnectionState::SUCCESS;
}

void AntennaTracker::stopTracking()
{
    if (!antennaTrackerConnected)
        return;

    // Disconnect the mavlink relay
    disconnect(this, SLOT(receiveHandler(std::shared_ptr<mavlink_global_position_int_t>)));
    antennaTrackerConnected = false;
    emit antennaTrackerDisconnected();
}

void AntennaTracker::disconnectArduino()
{
    if (arduinoSerial == nullptr)
        return;

    if (!arduinoSerial->isOpen())
        return;

    if (antennaTrackerConnected)
        stopTracking();

    disconnect(arduinoSerial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
               this, SLOT(arduinoDisconnected(QSerialPort::SerialPortError)));
    arduinoSerial->close();
    emit antennaTrackerDeviceDisconnected(AntennaTrackerSerialDevice::ARDUINO);
}

void AntennaTracker::disconnectZaber()
{
    if (zaberSerial == nullptr)
        return;

    if (!zaberSerial->isOpen())
        return;

    if (antennaTrackerConnected)
        stopTracking();

    disconnect(zaberSerial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
               this, SLOT(zaberControllerDisconnected(QSerialPort::SerialPortError)));
    zaberSerial->close();
    emit antennaTrackerDeviceDisconnected(AntennaTrackerSerialDevice::ZABER);
}


AntennaTracker::AntennaTrackerConnectionState AntennaTracker::getDeviceStatus(
        AntennaTracker::AntennaTrackerSerialDevice device)
{
    switch (device)
    {
        case AntennaTrackerSerialDevice::ARDUINO:
            if (arduinoSerial == nullptr)
                return AntennaTrackerConnectionState::ARDUINO_UNINITIALIZED;
            else if (!arduinoSerial->isOpen())
                return AntennaTrackerConnectionState::ARDUINO_NOT_OPEN;
            else
                return AntennaTrackerConnectionState::SUCCESS;

        case AntennaTrackerSerialDevice::ZABER:
            if (zaberSerial == nullptr)
                return AntennaTrackerConnectionState::ZABER_UNITIALIZED;
            else if (!zaberSerial->isOpen())
                return AntennaTrackerConnectionState::ZABER_NOT_OPEN;
            else
                return AntennaTrackerConnectionState::SUCCESS;

        default:
            return AntennaTrackerConnectionState::FAILED;
    }
}

void AntennaTracker::arduinoDisconnected(QSerialPort::SerialPortError error)
{
    if (antennaTrackerConnected)
        stopTracking();

    disconnectArduino();
}

void AntennaTracker::zaberControllerDisconnected(QSerialPort::SerialPortError error)
{
    if (antennaTrackerConnected)
        stopTracking();

    disconnectZaber();
}

void AntennaTracker::receiveHandler(std::shared_ptr<mavlink_global_position_int_t> droneGPSData)
{
    qDebug() << "Mavlink signal consumed!!!!!!" << endl;


    RequestMessage imuRequest(UASMessage::MessageID::DATA_IMU);
    arduinoFramer->frameMessage(imuRequest);
    (*arduinoDataStream) << (*arduinoFramer);

    // Wait till we recieve data from the IMU
    while (true)
    {
        arduinoDataStream->startTransaction();
        (*arduinoDataStream) >> (*arduinoFramer);
        if (arduinoFramer->status() == UASMessageSerialFramer::SerialFramerStatus::SUCCESS)
        {
            break;
        }
        else if (arduinoFramer->status() == UASMessageSerialFramer::SerialFramerStatus::INCOMPLETE_MESSAGE)
        {
            arduinoDataStream->rollbackTransaction();
        }
        else
        {
            arduinoDataStream->commitTransaction();
            return;
        }
        arduinoSerial->waitForReadyRead(3000);
    }
    arduinoDataStream->commitTransaction();


    std::shared_ptr<UASMessage> imuMessage = arduinoFramer->generateMessage();
    if ((imuMessage == nullptr) || (imuMessage->type() != UASMessage::MessageID::DATA_IMU))
        return;

    float yawBase= std::static_pointer_cast<IMUMessage>(imuMessage)->x;
    float pitchBase = std::static_pointer_cast<IMUMessage>(imuMessage)->z;

    // Calculate Zaber Movement and send it.
    QString zaberCommand = calcMovement(droneGPSData, yawBase, pitchBase);
    //qDebug() << zaberCommand;
    zaberSerial->write(zaberCommand.toStdString().c_str());
    zaberSerial->flush();

}

//NEEDS TO BE IMPLEMENTED
QString AntennaTracker::calcMovement(std::shared_ptr<mavlink_global_position_int_t> droneGPSData, float yawIMU, float pitchIMU)
{
    //Grabbing individual pieces of data from gpsData

    float droneLat = qDegreesToRadians(((float) droneGPSData->lat)/ 10000000);
    float droneLon = qDegreesToRadians(((float) droneGPSData->lon)/ 10000000);

    float yDiff = (droneLon-lonBase);

    float y = sin(yDiff) * cos(droneLat);
    float x = (cos(latBase) * sin(droneLat)) - (sin(latBase) * cos(droneLat) * cos(yDiff));

    float horizAngle = fmod((qRadiansToDegrees(atan2(y,x)) + 360), 360);
    droneAngle = horizAngle;
    qDebug() << "droneAngle is: " << droneAngle;


    // Find the quickest angle to reach the point
    angleDiff = droneAngle - (yawIMU);

    qDebug() << "angleDiff before is: " << angleDiff;
    if(angleDiff > 180) {
        angleDiff -= 360;
    }
    else if(angleDiff < -180) {
        angleDiff += 360;
    }
    else if(angleDiff > -1 && angleDiff < 1) {
        angleDiff = 0;
    }
    qDebug() << "yawIMU is: " << (yawIMU);
    qDebug() << "angleDiff after is: " << angleDiff;

    int microSteps = -1 * ANGLE_TO_MICROSTEPS(angleDiff);
    //microSteps = (microSteps*-1);



    return QString(zaberMoveCommandTemplate).arg(2).arg(microSteps);
}

//===================================================================
// Listing Functions
//===================================================================
QList<QString> AntennaTracker::getZaberList()
{
    QList<QString> zaberPorts;
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();    //get a list of all available ports

    //Iterate through list of ports
    foreach(QSerialPortInfo currPort, portList)
    {
        //check for zaber ports and add to list
        if(currPort.manufacturer().contains(ZABER_DEVICE_NAME))
            zaberPorts.append(currPort.portName());
    }

    if(zaberPorts.isEmpty())
    {
        zaberPorts.append(TEXT_ZABER_NO_CONTROLLER);
        return zaberPorts;
    }
    else
    {
        return zaberPorts;   //return list of zaber controllers
    }
}

QList<QString> AntennaTracker::getArduinoList()
{
    QList<QString> arduinoPorts;    //list of arduinos
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();    //get a list of all available ports

    //iterate through list of ports
    foreach(QSerialPortInfo currPort, portList)
    {
        //check for arduino ports and add to list
        if(currPort.manufacturer().contains(ARDUINO_DEVICE_NAME))
            arduinoPorts.append(currPort.portName());
    }

    if(arduinoPorts.isEmpty())
    {
        arduinoPorts.append(TEXT_ARDUINO_NOT_CONNECTED);
        return arduinoPorts;
    }
    else
    {
        return arduinoPorts;    //return list of arduinos
    }
}
