#ifndef IMAGETAGGER_HPP
#define IMAGETAGGER_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QObject>
// GCOM Includes
#include "modules/uas_dcnc/dcnc.hpp"
#include "modules/uas_message/image_tagger_message.hpp"
#include "modules/mavlink_relay/mavlink_relay_tcp.hpp"

//===================================================================
// Public Class Declaration
//===================================================================
/*!
 * \brief The ImageTagger class is designed to tag images and save them to disk
 * \details Current implementation receives signals and saves images to disk
 */
class ImageTagger : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief ImageTagger constructor that takes in the directory name
     * \param dir Qstring path of directory
     * \param DCNC pointer to constant data indicating sender of signal
     * \param MAVLink pointer to const data indicating if image is to be tagged or not
     */
    ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged);

    /*!
     *  \brief ImageTagger deconstructor
     */
    ~ImageTagger();
signals:
    // Data Signals
    void taggedImage(QString pathName);
private slots:
    /*!
     * \brief handleImageMessage saves image to disk and sends a signal with
     *        the tagged image's file name
     */
    void handleImageMessage(std::unique_ptr<ImageTaggerMessage> message);
private:
    QString directory;
    std::vector<unsigned char> seqNumArr;
};

#endif // IMAGETAGGER_HPP
