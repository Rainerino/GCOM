

//============================================
// Includes
//============================================
// UAS Includes
#include "uas_message_tcp_framer.hpp"
#include "uas_message.hpp"
// Qt Includes
#include <QDataStream>
#include <array>
#include <vector>


UASMessageTCPFramer::UASMessageTCPFramer()
{
    mStatus = false;
    mMessageData.clear();
}

bool UASMessageTCPFramer::frameMessage(UASMessage &uasMessage)
{
    // First we receive the serialized message and check its size
    mMessageData = uasMessage.serialize();
    size_t serializedMessageSize = mMessageData.size();
    if (serializedMessageSize <= 0 || serializedMessageSize > 0xFFFF)
        return false;
    // Then we append it's size to the front of the message (Big Endian)
    mMessageData.insert(mMessageData.begin(), (serializedMessageSize >> 8) & 0xFF);
    mMessageData.insert(mMessageData.begin(), serializedMessageSize & 0xFF);
    // Finally append the message ID to the very front of the message
    mMessageData.insert(mMessageData.begin(), uasMessage.type());
    return true;
}
