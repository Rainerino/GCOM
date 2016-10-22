#ifndef MISSIONPLANNERTCP_H
#define MISSIONPLANNERTCP_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>


class MissionPlannerSocket : public QObject
{
    Q_OBJECT
public:
    explicit MissionPlannerSocket(QObject *parent = 0);
    void Test();

signals:


public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readBytes();
private:
    QTcpSocket *socket;
};
#endif // MISSIONPLANNERTCP_H
