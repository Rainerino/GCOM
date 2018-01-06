#include "json_interpreter.hpp"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

JsonInterpreter::JsonInterpreter()
{
    //do nothing
}

JsonInterpreter::~JsonInterpreter()
{
    //do nothing
}

JsonInterpreter::ObstacleSet* JsonInterpreter::parseObstacles(QJsonDocument jsonDoc)
{
    QJsonObject jsonObj = jsonDoc.object();

    QJsonArray movingObstArr = jsonObj["moving_obstacles"].toArray();
    QJsonArray stationaryObstArr = jsonObj["stationary_obstacles"].toArray();

    QList<MovingObstacle*> movingObstList;
    foreach(const QJsonValue &value, movingObstArr)
    {
        QJsonObject obj = value.toObject();

        double altitudeMsl = obj["altitude_msl"].toDouble();
        double latitude = obj["latitude"].toDouble();
        double longitude = obj["longitude"].toDouble();
        double sphereRadius = obj["sphere_radius"].toDouble();

        movingObstList.append(new MovingObstacle(altitudeMsl, latitude, longitude, sphereRadius));
    }

    QList<StationaryObstacle*> stationaryObstList;
    foreach(const QJsonValue &value, stationaryObstArr)
    {
        QJsonObject obj = value.toObject();

        double cylinderHeight = obj["cylinder_height"].toDouble();
        double cylinderRadius = obj["cylinder_radius"].toDouble();
        double latitude = obj["latitude"].toDouble();
        double longitude = obj["longitude"].toDouble();

        stationaryObstList.append(new StationaryObstacle(cylinderHeight, cylinderRadius, latitude, longitude));
    }

    return new JsonInterpreter::ObstacleSet{movingObstList, stationaryObstList};
}
