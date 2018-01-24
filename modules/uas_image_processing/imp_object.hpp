#ifndef IMP_OBJECCT_H
#define IMP_OBJECCT_H


#include <QObject>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QImage>

#include "modules/uas_interop_system/InteropObjects/interop_odlc.hpp"
/**
 * @brief The imp_object class
 * This is an image processing object, the object that is aimed to where is inputs are loaded to
 * and where the output are produced. The main input call is the constructor with nothign
 * Keep using setter to set each elements and use.
 *
 * The image object each should be a valid result
 */
class imp_object : public QObject{
    Q_OBJECT
public:
    imp_object();
    //set functions
    //double longi, double lat, QString orient, QString shape,
    // QString bg_color, QString alpha, QString alpha_color);
    void setLong();
    void setLai();
    void setOrientation();
    void setShape();
    void setBgColor();
    void setAlpha();
    void setAlphaColor();

    void setFilePath();

    InteropOdlc returnInteropOdlc();

    //debugging purpose
    QString toString();
    //getters are optional since what we only care about are the interopOdlc

private:
    QString filePath;
    QImage objectImg;

    //what we need to get the absolute geo
    double fileLatitude;
    double fileLongitude;
    double fileAltitude;

    //the cropped image's location
    int x, y, w, h;

    //object members
    double objLongitude;
    double objLatitude;
    QString orient;
    QString shape;
    QString bg_color;

};


#endif // IMP_OBJECCT_H
