//===================================================================
// Includes
//===================================================================
// System Includes
#include <QDir>
#include <QFile>
#include <QDirIterator>
// GCOM Includes
#include "image_tagger.hpp"
// #include "exiv2/exiv2.hpp"

//===================================================================
// Constants
//===================================================================
const QString DUPLFOLDER = "/Duplicates";
const QString EXIF = "/GPSExifTags.txt";
const QString DUP = "/DUP_";
const QString IMG = "/IMG_";
const QString JPG = ".jpg";

//===================================================================
// Public Class Declaration
//===================================================================
ImageTagger::ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged)
{
    setupDirectoryPath(dir);
    connect(sender, &DCNC::receivedImageData,
            this, &ImageTagger::handleImageMessage);
    connect(toBeTagged, &MAVLinkRelay::mavlinkRelayCameraInfo,
            this, &ImageTagger::handleMavlinkRelay);

    gpsDataAvailable = toBeTagged != NULL ? 1 : 0;
}

ImageTagger::~ImageTagger() { }

void ImageTagger::setupDirectoryPath(QString dir)
{
    QDir homeDirectory(QDir::homePath());
    if (!homeDirectory.mkdir(dir))
        qDebug() << "Can't create folder";
    pathOfDir = QDir::homePath() + "/" + dir;
}

void ImageTagger::saveImageToDisc(QString filePath, unsigned char *data, size_t size)
{
    QFile image(filePath);
    if (image.open(QIODevice::ReadWrite))
        image.write(reinterpret_cast<const char *>(data), size);
    image.close();
}

void ImageTagger::tagImage(QString filePath, mavlink_camera_feedback_t *tags)
{
    /*
    std::string pathOfImage = filePath.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pathOfImage);
    assert(image.get() != 0);   // check if image was opened

    image->readMetadata();
    image->exifData()["Exif.GPSInfo.GPSLatitudeRef"] = Exiv2::AsciiValue("N");  // set ref. to NORTH
    image->exifData()["Exif.GPSInfo.GPSLatitude"] = Exiv2::Rational((int)tags->lat, 10000000);
    image->exifData()["Exif.GPSInfo.GPSLongitudeRef"] = Exiv2::AsciiValue("W"); // set ref. to WEST
    image->exifData()["Exif.GPSInfo.GPSLongitude"] = Exiv2::Rational((int)tags->lng, 10000000);
    image->exifData()["Exif.GPSInfo.GPSAltitudeRef"] = Exiv2::byte(0);  // set alt. ref. to AMSL
    image->exifData()["Exif.GPSInfo.GPSAltitude"] = Exiv2::Rational((int)(tags->alt_msl * 1000), 1000);
    image->writeMetadata();
    */
    QFile::rename(filePath, QFileInfo(filePath).absolutePath() + "/" +
                  QFileInfo(filePath).baseName().left(4) + QString::number(tags->img_idx) + JPG);
}

void ImageTagger::tagImage(QString filePath, QStringList tags)
{
    /*
    std::string pathOfImage = filePath.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pathOfImage);
    assert(image.get() != 0);   // check if image was opened

    int lat = tags.at(1).toInt();
    int lng = tags.at(2).toInt();
    float alt_rel = tags.at(4).toFloat();

    image->readMetadata();
    image->exifData()["Exif.GPSInfo.GPSLatitudeRef"] = Exiv2::AsciiValue("N");  // set ref. to NORTH
    image->exifData()["Exif.GPSInfo.GPSLatitude"] = Exiv2::Rational(lat, 10000000);
    image->exifData()["Exif.GPSInfo.GPSLongitudeRef"] = Exiv2::AsciiValue("W"); // set ref. to WEST
    image->exifData()["Exif.GPSInfo.GPSLongitude"] = Exiv2::Rational(lng, 10000000);
    image->exifData()["Exif.GPSInfo.GPSAltitudeRef"] = Exiv2::byte(0);          // set alt. ref. to AMSL
    image->exifData()["Exif.GPSInfo.GPSAltitude"] = Exiv2::Rational((int)(alt_rel * 1000), 1000);
    image->writeMetadata();
    */
}

void ImageTagger::tagAllImages()
{
    QStringList exifTags;
    QString line, imageIndex;
    QFileInfo entry;
    int counter = 0;

    // Iterate through each line of EXIF tags
    QString filename = pathOfDir + EXIF;
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        while(!in.atEnd()) {
            line = in.readLine();
            exifTags = line.split(' ');

            // Iterate through each image file
            QDir imageDirectory(pathOfDir);
            QFileInfoList imageEntries = imageDirectory.entryInfoList(QStringList() << "*.jpg",
                                                                      QDir::Files, QDir::Name);
            entry = imageEntries.at(counter++);
            tagImage(entry.filePath(), exifTags);
            imageIndex = entry.baseName().mid(4);
            if (imageIndex != exifTags.at(0))
                qDebug() << "WARNING: Mismatch for " << entry.baseName();
        }
    }
    inputFile.close();
}

QString ImageTagger::getPathOfExifTags()
{
    return pathOfDir + EXIF;
}

void ImageTagger::handleMavlinkRelay(std::shared_ptr<mavlink_camera_feedback_t> cameraInfo)
{
    // Write EXIF tags to text file
    // IMAGE_INDEX LATITUDE LONGITUDE ALT_SEA_LVL REL_ALT
    qDebug() << "Photo has been taken";
    QString filename = pathOfDir + EXIF;
    QFile outputFile(filename);
    if (outputFile.open(QIODevice::ReadWrite | QIODevice::Append)) {
        QTextStream stream(&outputFile);
        stream << QString::number(cameraInfo->img_idx) << " "
               << QString::number(cameraInfo->lat) << " "
               << QString::number(cameraInfo->lng) << " "
               << QString::number(cameraInfo->alt_msl) << " "
               << QString::number(cameraInfo->alt_rel) << endl;
    }
}

void ImageTagger::handleImageMessage(std::shared_ptr<ImageTaggerMessage> message)
{
    QString filePath;
    ImageTaggerMessage *imageMessage = message.get();
    unsigned char uniqueSeqNum = imageMessage->getSequenceNumber();
    std::vector<unsigned char> imageData = imageMessage->getImageData();

    // A pointer to the image data
    unsigned char *imageArray = &imageData[0];
    size_t sizeOfData = imageData.size();

    // Iterate through vector of sequence numbers
    for (auto seqNum = seqNumArr.begin(); seqNum != seqNumArr.end(); ++seqNum) {
        // If duplicate is found
        if (uniqueSeqNum == *seqNum) {
            // Change path name for duplicate
            filePath = pathOfDuplicates + DUP + QString::number(++numOfDuplicates) + JPG;
            saveImageToDisc(filePath, imageArray, sizeOfData);
            if (gpsDataAvailable) {
                while(gpsData.isEmpty())
                    qDebug() << "No GPS data in the queue";
                tagImage(filePath, gpsData.dequeue());
            }
            emit taggedImage(filePath);
            return;
        }
    }

    // If no duplicate is found
    seqNumArr.push_back(uniqueSeqNum);
    filePath = pathOfDir + IMG + QString::number(++numOfImages) + JPG;
    saveImageToDisc(filePath, imageArray, sizeOfData);
    if (gpsDataAvailable) {
        while(gpsData.isEmpty())
            qDebug() << "No GPS data in the queue";
        tagImage(filePath, gpsData.dequeue());
    }
    emit taggedImage(filePath);
}
