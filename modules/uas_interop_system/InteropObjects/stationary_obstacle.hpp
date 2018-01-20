#ifndef STATIONARY_OBSTACLE_H
#define STATIONARY_OBSTACLE_H

class StationaryObstacle
{

public:

    StationaryObstacle(const double cylinderHeight, const double cylinderRadius, const double latitude, const double longitude);
    ~StationaryObstacle();

    double getCylinderHeight();
    double getCylinderRadius();
    double getLatitude();
    double getLongitude();

private:

    double cylinderHeight;
    double cylinderRadius;
    double latitude;
    double longitude;

};

#endif // STATIONARY_OBSTACLE_H
