#ifndef IMAGETAGGER_HPP
#define IMAGETAGGER_HPP

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
class ImageTagger: public UASMessage {
    public:
        // Public Member Methods
        /*!
         * \brief ImageTagger constructor
         * \param [in] seqNum a byte indicating the current image
         * \param [in] imageData[] a byte array holding the data of the current image
         */
        ImageTagger(unsigned char seqNum, unsigned char imageData[]);

        /*!
         * ~ImageTagger deconstructor
         */
        ~ImageTagger();

        /*!
         * \brief ImageTagger constructor to initialize a message using a serialized payload
         * \param [in] serializedMessage a byte vector containing the object's serialized contents
         */
        ImageTagger(const std::vector<unsigned char> &serializedMessage);

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
        // Private Member Variables
        unsigned char seqNum;
        std::vector<unsigned char> imageData;
};

#endif // IMAGETAGGER_HPP
