#ifndef IMAGETAGGER_HPP
#define IMAGETAGGER_HPP

//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QObject>
#include <QQueue>
#include <assert.h>
// GCOM Includes
#include "modules/uas_dcnc/dcnc.hpp"
#include "modules/uas_message/image_tagger_message.hpp"

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
     */
    void setupDirectoryPath(QString dir);

    /*!
     * \brief saveImageToDisc helper function that does the saving
     * \param filePath QString path of directory with filename
     * \param data unsigned char pointer to image data
     * \param size size_t the number of bytes to write to the file
     */
    void saveImageToDisc(QString filePath, unsigned char *data, size_t size);

    /*!
     * \brief tagImage write to image metadata the GPS information
     * \param filePath QString path to location of image file
     * \param *tags mavlink_camera_feedback_t pointer to the EXIF tags
     */
    void tagImage(QString filePath, mavlink_camera_feedback_t *tags);

    /*!
     * \brief tagImage function overload for alternative to tagging
     * \param filePath QString path to location of image file
     * \param tags QStringList list of EXIF tags
     */
    void tagImage(QString filePath, QStringList tags);

    /*!
     * \brief tagAllImages goes through all images and GPS data and
     *        tags them based on the EXIF tags (alternative tagging)
     */
    void tagAllImages();

    /*!
     * \brief getPathOfExifTags returns path to text file of EXIF tags
     */
    QString getPathOfExifTags();
signals:
    // Data Signals
    void taggedImage(QString filePath);
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
    QQueue<mavlink_camera_feedback_t *> gpsData;
};

#endif // IMAGETAGGER_HPP
