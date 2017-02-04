#include "antennatracker.hpp"

AntennaTracker::AntennaTracker()
{
    arduino_serial = nullptr;
    zaber_serial = nullptr;

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

    arduinoPortName = "Arduino";
    zaberPortName = "Zaber";
}

AntennaTracker::~AntennaTracker()
{
    //close serial connection to arduino if it's open
    if(arduinoSerial != nullptr && arduinoSerial->isOpen()) {
        arduinoSerial->close();
        delete zaber_serial;
    }

    //close serial connection to zaber if it's open
    if(zaberSerial != nullptr && zaberSerial->isOpen()) {
        zaberSerial->close();
        delete zaberSerial;
    }
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

QList<QString> AntennaTracker::getArduinoList()
{
    QList<QString> arduinoPorts;    //list of arduinos
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();    //get a list of all available ports
    //iterate through list of ports
    foreach(QSerialPortInfo currPort, portList) {
        //check for arduino ports and add to list
        if(currPort.manufacturer().contains(this->arduino_port_name)) {
            arduinoPorts.append(currPort.portName());
        }
    }

    return arduinoPorts;    //return list of arduinos
}

QList<QString> AntennaTracker::getZaberList()
{
    QList<QString> zaberPorts;  //list of zaber controllers
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();    //get a list of all available ports
    //iterate through list of ports
    foreach(QSerialPortInfo currPort, portList) {
        //check for zaber ports and add to list
        if(currPort.manufacturer().contains(this->zaber_port_name)) {
            zaberPorts.append(currPort.portName());
        }
    }

    return zaberPorts;   //return list of zaber controllers
}

void AntennaTracker::setupDevice(QString arduinoPort, QString zaberPort, QSerialPort::BaudRate arduinoBaud, QSerialPort::BaudRate zaberBaud)
{
    //convert arduino and zaber port names to qstring
    this->arduinoPort = arduinoPort;
    this->zaberPort = zaberPort;

    //intialize arduino serial port
    this->arduinoPort = new QSerialPort(this->arduinoPort);
    this->arduinoPort->setPortName("arduino");
    this->arduinoPort->setBaudRate(arduinoBaud);
    this->arduinoPort->setDataBits(QSerialPort::Data8);
    this->arduinoPort->setParity(QSerialPort::NoParity);
    this->arduinoPort->setStopBits(QSerialPort::OneStop);
    this->arduinoPort->setFlowControl(QSerialPort::NoFlowControl);

    //intializae zaber serial port
    this->zaberSerial = new QSerialPort(this->zaberPort);
    this->zaberSerial->setPortName("zaber controller");
    this->zaberSerial->setBaudRate(zaberBaud);
    this->zaberSerial->setDataBits(QSerialPort::Data8);
    this->zaberSerial->setParity(QSerialPort::NoParity);
    this->zaberSerial->setStopBits(QSerialPort::OneStop);
    this->zaberSerial->setFlowControl(QSerialPort::NoFlowControl);
}

AntennaTracker::deviceConnectionStat AntennaTracker::startDevice(MissionPlannerSocket * const relay)
{
    //if arduino or zaber uninitialized, initialize first item in the list

    //failed if either arduino_serial or zaber_serial is uninitialized
    if(this->arduino_serial == nullptr)
        return deviceConnectionStat::ARDUINO_UNINITIALIZED;

    if(this->zaber_serial == nullptr)
        return deviceConnectionStat::ZABER_UNITIALIZED;

    //open arduino_serial and zaber_serial
    bool zaberOpen = this->arduinoSerial->open(QIODevice::ReadWrite);
    bool arduinoOpen = this->zaberSerial->open(QIODevice::ReadWrite);

    //return true if both arduino and zaber are open, and false otherwise
    if(!arduinoOpen)
        return deviceConnectionStat::ARDUINO_NOT_OPEN;
    else if(!zaberOpen)
        return deviceConnectionStat::ZABER_NOT_OPEN;

    //connect mavlink relay
    connect(relay, SIGNAL(mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t>)), this, SLOT(receiveHandler(std::shared_ptr<mavlink_global_position_int_t>)));
    return deviceConnectionStat::SUCCESS;
}

//incomplete
void AntennaTracker::receiveHandler(std::shared_ptr<mavlink_global_position_int_t> gps_data)
{
    qDebug() << "Mavlink signal consumed!!!!!!" << endl;

    QByteArray request = QByteArray::fromStdString(this->arduino_request);  //convert request msg to qstring
    this->arduino_serial->write(request);   //write request message to arduino

    int delayMilsec = 2000;  //setup time-out delay (2 seconds)
    //read from arduino if ready to read
    //return if readyread has timed out
    if(arduino_serial->waitForReadyRead(delayMilsec)) {
        QByteArray arduinoInByte = this->arduino_serial->readAll();
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

    if(!(angle_diff > -0.01 && angle_diff < 0.01)) {
        tracker_angle = tracker_angle + (yawIMU - prevYawIMU);
    }

    float xDiff = (droneLat-baseLat) * degToRad;
    float yDiff = (droneLon-baseLon) * degToRad;
    float a = pow(sin(xDiff/2),2) + cos(baseLat*degToRad) * cos(droneLat*degToRad) * pow(sin(yDiff/2),2);
    float d = 2 * atan2(sqrt(a), sqrt(1-a));
    float distance = radiusEarth * d;

    float y = sin(yDiff) * cos(droneLat*degToRad);
    float x = cos(baseLat*degToRad) * sin(droneLat*degToRad) - sin(baseLat*degToRad) * cos(droneLat*degToRad) * cos(yDiff);

    float horizAngle = atan2(y,x)*radToDeg;
    droneAngle = horizAngle;
    float vertAngle = atan((droneRelativeAlt)/(distance*1000)) * 180/M_PI;

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
    if(angle_diff < 0.2 && angle_diff >= 0.03) {
        speedm = "10";
    }
    else if(angle_diff < 2 && angle_diff >= 0.2) {
        speedm = "20";
    }
    else if(angle_diff >= 0.2) {
        speedm = "300";
    }
    else {
        speedm = "0";
    }

    prevYawIMU = yawIMU;

    QString commandMessage = "/1 move rel " + commandDir + commandSpeed;
    return commandMessage;
}
