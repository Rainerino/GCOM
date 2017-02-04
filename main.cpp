#include "gcom_controller.hpp"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GcomController w;
    w.show();

    return a.exec();
}
