#ifndef __UAS_MESSAGE_IMAGE_TAGGER_MESAGE_HPP
#define __UAS_MESSAGE_IMAGE_TAGGER_MESAGE_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "modules/uas_message/uas_message.hpp"

//===================================================================
// Public Class Declaration
//===================================================================
/*!
 * \brief The Image Tagger module is responsible for tagging the
 *        images sent from the drone in-flight and saving it to disk
 *
 * \details Curent implementation only consists of constructors and
 *          serialization method
 *
 */
class ImageTaggerMessage: public UASMessage {
    public:
        // Public Member Methods
        /*!
         * \brief ImageTaggerMessage constructor
         * \param [in] sequenceNumber a byte indicating the current image
         * \param [in] imageData[] a byte array holding the data of the current image
         * \param [in] dataSize size_t indicating the size of the array
         */
        ImageTaggerMessage(unsigned char sequenceNumber, unsigned char imageData[], size_t dataSize);

        /*!
         * ~ImageTaggerMessage deconstructor
         */
        ~ImageTaggerMessage();

        /*!
         * \brief ImageTaggerMessage constructor to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        ImageTaggerMessage(const std::vector<unsigned char> &serializedMessage);

        /*!
         * \brief type returns the type of the message as a MeesageId
         * \return The type of the enclosed message as a MeeageId enum value
         */
        MessageID type();

        /*!
         * \brief serialize serializes the message into a unsigned char vector
         * \return A standard unsigned vector containing the message's serialized contents
         */
        std::vector<unsigned char> serialize();

        /*!
         * \brief getSequenceNumber returns the variable sequenceNumber
         * \return An unsigned char containing an image's sequence number
         */
        unsigned char getSequenceNumber();

        /*!
         * \brief getImageData returns the variable imageData
         * \return A standard unsigned vector containing the image's data
         */
        std::vector<unsigned char> getImageData();
    private:
        // Private Member Variables
        unsigned char sequenceNumber;
        std::vector<unsigned char> imageData;
};

#endif // __UAS_MESSAGE_IMAGE_TAGGER_MESAGE_HPP
