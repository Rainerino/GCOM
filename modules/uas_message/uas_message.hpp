#ifndef UASMESSAGE_HPP
#define UASMESSAGE_HPP

//===============================================
// Includes
//===============================================
// System Includes
#include <vector>


//===============================================
// Public Class Definitions
//===============================================
/*!
 * \brief UASMessage is an interface that any UAS message must inherit inorder to be handled by G-COM or Gremlin
 * \details The UASMessage interface should be subclassed by all message objects regardless of the way they will be transported
 *          UASMessage enforces that each implementation at least have a way to serialize its contents into a byte vector and a way to
 *          reconstruct it using the given array. The vector is specified to be std::vector<uchar> to ensure portability
 *          However, for the sake of simplicity on any given platform additional serialize methods that return other array types can be included.
 */
class UASMessage
{
    public:

        /*!
         * \brief The MeeageIds enum holds all the possible message IDs that UAS's G-COM should be able to handle
         */
        enum MessageID : unsigned char
        {
            request         = 0x0A,
            version         = 0x0B,
            name            = 0x0C,
            capabilities    = 0x0D,
            image           = 0x0E,
            unspecified     = 0xFF
        };

        //Public Methods
        /*!
         * \brief Constructor that creates a blank message so that it can be programmatically built up.
         */
        UASMessage();

        /*!
         * \brief Constructor designed to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        UASMessage(std::vector<unsigned char> serializedMessage);

        /*!
         * \brief ~UASMessage a virtual destructor that must be implemented in order for proper polymorphic behavior
         */
        virtual ~UASMessage()=0;

        /*!
         * \brief Pure virtual function that returns the type of the message as a MeesageId
         * \return The type of the enclosed message as a MeeageId enum value
         */
        virtual MessageID type()=0;

        /*!
         * \brief Pure virtual function that serializes the message into a unsigned char vector
         * \return An standard unsigned vector containing the message's serialized contents
         */
        virtual std::vector<unsigned char> serialize()=0;
};
#endif // UAS_MESSAGE_HPP
