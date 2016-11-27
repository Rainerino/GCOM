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
/*!
 * \brief The MAVLinkRelay class is designed to connect to Mission Planner, or
 *        any other TCP mavlink messagesource and relay the relevant messages to
 *        the rest of the program using relevant signals
 * \author Grant Nicol
 * \author Zeyad Tamimi
 */
class MAVLinkRelay : QObject
{
    Q_OBJECT

public:

    // Public Methods
    MAVLinkRelay();

    /*!
     * \brief setup creates the TCP socket with all the specified settings and
     *        connects all the internal slots. Multiple calls to setup will result
     *        in stop being called followed by setup
     * \param [in] ipAddress, the IP address of the TCP MAVLink stream
     * \param [in] port, the port that the TCP MAVLink stream is broadcasting on
     */
    void setup(QString ipAddress, qint16 port);

    /*!
     * \brief start, attempts to open the TCP socket and connect to the MAVLink
     *        stream
     * \return True if the connection was successful. False if the operation
     *         timed out or setup was never called
     */
    bool start(int timeout);

    /*!
     * \brief stop, tearsdown the TCP socket and disconnects the slots and
     *        signals
     * \return True if the TCP socket was torn down and False if stop was called
     *         before start
     */
    bool stop();

signals:
    void mavrelayConnected();
    void mavrelayDisconnected();
    void mavrelayGPSInfo(std::shared_ptr<mavlink_global_position_int_t> gpsSignal);
    void mavrelayCameraInfo(std::shared_ptr<mavlink_camera_feedback_t> cameraSignal);

private slots:
    void connected();
    void disconnected();
    void readBytes();


private:
    // Private Member Variables
    QTcpSocket *missionplannerSocket;
    QString ipaddress;
    qint16 port;
    mavlink_status_t lastStatus;
};
#endif // MAVLINKRELAYTCP_HPP
