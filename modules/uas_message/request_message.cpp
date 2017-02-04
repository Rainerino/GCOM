//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "request_message.hpp"
#include "uas_message.hpp"

//===================================================================
// Class Definitions
//===================================================================
RequestMessage::RequestMessage(MessageID requestedMessage)
{
    this->requestedMessage = requestedMessage;
}

RequestMessage::RequestMessage(const std::vector<unsigned char>& serializedMessage)
{
    this->requestedMessage = static_cast<MessageID>(serializedMessage.front());
}

RequestMessage::~RequestMessage()
{

}

UASMessage::MessageID RequestMessage::type()
{
    return MessageID::REQUEST;
}

std::vector<unsigned char> RequestMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(static_cast<unsigned char>(requestedMessage));
    return serializedMessage;
}
