#ifndef COMMANDMESSAGE_H
#define COMMANDMESSAGE_H

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
class CommandMessage : public UASMessage
{
    public:

        // Public Definitions
        /*!
         * \brief The Commands enum describes all possible commands that can be sent to the Gremlin.
         *        The meanings of the commands can be found in the relavent confluence pages
         */
        enum class Commands : unsigned char
        {
            SYSTEM_RESET            = 0x01,
            SYSTEM_PAUSE            = 0x02,
            SYSTEM_RESUME           = 0x03,
            IMAGE_RELAY_START       = 0x04,
            IMAGE_RELAY_STOP        = 0x05,
        };

        //Public Methods
        /*!
         * \brief CommandMessage's constructor creates a command message to send to the drone
         * \param [in] command, The command to send to the Gremlin
         */
        CommandMessage(Commands command);

        /*!
         * \brief CommandMessage constructor designed to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        CommandMessage(const std::vector<unsigned char> &serializedMessage);

        /*!
         * \brief ~CommandMessage destroys the message and frees all internally allocated memory
         */
        ~CommandMessage();

        /*!
         * \brief commandType returns the type of command that was requested
         * \return The type of command that was requested
         */
        Commands commandType();

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
        Commands command;
};

#endif // COMMANDMESSAGE_H
