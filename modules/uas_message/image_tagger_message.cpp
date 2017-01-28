//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
#include <algorithm>
#include <iterator>
// GCOM Includes
#include "image_tagger_message.hpp"
#include "modules/uas_message/uas_message.hpp"

//===================================================================
// Class Definitions
//===================================================================
ImageTaggerMessage::ImageTaggerMessage(unsigned char sequenceNumber,
                                       unsigned char imageData[], size_t dataSize)
{
    this->sequenceNumber = sequenceNumber;
    this->imageData.assign(imageData, imageData + dataSize / sizeof(char));
}

ImageTaggerMessage::ImageTaggerMessage(const std::vector<unsigned char> &serializedMessage)
{
    sequenceNumber = static_cast<unsigned char>(serializedMessage.front());
    imageData.assign(serializedMessage.begin() + 1, serializedMessage.end());
}

ImageTaggerMessage::~ImageTaggerMessage() { }

UASMessage::MessageID ImageTaggerMessage::type()
{
    return MessageID::IMAGE_DATA;
}

std::vector<unsigned char> ImageTaggerMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(sequenceNumber);
    serializedMessage.insert(std::end(serializedMessage),
                             std::begin(imageData), std::end(imageData));
    return serializedMessage;
}

unsigned char ImageTaggerMessage::getSequenceNumber()
{
    return sequenceNumber;
}

std::vector<unsigned char> ImageTaggerMessage::getImageData()
{
    return imageData;
}
