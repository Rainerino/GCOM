#include "antennatracker.hpp"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <Qtcore/QDebug>
#include <QList>
#include <vector>
#include <math.h>
#include "../Mavlink/ardupilotmega/mavlink.h"
#include "modules/mavlink_relay/mavlink_relay_tcp.hpp"

//===================================================================
// Constants
//===================================================================
const QString zaberPortKey = "Microsoft";
const QString zaberNoController = "NO ZABER CONTROLLER CONNECTED";
const QString arduinoPortKey = "Arduino";
const QString arduinoNotConnected = "NO ARDUINO CONNECTED";
// Zaber Command Templates
const QString zaberStopCommandTemplate= "/1 %d stop\n";
const QString zaberMoveCommandTemplate= "/1 %d move rel %d\n";


//===================================================================
// Class Definitions
//===================================================================
AntennaTracker::AntennaTracker()
{
    // Default Values
    arduinoSerial = nullptr;
    zaberSerial = nullptr;

    // Reste the state varaibles
    antennaTrackerConnected = false;

    degToRad = 0.01745329251;
    radToDeg = 57.2957795131;
    radiusEarth = 6378137;

    //initialize base coordinates with arbitrary garbage value
    baseLat = 9999;
    baseLon = 9999;

    prevYawIMU = 0;
    droneAngle = 0;
    trackerAngle = 0;
    angleDiff = 0;
}

AntennaTracker::~AntennaTracker()
{
    //close serial connection to arduino if it's open
    if(arduinoSerial != nullptr && arduinoSerial->isOpen()) {
        arduinoSerial->close();
        delete zaberSerial;
    }

    //close serial connection to zaber if it's open
    if(zaberSerial != nullptr && zaberSerial->isOpen()) {
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

        // Initialize arduino serial port
        arduinoSerial->setBaudRate(baud);
        arduinoSerial->setDataBits(QSerialPort::Data8);
        arduinoSerial->setParity(QSerialPort::NoParity);
        arduinoSerial->setStopBits(QSerialPort::OneStop);
        arduinoSerial->setFlowControl(QSerialPort::NoFlowControl);

        if(!arduinoSerial->open(QIODevice::ReadWrite))
            return false;

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
    // Double check that the conditions required for the connection is correct
    if (arduinoSerial == nullptr)
        return AntennaTrackerConnectionState::ARDUINO_UNINITIALIZED;

    if (zaberSerial == nullptr)
        return AntennaTrackerConnectionState::ZABER_UNITIALIZED;

    if (!arduinoSerial->isOpen())
        return AntennaTrackerConnectionState::ARDUINO_NOT_OPEN;

    if (!zaberSerial->isOpen())
        return AntennaTrackerConnectionState::ZABER_NOT_OPEN;

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

bool AntennaTracker::setStationPos(QString lon, QString lat)
{
    baseLon = lon.toDouble();
    baseLat = lat.toDouble();

    //true if lat/lon changed, false if still garbage value(9999)
    if(baseLon == 9999 || baseLat == 9999)
        return false;
    else
        return true;
}

//incomplete
void AntennaTracker::receiveHandler(std::shared_ptr<mavlink_global_position_int_t> gps_data)
{
    qDebug() << "Mavlink signal consumed!!!!!!" << endl;

    QByteArray request = QByteArray::fromStdString("put request msg here");  //convert request msg to qstring
    this->arduinoSerial->write(request);   //write request message to arduino

    int delayMilsec = 2000;  //setup time-out delay (2 seconds)
    //read from arduino if ready to read
    //return if readyread has timed out
    if(arduinoSerial->waitForReadyRead(delayMilsec)) {
        QByteArray arduinoInByte = this->arduinoSerial->readAll();
        QString arduinoInString = QString(arduinoInByte);
        qDebug() << "Received From Arduino: " << arduinoInString << endl;

        //QString moveCommand = calcMovement(gps_data);
    }
    else {
        return;
    }
}

//NEEDS TO BE IMPLEMENTED
QString AntennaTracker::calcMovement(std::shared_ptr<mavlink_global_position_int_t> gpsData, float yawIMU, float pitchIMU)
{
    //Grabbing individual pieces of data from gpsData
    float droneLat = gpsData->lat;
    float droneLon = gpsData->lon;

    int32_t droneAlt = gpsData->alt;
    int32_t droneRelativeAlt = gpsData->relative_alt;

    int16_t droneVX = gpsData->vx;
    int16_t droneVY = gpsData->vy;
    int16_t droneVZ = gpsData->vz;

    // Current Tracker Angle
    if(!(angleDiff > -0.01 && angleDiff < 0.01)) {
        trackerAngle = trackerAngle + (yawIMU - prevYawIMU);
    }

    float xDiff = (droneLat-baseLat) * degToRad;
    float yDiff = (droneLon-baseLon) * degToRad;
    float a = pow(sin(xDiff/2),2) + cos(baseLat*degToRad) * cos(droneLat*degToRad) * pow(sin(yDiff/2),2);
    float d = 2 * atan2(sqrt(a), sqrt(1-a));
    float distance = radiusEarth * d;

    float y = sin(yDiff) * cos((droneLat*degToRad));
    float x = cos(baseLat*degToRad) * sin(droneLat*degToRad) - sin(baseLat*degToRad) * cos(droneLat*degToRad) * cos(yDiff);

    float horizAngle = atan2(y,x)*radToDeg;
    droneAngle = horizAngle;
    float vertAngle = atan((droneRelativeAlt)/(distance*1000)) * 180/M_PI;

    // Find the quickest angle to reach the point
    angleDiff = droneAngle - trackerAngle;
    if(angleDiff > 180) {
        angleDiff -= 360;
    }
    else if(angleDiff < -180) {
        angleDiff += 360;
    }

    QString commandDir = "";
    if(angleDiff < 0) {
        commandDir = "";
        angleDiff *= -1;
    }
    else {
        commandDir = "-";
    }

    QString commandSpeed = "";
    if(angleDiff < 0.2 && angleDiff >= 0.03) {
        commandSpeed = "10";
    }
    else if(angleDiff < 2 && angleDiff >= 0.2) {
        commandSpeed = "20";
    }
    else if(angleDiff >= 2) {
        commandSpeed = "300";
    }
    else {
        commandSpeed = "0";
    }

    prevYawIMU = yawIMU;

    QString commandMessage = "/1 move rel " + commandDir + commandSpeed;
    return commandMessage;
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
        if(currPort.manufacturer().contains(zaberPortKey))
            zaberPorts.append(currPort.portName());
    }

    if(zaberPorts.isEmpty())
    {
        zaberPorts.append(zaberNoController);
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
        if(currPort.manufacturer().contains(arduinoPortKey))
            arduinoPorts.append(currPort.portName());
    }

    if(arduinoPorts.isEmpty())
    {
        arduinoPorts.append(arduinoNotConnected);
        return arduinoPorts;
    }
    else
    {
        return arduinoPorts;    //return list of arduinos
    }
}
