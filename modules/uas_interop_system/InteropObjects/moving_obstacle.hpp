#ifndef MOVING_OBSTACLE_H
#define MOVING_OBSTACLE_H

class MovingObstacle
{

public:

    MovingObstacle(const double latitude, const double longitude, const double altitudeMsl, const double sphereRadius);
    ~MovingObstacle();

    double getLatitude();
    double getLongitude();
    double getAltitudeMsl();
    double getSphereRadius();

private:

    double latitude;
    double longitude;
    double altitudeMsl;
    double sphereRadius;
};

#endif // MOVING_OBSTACLE_H
