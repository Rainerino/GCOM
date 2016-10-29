#ifndef MISSIONPLANNERTCP_HPP
#define MISSIONPLANNERTCP_HPP

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QtCore>


class MissionPlannerSocket : public QThread
{
    Q_OBJECT
public:
    void run();
    MissionPlannerSocket();
signals:


public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readBytes();
private:
    QTcpSocket *socket;
};
#endif // MISSIONPLANNERTCP_HPP
