//===================================================================
// Includes
//===================================================================
// GCOM Includes
#include "uas_message.hpp"
#include "mag_message.hpp"
// System Includes
#include <vector>

//===================================================================
// Defines
//===================================================================
// Functions
#define UNPACK_MAG(x) (x/1e2)

//===================================================================
// Constants
//===================================================================
const int SIZE_FIELD_X_Y_Z = 4;

MAGMessage::MAGMessage(const std::vector<unsigned char> &serializedMessage)
{
    // Reconstruct the x y z
    int32_t packedX = 0;
    int32_t packedY = 0;
    int32_t packedZ = 0;
    for (int byte_index = 0; byte_index < SIZE_FIELD_X_Y_Z; byte_index++)
    {
        packedX |= (serializedMessage[byte_index] << (8*(SIZE_FIELD_X_Y_Z - byte_index - 1)));
        packedY |= (serializedMessage[SIZE_FIELD_X_Y_Z + byte_index] << (8*(SIZE_FIELD_X_Y_Z - byte_index - 1)));
        packedZ |= (serializedMessage[2*SIZE_FIELD_X_Y_Z + byte_index] << (8*(SIZE_FIELD_X_Y_Z - byte_index - 1)));
    }

    x = (float) UNPACK_MAG(packedX);
    y = (float) UNPACK_MAG(packedY);
    z = (float) UNPACK_MAG(packedZ);
}

UASMessage::MessageID MAGMessage::type()
{
    return UASMessage::MessageID::DATA_MAG;
}

std::vector<unsigned char> MAGMessage::serialize()
{
    std::vector<unsigned char> serializedMessage;
    // TODO: FIX THIS!!
    return serializedMessage;
}

