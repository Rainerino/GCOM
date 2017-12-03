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
CommandMessage::CommandMessage(Commands command, Triggers trigger, uint8_t parameter)
{
    this->command   = command;
    this->trigger   = trigger;
    this->parameter = parameter;
}

CommandMessage::CommandMessage(Commands command)
{
    this->command   = command;
    this->trigger   = Triggers::NONE;
    this->parameter = 0x00;
}

CommandMessage::CommandMessage(const std::vector<uint8_t> &serializedMessage)
{
    command = static_cast<Commands>(serializedMessage.front());
    trigger = static_cast<Triggers>(serializedMessage[1]);
    parameter = static_cast<uint8_t>(serializedMessage[2]);
}

CommandMessage::~CommandMessage()
{

}

UASMessage::MessageID CommandMessage::type()
{
    return MessageID::COMMAND;
}

std::vector<uint8_t> CommandMessage::serialize()
{
    std::vector<uint8_t> serializedMessage;
    serializedMessage.push_back(static_cast<uint8_t>(command));
    serializedMessage.push_back(static_cast<uint8_t>(trigger));
    serializedMessage.push_back(parameter);
    return serializedMessage;
}
