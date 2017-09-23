#ifndef MOVING_OBSTACLE_H
#define MOVING_OBSTACLE_H

class MovingObstacle : QObject
{
    Q_OBJECT

public:

    MovingObstacle(const latitude, const longitude, const altitudeMsl, const sphereRadius);
    ~MovingObstacle();

    const double getLatitude();
    const double getLongitude();
    const double getAltitudeMsl();
    const double getSphereRadius();

protected:

private:

    double latitude;
    double longitude;
    double altitudeMsl;
    double sphereRadius;
};

#endif // MOVING_OBSTACLE_H
