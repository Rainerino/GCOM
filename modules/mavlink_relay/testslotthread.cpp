#include "testslotthread.hpp"
#include <QtCore>
#include <QDebug>
#include <QObject>
#include <Mavlink/common/mavlink.h>
#include "../Mavlink/ardupilotmega/ardupilotmega.h"
#include <memory>

// This Code exists solely for demonstration of how missionplannertcp signals are recieved
// It has no funtionality

myThread::myThread()
{

}

void myThread::run(){
    qDebug() << "running";
}

void myThread::testinput(int test){
    qDebug() << test;
}

void myThread::mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t> gpsSignal){
    qDebug() << gpsSignal->lat << gpsSignal->lon << "GPS Signal" << gpsSignal->time_boot_ms;
}

void myThread::mavlinkcamerainfo(std::shared_ptr<mavlink_camera_feedback_t> cameraSignal){
    qDebug() << cameraSignal->lat << cameraSignal->lng << "CAM Signal" << cameraSignal->time_usec;
}
