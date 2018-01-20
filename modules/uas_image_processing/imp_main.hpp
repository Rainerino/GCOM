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

class IMP: public QObject{
    Q_OBJECT
private:

    QString dirName;



    //output
    struct color
    {
        int r, g, b;
        color(int r, int g, int b) {
            this->b = b;
            this->r = r;
            this->g = g;
        }
    };

    //this is the output object, can be converted to interopOdlc object
    struct object
    {
        //to be completed
        object(double longi, double lat, QString orient, QString shape, QString bg_color, QString alpha, QString alpha_color) {}
    };

    QList<object> objectList;


public:

    IMP();
    void imageSetup();
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
