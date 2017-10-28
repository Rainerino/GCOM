#ifndef IMAGE_TAGGED_MESSAGE_HPP
#define IMAGE_TAGGED_MESSAGE_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <vector>
// GCOM Includes
#include "modules/uas_message/uas_message.hpp"
#include "modules/uas_message/image_untagged_message.hpp"

//===================================================================
// Public Class Declaration
//===================================================================
/*!
 * \brief The Image Tagger module is responsible for tagging the
 *        images sent from the drone in-flight and saving it to disk
 *
 * \details ImageTaggedMessage includes GPS coordinates of the image
 * \details Curent implementation only consists of constructors and
 *          serialization method
 * \details GPS coordinates as int32_t are converted from doubles by a factor of 1e7
 *
 */
class ImageTaggedMessage : public ImageUntaggedMessage {
    public:
        // Public Member Methods
        /*!
         * \brief ImageTaggedMessage constructor
         * \param [in] sequenceNumber a byte indicating the current image
         * \param [in] latitude of the current image
         * \param [in] longitude of the current image
         * \param [in] imageData[] a byte array holding the data of the current image
         * \param [in] dataSize size_t indicating the size of the array
         */
        ImageTaggedMessage(uint8_t sequenceNumber, double latitude, double longitude,
                           uint8_t* imageData, size_t dataSize);

        /*!
         * \brief ImageTaggedMessage constructor
         * \param [in] sequenceNumber a byte indicating the current image
         * \param [in] latitude of the current image as a converted int32_t
         * \param [in] longitude of the current image as a converted int32_t
         * \param [in] imageData[] a byte array holding the data of the current image
         * \param [in] dataSize size_t indicating the size of the array
         */
        ImageTaggedMessage(uint8_t sequenceNumber, int32_t latitude, int32_t longitude,
                           uint8_t* imageData, size_t dataSize);

        /*!
         * ~ImageTaggedMessage deconstructor
         */
        ~ImageTaggedMessage();

        /*!
         * \brief ImageTaggedMessage constructor to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        ImageTaggedMessage(const std::vector<uint8_t> &serializedMessage);

        /*!
         * \brief type returns the type of the message as a MeesageId
         * \return The type of the enclosed message as a MeeageId enum value
         */
        MessageID type();

        /*!
         * \brief serialize serializes the message into a unsigned char vector
         * \return A standard unsigned vector containing the message's serialized contents
         */
        std::vector<uint8_t> serialize();

        /*!
         * \brief Returns latitude as int32_t
         * \return latitude
         */
        int32_t raw_lat();

        /*!
         * \brief Returns longitude as int32_t
         * \return longitude
         */
        int32_t raw_lon();

        /*!
         * \brief Converts latitude to double
         * \return latitude as a double
         */
        double lat();

        /*!
         * \brief Returns longitude as double
         * \return longitude as a double
         */
        double lon();

    private:
        /*!
         * \brief latitude of the image
         */
        int32_t latitude;

        /*!
         * \brief longitude of the image
         */
        int32_t longitude;
};

#endif // IMAGE_TAGGED_MESSAGE_HPP
