#ifndef MISSIONPLANNERTCP_HPP
#define MISSIONPLANNERTCP_HPP

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QtCore>
#include "Mavlink/common/mavlink.h"
#include "../Mavlink/ardupilotmega/ardupilotmega.h"
#include <string>
#include <memory>


class MissionPlannerSocket : public QThread
{
    Q_OBJECT
public:
    void run();

    void setup(QString ipaddress, qint16 port, int timeout = 1000);
    MissionPlannerSocket();
signals:
    void connectedtomavlink();
    void disconnectedfrommavlink();
    void mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t> gpsSignal);
    void mavlinkcamerainfo(std::shared_ptr<mavlink_camera_feedback_t> cameraSignal);
public slots:
    void connected();
    void disconnected();
    void readBytes();
    void end();


private:
    QTcpSocket *socket;
    QString ipaddress;
    qint16 port;
    int timeout;
    mavlink_status_t lastStatus;

};
#endif // MISSIONPLANNERTCP_HPP
