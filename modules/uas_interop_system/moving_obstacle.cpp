#include "moving_obstacle.hpp"

MovingObstacle::MovingObstacle(const latitude, const longitude, const altitudeMsl, const sphereRadius)
{
    this->latitude = latitude;
    this->longitude = longitude;
    this->altitudeMsl = altitudeMsl;
    this->sphereRadius = sphereRadius;
}

MovingObstacle::~MovingObstacle()
{
    // do nothing
}

MovingObstacle::getLatitude()
{
    return this->latitude;
}

MovingObstacle::getLongitude()
{
    return this->longitude;
}

MovingObstacle::getAltitudeMsl()
{
    return this->altitudeMsl;
}

MovingObstacle::getSphereRadius()
{
    return this->sphereRadius;
}
