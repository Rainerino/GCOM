//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
#include <algorithm>
#include <iterator>
// GCOM Includes
#include "modules/uas_message/image_tagged_message.hpp"

//===================================================================
// Defines
//===================================================================
#define PACK_LAT_LON(x) (float) (x*1e7)
#define UNPACK_LAT_LON(x) (float) (x/1e7)

//===================================================================
// Constants
//===================================================================
const int SIZE_LAT_LON = 4;
const int COORD_OFFSET = 1;
const int IMAGE_DATA_OFFSET = COORD_OFFSET + SIZE_LAT_LON * 2;
const float LAT_INVALID = 100.0;
const int LAT_RANGE = 90;
const int LON_RANGE = 180;

//===================================================================
// Class Definitions
//===================================================================
ImageTaggedMessage::ImageTaggedMessage(uint8_t sequenceNumber, float latitude,
                           float longitude, uint8_t* imageData, size_t dataSize) :
                           ImageUntaggedMessage(sequenceNumber, imageData, dataSize)
{
    this->latitude = latitude;
    this->longitude = longitude;
}

ImageTaggedMessage::ImageTaggedMessage(uint8_t sequenceNumber, int32_t latitude,
                           int32_t longitude, uint8_t* imageData, size_t dataSize) :
                           ImageUntaggedMessage(sequenceNumber, imageData, dataSize)
{
    this->latitudeInt = latitude;
    this->longitudeInt = longitude;

    // Set an invalid float latitude if latitude has been converted already, to distinguish for serialize()
    this->latitude = LAT_INVALID;
}

ImageTaggedMessage::ImageTaggedMessage(const std::vector<uint8_t> &serializedMessage)
{
    sequenceNumber = serializedMessage.front();

    // Reconstruct the latitude and longitude

    // Extract subvector
    std::vector<uint8_t> serializedCoords(std::begin(serializedMessage) + COORD_OFFSET,
                     std::begin(serializedMessage) + (IMAGE_DATA_OFFSET - COORD_OFFSET));

    uint32_t packedLat = 0;
    uint32_t packedLon = 0;

    for (int byte_index = 0; byte_index < SIZE_LAT_LON; byte_index++)
    {
        packedLat |= (serializedCoords[byte_index] << (8*(SIZE_LAT_LON - byte_index - 1)));
        packedLon |= (serializedCoords[SIZE_LAT_LON + byte_index] << (8*(SIZE_LAT_LON - byte_index - 1)));
    }

    latitude = UNPACK_LAT_LON(packedLat) - LAT_RANGE;
    longitude = UNPACK_LAT_LON(packedLon) - LON_RANGE;

    imageData.assign(serializedMessage.begin() + IMAGE_DATA_OFFSET, serializedMessage.end());
}

ImageTaggedMessage::~ImageTaggedMessage()
{

}

UASMessage::MessageID ImageTaggedMessage::type()
{
    return MessageID::DATA_IMAGE_TAGGED;
}

std::vector<uint8_t> ImageTaggedMessage::serialize()
{
    std::vector<uint8_t> serializedMessage = ImageUntaggedMessage::serialize();

    uint32_t packedLat;
    uint32_t packedLon;

    // Pack the latitude and longitude if they are floats
    if (latitude != LAT_INVALID) {
        latitude += LAT_RANGE;
        longitude += LON_RANGE;
        packedLat = PACK_LAT_LON(latitude);
        packedLon = PACK_LAT_LON(longitude);
    }
    // Skip if they are ints
    else {
        packedLat = latitudeInt + PACK_LAT_LON(LAT_RANGE);
        packedLon = longitudeInt + PACK_LAT_LON(LON_RANGE);
    }

    // Serialize the coordinates
    std::vector<uint8_t> serializedCoords;
    serializedCoords.resize(SIZE_LAT_LON * 2);

    for (int byte_index = 0; byte_index < SIZE_LAT_LON; byte_index++)
    {
        serializedCoords[byte_index] = (packedLat >> (8*(SIZE_LAT_LON - byte_index - 1))) & 0xFF;
        serializedCoords[SIZE_LAT_LON + byte_index] = (packedLon >> (8*(SIZE_LAT_LON - byte_index - 1))) & 0xFF;
    }

    // Insert serialized coordinates into byte vector created by superclass
    serializedMessage.insert(std::begin(serializedMessage) + COORD_OFFSET,
                             std::begin(serializedCoords), std::end(serializedCoords));

    return serializedMessage;
}
