#ifndef UAS_MESSAGE_SERIAL_FRAMER_HPP
#define UAS_MESSAGE_SERIAL_FRAMER_HPP

//===================================================================
// Includes
//===================================================================
// UAS Includes
#include "uas_message.hpp"
// Qt Includes
#include <QDataStream>
// System Includes
#include <vector>
#include <memory>
#include <QByteArray>


//===================================================================
// Public Class Declaration
//===================================================================
/*!
 * \brief The UASMessageTCPFramer class acts as a wrapper around a UAS Message.
 *        It is responsible for packing (framing) and unpacking the message through a
 *        QDataStream over serial.
 * \details The object is meant to be used in 2 ways:
 *          - Message Framer & Serializer:
 *            -# Construct the object using a UASMessage or use the frameMessage function
 *            -# Check the status of the operation using status()
 *            -# Send the data to the stream using the << operator
 *          - Message Unpacker:
 *            -# Construct the object using the default constructor
 *            -# Attempt to read a UASMessage from a stream using the >> Operator
 *            -# Check the status of the operation using status()
 *            -# Use the generateMessage() method to return a new UASMessage
 */
class UASMessageSerialFramer
{
public:
    enum class SerialFramerStatus
    {
        INCOMPLETE_MESSAGE,
        FAILED_FLETCHERS,
        COBS_FAILURE,
        INVALID_MESSAGE,
        SEND_FAILURE,
        SUCCESS
    };

    // Public Methods
    /*!
     * \brief UASMessageSerialFramer, constructs an empty framer
     */
    UASMessageSerialFramer();

    /*!
     * \brief status, reports the status of the framer to the user.
     * \details Useful when using the >> operator and wanting to check that the deserialization was successful
     * \return SerialFramerStatus.
     */
    SerialFramerStatus status();

    /*!
     * \brief clearMessage, deletes the message enclosed in the framer.
     */
    void clearMessage();

    /*!
     * \brief frameMessage, frames the message for sending over a Serial connection
     * \param uasMessage, the message to be framed.
     * \return True if the operation was successful, False otherwise.
     */
    bool frameMessage(UASMessage &uasMessage);

    /*!
     * \brief generateMessage given the contents of a framed serialized UAS message this function generates the message object
     * \return A smart unique pointer to a dynamically allocated UASMessage, if status is false nullptr is returned
     */
    std::unique_ptr<UASMessage> generateMessage();

    /*!
     * \brief operator <<
     * \param out, the output QDataStream that the serialized framed message should be sent over
     * \param UASMessageSerialFramer, the framed message to be sent though the stream
     * \return A pointer to the QDataStream to allow for linking << statements
     */
    friend QDataStream& operator<<(QDataStream& out, UASMessageSerialFramer& uasMessageSerialFramer);
    static bool decodeCOBS(std::vector<unsigned char> &messageData);

    /*!
     * \brief operator >>
     * \param in, the input stream from which to retrieve the serialized framed message
     * \param UASMessageSerialFramer, the message framer to populate with the serialzed contents
     * \return A pointer to the QDataStream to allow for linking << statements
     */
    friend QDataStream& operator>>(QDataStream& in, UASMessageSerialFramer& uasMessageSerialFramer);
    static void appendFletchers16(std::vector<unsigned char> messageData);
    static void encodeCOBS(std::vector<unsigned char> &messageData);


private:
    // Private Member Variables
    SerialFramerStatus framerStatus;
    std::vector<unsigned char> messageData;
    // Private Member Functions
    void initializeDefaults();
};

#endif // UAS_MESSAGE_SERIAL_FRAMER_HPP
