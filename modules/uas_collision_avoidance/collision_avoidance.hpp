#ifndef COLLISION_AVOIDANCE_H
#define COLLISION_AVOIDANCE_H

#include "modules/uas_interop_system/InteropObjects/stationary_obstacle.hpp"
#include "modules/uas_interop_system/InteropObjects/interop_mission.hpp"

class CollisionAvoidance
{
public:
    CollisionAvoidance(InteropMission);
    ~CollisionAvoidance();

private:

    // ----PROPERTIES

    float longitudeToX(float);
    float latitudeToY(float);

    // Waypoints (points that must be hit)
    QList<InteropMission::Waypoint> missionWaypoints;

    // Waypoints used to generate actual flight path
    QVector<InteropMission::Waypoint> generateWaypoints;

    // Stationary obstacles
    QList<StationaryObstacle> stationaryObstacles;

    // ----METHODS

    // Collision detection
    bool collisionDetected(InteropMission::Waypoint waypointA,
                                               InteropMission::Waypoint waypointB,
                                               QList<StationaryObstacle> obstacles);

    void generateWaypointFile();
};


#endif COLLISION_AVOIDANCE_H
