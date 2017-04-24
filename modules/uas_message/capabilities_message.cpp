#include "capabilities_message.hpp"
#include "modules/uas_message/uas_message.hpp"

CapabilitiesMessage::CapabilitiesMessage(Capabilities capabilities)
{
    this->capabilities = capabilities;
}

CapabilitiesMessage::CapabilitiesMessage(const std::vector<unsigned char> &serializedMessage)
{
    uint32_t serializedCapabilities = serializedMessage[0] << 24 | serializedMessage[1] <<  16 |
                                      serializedMessage[2] << 8  | serializedMessage[3];
    this->capabilities = static_cast<CapabilitiesMessage::Capabilities>(serializedCapabilities);
}

CapabilitiesMessage::~CapabilitiesMessage()
{

}

CapabilitiesMessage::Capabilities CapabilitiesMessage::getCapabilities()
{
    return this->capabilities;
}

UASMessage::MessageID CapabilitiesMessage::type()
{
    return UASMessage::MessageID::MESG_CAPABILITIES;
}

std::vector<unsigned char> CapabilitiesMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    serializedMessage.push_back((static_cast<uint32_t>(capabilities) >> 24) & 0xFF);
    serializedMessage.push_back((static_cast<uint32_t>(capabilities) >> 16) & 0xFF);
    serializedMessage.push_back((static_cast<uint32_t>(capabilities) >> 8) & 0xFF);
    serializedMessage.push_back((static_cast<uint32_t>(capabilities)) & 0xFF);
    return serializedMessage;
}
