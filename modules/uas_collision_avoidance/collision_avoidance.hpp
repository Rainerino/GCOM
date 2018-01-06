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

    float longitudeToX(float);
    float latitudeToY(float);

    // Waypoints (points that must be hit)
    QList<InteropMission::Waypoint> missionWaypoints;
};


#endif COLLISION_AVOIDANCE_H
