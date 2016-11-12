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

#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/uas_message_tcp_framer.hpp"
#include "Mavlink/common/mavlink.h"
#include "modules/mavlink_relay/missionplannertcp.hpp"

class AntennaTracker : QObject
{
    Q_OBJECT

public:
    explicit AntennaTracker();
    ~AntennaTracker();

    QList<QString> getArduinoList();
    QList<QString> getZaberList();

    void setupDevice(std::string arduino_port, std::string zaber_port);
    bool startDevice(MissionPlannerSocket * const relay);

public slots:
    //change this late
    void receiveHandler();

private:
    QString arduino_port;
    QString zaber_port;
    QSerialPort *arduino_serial;
    QSerialPort *zaber_serial;

    //mavlink_msg_global_position_int gpsData;

    float base_lat;
    float base_lon;

    QString arduino_port_name = "Arduino";
    QString zaber_port_name = "Zaber";
    std::string arduino_request = "GCOM_ready";
    //std::string calcMovement();
};

#endif // ANTENNATRACKER_H
