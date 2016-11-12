#include "antennatracker.hpp"
//#include <QSerialPort>

AntennaTracker::AntennaTracker()
{

}

AntennaTracker::~AntennaTracker(){
    if(this->arduino_serial && this->arduino_serial->isOpen())
        this->arduino_serial->close();

    if(this->zaber_serial && this->zaber_serial->isOpen())
        this->zaber_serial->close();
}

QList<QString> AntennaTracker::getArduinoList() {
    QList<QString> available;
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo currPort, portList) {
        if(currPort.manufacturer().contains(this->arduino_port_name)) {
            available.append(currPort.portName());
        }
    }

    return available;
}

QList<QString> AntennaTracker::getZaberList() {
    QList<QString> available;
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo currPort, portList) {
        if(currPort.manufacturer().contains(this->zaber_port_name)) {
            available.append(currPort.portName());
        }
    }

    return available;
}

void AntennaTracker::setupDevice(std::string arduino_port="COM5", std::string zaber_port="COM3") {
    this->arduino_port = QString::fromStdString(arduino_port);
    this->zaber_port = QString::fromStdString(zaber_port);

    //intialize arduino serial port
    this->arduino_serial = new QSerialPort(this->arduino_port);
    this->arduino_serial->setPortName("arduino");
    this->arduino_serial->setBaudRate(QSerialPort::Baud9600);
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

bool AntennaTracker::startDevice() {
    if(this->arduino_serial == NULL || this->zaber_serial == NULL)
        return false;

    this->arduino_serial->open(QIODevice::ReadWrite);
    this->zaber_serial->open(QIODevice::ReadWrite);

    //connect mavlink relay
    //connect(clientConnection, SIGNAL(readyRead()), this, SLOT(receiveHandler());

    if(this->arduino_serial->isOpen() && this->zaber_serial->isOpen())
        return true;
    else
        return false;
}

//incomplete
void AntennaTracker::receiveHandler(){
    QByteArray request = QByteArray::fromStdString(this->arduino_request);
    this->arduino_serial->write(request);
    QByteArray arduinoInByte = this->arduino_serial->readAll();
    QString arduinoInString = QString(arduinoInByte);
    //this->gpsData = mavlinkData;

    //do stuff
}
