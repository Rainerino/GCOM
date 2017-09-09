//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "command_message.hpp"
#include "uas_message.hpp"

//===================================================================
// Class Definitions
//===================================================================
CommandMessage::CommandMessage(Commands command)
{
    this->command = command;
}

CommandMessage::CommandMessage(const std::vector<unsigned char> &serializedMessage)
{
    command = static_cast<Commands>(serializedMessage.front());
}

CommandMessage::~CommandMessage()
{

}

CommandMessage::Commands CommandMessage::commandType()
{
    return command;
}

UASMessage::MessageID CommandMessage::type()
{
    return MessageID::COMMAND;
}

std::vector<unsigned char> CommandMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(static_cast<unsigned char>(command));
    return serializedMessage;
}
