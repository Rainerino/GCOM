#ifndef DCNC_HPP
#define DCNC_HPP

#include <string>
#include <iostream>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <atomic>
#include <memory>

#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/uas_message_tcp_framer.hpp"

class DCNC : QObject
{
    Q_OBJECT

public:
    /*!
     * \brief DCNC constructor
     */
    explicit DCNC();

    /*!
     * \brief DCNC destructor
     */
    ~DCNC();

    /*!
     * \brief startServer runs the server on the specified port and address
     * \param port the server runs on
     * \param addr the address of the server
     */
    void startServer(int port, std::string addr);
    void startServer(int port);
   
   /*!
     * \brief cancels the current connection to the client
     * \returns the status of the cancellation
     */
    bool cancelConnection();

    //im unsure about this rn
    bool startImageTransfer();
    bool stopImageTransfer();
   
private:
    int port;
    std::atomic<bool> isConnected;
    std::atomic<bool> previouslyDropped;

    QString address;
    QHostAddress hostAddress;
    QTcpServer *server;
    QTcpSocket *clientConnection;
    QDataStream dataIn;
    std::unique_ptr<UASMessage> message;
    UASMessageTCPFramer messageFramer;

    //further commands to be added later on - will document in full later
    bool requestCapabilities();
    bool requestID();
    bool requestVerNumber();


signals:
    /*!
     * \brief receivedImageData is a signal that notifies listeners when image data has been recieved
     *          from the gremlin
     */
    void receivedImageData();
    void receivedNewConnection();
    void receivedReconnection();
    void droppedClient();

public slots:
    /*!
     * \brief handleConection is a slot that gets notified whenever a new connection is recieved.
     */
    void handleConection();

private slots:
    /*!
     * \brief handleMessage parses the message it recieves and carries out tasks accordingly
     */
    void handleData();

    /*!
     * \brief droppedConnection sets the system variables in the case we lose the client
     */
    void droppedConnection();

};

//create a slot to handle dropped connection - set is connected to false
//check ID message if client is dropped flag
//1. send a signal if a client is reconnection from dropout
//2. send client a message to acknowledge that client dropped before

//create new connection signal for UI if you get a new connection after a drop
//create connection dropped signal

//create method to force drop connection
//if i get a dirty message tell the gremlin to reset => gaurantees always get an ID message after it
//otherwise its a new connection and treat normally
//

#endif // DCNC_HPP
