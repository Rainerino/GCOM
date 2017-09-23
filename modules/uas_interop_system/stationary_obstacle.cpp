#include "stationary_obstacle.hpp"

StationaryObstacle::StationaryObstacle(const latitude, const longitude, const cylinderHeight, const cylinderRadius)
{
    this->latitude = latitude;
    this->longitude = longitude;
    this->cylinderHeight = cylinderHeight;
    this->cylinderRadius = cylinderRadius;
}

StationaryObstacle::~StationaryObstacle()
{
    // do nothing
}

StationaryObstacle::getLatitude()
{
    return this->latitude;
}

StationaryObstacle::getLongitude()
{
    return this->longitude;
}

StationaryObstacle::cylinderHeight()
{
    return this->cylinderHeight;
}

StationaryObstacle::cylinderRadius()
{
    return this->cylinderRadius;
}
