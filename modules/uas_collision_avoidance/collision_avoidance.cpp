#define M_PI = 3.141592653589793238462643;
#define M_PI_4 = (M_PI / 4.0);

// include other modules
#include "collision_avoidance.hpp"
#include "modules/uas_interop_system/InteropObjects/stationary_obstacle.hpp"
#include "modules/uas_interop_system/InteropObjects/interop_mission.hpp"

// include libraries
#include "math.h"

// initialize collision avoidance
CollisionAvoidance::CollisionAvoidance(InteropMission mission) {
    // retrieve mission waypoints from interop
    missionWaypoints = mission.getMissionWaypoints();

    generateWaypoints = {};

    // will also eventually retrieve from interop
    stationaryObstacles = {};
}

CollisionAvoidance::~CollisionAvoidance() {
    // do nothing
}

// methods
void CollisionAvoidance::generateWaypointFile() {
    // using the following format:
    // QGC WPL <VERSION>
    // <INDEX> <CURRENT WP> <COORD FRAME> <COMMAND> <PARAM1> <PARAM2> <PARAM3> <PARAM4> <PARAM5/X/LONGITUDE> <PARAM6/Y/LATITUDE> <PARAM7/Z/ALTITUDE> <AUTOCONTINUE>

}

// determine if there is a collision between two waypoints
// will need to simulate a obstacle for now
bool CollisionAvoidance::collisionDetected(InteropMission::Waypoint waypointA,
                                           InteropMission::Waypoint waypointB,
                                           QList<StationaryObstacle> obstacles) {
    return false;
}

// calculations
float CollisionAvoidance::longitudeToX(float lon) {
    return lon * 6371000.0 * M_PI / 180;
}

float CollisionAvoidance::latitudeToY(float lat) {
    return log(tan(M_PI_4 + (lat * M_PI / 360))) * 6371000.0;
}

double distanceBetweenTwoPoints(double Ax, double Ay, double Bx, double By) {
    return sqrt(pow((Ax - Bx),2) + pow((Ay - By),2));
}






// def is_between(a,c,b):
//    return distance(a,c) + distance(c,b) == distance(a,b)














