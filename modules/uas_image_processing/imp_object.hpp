#ifndef IMP_OBJECCT_H
#define IMP_OBJECCT_H


#include <QObject>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QImage>

/**
 * @brief The imp_object class
 * This is an image processing object, the object that is aimed to be
 */
class imp_object : public QObject{
    Q_OBJECT
public:
    imp_object();
    //set functions
    void setData(double longi, double lat, QString orient, QString shape, QString bg_color, QString alpha, QString alpha_color);
    void clearData();

    //getters
    QString toString();

    double getlat();
    double getlong();
    void setFilePath();
    QString getFilePath();
    Mat getImage();
    double getalt();

private:
    QString filePath;
    QImage objectImg;
    double fileLatitude;
    double fileLongitude;
    double fileAltitude;

    //object members

    double objLongitude;
    double objLatitude;
    QString orient;
    QString shape;
    QString bg_color;
};


#endif // IMP_OBJECCT_H
