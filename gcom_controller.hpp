#ifndef GCOMCONTROLLER_HPP
#define GCOMCONTROLLER_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <QMainWindow>
#include <QString>
#include <QMovie>
#include <QTimer>
// GCOM Includes
#include "modules/mavlink_relay/mavlink_relay_tcp.hpp"
#include "modules/uas_dcnc/dcnc.hpp"
#include "modules/uas_message/image_untagged_message.hpp"
#include "modules/uas_antenna_tracker/antennatracker.hpp"
#include "modules/uas_interop_system/interop.hpp"

//===================================================================
// Namespace Declarations
//===================================================================
namespace Ui
{
    class GcomController;
}

//===================================================================
// Class Declarations
//===================================================================
class GcomController : public QMainWindow
{
    Q_OBJECT

public:
    explicit GcomController(QWidget *parent = 0);
    ~GcomController();

private slots:
    // UI Slots
    void on_mavlinkConnectionButton_clicked();
    void on_dcncConnectionButton_clicked();
    void on_arduinoRefreshButton_clicked();
    void on_tabWidget_tabBarClicked(int index);
    void on_dcncDropGremlin_clicked();
    // MAVLinkRelay Slots
    void mavlinkRelayConnected();
    void mavlinkRelayDisconnected();
    void mavlinkTimerTimeout();
    // DCNC Slots
    void dcncConnected();
    void dcncDisconnected();
    void dcncTimerTimeout();
    void dcncSearchTimeout();
    void gremlinInfo(QString systemId, uint16_t versionNumber, bool dropped);
    void gremlinCapabilities(CapabilitiesMessage::Capabilities capabilities);
    // Antenna Tracker Slots
    void on_arduinoConnectButton_clicked();
    void on_zaberRefreshButton_clicked();
    void on_zaberConnectButton_clicked();
    void on_startTrackButton_clicked();
    void on_antennaTrackerGPSOverrideCheckBox_toggled(bool checked);
    void on_antennaTrackerCalibrateIMUButton_clicked();
    void on_antennaTrackerOverrideHeadingCheckBox_toggled(bool checked);
    void on_antennaTrackerOverrideElevationCheckBox_toggled(bool checked);
    void on_antennaTrackerOverrideElevationField_editingFinished();
    void on_antennaTrackerOverrideHeadingField_editingFinished();
    void antennaTrackerUpdateStatusGUI(float latitude, float longitude, float elevation, float heading);
    void disableAntennaTrackingGUI(bool toggle);
    // AUVSI Interop Slots
    void on_interopConnectButton_clicked();

private:
    // Private Member Variables
    Ui::GcomController *ui;
    // Private Member Methods
    QString formatDuration(unsigned long seconds);

    // MAVLinkRelay Variables
    MAVLinkRelay *mavlinkRelay;
    QTimer *mavlinkConnectionTimer;
    unsigned long mavlinkConnectionTime;
    QMovie *mavlinkConnectingMovie;
    QMovie *mavlinkConnectedMovie;
    bool mavlinkButtonDisconnect;
    // Methods
    void restMavlinkGUI();

    // DCNC Variables
    DCNC *dcnc;
    QTimer *dcncConnectionTimer;
    QTimer *dcncSearchTimeoutTimer;
    unsigned long dcncConnectionTime;
    QMovie *dcncConnectingMovie;
    QMovie *dcncConnectedMovie;
    // Methods
    void resetDCNCGUI();

    // Antenna Tracker Variables
    AntennaTracker *tracker;
    // Methods
    void updateStartTrackerButton();

    Interop *interop;
};

#endif // GCOMCONTROLLER_HPP
