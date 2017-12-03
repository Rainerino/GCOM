#ifndef TEST_DCNC_HPP
#define TEST_DCNC_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
// GCOM Includes
#include "modules/uas_dcnc/dcnc.hpp"
#include "modules/uas_message/uas_message_tcp_framer.hpp"

Q_DECLARE_METATYPE(UASMessage::MessageID)
Q_DECLARE_METATYPE(CapabilitiesMessage::Capabilities)
Q_DECLARE_METATYPE(CommandMessage::Commands)

class TestDCNC: public QObject
{
    Q_OBJECT

signals:
    void receivedRequest(UASMessage::MessageID request);
    void receivedCommand(CommandMessage::Commands command);
    void receivedGremlinCapabilities(CapabilitiesMessage::Capabilities capabilities);

public slots:
    // Socket slots
    void socketConnected();
    void socketDisconnected();

    // Receive handlers
    void handleClientData();
    void handleClientMessage(std::shared_ptr<UASMessage> message);

    // Message compares
    void compareRequestMessage(UASMessage::MessageID request);
    void compareSendUASMessage_capabilities(CapabilitiesMessage::Capabilities capabilities);
    void compareStartImageRelay(CommandMessage::Commands command);
    void compareStopImageRelay(CommandMessage::Commands command);

private slots:
    // Functions to be tested
    void testSendUASMessage();
    void testSendUASMessage_data();

    void testStartImageRelay();
    void testStopImageRelay();

    void testCancelConnection();

private Q_SLOTS:
    // Project initialization and cleanup
    void initTestCase();
    void cleanupTestCase();

private:
    // Connection Variables
    DCNC *dcnc;
    QTcpSocket *socket;
    QDataStream connectionDataStream;
    UASMessageTCPFramer messageFramer;

    CapabilitiesMessage::Capabilities sentCapabilities;

};

#endif // TEST_DCNC_HPP
