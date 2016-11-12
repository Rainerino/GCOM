#include "antennatracker.hpp"
//#include <QSerialPort>

AntennaTracker::AntennaTracker()
{
    //nothing needs to go here
}

AntennaTracker::~AntennaTracker()
{
    //close serial connection to arduino if it's open
    if(this->arduino_serial && this->arduino_serial->isOpen())
        this->arduino_serial->close();

    //close serial connection to zaber if it's open
    if(this->zaber_serial && this->zaber_serial->isOpen())
        this->zaber_serial->close();
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

void AntennaTracker::setupDevice(std::string arduino_port, std::string zaber_port)
{
    //convert arduino and zaber port names to qstring
    this->arduino_port = QString::fromStdString(arduino_port);
    this->zaber_port = QString::fromStdString(zaber_port);

    //intialize arduino serial port
    this->arduino_serial = new QSerialPort(this->arduino_port);
    this->arduino_serial->setPortName("arduino");
    this->arduino_serial->setBaudRate(QSerialPort::Baud57600);
    this->arduino_serial->setDataBits(QSerialPort::Data8);
    this->arduino_serial->setParity(QSerialPort::NoParity);
    this->arduino_serial->setStopBits(QSerialPort::OneStop);
    this->arduino_serial->setFlowControl(QSerialPort::NoFlowControl);

    //intializae arduino serial port
    this->zaber_serial = new QSerialPort(this->zaber_port);
    this->zaber_serial->setPortName("zaber controller");
    this->zaber_serial->setBaudRate(QSerialPort::Baud9600);
    this->zaber_serial->setDataBits(QSerialPort::Data8);
    this->zaber_serial->setParity(QSerialPort::NoParity);
    this->zaber_serial->setStopBits(QSerialPort::OneStop);
    this->zaber_serial->setFlowControl(QSerialPort::NoFlowControl);
}

bool AntennaTracker::startDevice(MissionPlannerSocket * const relay)
{
    //failed if either arduino_serial or zaber_serial is uninitialized
    if(this->arduino_serial == NULL || this->zaber_serial == NULL)
        return false;

    //open arduino_serial and zaber_serial
    bool zaberOpen = this->arduino_serial->open(QIODevice::ReadWrite);
    bool arduinoOpen = this->zaber_serial->open(QIODevice::ReadWrite);

    //connect mavlink relay
    connect(relay, SIGNAL(mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t>)), this, SLOT(receiveHandler(std::shared_ptr<mavlink_global_position_int_t>)));

    //return true if both arduino and zaber are open, and false otherwise
    if(zaberOpen && arduinoOpen)
        return true;
    else
        return false;
}

//incomplete
void AntennaTracker::receiveHandler(std::shared_ptr<mavlink_global_position_int_t>)
{
    qDebug() << "Mavlink signal consumed!!!!!!" << endl;

    QByteArray request = QByteArray::fromStdString(this->arduino_request);  //convert request msg to qstring
    this->arduino_serial->write(request);   //write request message to arduino

    int delayMilsec = 200;  //setup time-out delay
    //read from arduino if ready to read
    //return if readyread has timed out
    if(arduino_serial->waitForReadyRead(delayMilsec)) {
        QByteArray arduinoInByte = this->arduino_serial->readAll();
        QString arduinoInString = QString(arduinoInByte);
        qDebug() << "Received From Arduino: " << arduinoInString << endl;
    }
    else {
        return;
    }

    //do stuff
}

//NEEDS TO BE IMPLEMENTED
std::string AntennaTracker::calcMovement(){
    return null;
}
