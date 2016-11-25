#include "gcom_controller.hpp"
#include "ui_gcomcontroller.h"

GcomController::GcomController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GcomController)
{
    ui->setupUi(this);
}

GcomController::~GcomController()
{
    delete ui;
}
