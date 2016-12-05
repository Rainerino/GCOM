//===================================================================
// Includes
//===================================================================
// UAS Includes
#include "uas_message_tcp_framer.hpp"
#include "uas_message.hpp"
#include "request_message.hpp"
#include "system_info_message.hpp"
// Qt Includes
#include <QDataStream>
#include <array>
#include <vector>
// System Includes
#include <memory>

//===================================================================
// Constants
//===================================================================
const int FRAMED_MESG_SIZE_FIELD_SIZE = 4;
const int FRAMED_MESG_ID_FIELD_SIZE = 1;

//===================================================================
// Class Definitions
//===================================================================
UASMessageTCPFramer::UASMessageTCPFramer()
{
    initializeDefaults();
}

void UASMessageTCPFramer::initializeDefaults()
{
    framerStatus = false;
    messageData.clear();
}

bool UASMessageTCPFramer::frameMessage(UASMessage &uasMessage)
{
    // First we receive the serialized message and check its size
    messageData = uasMessage.serialize();
    size_t serializedMessageSize = messageData.size();
    // We only allow messages whos size can be represented in 4 Bytes
    if (serializedMessageSize <= 0 || serializedMessageSize > 0xFFFFFFFF)
        return false;
    // Then we append its size to the front of the message (Big Endian)
    for (int sizeFieldByte = 0; sizeFieldByte < FRAMED_MESG_SIZE_FIELD_SIZE; sizeFieldByte++)
         messageData.insert(messageData.begin(), (serializedMessageSize >> (8 * sizeFieldByte ))& 0xFF);
    // Finally append the message ID to the very front of the message
    messageData.insert(messageData.begin(), static_cast<unsigned char>(uasMessage.type()));
    return true;
}

std::unique_ptr<UASMessage> UASMessageTCPFramer::generateMessage()
{
    // If there is no valid message then we return a nullptr
    if (framerStatus == false)
        return nullptr;
    // We know that starting from the fifth byte all data will be the message's serial contents
    std::vector<unsigned char> serialMessagePayload(messageData.begin() + FRAMED_MESG_ID_FIELD_SIZE
                                                    + FRAMED_MESG_SIZE_FIELD_SIZE, messageData.end());
    // Next we switch on the type of the message so that we can construct the appropriate object and return it
    switch (static_cast<UASMessage::MessageID>(messageData.front()))
    {
        case UASMessage::MessageID::REQUEST:
        {
            std::unique_ptr<UASMessage> message(new RequestMessage(serialMessagePayload));
            return message;
        }
        case UASMessage::MessageID::SYSTEM_INFO:
        {
            std::unique_ptr<UASMessage> message(new SystemInfoMessage(serialMessagePayload));
            return message;
        }
        default:
            return nullptr;
    }
}


void UASMessageTCPFramer::clearMessage()
{
    messageData.clear();
}


QDataStream& operator>>(QDataStream& inputStream, UASMessageTCPFramer& uasMessageTCPFramer)
{
    // Reset the state of the framer to its default state
    uasMessageTCPFramer.initializeDefaults();
    // Then we need to check the state of the input stream
    if (inputStream.status() != QDataStream::Ok)
        return inputStream;
    // Then we read a the ID byte from the input stream
    inputStream >> uasMessageTCPFramer.messageData[0];
    // Next we attempt to read the length of the message
    uint32_t messageSize;
    inputStream >> messageSize;
    // Check if not enough data is present
    if (inputStream.status() != QDataStream::Ok)
        return inputStream;
    // Split the the length field into 4 bytes
    for (int sizeFieldByte = 1; sizeFieldByte <= FRAMED_MESG_SIZE_FIELD_SIZE; sizeFieldByte++)
         uasMessageTCPFramer.messageData.push_back((messageSize >> (8 * FRAMED_MESG_SIZE_FIELD_SIZE - sizeFieldByte ))& 0xFF);
    // Attempt to retrieve the payload data
    uasMessageTCPFramer.messageData.resize(uasMessageTCPFramer.messageData.size() + messageSize);
    int readLength = inputStream.readRawData(reinterpret_cast<char*>(uasMessageTCPFramer.messageData.data()
                                             + FRAMED_MESG_ID_FIELD_SIZE
                                             + FRAMED_MESG_SIZE_FIELD_SIZE)
                                             , messageSize);
    // Finally we check if the entire payload was read successfully
    if ((inputStream.status() != QDataStream::Ok) || (readLength != messageSize))
        return inputStream;

    uasMessageTCPFramer.framerStatus = true;
    return inputStream;
}

QDataStream& operator<<(QDataStream& outputStream, UASMessageTCPFramer& uasMessageTCPFramer)
{
    // If there is no valid message then we return straight away
    if (uasMessageTCPFramer.framerStatus == false)
        return outputStream;

    // Attempt to write the message's contents to the stream
    int writtenBytes = outputStream.writeRawData(reinterpret_cast<char*>(uasMessageTCPFramer.messageData.data()),
                                                 uasMessageTCPFramer.messageData.size());
    // If there was an error in writing the data then set the internal flag
    if ((outputStream.status() != QDataStream::Ok) || (writtenBytes != uasMessageTCPFramer.messageData.size()))
        uasMessageTCPFramer.framerStatus = false;

    return outputStream;
}

