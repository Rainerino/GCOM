#ifndef STATIONARY_OBSTACLE_H
#define STATIONARY_OBSTACLE_H

class StationaryObstacle : QObject
{
    Q_OBJECT

public:

    StationaryObstacle(const latitude, const longitude, const cylinderHeight, const cylinderRadius);
    ~StationaryObstacle();

    const double getLatitude();
    const double getLongitude();
    const double getCylinderHeight();
    const double getCylinderRadius();

protected:

private:

    double latitude;
    double longitude;
    double cylinderHeight;
    double cylinderRadius;

};

#endif // STATIONARY_OBSTACLE_H
