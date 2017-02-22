//===================================================================
// Includes
//===================================================================
// System Includes
#include <QDir>
#include <QFile>
// GCOM Includes
#include "image_tagger.hpp"

//===================================================================
// Constants
//===================================================================
const QString IMG = "\\IMG_";
const QString JPG = ".jpg";

//===================================================================
// Public Class Declaration
//===================================================================
ImageTagger::ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged)
{
    // Setup
    setupDirectoryPath(dir);
    connect(sender, &DCNC::receivedImageData,
            this, &ImageTagger::handleImageMessage);

    if (toBeTagged != NULL) { }     // Tag image (not implemented yet)
    else { }                        // Don't tag image
}

ImageTagger::~ImageTagger() { }

void ImageTagger::setupDirectoryPath(QString dir)
{
    QDir directory(dir);
    QStringList filters;
    filters << "*.jpg";
    directory.setNameFilters(filters);
    pathOfDir = directory.absolutePath();
    numOfImages = directory.entryList(QDir::NoDotAndDotDot).count();
}

void ImageTagger::saveImageToDisc(QString pathName, unsigned char *data)
{
    QFile image(pathName);
    if (image.open(QIODevice::ReadWrite))
        image.write(reinterpret_cast<const char *>(data));
    image.close();
}

void ImageTagger::handleImageMessage(std::shared_ptr<ImageTaggerMessage> message)
{
    // Setup local variables
    ImageTaggerMessage *imageMessage = message.get();
    unsigned char uniqueSeqNum = imageMessage->getSequenceNumber();
    std::vector<unsigned char> imageData = imageMessage->getImageData();
    QString pathName;

    // Convert image data from vector to array
    unsigned char *imageArray = &imageData[0];

    // Iterate through vector of sequence numbers
    for (auto seqNum = seqNumArr.begin(); seqNum != seqNumArr.end(); ++seqNum) {
        // If duplicate is found
        if (uniqueSeqNum == *seqNum) {
            // TODO: Change path name for duplicate
            pathName = pathOfDir + IMG + QString::number(++numOfImages) + JPG;
            saveImageToDisc(pathName, imageArray);
            emit taggedImage(pathName);
            return;
        }
    }

    // If no duplicate is found
    seqNumArr.push_back(uniqueSeqNum);
    pathName = pathOfDir + IMG + QString::number(++numOfImages) + JPG;
    saveImageToDisc(pathName, imageArray);
    emit taggedImage(pathName);
}
