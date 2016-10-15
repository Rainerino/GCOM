#ifndef UAS_MESSAGE_HPP
#define UAS_MESSAGE_HPP


/*!
 *
 */
namespace UasMessage
{

/*!
 * \brief The MeeageIds enum holds all the possible message IDs that UAS's G-COM should be able to handle
 */
enum class MeeageIds : uint8_t
{
    request         = 0x0A,
    version         = 0x0B,
    name            = 0x0C,
    capabilities    = 0x0D,
    image           = 0x0E,
    unspecified     = 0xFF
};

/*!
 * \brief UASMessage is an interface that any UAS message must inherit inorder to be handled by G-COM or Gremlin
 * \details The UASMessage interface should be subclassed by all message objects regardless of the way they will be transporterd
 *          UASMessage enforces that each implimentation have atleast a way to serialize its contents into a byte array and a way to
 *          reconstruct it using the given array. The array is specified to be a primative array of bytes to ensure portability.
 *          However, for the sake of simplicity on any given platform additional serialize methods that return other array types.
 */
class UASMessage
{
    public:
        //Public Methods
        /*!
         * \brief Pure virtual constructor that creates a blank message so that it can be programatically built up.
         */
        virtual UASMessage()=0;

        /*!
         * \brief Pure virtual constructor designed to initialize a message using a serialized payload
         * \param [in] messagePayload A pointer to the serialized message's const byte array
         * \param [in] size The size of the payload array
         */
        virtual UASMessage(const byte* messagePayload, size_t size)=0;

        /*!
         * \brief Pure virtual function that serializes the message into a byte array
         * \param [out] messagePayload A pointer that will point to a newly allocated byte array
         * \return The size of the array created
         * \warning The array produced by this method needs to be deleted to avoid memory leaks
         */
        virtual int serialize(byte* messagePayload)=0;

        /*!
         * \brief This function returns the type of the message
         * \return The type of the enclosed message as a MeeageIds enum value
         */
        virtual MeeageIds type()=0;
};
}

#endif // UAS_MESSAGE_HPP
