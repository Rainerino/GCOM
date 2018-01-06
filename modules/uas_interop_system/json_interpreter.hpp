#ifndef JSON_INTERPRETER_H
#define JSON_INTERPRETER_H

#include<QList>
#include<QJsonDocument>

#include"InteropObjects/moving_obstacle.hpp"
#include"InteropObjects/stationary_obstacle.hpp"

class JsonInterpreter
{

public:

    JsonInterpreter();
    ~JsonInterpreter();

    struct ObstacleSet
    {
        QList<MovingObstacle*> movingObstacles;
        QList<StationaryObstacle*> stationaryObstacles;
    };

    ObstacleSet* parseObstacles(QJsonDocument json);


private:
};

#endif // JSON_INTERPRETER_H
