#include "gcom_controller.hpp"
#include "ui_gcomcontroller.h"
#include <QDebug>

GcomController::GcomController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GcomController)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(released()),
            this,SLOT(clicked()));
}
GcomController::~GcomController()
{
    delete ui;
}

void GcomController::clicked(){
   emit testclick();
}
