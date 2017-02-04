#ifndef SYSTEMINFOMESSAGE_H
#define SYSTEMINFOMESSAGE_H

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "uas_message.hpp"

//===================================================================
// Public Class Declarations
//===================================================================
class SystemInfoMessage : public UASMessage
{
    public:


        //Public Methods

        /*!
         * \brief SystemInfoMessage The base constructor for the system info message
         * \param [in] systemId, the unique identifier for the system
         * \param [in] versionNumber, version number for the system
         */
        SystemInfoMessage(unsigned char systemId, unsigned char versionNumber);

        /*!
         * \brief SystemInfoMessage constructor designed to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        SystemInfoMessage(const std::vector<unsigned char> &serializedMessage);

        /*!
         * \brief ~SystemInfoMessage destroys the message and frees all internally allocated memory
         */
        ~SystemInfoMessage();

        /*!
         * \brief type returns the type of the message as a MeesageId
         * \return The type of the enclosed message as a MeeageId enum value
         */
        MessageID type();

        /*!
         * \brief serialize serializes the message into a unsigned char vector
         * \return An standard unsigned vector containing the message's serialized contents
         */
        std::vector<unsigned char> serialize();

        // Member Variables
        unsigned char  systemId;
        unsigned char  versionNumber;
        bool cameraRelay;
};

#endif // SYSTEMINFOMESSAGE_H
