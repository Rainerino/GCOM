#ifndef MAG_MESSAGE_HPP
#define MAG_MESSAGE_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "uas_message.hpp"

class MAGMessage : public UASMessage
{
public:
    MAGMessage(const std::vector<unsigned char> &serializedMessage);
    MessageID type();
    std::vector<unsigned char> serialize();

    // Public Member Variables
    float x;
    float y;
    float z;
};

#endif // MAG_MESSAGE_HPP
