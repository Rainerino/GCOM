#include "gcomcontroller.hpp"
#include "missionplannertcp.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GcomController w;
    MissionPlannerSocket mTest;
    mTest.run();
    w.show();


    return a.exec();
}
