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
}

SystemInfoMessage::SystemInfoMessage(const std::vector<unsigned char> &serializedMessage)
{
    systemId = serializedMessage[0];
    versionNumber = serializedMessage[1];
    cameraRelay = serializedMessage[2];
}

SystemInfoMessage::~SystemInfoMessage()
{

}

UASMessage::MessageID SystemInfoMessage::type()
{
    return MessageID::REQUEST;;
}

std::vector<unsigned char> SystemInfoMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back(systemId);
    serializedMessage.push_back(versionNumber);
    serializedMessage.push_back(cameraRelay);
    return serializedMessage;
}


