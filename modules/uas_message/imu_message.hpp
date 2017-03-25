#ifndef IMU_MESSAGE_HPP
#define IMU_MESSAGE_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "uas_message.hpp"

class IMUMessage : public UASMessage
{
public:
    IMUMessage(const std::vector<unsigned char> &serializedMessage);
    MessageID type();
    std::vector<unsigned char> serialize();

    // Public Member Variables
    float x;
    float y;
    float z;
};

#endif // IMU_MESSAGE_HPP
