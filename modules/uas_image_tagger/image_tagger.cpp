//===================================================================
// Includes
//===================================================================
// System Includes
#include <QDir>
#include <QFile>
#include <QDirIterator>
// GCOM Includes
#include "image_tagger.hpp"
#include "exiv2/exiv2.hpp"

//===================================================================
// Constants
//===================================================================
const QString DUPLFOLDER = "\\Duplicates";
const QString DUP = "\\DUP_";
const QString IMG = "\\IMG_";
const QString JPG = ".jpg";

//===================================================================
// Public Class Declaration
//===================================================================
ImageTagger::ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged)
{
    setupDirectoryPath(dir, 0);
    setupDirectoryPath(dir + DUPLFOLDER, 1);

    connect(sender, &DCNC::receivedImageData,
            this, &ImageTagger::handleImageMessage);
    connect(toBeTagged, &MAVLinkRelay::mavlinkRelayCameraInfo,
            this, &ImageTagger::handleMavlinkRelay);

    gpsDataAvailable = toBeTagged != NULL ? 1 : 0;
}

ImageTagger::~ImageTagger() { }

void ImageTagger::setupDirectoryPath(QString dir, int createDuplicates)
{
    QDir directory(dir);
    if (!directory.exists())
        directory.mkpath(".");
    directory.setNameFilters(QStringList() << "*.jpg");

    if (createDuplicates) {     // if duplicates folder is to be created
        pathOfDuplicates = directory.absolutePath();
        numOfDuplicates = directory.count();
    }
    else {                      // else for parent directory
        pathOfDir = directory.absolutePath();
        numOfImages = directory.count();
    }
}

void ImageTagger::saveImageToDisc(QString filePath, unsigned char *data)
{
    QFile image(filePath);
    if (image.open(QIODevice::ReadWrite))
        image.write(reinterpret_cast<const char *>(data));
    image.close();
}

void ImageTagger::tagImage(QString filePath, mavlink_camera_feedback_t *tags)
{
    std::string pathOfImage = filePath.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pathOfImage);
    assert(image.get() != 0);   // check if image was opened

    image->readMetadata();
    image->exifData()["Exif.GPSInfo.GPSLatitude"] = Exiv2::Rational(tags->lat, 10000000);
    image->exifData()["Exif.GPSInfo.GPSLongitude"] = Exiv2::Rational(tags->lng, 10000000);
    image->exifData()["Exif.GPSInfo.GPSAltitudeRef"] = Exiv2::byte(0);  // set alt. ref. to AMSL
    image->exifData()["Exif.GPSInfo.GPSAltitude"] = Exiv2::Rational(tags->alt_msl, 1);
    image->writeMetadata();

    QFile::rename(filePath, QFileInfo(filePath).absolutePath() + "/" +
                  QFileInfo(filePath).baseName().left(4) + QString::number(tags->img_idx) + JPG);
}

void ImageTagger::tagImage(QString filePath, QStringList tags)
{
    std::string pathOfImage = filePath.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pathOfImage);
    assert(image.get() != 0);   // check if image was opened

    image->readMetadata();
    image->exifData()["Exif.GPSInfo.GPSLatitude"] = Exiv2::Rational(tags.at(1).toFloat(), 10000000);
    image->exifData()["Exif.GPSInfo.GPSLongitude"] = Exiv2::Rational(tags.at(2).toFloat(), 10000000);
    image->exifData()["Exif.GPSInfo.GPSAltitudeRef"] = Exiv2::byte(0);  // set alt. ref. to AMSL
    image->exifData()["Exif.GPSInfo.GPSAltitude"] = Exiv2::Rational(tags.at(3).toFloat(), 1);
    image->writeMetadata();
}

void ImageTagger::tagAllImages()
{
    QStringList exifTags;
    QString line, imageIndex;

    // Iterate through each line of EXIF tags
    QString filename = pathOfDir + "\\GPSExifTags.txt";
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        while(!in.atEnd()) {
            line = in.readLine();
            exifTags = line.split(' ');

            // Iterate through each image file (begins before first entry)
            QDirIterator it(pathOfDir, QStringList() << "*.jpg", QDir::Files,
                            QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                imageIndex = it.fileInfo().baseName().mid(4);

                if (imageIndex == exifTags.at(0)) {
                    tagImage(it.filePath(), exifTags);
                    qDebug() << "Tagged: " << it.fileInfo().baseName();
                }
            }
        }
    }
    inputFile.close();
}

void ImageTagger::handleMavlinkRelay(std::shared_ptr<mavlink_camera_feedback_t> cameraInfo)
{
    mavlink_camera_feedback_t *tags = cameraInfo.get();
    gpsData.enqueue(tags);

    // Write EXIF tags to text file
    // IMAGE_INDEX LATITUDE LONGITUDE ALT_SEA_LVL REL_ALT
    QString filename = pathOfDir + "\\GPSExifTags.txt";
    QFile outputFile(filename);
    if (outputFile.open(QIODevice::ReadWrite | QIODevice::Append)) {
        QTextStream stream(&outputFile);
        stream << QString::number(tags->img_idx) << " "
               << QString::number(tags->lat) << " "
               << QString::number(tags->lng) << " "
               << QString::number(tags->alt_msl) << " "
               << QString::number(tags->alt_rel) << endl;
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

    // Iterate through vector of sequence numbers
    for (auto seqNum = seqNumArr.begin(); seqNum != seqNumArr.end(); ++seqNum) {
        // If duplicate is found
        if (uniqueSeqNum == *seqNum) {
            // Change path name for duplicate
            filePath = pathOfDuplicates + DUP + QString::number(++numOfDuplicates) + JPG;
            saveImageToDisc(filePath, imageArray);
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
    saveImageToDisc(filePath, imageArray);
    if (gpsDataAvailable) {
        while(gpsData.isEmpty())
            qDebug() << "No GPS data in the queue";
        tagImage(filePath, gpsData.dequeue());
    }
    emit taggedImage(filePath);
}
