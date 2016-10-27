#ifndef UASMESSAGETCPFRAMER_HPP
#define UASMESSAGETCPFRAMER_HPP

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
 *        QDataStream.
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
class UASMessageTCPFramer
{
    public:

        // Public Methods
        /*!
         * \brief UASMessageTCPFramer, constructs an empty framer
         */
        UASMessageTCPFramer();


        /*!
         * \brief status, reports the status of the framer to the user.
         * \details Useful when using the >> operator and wanting to check that the deserialization was successful
         * \return True if the framer contains a valid message. Otherwise, returns False.
         */
        bool status();

        /*!
         * \brief clearMessage, deletes the message enclosed in the framer.
         */
        void clearMessage();

        /*!
         * \brief frameMessage, frames the message for sending over a TCP connection
         * \param uasMessage, the message to be framed.
         * \return True if the operation was successful, False otherwise.
         */
        bool frameMessage(UASMessage &uasMessage);


        /*!
         * \brief generateMessage given the contents of a framed serialized UAS message this function generates the message object
         * \return A smart unique pointer to a dynamically allocated UASMessage, if status is false nullptr is returned
         */
        std::unique_ptr<UASMessage> generateMessage();

        // Publicly overloaded operators
        /*!
         * \brief operator <<
         * \param out, the output QDataStream that the serialized framed message should be sent over
         * \param uasMessageTCPFramer, the framed message to be sent though the stream
         * \return A pointer to the QDataStream to allow for linking << statements
         */
        friend QDataStream& operator<<(QDataStream& out, UASMessageTCPFramer& uasMessageTCPFramer);

        /*!
         * \brief operator >>
         * \param in, the input stream from which to retrieve the serialized framed message
         * \param uasMessageTCPFramer, the message framer to populate with the serialzed contents
         * \return A pointer to the QDataStream to allow for linking << statements
         */
        friend QDataStream& operator>>(QDataStream& in, UASMessageTCPFramer& uasMessageTCPFramer);


    private:
        // Private Member Variables
        bool framerStatus;
        std::vector<unsigned char> messageData;
        // Private Member Functions
        void initializeDefaults();
};

#endif // UASMESSAGETCPFRAMER_HPP
