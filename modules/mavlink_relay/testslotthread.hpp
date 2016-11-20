#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QtCore>
#include <QObject>
#include <Mavlink/common/mavlink.h>
#include "../Mavlink/ardupilotmega/ardupilotmega.h"
#include <memory>


class myThread: public QThread
{
    Q_OBJECT
public:
    myThread();
    void run();
public slots:
    void testinput(int);
    void mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t> gpsSignal);
    void mavlinkcamerainfo(std::shared_ptr<mavlink_camera_feedback_t> cameraSignal);
};

#endif // MYTHREAD_H
