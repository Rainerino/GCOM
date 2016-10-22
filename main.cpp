#include "gcomcontroller.hpp"
#include "missionplannertcp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GcomController w;
    MissionPlannerSocket mTest;
    mTest.Test();
    w.show();

    return a.exec();
}
