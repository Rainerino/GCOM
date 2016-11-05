#include "antennatracker.hpp"
#include "ui_gcomcontroller.h"
//#include <QSerialPort>
#include <string.h>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <Qtcore/QDebug>

QSerialPort *serial;

AntennaTracker::AntennaTracker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AtennaTracker)
{
    ui->setupUi(this);

    QSerialPortInfo info("COM5");
    qDebug() << "Name: " << info.portName() << endl;
    qDebug() << "Manufacturer: " << info.manufacturer() << endl;
    qDebug() << "Busy: " << info.isBusy() << endl;

    serial = new QSerialPort(this);
    serial->setPortName("COM5");
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(serial, SIGNAL(readyRead()), this, SLOT(receiveHandler());
}

AntennaTracker::~AntennaTracker(){
    delete ui;
}

void MainWindow::receiveHandler(){
    QByteArray inByteArray;
    inByte = serial->readAll();
    QString inData = QString(inByteArray);

    qDebug() << "response: " << inData << endl;
}
