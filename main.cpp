#include "gcomcontroller.hpp"
#include "mavlink_relay/missionplannertcp.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GcomController w;
    MissionPlannerSocket mTest;
    mTest.run(); //add slot for exit


    w.show();
    //mTest.exit(); //Have a more gracefull exit
    return a.exec();
}
