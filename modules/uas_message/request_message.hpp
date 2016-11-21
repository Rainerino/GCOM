#ifndef REQUEST_MESSAGE_H
#define REQUEST_MESSAGE_H

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
class RequestMessage : public UASMessage
{
    public:
        //Public Methods

        /*!
         * \brief RequestMessage constructor that requests a specific message
         * \param [in] requestedMessage the message ID to request
         */
        RequestMessage(UASMessage::MessageID requestedMessage);

        /*!
         * \brief RequestMessage constructor designed to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        RequestMessage(const std::vector<unsigned char> &serializedMessage);

        /*!
         * \brief ~RequestMessage destroys the message and frees all internally allocated memory
         */
        ~RequestMessage();

        /*!
         * \brief request returns the message ID that that this message is requesting
         * \return The MeesageId of the message ID we are requesting
         */
        MessageID request();

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

    private:
        // Private Member Vriable
        MessageID requestedMessage;
};

#endif // REQUEST_MESSAGE_H
