#ifndef DCNC_HPP
#define DCNC_HPP


#include <string>
#include <QObject>
#include <QtNetwork>
#include <atomic>
#include <memory>

#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/uas_message_tcp_framer.hpp"

class DCNC : public QTcpServer
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

    /*!
     * \brief cancels the current connection to the client
     * \returns the status of the cancellation
     */
    bool stopConnection();

    /*!
     * \brief stops the server
     * \returns the status of the cancellation
     */
    bool stopServer();

    //im unsure about this rn
    bool startImageTransfer();
    bool stopImageTransfer();

private:
    std::string address;
    int port;
    QTcpServer server;
    QTcpSocket *client_connection;
    QDataStream data_in;
    std::atomic<bool> is_connected;
    unique_ptr<UASMessage> message;

    //further commands to be added later on - will document in full later
    bool requestCapabilities();
    bool requestID();
    bool requestVerNumber();

signals:
    /*!
     * \brief receivedCompleteMessage is a signal that notifies connected slots and carries the
     *          message object to be handled
     * \param new_message is the message object carried by the signal
     */
    void receivedCompleteMessage(UASMessage* new_message);

    /*!
     * \brief receivedImageData is a signal that notifies listeners when image data has been recieved
     *          from the gremlin
     */
    void receivedImageData();

public slots:
    /*!
     * \brief handleMessage parses the message it recieves and carries out tasks accordingly
     * \param message to be parsed
     */
    handleMessage(UASMessage* message);

    /*!
     * \brief handleConection is a slot that gets notified whenever a new connection is recieved.
     */
    handleConection();

};

#endif // DCNC_HPP
