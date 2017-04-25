//===================================================================
// Includes
//===================================================================
// System Includes
#include <QMovie>
#include <QString>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QRegExp>
#include <QTimer>
#include <QDebug>
// GCOM Includes
#include "gcom_controller.hpp"
#include "ui_gcomcontroller.h"
#include "modules/mavlink_relay/mavlink_relay_tcp.hpp"
#include "modules/uas_dcnc/dcnc.hpp"

//===================================================================
// Constants
//===================================================================
const QString DISCONNECT_LABEL("<font color='#D52D2D'> DISCONNECTED </font>"
                               "<img src=':/connection/disconnected.png'>");
const QString CONNECTING_LABEL("<font color='#EED202'> CONNECTING </font>"
                               "<img src=':/connection/connecting.png'>");
const QString CONNECTED_LABEL("<font color='#05c400'> CONNECTED </font>"
                               "<img src=':/connection/connected.png'>");
const QString SEARCHING_LABEL("<font color='#EED202'> SEARCHING </font>"
                               "<img src=':/connection/connecting.png'>");
const QRegExp IP_REGEX("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$");

// MAVLink Constants
const QString CONNECT_BUTTON_TEXT("Connect");
const QString CONNECTING_BUTTON_TEXT("Cancel Connecting");
const QString DISCONNECT_BUTTON_TEXT("Disconnect");

// DCNC Constants
const QString START_SEARCHING_BUTTON_TEXT("Start Searching");
const QString STOP_SEARCHING_BUTTON_TEXT("Stop Searching");
const QString STOP_SERVER_BUTTON_TEXT("Stop Server");

//===================================================================
// Class Declarations
//===================================================================
GcomController::GcomController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GcomController)
{
    // General UI Setup
    ui->setupUi(this);
    // Set up field validation
    ui->mavlinkPortField->setValidator(new QIntValidator(0,1000000));
    ui->mavlinkIPField->setValidator(new QRegExpValidator(IP_REGEX));
    ui->dcncServerPortField->setValidator(new QIntValidator(0,1000000));
    ui->dcncServerIPField->setValidator(new QRegExpValidator(IP_REGEX));
    restMavlinkGUI();

    // Mavlink Setup
    mavlinkRelay = new MAVLinkRelay();
    mavlinkConnectionTimer = new QTimer();
    connect(mavlinkConnectionTimer, SIGNAL(timeout()),
            this, SLOT(mavlinkTimerTimeout()));
    connect(mavlinkRelay, SIGNAL(mavlinkRelayConnected()),
            this, SLOT(mavlinkRelayConnected()));
    connect(mavlinkRelay, SIGNAL(mavlinkRelayDisconnected()),
            this, SLOT(mavlinkRelayDisconnected()));
    mavlinkButtonDisconnect = false;
    mavlinkConnectingMovie = new QMovie (":/connection/mavlink_connecting.gif");
    mavlinkConnectedMovie = new QMovie (":/connection/mavlink_connected.gif");

    // DCNC Setup
    dcnc = new DCNC();
    connect(dcnc, SIGNAL(receivedConnection()), this, SLOT(dcncConnected()));
    connect(dcnc, SIGNAL(droppedConnection()), this, SLOT(dcncDisconnected()));
    connect(dcnc, SIGNAL(receivedGremlinInfo(QString,uint16_t,bool)),
            this, SLOT(gremlinInfo(QString,uint16_t,bool)));
    connect (dcnc, SIGNAL(receivedGremlinCapabilities(CapabilitiesMessage::Capabilities)),
             this, SLOT(gremlinCapabilities(CapabilitiesMessage::Capabilities)));
    dcncConnectingMovie = new QMovie (":/connection/dcnc_connecting.gif");
    dcncConnectedMovie = new QMovie (":/connection/mavlink_connected.gif");
    dcncConnectionTimer = new QTimer();
    dcncSearchTimeoutTimer = new QTimer();
    connect(dcncConnectionTimer, SIGNAL(timeout()), this, SLOT(dcncTimerTimeout()));
    resetDCNCGUI();
}

GcomController::~GcomController()
{
    delete ui;
    delete mavlinkRelay;
    delete mavlinkConnectionTimer;
    delete mavlinkConnectingMovie;
    delete dcnc;
}

//===================================================================
// MAVLink Methods
//===================================================================

void GcomController::restMavlinkGUI()
{
    mavlinkConnectionTime = 0;
    ui->mavlinkStatusField->setText(DISCONNECT_LABEL);
    ui->mavlinkConnectionStatusField->setText(DISCONNECT_LABEL);
    ui->mavlinkConnectionTime->display(formatDuration(mavlinkConnectionTime));
    ui->mavlinkConnectionButton->setText(CONNECT_BUTTON_TEXT);
    // Enable all input fields
    ui->mavlinkIPField->setDisabled(false);
    ui->mavlinkPortField->setDisabled(false);
}

void GcomController::mavlinkTimerTimeout()
{
    ui->mavlinkConnectionTime->display(formatDuration(++mavlinkConnectionTime));
}

void GcomController::on_mavlinkConnectionButton_clicked()
{
    // If the MAVLink relay was disconnected the state machine progresses to
    // the connection stage
    if (mavlinkRelay->status() == MAVLinkRelay::MAVLinkRelayStatus::DISCCONNECTED)
    {
        // Disable all input fields
        ui->mavlinkIPField->setDisabled(true);
        ui->mavlinkPortField->setDisabled(true);
        // Change the GUI elements to reflect that the relay is in the
        // connecting stage
        ui->mavlinkConnectionButton->setText(CONNECTING_BUTTON_TEXT);
        ui->mavlinkStatusField->setText(CONNECTING_LABEL);
        ui->mavlinkConnectionStatusField->setText(CONNECTING_LABEL);
        mavlinkConnectedMovie->stop();
        ui->mavlinkStatusMovie->setMovie(mavlinkConnectingMovie);
        mavlinkConnectingMovie->start();
        // Start the MAVLinkRelay
        mavlinkRelay->setup(ui->mavlinkIPField->text(), ui->mavlinkPortField->text().toInt());
        mavlinkRelay->start();
    }
    // If the relay is in any other state (Connecting or Connected) then it is
    // stopped
    else
    {
        mavlinkButtonDisconnect = true;
        mavlinkRelay->stop();
    }
}

void GcomController::mavlinkRelayConnected()
{
    // Update the labels the on screen text to indicate that we have connected
    ui->mavlinkStatusField->setText(CONNECTED_LABEL);
    ui->mavlinkConnectionStatusField->setText(CONNECTED_LABEL);
    ui->mavlinkConnectionButton->setText(DISCONNECT_BUTTON_TEXT);
    // Stop the connection movie and start the connected movie
    mavlinkConnectingMovie->stop();
    ui->mavlinkStatusMovie->setMovie(mavlinkConnectedMovie);
    mavlinkConnectedMovie->start();
    // Start the timer
    mavlinkConnectionTimer->start(1000);
}

void GcomController::mavlinkRelayDisconnected()
{
    if (ui->mavlinkAutoReconnect->isChecked() && mavlinkButtonDisconnect != true)
    {
        on_mavlinkConnectionButton_clicked();
        return;
    }
    // When a disconnection is detected then the GUI is reset to the unconnected
    // state
    restMavlinkGUI();
    // Stop any movies
    ui->mavlinkStatusMovie->setText(" ");
    mavlinkConnectedMovie->stop();
    mavlinkConnectingMovie->stop();
    // Stop the timer
    mavlinkConnectionTimer->stop();
    // Reset the button method
    mavlinkButtonDisconnect = false;
}

//===================================================================
// DCNC Methods
//===================================================================

void GcomController::resetDCNCGUI()
{
    dcncConnectionTime = 0;
    ui->dcncConnectionTime->display(formatDuration(dcncConnectionTime));
    ui->dcncConnectionStatusField->setText(DISCONNECT_LABEL);
    ui->dcncStatusField->setText(DISCONNECT_LABEL);
    ui->dcncConnectionButton->setText(START_SEARCHING_BUTTON_TEXT);
    // Enable all input input fields
    ui->dcncServerIPField->setDisabled(false);
    ui->dcncServerPortField->setDisabled(false);
    // Reset the animations
    dcncConnectedMovie->stop();
    dcncConnectingMovie->stop();
    ui->dcncStatusMovie->setText(" ");
}

void GcomController::on_dcncConnectionButton_clicked()
{
    bool status;
    switch(dcnc->status())
    {
        // If we are offline start the search
        case DCNC::DCNCStatus::OFFLINE:
        {
            status = dcnc->startServer(
                        ui->dcncServerIPField->text(),
                        ui->dcncServerPortField->text().toInt());
            if (status == false)
                return;
            // Change the connection status icons, buttons, and lock the status fields
            ui->dcncServerIPField->setDisabled(true);
            ui->dcncServerIPField->setDisabled(false);
            // Update UI text to indicate searching
            ui->dcncConnectionButton->setText(STOP_SEARCHING_BUTTON_TEXT);
            ui->dcncStatusField->setText(SEARCHING_LABEL);
            // Update the status line
            dcncConnectingMovie->stop();
            ui->dcncStatusMovie->setMovie(dcncConnectingMovie);
            dcncConnectingMovie->start();
            // Start the timeout timer
            //dcncSearchTimeoutTimer->start(ui->dcncServerTimeoutField-);
        }
        break;
        // If we are searching then disconnect
        case DCNC::DCNCStatus::SEARCHING:
        case DCNC::DCNCStatus::CONNECTED:
        {
            dcnc->stopServer();
            resetDCNCGUI();
        }
        break;
    }
}

void GcomController::on_dcncDropGremlin_clicked()
{
    dcnc->cancelConnection();
}

void GcomController::dcncConnected()
{
    // When we are connected then change the button to dissconnect server
    ui->dcncConnectionButton->setText(STOP_SERVER_BUTTON_TEXT);
    ui->dcncConnectionStatusField->setText(CONNECTED_LABEL);
    ui->dcncStatusField->setText(CONNECTED_LABEL);
    // Start the timer
    dcncConnectionTimer->start(1000);
    // Update the UI
    ui->dcncStatusMovie->setMovie(dcncConnectedMovie);
    dcncConnectingMovie->start();
    // Activate the drop gremlin button
    ui->dcncDropGremlin->setDisabled(true);
}

void GcomController::dcncDisconnected()
{
    // When we are connected then change the button to dissconnect server
    ui->dcncConnectionButton->setText(STOP_SEARCHING_BUTTON_TEXT);
    ui->dcncConnectionStatusField->setText(DISCONNECT_LABEL);
    ui->dcncStatusField->setText(CONNECTING_LABEL);
    // Start the timer
    dcncConnectionTimer->stop();
    // Update the UI
    ui->dcncStatusMovie->setMovie(dcncConnectingMovie);
    dcncConnectingMovie->start();
    // Activate the drop gremlin button
    ui->dcncDropGremlin->setDisabled(false);
}

void GcomController::gremlinInfo(QString systemId, uint16_t versionNumber, bool dropped)
{
    // TODO Handle Dropped Case for auto resume and manual resume.
    (void) dropped;
    ui->dcncDeviceIDField->setText(systemId);
    QString versionString = QString(versionNumber);
    ui->dcncVersionNumberField->setText(versionString);
}

void GcomController::gremlinCapabilities(CapabilitiesMessage::Capabilities capabilities)
{
    if (static_cast<uint32_t>(capabilities & CapabilitiesMessage::Capabilities::IMAGE_RELAY))
        ui->dcncCapabilitiesField->addItem("Image Relay");
}

void GcomController::dcncTimerTimeout()
{
    ui->dcncConnectionTime->display(formatDuration(++dcncConnectionTime));
}

//===================================================================
// Utility Methods
//===================================================================
QString GcomController::formatDuration(unsigned long seconds)
{
    // Convert seconds into
    int minutes = seconds / 60;
    seconds = seconds % 60;
    int hours = minutes / 60;
    minutes = minutes % 60;

    return QString("%1:%2:%3").arg(hours).arg(minutes).arg(seconds);
}
