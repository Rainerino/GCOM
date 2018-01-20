#ifndef JSON_INTERPRETER_H
#define JSON_INTERPRETER_H

#include <QList>
#include <QJsonDocument>

#include "InteropObjects/interop_mission.hpp"
#include "InteropObjects/interop_telemetry.hpp"
#include "InteropObjects/moving_obstacle.hpp"
#include "InteropObjects/stationary_obstacle.hpp"
#include "InteropObjects/interop_odlc.hpp"

class InteropJsonInterpreter
{

public:

    InteropJsonInterpreter();
    ~InteropJsonInterpreter();

    struct ObstacleSet
    {
        QList<MovingObstacle*> movingObstacles;
        QList<StationaryObstacle*> stationaryObstacles;
    };

    InteropMission* parseSingleMission(QJsonDocument json);
    QList<InteropMission*> parseMultipleMissions(QJsonDocument json);
    InteropTelemetry* parseTelemetry(QJsonDocument json);
    ObstacleSet* parseObstacles(QJsonDocument json);
    InteropOdlc* parseInteropOdlc(QJsonDocument json);
    QJsonDocument encodeInteropOdlc(InteropOdlc odlc);


private:
    InteropMission* parseMission(QJsonObject obj);

};

#endif // JSON_INTERPRETER_H
