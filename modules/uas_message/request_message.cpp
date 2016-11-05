//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "request_message.hpp"
#include "uas_message.hpp"


RequestMessage::RequestMessage(MessageId requestedMessage)
{
    messageType = MessageId::REQUEST;
    this->requestedMessage = requestedMessage;
}

RequestMessage::RequestMessage(const std::vector<unsigned char>& serializedMessage)
{
    messageType = MessageId::REQUEST;
    this->requestedMessage = static_cast<MessageId>(serializedMessage.front());
}

RequestMessage::~RequestMessage()
{

}

UASMessage::MessageId RequestMessage::type()
{
    return messageType;
}

std::vector<unsigned char> RequestMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(requestedMessage);
    return serializedMessage;
}
