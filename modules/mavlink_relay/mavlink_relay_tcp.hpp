#ifndef MAVLINKRELAYTCP_HPP
#define MAVLINKRELAYTCP_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <string>
#include <memory>
// GCOM Includes
#include "../Mavlink/ardupilotmega/mavlink.h"


//===================================================================
// Class Declarations
//===================================================================
class MAVLinkRelay : QObject
{
    Q_OBJECT
public:
    // Public Methods
    MAVLinkRelay();

    bool start();
    void setup(QString ipaddress, qint16 port, int timeout = 1000);
    void stop();
signals:
    void connectedtomavlink();
    void disconnectedfrommavlink();
    void mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t> gpsSignal);
    void mavlinkcamerainfo(std::shared_ptr<mavlink_camera_feedback_t> cameraSignal);

public slots:
    void connected();
    void disconnected();
    void readBytes();


private:
    // Private Member Variables
    QTcpSocket *missionplannerSocket;
    QString ipaddress;
    qint16 port;
    int timeout;
    mavlink_status_t lastStatus;
};
#endif // MAVLINKRELAYTCP_HPP
