#include "gcomcontroller.hpp"
#include "modules/mavlink_relay/missionplannertcp.hpp"
#include "../Mavlink/ardupilotmega/ardupilotmega.h"
#include <QApplication>
#include <QtCore>
#include <memory>
#include <QDebug>
#include <windows.h>

#include "modules/mavlink_relay/testslotthread.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GcomController w;



    MissionPlannerSocket relayTest;
    relayTest.setup("127.0.0.1", 14550, 1000);

    // An example of how the signals emitted from the mavlink relay can be used
    // is implemented below. A simple thread myThread has slots to recieve and print
    // data emitted by the MissionPlannerSocket

    myThread testSlotsThread;

    QObject::connect(&relayTest,SIGNAL(mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t>)), // GPS signal emitted from mavlink relay
                     &testSlotsThread,SLOT(mavlinkgpsinfo(std::shared_ptr<mavlink_global_position_int_t>))); // GPS example slot which recievs and prints the GPS data

    QObject::connect(&relayTest,SIGNAL(mavlinkcamerainfo(std::shared_ptr<mavlink_camera_feedback_t>)),    // Camera signal emitted from mavlink relay
                     &testSlotsThread,SLOT(mavlinkcamerainfo(std::shared_ptr<mavlink_camera_feedback_t>))); // Camera example slot which recieves the data
    relayTest.run();
    qDebug() << "This message doesn't exist when exec() is called within the run() function";

    QObject::connect(&w,SIGNAL(testclick()), &relayTest,SLOT(end()));


    w.show();
    return a.exec();

}
