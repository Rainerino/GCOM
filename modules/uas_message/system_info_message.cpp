//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "system_info_message.hpp"
#include "uas_message.hpp"

//===================================================================
// Class Definitions
//===================================================================
SystemInfoMessage::SystemInfoMessage(unsigned char systemId, unsigned char versionNumber)
{
    this->systemId = systemId;
    this->versionNumber = versionNumber;
    cameraRelay = false;
    messageType = MessageId::REQUEST;
}

SystemInfoMessage::SystemInfoMessage(const std::vector<unsigned char> &serializedMessage)
{
    cameraRelay = serializedMessage.pop_back();
    versionNumber = serializedMessage.pop_back();
    systemId = serializedMessage.pop_back();
}

SystemInfoMessage::~SystemInfoMessage()
{

}

UASMessage::MessageId SystemInfoMessage::type()
{
    return messageType;
}

std::vector<unsigned char> SystemInfoMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(systemId);
    serializedMessage.push_back(versionNumber);
    serializedMessage.push_back(cameraRelay);
}


