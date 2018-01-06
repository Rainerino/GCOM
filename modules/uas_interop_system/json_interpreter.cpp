#include "json_interpreter.hpp"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

JsonInterpreter::JsonInterpreter()
{
    //do nothing
}

JsonInterpreter::~JsonInterpreter()
{
    //do nothing
}

InteropMission* JsonInterpreter::parseSingleMission(QJsonDocument json)
{
    QJsonObject jsonObj = json.object();
    return this->parseMission(jsonObj);
}

QList<InteropMission*> JsonInterpreter::parseMultipleMissions(QJsonDocument json)
{
    QList<InteropMission*> parsedInteropMissions;

    QJsonArray jsonArr = json.array();
    foreach(const QJsonValue& arrayVal, jsonArr)
    {
        QJsonObject arrayObj = arrayVal.toObject();
        InteropMission* parsedMission = this->parseMission(arrayObj);
        parsedInteropMissions.append(parsedMission);
    }

    return parsedInteropMissions;
}

JsonInterpreter::ObstacleSet* JsonInterpreter::parseObstacles(QJsonDocument json)
{
    QJsonObject jsonObj = json.object();

    QJsonArray movingObstArr = jsonObj["moving_obstacles"].toArray();
    QJsonArray stationaryObstArr = jsonObj["stationary_obstacles"].toArray();

    QList<MovingObstacle*> movingObstList;
    foreach(const QJsonValue& value, movingObstArr)
    {
        QJsonObject obj = value.toObject();

        double altitudeMsl = obj["altitude_msl"].toDouble();
        double latitude = obj["latitude"].toDouble();
        double longitude = obj["longitude"].toDouble();
        double sphereRadius = obj["sphere_radius"].toDouble();

        movingObstList.append(new MovingObstacle(altitudeMsl, latitude, longitude, sphereRadius));
    }

    QList<StationaryObstacle*> stationaryObstList;
    foreach(const QJsonValue& value, stationaryObstArr)
    {
        QJsonObject obj = value.toObject();

        double cylinderHeight = obj["cylinder_height"].toDouble();
        double cylinderRadius = obj["cylinder_radius"].toDouble();
        double latitude = obj["latitude"].toDouble();
        double longitude = obj["longitude"].toDouble();

        stationaryObstList.append(new StationaryObstacle(cylinderHeight, cylinderRadius, latitude, longitude));
    }

    return new JsonInterpreter::ObstacleSet{movingObstList, stationaryObstList};
}

InteropMission* JsonInterpreter::parseMission(QJsonObject obj)
{
    InteropMission* parsedMission = new InteropMission();

    parsedMission->setId(obj["id"].toInt());

    parsedMission->setActive(obj["active"].toBool());

    QJsonObject airDropPosObj = obj["air_drop_pos"].toObject();
    InteropMission::Position airDropPos =
            InteropMission::Position{
                                airDropPosObj["latitude"].toDouble(),
                                airDropPosObj["longitude"].toDouble()};
    parsedMission->setAirDropPos(airDropPos);

    QList<InteropMission::FlyZone> flyZonesList;
    QJsonArray flyZonesArr = obj["fly_zones"].toArray();
    foreach(const QJsonValue& flyZoneVal, flyZonesArr)
    {
        QJsonObject flyZoneObj = flyZoneVal.toObject();

        double altMslMax = flyZoneObj["altitude_msl_max"].toDouble();
        double altMslMin = flyZoneObj["altitude_msl_min"].toDouble();
        QJsonArray boundaryPtsArr = flyZoneObj["boundary_pts"].toArray();
        QList<InteropMission::BoundaryPoint> boundaryPtsList;

        foreach(const QJsonValue& value, boundaryPtsArr)
        {
            QJsonObject bondaryPtObj = value.toObject();

            double lat = bondaryPtObj["latitude"].toDouble();
            double lon = bondaryPtObj["longitude"].toDouble();
            int order = bondaryPtObj["order"].toInt();

            InteropMission::BoundaryPoint boundaryPt =
                    InteropMission::BoundaryPoint{lat, lon, order};

            boundaryPtsList.append(boundaryPt);
        }

        InteropMission::FlyZone flyZone =
                InteropMission::FlyZone{altMslMax, altMslMin, boundaryPtsList};
        flyZonesList.append(flyZone);
    }
    parsedMission->setFlyZones(flyZonesList);

    QJsonObject homePosObj = obj["home_pos"].toObject();
    InteropMission::Position homePos =
            InteropMission::Position{
                                homePosObj["latitude"].toDouble(),
                                homePosObj["longitude"].toDouble()};
    parsedMission->setHomePosition(homePos);

    QList<InteropMission::Waypoint> missionWaypointsList;
    QJsonArray missionWaypointsArr = obj["mission_waypoints"].toArray();
    foreach(const QJsonValue& missionWaypointVal, missionWaypointsArr)
    {
        QJsonObject missionWaypointObj = missionWaypointVal.toObject();

        double altMsl = missionWaypointObj["altitude_msl"].toDouble();
        double lat = missionWaypointObj["latitude"].toDouble();
        double lon = missionWaypointObj["longitude"].toDouble();
        int order = missionWaypointObj["order"].toInt();

        InteropMission::Waypoint missionWaypoint =
                InteropMission::Waypoint{altMsl, lat, lon, order};
        missionWaypointsList.append(missionWaypoint);
    }
    parsedMission->setMissionWaypoints(missionWaypointsList);

    QJsonObject offAxisOdlcPosObj = obj["off_axis_odlc_pos"].toObject();
    InteropMission::Position offAxisOdlcPos =
            InteropMission::Position{
                                offAxisOdlcPosObj["latitude"].toDouble(),
                                offAxisOdlcPosObj["longitude"].toDouble()};
    parsedMission->setOffAxisOdlcPos(offAxisOdlcPos);

    QJsonObject emergentLastKnownPosObj = obj["emergent_last_known_pos"].toObject();
    InteropMission::Position emergentLastKnownPos =
            InteropMission::Position{
                                emergentLastKnownPosObj["latitude"].toDouble(),
                                emergentLastKnownPosObj["longitude"].toDouble()};
    parsedMission->setEmergentLastKnownPos(emergentLastKnownPos);

    QList<InteropMission::Waypoint> searchGridPtsList;
    QJsonArray searchGridPtsListArr = obj["search_grid_points"].toArray();
    foreach(const QJsonValue& searchGridPtVal, searchGridPtsListArr)
    {
        QJsonObject searchGridPtObj = searchGridPtVal.toObject();

        double altMsl = searchGridPtObj["altitude_msl"].toDouble();
        double lat = searchGridPtObj["latitude"].toDouble();
        double lon = searchGridPtObj["longitude"].toDouble();
        int order = searchGridPtObj["order"].toInt();

        InteropMission::Waypoint searchGridPt =
                InteropMission::Waypoint{altMsl, lat, lon, order};
        searchGridPtsList.append(searchGridPt);
    }
    parsedMission->setMissionWaypoints(searchGridPtsList);

    return parsedMission;
}

