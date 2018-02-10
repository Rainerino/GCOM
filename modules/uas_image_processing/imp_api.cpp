
#include "modules/uas_image_processing/imp_api.hpp"
/**
 * @brief impApi
 * This constructor just initializa everything.
 */
impApi::impApi(){

}

/**
 * @brief setup
 * @param inputPath
 * Set up the reading path of the files.
 *
 */
void impApi::setupImpApi(QString dirName, QString jsonReadingFile, QString jsonStoringFile, QStringList arguments){

}

/**
 * @brief OneimageProcessing
 * his will take one image and process it with out algorithm, enable us real time processing and uploading
 * @return json file list of the output
 */
void impApi::OneimageProcessing(QString fileName){

}

/**
 * @brief updateImpObjectList
 * This function takes new object list and update the list by comparing data between them.
 * It will call the overlap as its helper.
 */
void impApi::updateImpObjectList( QString jsonStoringFile){

}

/**
 * @brief getNewFileList
 * This is go to the predefined dir location and get the new files that have been added since.
 * when on auto mode, this should be able to update one images at an time. Since we waant to compare
 * images that are taken at a short interval, outputing a list is more ideal.
 * @return new files list in QString
 */
QStringList impApi::getNewFileList(QString dirName){

}


/**
 * @brief objectOverLapCheck
 * check the objects 1 by 1 to make sure there is no oeverlap. Modify the file if there is
 * @return nothing
 */
void impApi::objectOverLapCheck(QList<QString>){

}


void impApi::imageProcessDone(int exitCode, QProcess::ExitStatus exitStatus){

}
