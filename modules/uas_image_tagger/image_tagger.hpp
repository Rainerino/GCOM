#ifndef IMAGETAGGER_HPP
#define IMAGETAGGER_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QObject>
#include <assert.h>
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
     * \param dir QString path of directory
     * \param DCNC pointer to constant data indicating sender of signal
     * \param MAVLink pointer to const data indicating if image is to be tagged or not
     */
    ImageTagger(QString dir, const DCNC *sender, const MAVLinkRelay *toBeTagged);

    /*!
     *  \brief ImageTagger deconstructor
     */
    ~ImageTagger();

    /*!
     * \brief setupDirectoryPath helper function to setup path name
     * \param dir QString path of directory
     * \param createDuplicates int 0 or 1 if duplicate folder is to be created
     */
    void setupDirectoryPath(QString dir, int createDuplicates);

    /*!
     * \brief saveImageToDisc helper function that does the saving
     * \param pathName QString path of directory with filename
     * \param data unsigned char pointer to image data
     */
    void saveImageToDisc(QString pathName, unsigned char *data);

    /*!
     * \brief tagImage write to image metadata the GPS information
     * \param pathName QString path to location of image file
     */
    void tagImage(QString pathName);
signals:
    // Data Signals
    void taggedImage(QString pathName);
private slots:
    /*!
     * \brief handleImageMessage saves image to disc and sends a signal with
     *        the tagged image's file name
     */
    void handleImageMessage(std::shared_ptr<ImageTaggerMessage> message);

    /*!
     * \brief handleMavlinkRelay copies camera feedback info to private pointer gpsData
     */
    void handleMavlinkRelay(std::shared_ptr<mavlink_camera_feedback_t> cameraInfo);
private:
    QString pathOfDir;
    QString pathOfDuplicates;
    int numOfImages;
    int numOfDuplicates;
    int gpsDataAvailable;
    std::vector<unsigned char> seqNumArr;
    mavlink_camera_feedback_t *gpsData;
};

#endif // IMAGETAGGER_HPP
