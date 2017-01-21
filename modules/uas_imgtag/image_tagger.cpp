//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
#include <algorithm>
#include <iterator>
// GCOM Includes
#include "image_tagger.hpp"
#include "modules/uas_message/uas_message.hpp"

//===================================================================
// Class Definitions
//===================================================================
ImageTagger::ImageTagger(unsigned char sequenceNumber, unsigned char imageData[])
{
    this->sequenceNumber = sequenceNumber;

    int len = sizeof(imageData) / sizeof(unsigned char);
    std::copy(imageData, imageData + len, this->imageData.begin());
}

ImageTagger::ImageTagger(const std::vector<unsigned char> &serializedMessage)
{
    this->sequenceNumber = static_cast<unsigned char>(serializedMessage.front());
    std::copy(serializedMessage.begin() + 1, serializedMessage.end(),
              back_inserter(imageData));
}

ImageTagger::~ImageTagger(){}

UASMessage::MessageID ImageTagger::type()
{
    return MessageID::IMAGE_DATA;
}

std::vector<unsigned char> ImageTagger::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(sequenceNumber);
    serializedMessage.insert(std::end(serializedMessage),
                             std::begin(imageData), std::end(imageData));
    return serializedMessage;
}
