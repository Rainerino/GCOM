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
#include "modules/uas_antenna_tracker/antennatracker.hpp"

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
    void on_arduinoRefreshButton_clicked();
    void on_tabWidget_tabBarClicked(int index);
    // MAVLinkRelay Slots
    void mavlinkRelayConnected();
    void mavlinkRelayDisconnected();
    void mavlinkTimerTimeout();
    // Antenna Tracker Slots
    void on_arduinoConnectButton_clicked();
    void on_zaberRefreshButton_clicked();
    void on_zaberConnectButton_clicked();
    void on_startTrackButton_clicked();

private:
    // private member variables
    Ui::GcomController *ui;
    // MAVLinkRelay Variables
    MAVLinkRelay *mavlinkRelay;
    QTimer *mavlinkConnectionTimer;
    void restMavlinkGUI();
    unsigned long mavlinkConnectionTime;
    QMovie *mavlinkConnectingMovie;
    // Antenna Tracker
    AntennaTracker *tracker;
    // private member methods
    QString formatDuration(unsigned long seconds);
    void updateStartTrackerButton();
};

#endif // GCOMCONTROLLER_HPP
