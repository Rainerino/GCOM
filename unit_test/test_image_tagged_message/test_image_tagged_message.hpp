#ifndef TEST_IMAGE_TAGGED_MESSAGE_HPP
#define TEST_IMAGE_TAGGED_MESSAGE_HPP

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

Q_DECLARE_METATYPE(std::shared_ptr<ImageTaggedMessage>)

class TestImageTaggedMessage : public QObject
{
    Q_OBJECT

public slots:
    // Socket slots
    void socketConnected();
    void socketDisconnected();

    void compareImageTaggedData(std::shared_ptr<ImageTaggedMessage> message);

private slots:
    // Function to be tested
    void testSendImageTagged();
    void testSendImageTagged_data();

private Q_SLOTS:
    // Functions called before and after all tests have been run
    void initTestCase();
    void cleanupTestCase();

private:
    // Connection Variables
    DCNC *dcnc;
    QTcpSocket *socket;
    QDataStream connectionDataStream;
    UASMessageTCPFramer messageFramer;

    // Test variables
    uint8_t *sentImageData;
    size_t imageSize;
    uint8_t seq;
    int32_t lat;
    int32_t lon;
    int32_t altAbs;
    int32_t altRel;
    uint16_t hdg;
};
#endif // TEST_IMAGE_TAGGED_MESSAGE_HPP
