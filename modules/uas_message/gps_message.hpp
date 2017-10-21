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
        GPSMessage(const std::vector<uint8_t> &serializedMessage);
        MessageID type();
        std::vector<uint8_t> serialize();
        // Public Member Variables
        float lat;
        float lon;
};

#endif // GPS_MESSAGE_HPP
