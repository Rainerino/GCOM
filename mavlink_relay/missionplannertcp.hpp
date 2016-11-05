#ifndef MISSIONPLANNERTCP_HPP
#define MISSIONPLANNERTCP_HPP

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QtCore>
#include "Mavlink/common/mavlink.h"
#include <QSharedPointer>
#include <string>
#include <memory>


class MissionPlannerSocket : public QThread
{
    Q_OBJECT
public:
    void run(const std::string &ipaddress = "127.0.0.1", qint16 port = 14550, int timeout =1000);
    void exit();
    MissionPlannerSocket();
signals:
    void connectedtomavlink();
    void disconnectedfrommavlink();
    void mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t>);
    void mavlinkcamerainfo(std::shared_ptr<mavlink_camera_trigger_t>);

public slots:
    void connected();
    void disconnected();
    void readBytes();
private:
    QTcpSocket *socket;
};
#endif // MISSIONPLANNERTCP_HPP
