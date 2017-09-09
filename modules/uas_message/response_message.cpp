//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "uas_message.hpp"
#include "command_message.hpp"
#include "response_message.hpp"

//===================================================================
// Constants
//===================================================================
const int COMMAND_FILED_INDEX = 0;
const int RESPONSE_FIELD_INDEX = 1;

//===================================================================
// Class Definitions
//===================================================================
ResponseMessage::ResponseMessage(CommandMessage::Commands command, ResponseCodes responseCode)
{
    responseCommand = command;
    response = responseCode;
}

ResponseMessage::ResponseMessage(const std::vector<unsigned char> &serializedMessage)
{
    responseCommand = static_cast<CommandMessage::Commands>(serializedMessage[COMMAND_FILED_INDEX]);
    response = static_cast<ResponseCodes>(serializedMessage[RESPONSE_FIELD_INDEX]);
}

ResponseMessage::~ResponseMessage()
{

}

UASMessage::MessageID ResponseMessage::type()
{
    return MessageID::RESPONSE;
}

CommandMessage::Commands ResponseMessage::command()
{
    return responseCommand;
}

ResponseMessage::ResponseCodes ResponseMessage::responseCode()
{
    return response;
}

std::vector<unsigned char> ResponseMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(static_cast<unsigned char>(responseCommand));
    serializedMessage.push_back(static_cast<unsigned char>(response));
    return serializedMessage;
}
