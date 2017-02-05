//===================================================================
// Includes
//===================================================================
// System Includes
#include <QDir>
#include <QPixmap>
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
    // Setup path of directory and filter for images
    QDir directory(dir);
    QStringList filters;
    filters << "*.jpg";
    directory.setNameFilters(filters);
    pathOfDir = directory.absolutePath();
    numOfImages = directory.entryList(QDir::NoDotAndDotDot).count();

    connect(sender, &DCNC::receivedImageData,
            this, &ImageTagger::handleImageMessage);

    if (toBeTagged != NULL) { }     // Tag image (not implemented yet)
    else { }                        // Don't tag image
}

ImageTagger::~ImageTagger() { }

void ImageTagger::handleImageMessage(std::shared_ptr<ImageTaggerMessage> message)
{
    // Setup local variables
    ImageTaggerMessage *imageMessage = message.get();
    unsigned char uniqueSeqNum = imageMessage->getSequenceNumber();
    std::vector<unsigned char> imageData = imageMessage->getImageData();
    QString pathName = pathOfDir + IMG + QString(++numOfImages) + JPG;

    // Convert image data to QPixmap
    int numOfBytes = 0;
    unsigned char *imageArray = &imageData[0];
    for (auto data = imageData.begin(); data != imageData.end(); ++data)
        numOfBytes++;
    QPixmap image;
    image.loadFromData(imageArray, numOfBytes, "JPG");

    // Iterate through vector of sequence numbers
    for (auto seqNum = seqNumArr.begin(); seqNum != seqNumArr.end(); ++seqNum) {
        // If duplicate is found
        if (uniqueSeqNum == *seqNum) {
            // TODO: Change path name for duplicate
            image.save(pathName, "JPG");
            emit taggedImage(pathName);
            return;
        }
    }

    // If no duplicate is found
    seqNumArr.push_back(uniqueSeqNum);
    image.save(pathName, "JPG");
    emit taggedImage(pathName);
}
