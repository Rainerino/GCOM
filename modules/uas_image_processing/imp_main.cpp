#include <QObject>


class CAS: public QObject{
    Q_OBJECT
public:
    CAS();
    void stationaryReceiver();
    void dynamicalReceiver();



}
