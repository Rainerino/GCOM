//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QFile>
// GCOM Includes
#include "test_image_tagged_message.hpp"

const QString IP_ADDRESS = "127.0.0.1";
const int PORT = 4206;

const int TEST_START_INDEX = 0;
const int TEST_END_INDEX = 6;
const QString SEQ_ARRAY[] = {"1", "10", "34", "100", "178", "201", "255"};
const QString LAT_ARRAY[] = {"0", "1", "-1", "12345678", "700000001", "-900000000", "900000000"};
const QString LON_ARRAY[] = {"0", "1", "-1", "987654321", "1299999999" "-1800000000", "1800000000"};
const QString ALT_ABS_ARRAY[] = {"0", "1", "-1", "293843", "400001", "-500000", "500000"};
const QString ALT_REL_ARRAY[] = {"0", "1", "-1", "192723", "399999", "-500000", "500000"};
const QString HDG_ARRAY[] = {"0", "1", "10001", "23456", "30492", "32111", "36000"};
const QString IMG_PATH_ARRAY[] = {"images/connected.png", "images/kingfisher.jpg",
                                  "images/flower.jpeg", "images/marbles.bmp",
                                  "images/walle.jpg", "images/planet.jpg",
                                  "images/nebula.tif"};

const int WAIT_DURATION = 60000;

QTEST_MAIN(TestImageTaggedMessage)

void TestImageTaggedMessage::initTestCase()
{
    // Allow shared ptr to be used in signal
    qRegisterMetaType<std::shared_ptr<ImageTaggedMessage>>();

    dcnc = new DCNC();
    dcnc->startServer(IP_ADDRESS, PORT);

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(dcnc, SIGNAL(receivedImageTaggedData(std::shared_ptr<ImageTaggedMessage>)),
           this, SLOT(compareImageTaggedData(std::shared_ptr<ImageTaggedMessage>)));

    socket->connectToHost(IP_ADDRESS, PORT);
    // Make sure socket connects
    QVERIFY(socket->waitForConnected(5000));

}

void TestImageTaggedMessage::cleanupTestCase()
{
    socket->disconnectFromHost();
    dcnc->stopServer();

    disconnect(socket, SIGNAL(connected()), this, SLOT(connected()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    disconnect(dcnc, SIGNAL(receivedImageTaggedData(std::shared_ptr<ImageTaggedMessage>)),
           this, SLOT(compareImageTaggedData(std::shared_ptr<ImageTaggedMessage>)));

    delete socket;
    delete dcnc;
    delete sentImageData;
}

void TestImageTaggedMessage::connected()
{
    qInfo() << "Socket Connected";
    connectionDataStream.resetStatus();
    connectionDataStream.setDevice(socket);
}

void TestImageTaggedMessage::disconnected()
{
    qInfo() << "Socket Disconnected";
    connectionDataStream.resetStatus();
    connectionDataStream.unsetDevice();
}

void TestImageTaggedMessage::sendTaggedImage_data() {
    // Create a data table with each test as a row

    // Add headings
    QTest::addColumn<QString>("sequence");
    QTest::addColumn<QString>("latitude");
    QTest::addColumn<QString>("longitude");
    QTest::addColumn<QString>("altitudeAbs");
    QTest::addColumn<QString>("altitudeRel");
    QTest::addColumn<QString>("heading");
    QTest::addColumn<QString>("imagePath");

    // Add rows
    for (int i = TEST_START_INDEX; i <= TEST_END_INDEX; i++) {
        QTest::newRow(QString::number(i).toStdString().c_str())
                << SEQ_ARRAY[i] << LAT_ARRAY[i] << LON_ARRAY[i] << ALT_ABS_ARRAY[i]
                << ALT_REL_ARRAY[i] << HDG_ARRAY[i] << IMG_PATH_ARRAY[i];
    }
}

void TestImageTaggedMessage::sendTaggedImage()
{
    QFETCH(QString, sequence);
    QFETCH(QString, latitude);
    QFETCH(QString, longitude);
    QFETCH(QString, altitudeAbs);
    QFETCH(QString, altitudeRel);
    QFETCH(QString, heading);
    QFETCH(QString, imagePath);

    QFile image(imagePath);
    // Verify image opened correctly
    QVERIFY(image.open(QIODevice::ReadOnly));

    imageSize = image.size();

    sentImageData = new uint8_t[imageSize];

    // Extract all bytes into byte array and copy it to sentImageData
    QByteArray imageByteArray = image.readAll();
    std::copy(imageByteArray.begin(), imageByteArray.end(), sentImageData);

    seq = sequence.toInt();
    lat = latitude.toInt();
    lon = longitude.toInt();
    altAbs = altitudeAbs.toInt();
    altRel = altitudeRel.toInt();
    hdg = heading.toInt();

    ImageTaggedMessage outgoingMessage(seq, lat, lon, altAbs, altRel, hdg, sentImageData, imageSize);
    messageFramer.frameMessage(outgoingMessage);
    connectionDataStream << messageFramer;

    image.close();

    // Create a signal spy to check when signal will be emitted
    QSignalSpy taggedImageMessageSpy(dcnc,
               SIGNAL(receivedImageTaggedData(std::shared_ptr<ImageTaggedMessage>)));
    QVERIFY(taggedImageMessageSpy.isValid());

    // Create an event loop that waits until signal is emitted
    QVERIFY(taggedImageMessageSpy.wait(WAIT_DURATION));
}

void TestImageTaggedMessage::compareImageTaggedData(std::shared_ptr<ImageTaggedMessage> image)
{
    // Compare all values before sending and after sending
    QCOMPARE(image->sequenceNumber, seq);
    QCOMPARE(image->latitudeRaw, lat);
    QCOMPARE(image->longitudeRaw, lon);
    QCOMPARE(image->altitudeAbsRaw, altAbs);
    QCOMPARE(image->altitudeRelRaw, altRel);
    QCOMPARE(image->headingRaw, hdg);

    // Compare the images byte by byte
    int numBytes;
    for (numBytes = 0; numBytes < image->imageData.size(); numBytes++) {
        QCOMPARE(image->imageData[numBytes], *(sentImageData + numBytes));
    }
    qInfo() << "Number of bytes checked: " << numBytes;
}
