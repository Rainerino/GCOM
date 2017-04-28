//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
#include <string>
// GCOM Includes
#include "system_info_message.hpp"
#include "uas_message.hpp"

//===================================================================
// Class Definitions
//===================================================================
SystemInfoMessage::SystemInfoMessage(std::string systemId, uint16_t versionNumber, bool dropped)
{
    this->systemId = systemId;
    this->versionNumber = versionNumber;
    this->dropped = dropped;
}

SystemInfoMessage::SystemInfoMessage(const std::vector<unsigned char> &serializedMessage)
{

    versionNumber = (serializedMessage[0] << 8) ||  serializedMessage[1];
    dropped = serializedMessage[2];
    systemId = std::string(serializedMessage.begin()+3,serializedMessage.end());

}

SystemInfoMessage::~SystemInfoMessage()
{

}

UASMessage::MessageID SystemInfoMessage::type()
{
    return MessageID::SYSTEM_INFO;
}

std::vector<unsigned char> SystemInfoMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(versionNumber >> 8);
    serializedMessage.push_back(versionNumber & 0xFF);
    serializedMessage.push_back(dropped);
    serializedMessage.insert(serializedMessage.end(), systemId.begin(),systemId.end());
    return serializedMessage;
}


