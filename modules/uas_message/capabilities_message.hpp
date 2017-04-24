#ifndef CAPABILITIES_MESSAGE_HPP
#define CAPABILITIES_MESSAGE_HPP
//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
#include <cstdint>
// GCOM Includes
#include "uas_message.hpp"

//===================================================================
// Public Class Declarations
//===================================================================
class CapabilitiesMessage : public UASMessage
{
    public:

        // Public Definitions
        /*!
         * \brief The Capabilities enum describes all possible capabilities that the
         *        drone supports, these capabilities denote what type of requests/commands
         *        the Gremlin will be able to handle
         */
        enum class Capabilities : uint32_t
        {
            IMAGE_RELAY             = 0x01,
        };

        //Public Methods
        /*!
         * \brief CapabilitiesMessage's constructor creates a capabilities message which will be
         *        returned by the Gremlin
         * \param [in] capabilities, The capabilities that the drone supports (can use the |
         *        operator)
         */
        CapabilitiesMessage(Capabilities capabilities);

        /*!
         * \brief CapabilitiesMessage constructor designed to initialize a message using a
         *        serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        CapabilitiesMessage(const std::vector<unsigned char> &serializedMessage);

        /*!
         * \brief ~CapabilitiesMessage destroys the message and frees all internally allocated
         *        memory
         */
        ~CapabilitiesMessage();

        /*!
         * \brief getCapabilities returns the capabilities reported by the drone
         * \note  These can be several |ed capabilites.
         * \return The drone's capabilities
         */
        Capabilities getCapabilities();

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
        Capabilities capabilities;
};


/*!
 * \brief operator |
 * \param a a Capabilities enum value
 * \param b a Capabilities enum value
 * \return
 */

#endif // CAPABILITIES_MESSAGE_HPP
