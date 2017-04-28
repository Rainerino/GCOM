#ifndef GPS_MESSAGE_HPP
#define GPS_MESSAGE_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "uas_message.hpp"

class GPSMessage : public UASMessage
{
public:
    GPSMessage(const std::vector<unsigned char> &serializedMessage);
    MessageID type();
    std::vector<unsigned char> serialize();
    // Public Member Variables
    float lat;
    float lon;
};

#endif // GPS_MESSAGE_HPP
