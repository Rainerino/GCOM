#ifndef IMP_API_HPP
#define IMP_API_HPP

#include <QObject>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QSize>
#include <QTextStream>
#include <QImageReader>
#include <QImage>
#include <QtCore/qmath.h>
#include <math.h>
#include <iomanip>

#include "imp_main.hpp"

using namespace std;

/**
 * @brief The IMP_API class
 * This is the Qt wrapper class for imp_main.
 * The input for the wrapper is a direcctory Name with the images and a text file of each geo reference
 * TODO: or whatever the input is formatted to.
 *
 * The main output is Qlist<InteropOdlc>
 *
 * sercondary monitorary ouputs
 * The output is an UI, the goal of UI is be able to monitor the image processing results, with each of the i
 * images and its tags displayed, ,and at the same time, delete objects that are observed to be incorrect
 */
class IMP_API : public QObject{
    Q_OBJECT
 public:
    IMP_API();

    void setup(QString inputPath);
    bool start();
    void stop();

    /**
     * @brief OneimageProcessing
     * his will take one image and process it with out algorithm, enable us real time processing and uploading
     * @return list of imp_object
     */
    QList<imp_object> OneimageProcessing(QString fileName);

    /**
     * @brief updateImpObjectList
     * This function takes new object list and update the list by comparing data between them.
     * It will call the overlap as its helper
     */
    void updateImpObjectList( QList<imp_object>);
    /**
     * @brief getNewFileList
     * This is go to the predefined dir location and get the new files that have been added since.
     * when on auto mode, this should be able to update one images at an time. Since we waant to compare
     * images that are taken at a short interval, outputing a list is more ideal.
     * @return new files list in QString
     */
    QList<QString> getNewFileList();




private:
    QList<imp_object> objectList;

    QString dirName;

    //for coordinates calculations
    const double EARTH_RADIUS = 6356.7523;
    const double DEGREE_FOV_H = 80.5;
    const double DEGREE_FOV_V = 60.3;


    //helper internel functions
    /**
     * @brief objectOverLapCheck
     * check the objects 1 by 1 to make sure there is no oeverlap.
     * @return no overlapped objects
     */
    QList<imp_object> objectOverLapCheck(QList<imp_object>);



};

#endif
