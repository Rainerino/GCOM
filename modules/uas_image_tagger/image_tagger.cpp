//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QImage>
// GCOM Includes
#include "image_tagger.hpp"

//===================================================================
// Constants
//===================================================================
const QString IMG = "\IMG_";
const QString JPG = ".JPG";

//===================================================================
// Public Class Declaration
//===================================================================
ImageTagger::ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged)
{
    directory = dir;
    QObject::connect(sender, &DCNC::receivedImageData,
                     this, &ImageTagger::handleImageMessage);

    if (toBeTagged != NULL) {
        // TODO: Tag image
    }
    else {
        // TODO: Don't tag image
    }
}

ImageTagger::~ImageTagger() { }

void ImageTagger::handleImageMessage(std::unique_ptr<ImageTaggerMessage> message)
{
    // Setup local variables
    QString pathName = directory + IMG + JPG;   // eventually will have unique #
    ImageTaggerMessage imgMessage = message.get();
    unsigned char uniqueSeqNum = imgMessage.getSequenceNumber();
    std::vector<unsigned char> imageData = imgMessage.getImageData();

    // Convert image data to QImage
    QByteArray imgArray = new QByteArray(imageData.data(), imageData.size());
    QImage image = QImage::loadFromData(imgArray);

    // Iterate through vector of sequence numbers
    for (std::vector<unsigned char>::iterator seqNum = seqNumArr.begin();
         seqNum != seqNumArr.end(); ++seqNum) {
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
    image.save(fileName, "JPG");
    emit taggedImage(pathName);
}
