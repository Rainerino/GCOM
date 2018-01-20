//===================================================================
// Includes
//===================================================================
// System Includes
#include <Qfile>
#include <QTextStream>
#include "math.h"

// GCOM Includes
#include "collision_avoidance.hpp"
#include "modules/uas_interop_system/InteropObjects/stationary_obstacle.hpp"
#include "modules/uas_interop_system/InteropObjects/interop_mission.hpp"
#include "modules/uas_collision_avoidance/collision_avoidance.hpp";

//#define M_PI = 3.141592653589793238462643;
//#define M_PI_4 = (M_PI / 4.0);

// Strings
const QString QGC_VERISON = "QGC WPL 110";

// Mission Planner Commands
const quint8 WAYPOINT = 16;
const quint8 SPLINE_WAYPOINT = 82;
const quint8 LOITER_TURNS = 18;
const quint8 LOITER_TIME = 19;
const quint8 LOITER_UNLIM = 17;
const quint8 RETURN_TO_LAUNCH = 20;
const quint8 LAND = 21;
const quint8 TAKEOFF = 22;
const quint8 DELAY = 93;
const quint8 GUIDED_ENABLE = 92;
const quint8 PAYLOAD_PLACE = 94;
const quint8 DO_CHANGE_SPEED = 178;

//===================================================================
// Constructor / Deconstructor
//===================================================================

CollisionAvoidance::CollisionAvoidance() {
    // retrieve mission waypoints from interop
    // missionWaypoints = mission.getMissionWaypoints();

    // will also eventually retrieve from interop
    stationaryObstacles = {};
}

CollisionAvoidance::~CollisionAvoidance() {
    // do nothing
}

//===================================================================
// Methods
//===================================================================

void CollisionAvoidance::generateWaypointFile() {
    // waypoint file format:
    // QGC WPL <VERSION>
    // <INDEX> <CURRENT WP> <COORD FRAME> <COMMAND> <PARAM1> <PARAM2> <PARAM3> <PARAM4> <PARAM5/X/LONGITUDE> <PARAM6/Y/LATITUDE> <PARAM7/Z/ALTITUDE> <AUTOCONTINUE>

    QString filename = "missionplanwaypoints.waypoints";
    QFile file(filename);

    if(file.open(QFile::ReadWrite)) {
        QTextStream stream(&file);
        stream << QGC_VERISON << endl;
        stream << "0	1	0	0	0	0	0	0	0	0	0	1" << endl;
        stream << "1	0	3	16	0.000000	0.000000	0.000000	0.000000	49.037868	-81.562500	100.000000	1" << endl;
    }
}

bool CollisionAvoidance::collisionDetectedBetweenTwoWaypoints(InteropMission::Waypoint waypointA,
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














