#include "gcomcontroller.hpp"
#include "modules/mavlink_relay/missionplannertcp.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GcomController w;

    MissionPlannerSocket mTest;
    mTest.setup("127.0.0.1", 14550, 1000);
    mTest.run();


    w.show();

    return a.exec();
}
