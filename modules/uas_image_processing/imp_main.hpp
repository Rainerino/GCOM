/*
 * This module should create an control interface and caller for the imp
 * assume the python script works, and this module should be able to wrap it up.
 *
 * input: gps locations(long, lat), altitude, filename 
 * 
 * outptut: odlc
 * latitude, longitude, orientation, shape, background_color, alpha, alpha colar
 */
#include <QObject>
#include <QString>
#include <QList>
#include "imp_object.hpp"


class IMP_Main: public QObject{
    Q_OBJECT
private:

    QString dirName;
    //this is the output object, can be converted to interopOdlc object

    QList<imp_object> objectList;

    QList<InteropOdlc> returnedInteropOdlcList;
public:

    IMP_Main();
    //set up the file location and such
    void imageSetup();
    //whenever there is a new image, process it.
    void impTrigger();

    //assume it will be a matrix of filename
    //with each image processed
    void preprocess();

    //take 64 * 64 images and tag it
    void shapeRecongition();
    void ocr();
    void orientation();

    void geoTagImages();
    void contrustObject();

};
