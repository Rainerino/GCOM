#ifndef DCNC_HPP
#define DCNC_HPP


#include <string>
#include <QObject>
#include <QtNetwork>
#include <atomic>

#include "modules/uas_message/uas_message.hpp"

class DCNC : public QTcpServer
{
    Q_OBJECT

public:
    explicit DCNC();
    ~DCNC();

    //make startServer connect slots with signals
    int startServer(int port, std::string addr);
    bool stopConnection();
    bool stopServer();

    bool startImageTransfer();
    bool stopImageTransfer();

private:
    std::string address;
    int port;
    QTcpServer *server;
    QTcpSocket client_connection;
    //start, stop abort transaction -> read on
    QDataStream data_in;
    std::atomic<bool> isConnected;

    bool requestCapabilities();
    bool requestID();
    bool requestVerNumber();

signals:
    void receivedImageData();

public slots:
    /*peek buffer for complete message if not exit; do not modify buffer
     *loop through to make sure all complete messages are handled
     */
    handleMessage();
    handleConection(QTCPSocket socket);

};

#endif // DCNC_HPP
