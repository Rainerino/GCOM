//===================================================================
// Includes
//===================================================================
// UAS Includes
#include "uas_message_serial_framer.hpp"
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
// Defines
//===================================================================
#define FinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)

//===================================================================
// Constants
//===================================================================
const int FRAMED_MESG_SIZE_FIELD_SIZE = 2;
const int FRAMED_MESG_ID_FIELD_SIZE = 1;
const int COBS_OVERHEAD = 2;

//===================================================================
// Class Definitions
//===================================================================
UASMessageSerialFramer::UASMessageSerialFramer()
{
    initializeDefaults();
}

void UASMessageSerialFramer::initializeDefaults()
{
    framerStatus = SerialFramerStatus::INVALID_MESSAGE;
    messageData.clear();
}

bool UASMessageSerialFramer::frameMessage(UASMessage &uasMessage)
{
    // First we receive the serialized message and check its size
    messageData = uasMessage.serialize();
    size_t serializedMessageSize = messageData.size();
    // We only allow messages whos size can be represented in 2 Bytes
    if (serializedMessageSize <= 0 || serializedMessageSize > 0xFFFF)
        return false;
    // Then we append its size to the front of the message (Big Endian)
    messageData.insert(messageData.begin(), serializedMessageSize & 0xFF);
    messageData.insert(messageData.begin(), serializedMessageSize >> 8 & 0xFF);
    // Append the message ID to the very front of the message
    messageData.insert(messageData.begin(), static_cast<unsigned char>(uasMessage.type()));
    // Calculate Flecher's 16
    appendFletchers16(messageData);
    // Apply the cobs
    encodeCOBS(messageData);
    // update the status
    ramerStatus = UASMessageSerialFramer::SerialFramerStatus::SUCCESS;
    return true;
}

void UASMessageSerialFramer::appendFletchers16(std::vector<unsigned char> messageData)
{

    auto data = messageData.begin();
    size_t bytes = messageData.size();

    uint16_t sum1 = 0xff, sum2 = 0xff;
    size_t tlen;

    while (bytes) {
            tlen = ((bytes >= 20) ? 20 : bytes);
            bytes -= tlen;
            do {
                    sum2 += sum1 += *data++;
                    tlen--;
            } while (tlen);
            sum1 = (sum1 & 0xff) + (sum1 >> 8);
            sum2 = (sum2 & 0xff) + (sum2 >> 8);
    }
    /* Second reduction step to reduce sums to 8 bits */
    sum1 = (sum1 & 0xff) + (sum1 >> 8);
    sum2 = (sum2 & 0xff) + (sum2 >> 8);

    messageData.push_back(sum2);
    messageData.push_back(sum1);
}

void UASMessageSerialFramer::encodeCOBS(std::vector<unsigned char> &messageData)
{
    std::vector<unsigned char> encodedMessageData;
    encodedMessageData.resize(messageData.size() + COBS_OVERHEAD);

    uint8_t *dst = encodedMessageData.data();
    uint8_t *ptr = messageData.data();
    const uint8_t *end = ptr + messageData.size();
    uint8_t *code_ptr = dst++;
    uint8_t code = 0x01;

    while (ptr < end)
    {
      if (*ptr == 0)
        FinishBlock(code);
      else
      {
        *dst++ = *ptr;
        if (++code == 0xFF)
          FinishBlock(code);
      }
      ptr++;
    }

    FinishBlock(code);
    messageData.assign(encodedMessageData.begin(), encodedMessageData.end());
}

QDataStream& operator<<(QDataStream& outputStream, UASMessageSerialFramer& uasMessageSerialFramer)
{
    // If there is no valid message then we return straight away
    if (uasMessageSerialFramer.framerStatus != UASMessageSerialFramer::SerialFramerStatus::SUCCESS)
        return outputStream;

    // Attempt to write the message's contents to the stream
    int writtenBytes = outputStream.writeRawData(reinterpret_cast<char*>(uasMessageSerialFramer.messageData.data()),
                                                 uasMessageTCPFramer.messageData.size());

    // If there was an error in writing the data then set the internal flag
    if ((outputStream.status() != QDataStream::Ok) || (writtenBytes != uasMessageTCPFramer.messageData.size()))
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::SEND_FAILURE;


    uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::SUCCESS;
    return outputStream;
}

/*
QDataStream& operator>>(QDataStream& inputStream, UASMessageSerialFramer& uasMessageSerialFramer)
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
}*/
