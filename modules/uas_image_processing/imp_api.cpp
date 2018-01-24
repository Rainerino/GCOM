#ifndef IMP_API_HPP
#define IMP_API_HPP
#include "imp_api.hpp"

/**
vector<String> imageDir;
String coordinateFilePath;
Coordinate coordinate;
Mat inputImage, outputImage;
 //Input
std::vector<goose>
//Outtput
std::vector<matingPair>
struct goose {
  int species;
  double xcoord;
  double ycoord;
  std::string filepath;
} ;
struct nest {
    double xcoord;
    double ycoord;
};
struct matingPair{
    goose maleGoose;
    goose femaleGoose;
    double distance;
    nest nestCentroid;
};
 */

IMP_API::IMP_API(){
    goose = new CanadaGoose();
}

void IMP_API::setup(QString inputPath, QString outputPath ){
    qInfo() << "SET UP, Display file list";
    imagePath.clear();
    imageDir = inputPath;
    QDirIterator it(inputPath, QDir::Files, QDirIterator::NoIteratorFlags);
    while(it.hasNext()){
        it.next();
        imagePath.push_back(it.filePath());
        //qInfo() << it.filePath();
    }
    savePath = outputPath;
    goose->setSavePath(outputPath);
    //inputimage is filled, and path is stored



}

bool IMP_API::start(){
    qInfo() << "START!";
    for (unsigned int i = 0; i < imagePath.size(); i ++){
        //take a string and then store the file in a different path
        //qInfo() << imagePath[i];
        goose->cascadeRecognition(QString::number(i).append(".jpg"), imagePath[i]);
        Goose_Data* data = new Goose_Data();
        data->setData(goose->getContour(), goose->getColor(), 0, 0,goose->getSavePath(), 0, goose->getOutputImage(), imagePath[i]);
        gooseSwap.push_back(data);
        //get the filepath
        //calculate the coord   F
        qInfo() << "Processed";
        //qInfo() << data->toString();

    }

    //get image fucntion implementation in Goose
    //return?
    qInfo() << "FINISH!";
    return true;
}

void IMP_API::stop(){
}


void IMP_API::computeCoordinates(QString textfilePath){

    //ASJNDuwaubdiawbdiubwa
}

QString IMP_API::getProcessedImageDir(){

    return savePath;
    //return "";
}
IMP_API::Goose IMP_API::setGooseData(int species, double xcoord, double ycoord, string filePath){
    Goose single;
    single.filePath = filePath;
    single.species = species;
    single.xcoord = xcoord;
    single.ycoord = ycoord;

    gooseReturn.push_back(single);
    return single;
}

vector<Goose_Data*> IMP_API::getGooseSwapData(){
    return gooseSwap;
}
void IMP_API::setGooseSwapData(vector<Goose_Data*> gooseSwap){
    this->gooseSwap.clear();
    for(int i = 0; i < gooseSwap.size(); i ++){
        this->gooseSwap.push_back(gooseSwap[i]);
    }
}
void IMP_API::setData(QString txtPath, vector<vector<QRect>> contour, QString imageDir){
    this->txtPath = txtPath;
    this->contour = contour;
    this->imageDir = imageDir;
}

void IMP_API::calculatePoints(){
    int counter = 0;
   QDirIterator it(imageDir, QDir::Files, QDirIterator::NoIteratorFlags);
   filePaths.clear();
    while(it.hasNext()){
        it.next();
        filePaths.push_back(it.filePath());
        counter ++;
    }
    rawData.resize(counter);
    points.resize(counter);


    QFile file(txtPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "No FILE!";
        return;
    }
    QTextStream in(&file);
    for (int j = 0; !in.atEnd(); j ++) {
       QString line = in.readLine();
       QStringList list = line.split(QRegExp("\t|\s"),QString::SkipEmptyParts);
       if(!list.empty()) {
           for(int i = 0; i < list.size(); i++){
                QString lat("48.5");
                QString longi("71.6");
                QString zero("0.0");
                list[i].replace(list[i].indexOf(longi), longi.size(), zero);
                list[i].replace(list[i].indexOf(lat), lat.size(), zero);

                rawData[j].push_back((list[i]).toDouble());
           }

       }
       list.clear();
    }
    file.close();
    qInfo() << "list all good, fixlater" ;

    setprecision(10);
    for( int k = 0 ; k < filePaths.size(); k ++){

        QImageReader reader(filePaths[k]);
        reader.setAutoTransform(true);
        QImage newImage = reader.read();
        QSize size(newImage.width(), newImage.height());
        QPointF distance(rawData[k][3] * qTan(qreal(DEGREE_FOV_H)), rawData[k][3] * qTan(qreal(DEGREE_FOV_V)));
        if(!contour.empty()){
            for(int m = 0; m < contour[k].size(); m ++){
                   points[k].push_back(contour[k][m].center());
            }

            for(int i = 0 ; i < points[k].size(); i ++){
                    double tempx = (double)points[k][i].x() ;
                    tempx -= size.width()/2;
                    tempx *= distance.x() * 1.0 / size.width();
                    tempx /= 2 * M_PI * EARTH_RADIUS;
                    tempx += rawData[k][0];
                    double tempy = points[k][i].y();
                    tempy -= size.height()/2;
                    tempy *= distance.y() * 1.0 / size.height();
                    tempy /= 2 * M_PI * EARTH_RADIUS;
                    tempy += rawData[k][1];

                    points[k][i].setX(tempx);
                    points[k][i].setY(tempy);
                    /**
                    .setX(points[i][j].x() - size.width());
                    points[i][j].setX(points[i][j].x() * distance.x() / size.width());
                    points[i][j].setX(points[i][j].x() / (2 * M_PI * EARTH_RADIUS));
                    points[i][j].setX(points[i][j].x() + rawData[i][2]);
                    qInfo() << points[i][j];
                    points[i][j].setY(points[i][j].y() - size.height());
                    points[i][j].setY(points[i][j].y() * distance.y() / size.height());
                    points[i][j].setY(points[i][j].y() / (2 * M_PI * EARTH_RADIUS));
                    points[i][j].setY(points[i][j].y() + rawData[i][1]);
                    qInfo() << points[i][j];
                    */
            }
        }else{
            qDebug() <<"change erro!";
        }

    }

    qInfo() << "Step2 done!";
}

void IMP_API::deletePairs(){
    vector<vector<QPointF>> matingPair;
    for(int i = 0 ; i < points.size(); i ++ ){
        for(int j = 0 ; j < points[i].size() ; j ++){

        }
    }
}

vector<vector<QPointF>> IMP_API::getGPS(){
    return points;
}

void IMP_API::saveData2Text(){
    QFile textOutput(TEXT_FILE_PATH);
    if(textOutput.open(QIODevice::ReadWrite|QIODevice::Truncate | QIODevice::Text)){
        QTextStream stream(&textOutput);
        for (int i = 0 ; i < points.size(); i ++) {
            for (int j = 0 ; j < points[i].size(); j ++){
                stream << QString::number(points[i][j].x() + 48.5, 'g', 8 )<< "," << QString::number(points[i][j].y() -71.6, 'g', 8 )<< "," <<   endl;
            }
        }
    textOutput.close();
    }
}
QString IMP_API::getImageDir(){
    return imageDir;
}


#endif
