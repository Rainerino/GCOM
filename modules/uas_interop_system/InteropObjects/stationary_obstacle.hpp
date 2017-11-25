#ifndef STATIONARY_OBSTACLE_H
#define STATIONARY_OBSTACLE_H

class StationaryObstacle
{

public:

    StationaryObstacle(const double latitude, const double longitude, const double cylinderHeight, const double cylinderRadius);
    ~StationaryObstacle();

    double getLatitude();
    double getLongitude();
    double getCylinderHeight();
    double getCylinderRadius();

private:

    double latitude;
    double longitude;
    double cylinderHeight;
    double cylinderRadius;

};

#endif // STATIONARY_OBSTACLE_H
