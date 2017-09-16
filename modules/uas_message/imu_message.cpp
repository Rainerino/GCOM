//===================================================================
// Includes
//===================================================================
// GCOM Includes
#include "uas_message.hpp"
#include "imu_message.hpp"
// System Includes
#include <vector>

//===================================================================
// Defines
//===================================================================
// Functions
#define UNPACK_IMU(x) (x/1e2)

//===================================================================
// Constants
//===================================================================
const int SIZE_FIELD_X_Y_Z = 4;

IMUMessage::IMUMessage(const std::vector<uint8_t> &serializedMessage)
{
    // Reconstruct the latitude and longitude
    int32_t packedX = 0;
    int32_t packedY = 0;
    int32_t packedZ = 0;
    for (int byte_index = 0; byte_index < SIZE_FIELD_X_Y_Z; byte_index++)
    {
        packedX |= (serializedMessage[byte_index] << (8*(SIZE_FIELD_X_Y_Z - byte_index - 1)));
        packedY |= (serializedMessage[SIZE_FIELD_X_Y_Z + byte_index] << (8*(SIZE_FIELD_X_Y_Z - byte_index - 1)));
        packedZ |= (serializedMessage[2*SIZE_FIELD_X_Y_Z + byte_index] << (8*(SIZE_FIELD_X_Y_Z - byte_index - 1)));
    }

    x = (float) UNPACK_IMU(packedX);
    y = (float) UNPACK_IMU(packedY);
    z = (float) UNPACK_IMU(packedZ);
}

UASMessage::MessageID IMUMessage::type()
{
    return UASMessage::MessageID::DATA_IMU;
}

std::vector<uint8_t> IMUMessage::serialize()
{
    std::vector<uint8_t> serializedMessage;
    // TODO: FIX THIS!!
    return serializedMessage;
}

