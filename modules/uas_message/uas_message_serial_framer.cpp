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
const int SIZE_FIELD_ID = 1;
const int SIZE_FIELD_SIZE = 1;
const int SIZE_HEADER = SIZE_FIELD_ID + SIZE_FIELD_SIZE;
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
    framerStatus = UASMessageSerialFramer::SerialFramerStatus::SUCCESS;
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


bool UASMessageSerialFramer::decodeCOBS(std::vector<unsigned char> &messageData)
{
    std::vector<unsigned char> decodedMessageData;
    decodedMessageData.resize(messageData.size());
    const uint8_t *ptr = messageData.data();
    uint8_t *dst = decodedMessageData.data();
    const uint8_t *end = ptr + messageData.size();


    while (ptr < end)
    {
        int code = *ptr++;
        for (int i = 1; ptr < end && i < code; i++)
          *dst++ = *ptr++;
        if (code < 0xFF)
          *dst++ = 0;
    }

    if (ptr > end)
        return false;

    messageData.assign(decodedMessageData.begin(), decodedMessageData.end());
    messageData.resize(messageData.size() - COBS_OVERHEAD);
    return true;
}

QDataStream& operator<<(QDataStream& outputStream, UASMessageSerialFramer& uasMessageSerialFramer)
{
    // If there is no valid message then we return straight away
    if (uasMessageSerialFramer.framerStatus != UASMessageSerialFramer::SerialFramerStatus::SUCCESS)
        return outputStream;

    // Attempt to write the message's contents to the stream
    int writtenBytes = outputStream.writeRawData(reinterpret_cast<char*>(uasMessageSerialFramer.messageData.data()),
                                                 uasMessageSerialFramer.messageData.size());

    // If there was an error in writing the data then set the internal flag
    if ((outputStream.status() != QDataStream::Ok) || (writtenBytes != uasMessageSerialFramer.messageData.size()))
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::SEND_FAILURE;


    uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::SUCCESS;
    return outputStream;
}


QDataStream& operator>>(QDataStream& inputStream, UASMessageSerialFramer& uasMessageSerialFramer)
{
    int bytesRead;
    // Reset the state of the framer to its default state
    uasMessageSerialFramer.initializeDefaults();
    uasMessageSerialFramer.messageData.resize(SIZE_HEADER);
    // Then we need to check the state of the input stream
    if (inputStream.status() != QDataStream::Ok)
        return inputStream;
    // Then we read the header of the message
    bytesRead = inputStream.readRawData(reinterpret_cast<char *>(uasMessageSerialFramer.messageData.data()), SIZE_HEADER);
    if (bytesRead < SIZE_HEADER)
    {
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::INCOMPLETE_MESSAGE;
        return inputStream;
    }
    // Check if not enough data is present
    if (inputStream.status() != QDataStream::Ok)
    {
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::INCOMPLETE_MESSAGE;
        return inputStream;
    }

    // Attempt to retrieve the payload data
    uasMessageSerialFramer.messageData.resize(uasMessageSerialFramer.messageData.size() + uasMessageSerialFramer.messageData[1]);
    bytesRead = inputStream.readRawData(reinterpret_cast<char*>(uasMessageSerialFramer.messageData.data()
                                                                + SIZE_HEADER)
                                        , uasMessageSerialFramer.messageData[1]);

    // Finally we check if the entire payload was read successfully
    if ((inputStream.status() != QDataStream::Ok) || (bytesRead < uasMessageSerialFramer.messageData[1]))
    {
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::INCOMPLETE_MESSAGE;
        return inputStream;
    }

    // Attempt to decode the COBS
    if(!UASMessageSerialFramer::decodeCOBS(uasMessageSerialFramer.messageData))
    {
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::COBS_FAILURE;
        return inputStream;
    }

    // Check the checksum
    uint8_t checkSumLower = uasMessageSerialFramer.messageData.back();
    uasMessageSerialFramer.messageData.pop_back();
    uint8_t checkSumUpper = uasMessageSerialFramer.messageData.back();
    uasMessageSerialFramer.messageData.pop_back();

    UASMessageSerialFramer::appendFletchers16(uasMessageSerialFramer.messageData);
    if((checkSumLower != uasMessageSerialFramer.messageData[uasMessageSerialFramer.messageData.size() - 1]) ||
       (checkSumUpper != uasMessageSerialFramer.messageData[uasMessageSerialFramer.messageData.size() - 2]))
    {
        uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::FAILED_FLETCHERS;
        return inputStream;
    }

    uasMessageSerialFramer.framerStatus = UASMessageSerialFramer::SerialFramerStatus::SUCCESS;
    return inputStream;
}
