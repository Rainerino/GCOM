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
class impApi : public QObject{
    Q_OBJECT
 public:

    /**
     * @brief impApi
     * This constructor just initializa everything.
     */
    impApi();

    /**
     * @brief setup
     * @param inputPath
     * Set up the reading path of the files.
     *
     */
    void setupImpApi(QString dirName, QString jsonReadingFile, QString jsonStoringFile, QStringList arguments);

    /**
     * @brief OneimageProcessing
     * his will take one image and process it with out algorithm, enable us real time processing and uploading
     * @return json file list of the output
     */
    void OneimageProcessing(QString fileName);

    /**
     * @brief updateImpObjectList
     * This function takes new object list and update the list by comparing data between them.
     * It will call the overlap as its helper.
     */
    void updateImpObjectList( QString jsonStoringFile);

    /**
     * @brief getNewFileList
     * This is go to the predefined dir location and get the new files that have been added since.
     * when on auto mode, this should be able to update one images at an time. Since we waant to compare
     * images that are taken at a short interval, outputing a list is more ideal.
     * @return new files list in QString
     */
    QStringList getNewFileList(QString dirName);


    /**
     * @brief objectOverLapCheck
     * check the objects 1 by 1 to make sure there is no oeverlap. Modify the file if there is
     * @return nothing
     */
    void objectOverLapCheck(QList<QString>);


private:

    //this is the reading directary
    QString dirName;

    //This is the file that will contains the processed files
    QString jsonStoringFile;

    //This is the filename that will contains the newly processed files
    QString jsonReadingFile;

    //this contains the processed files.
    QStringList processedFileList;

    QProcess imageProcess;

    // THis will store all the pending images to be processed
    //Queue<STUFF>

    // TODO
    // SLOT THAT TAKES IN INPUT FROM IGORS MODULE
    // THis will perform the following, 1. it will check if process is running, if it is then add the
    // data received to the queue, if the proces was not started then strat the process with the data and dot add to queue



private slots:
    // TODO If queue is not emty restart the proces again on the first item from the queue
    /**
     * @brief imageProcessDone
     * @param exitCode
     * @param exitStatus
     */
    void imageProcessDone(int exitCode, QProcess::ExitStatus exitStatus);


};

#endif
