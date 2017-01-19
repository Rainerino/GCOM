//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "image_tagger.hpp"
#include "modules/uas_message/uas_message.hpp"

//===================================================================
// Constants
//===================================================================
const int SEQ_NUM_INDEX = 0;
const int IMAGE_DATA_INDEX = 1;

//===================================================================
// Class Definitions
//===================================================================
ImageTagger::ImageTagger(unsigned char seqNum, unsigned char imageData[])
{
    this.seqNum = seqNum;
    this.imageData = imageData;
}

ImageTagger::ImageTagger(const std::vector<unsigned char> &serializedMessage)
{
    seqNum = serializedMessage[SEQ_NUM_INDEX];
    // TODO: Set imageData to the rest of the data in serializedMessage
}

ImageTagger::~ImageTagger()
{

}

UASMessage::MessageID ImageTagger::type()
{
    return MessageID::IMAGE_DATA;
}

std::vector<unsigned char> ImageTagger::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(seqNum);
    serializedMessage.insert(std::end(serializedMessage),
                             std::begin(imageData), std::end(imageData)); // ending included?
    return serializedMessage;
}
