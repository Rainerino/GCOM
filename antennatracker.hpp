#ifndef ANTENNATRACKER_H
#define ANTENNATRACKER_H

/*
 * currently runs off main wind
 * SHOULD BE CHANGED TO RUN OFF A THREAD LATER ON!!!!
 */
#include <QMainWindow>

namespace Ui {
class AtennaTracker;
}

class AntennaTracker : public QMainWindow
{
    Q_OBJECT

public:
    explicit AntennaTracker(QWidget *parent = 0);
    ~AntennaTracker();

private slots:
    void receiveHandler();

private:
    Ui::AntennaTracker *ui;
};

#endif // ANTENNATRACKER_H
