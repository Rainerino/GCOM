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

    if (toBeTagged != NULL)
        gpsDataAvailable = 1;   // tag image
    else
        gpsDataAvailable = 0;   // do not tag image
}

ImageTagger::~ImageTagger() { }

void ImageTagger::setupDirectoryPath(QString dir, int createDuplicates)
{
    QDir directory(dir);
    if (!directory.exists())
        directory.mkpath(".");

    if (createDuplicates)   // if duplicates folder is to be created
        pathOfDuplicates = directory.absolutePath();
    else                    // else for parent directory
        pathOfDir = directory.absolutePath();
}

void ImageTagger::saveImageToDisc(QString pathName, unsigned char *data)
{
    QFile image(pathName);
    if (image.open(QIODevice::ReadWrite))
        image.write(reinterpret_cast<const char *>(data));
    image.close();
}

void ImageTagger::tagImage(QString pathName)
{
    std::string pathOfImage = pathName.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pathOfImage);
    assert(image.get() != 0);   // check if image was opened

    image->readMetadata();
    image->exifData()["Exif.GPSInfo.GPSLatitude"] = Exiv2::Rational(gpsData->lat, 10000000);
    image->exifData()["Exif.GPSInfo.GPSLongitude"] = Exiv2::Rational(gpsData->lng, 10000000);
    image->exifData()["Exif.GPSInfo.GPSAltitudeRef"] = Exiv2::byte(0);  // set alt. ref. to AMSL
    image->exifData()["Exif.GPSInfo.GPSAltitude"] = Exiv2::Rational(gpsData->alt_msl, 1);
    image->writeMetadata();
}

void ImageTagger::tagImage(QString pathName, QStringList tags)
{
    std::string pathOfImage = pathName.toStdString();
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(pathOfImage);
    assert(image.get() != 0);   // check if image was opened

    image->readMetadata();
    image->exifData()["Exif.GPSInfo.GPSLatitude"] = Exiv2::Rational(tags.at(1).toInt(), 10000000);
    image->exifData()["Exif.GPSInfo.GPSLongitude"] = Exiv2::Rational(tags.at(2).toInt(), 10000000);
    image->exifData()["Exif.GPSInfo.GPSAltitudeRef"] = Exiv2::byte(0);  // set alt. ref. to AMSL
    image->exifData()["Exif.GPSInfo.GPSAltitude"] = Exiv2::Rational(tags.at(3).toFloat(), 1);
    image->writeMetadata();
}

void ImageTagger::tagAllImages() {
    QFileInfo image;
    QStringList exifTags;
    QString line, imageName, imageIndex;
    // Iterator to go through main directory, as well as duplicates folder
    QDirIterator it(pathOfDir, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);

    // Iterate through each line of EXIF tags
    QString filename = pathOfDir + "\\GPSExifTags.txt";
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        while(!in.atEnd()) {
            line = in.readLine();
            exifTags = line.split('\t');

            // Iterate through each image file (begins before first entry)
            while (it.hasNext()) {
                qDebug() << it.next();
                image = it.fileInfo();
                imageName = image.baseName();   // name is either IMG_* | DUP_*
                imageIndex = imageName.mid(4);  // get everything after IMG_ | DUP_

                if (imageIndex == exifTags.at(0))
                    tagImage(it.filePath(), exifTags);
            }
        }
    }
    inputFile.close();
}

void ImageTagger::handleMavlinkRelay(std::shared_ptr<mavlink_camera_feedback_t> cameraInfo)
{
    mavlink_camera_feedback_t *copyOfCameraInfo = cameraInfo.get();
    gpsData = copyOfCameraInfo;

    // Write EXIF tags to text file
    // IMAGE_INDEX  LATITUDE  LONGITUDE  ALT_SEA_LVL  REL_ALT
    QString filename = pathOfDir + "\\GPSExifTags.txt";
    QFile outputFile(filename);
    if (outputFile.open(QIODevice::ReadWrite)) {
        QTextStream stream(&outputFile);
        stream << QString::number(gpsData->img_idx) << "\t"
               << QString::number(gpsData->lat) << "\t"
               << QString::number(gpsData->lng) << "\t"
               << QString::number(gpsData->alt_msl) << "\t"
               << QString::number(gpsData->alt_rel) << endl;
    }
}

void ImageTagger::handleImageMessage(std::shared_ptr<ImageTaggerMessage> message)
{
    QString pathName;
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
            pathName = pathOfDuplicates + DUP + QString::number(gpsData->img_idx) + JPG;
            saveImageToDisc(pathName, imageArray);
            if (gpsDataAvailable)
                tagImage(pathName);     // tag image if MavLinkRelay was not NULL
            emit taggedImage(pathName);
            return;
        }
    }

    // If no duplicate is found
    seqNumArr.push_back(uniqueSeqNum);
    pathName = pathOfDir + IMG + QString::number(gpsData->img_idx) + JPG;
    saveImageToDisc(pathName, imageArray);
    if (gpsDataAvailable)
        tagImage(pathName);             // tag image if MavLinkRelay was not NULL
    emit taggedImage(pathName);
}
