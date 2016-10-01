#ifndef GCOMCONTROLLER_HPP
#define GCOMCONTROLLER_HPP

#include <QMainWindow>

namespace Ui {
class GcomController;
}

class GcomController : public QMainWindow
{
    Q_OBJECT

public:
    explicit GcomController(QWidget *parent = 0);
    ~GcomController();

private:
    Ui::GcomController *ui;
};

#endif // GCOMCONTROLLER_HPP
