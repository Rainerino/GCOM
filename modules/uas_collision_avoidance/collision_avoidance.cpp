#define M_PI = 3.141592653589793238462643;
#define M_PI_4 = (M_PI / 4.0);

#include "collision_avoidance.hpp"
#include "modules/uas_interop_system/InteropObjects/stationary_obstacle.hpp"
#include "modules/uas_interop_system/InteropObjects/interop_mission.hpp"
#include "math.h"

CollisionAvoidance::CollisionAvoidance(InteropMission mission) {
    missionWaypoints = mission.getMissionWaypoints();
}

CollisionAvoidance::~CollisionAvoidance() {

}

float CollisionAvoidance::longitudeToX(float lon) {
    return lon * 6371000.0 * M_PI / 180;
}

float CollisionAvoidance::latitudeToY(float lat) {
    return log(tan(M_PI_4 + (lat * M_PI / 360))) * 6371000.0;
}
