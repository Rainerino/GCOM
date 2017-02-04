//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QPixmap>
// GCOM Includes
#include "image_tagger.hpp"

//===================================================================
// Constants
//===================================================================
const QString IMG = "\\IMG_";
const QString JPG = ".JPG";

//===================================================================
// Public Class Declaration
//===================================================================
ImageTagger::ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged)
{
    directory = dir;
    connect(sender, &DCNC::receivedImageData,
            this, &ImageTagger::handleImageMessage);

    if (toBeTagged != NULL) { }     // Tag image (not implemented yet)
    else { }                        // Don't tag image
}

ImageTagger::~ImageTagger() { }

void ImageTagger::handleImageMessage(std::shared_ptr<ImageTaggerMessage> message)
{
    // Setup local variables
    QString pathName = directory + IMG + JPG;   // eventually will have unique #
    ImageTaggerMessage *imageMessage = message.get();
    unsigned char uniqueSeqNum = imageMessage->getSequenceNumber();
    std::vector<unsigned char> imageData = imageMessage->getImageData();

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
