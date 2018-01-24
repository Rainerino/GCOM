#include <QObject>
#include "../uas_interop_system/InteropObjects/interop_mission.hpp"

/**
 * @brief The CAS class
 * Goal: receives the mission way points and based on its locations draw a map of all the points annd
 * stationary obstacles and
 * hopefully display it on the map.
 */
class CAS: public QObject{
    Q_OBJECT
public:
    CAS();
    //mode changing to guided mode, check for all the pre-requirements
    void setupCAS();

    //this will add a new stationary obstacle to the map
    void addStationary();
    void addDynamical();

    void receiveMission();

    //take the list and decide the order
    void createMissionMap();

private:
    QList<Waypoints> missions;
    /**
    struct Waypoint
    {
        double altitudeMsl;
        double latitude;
        double longitude;
        int order;
    };
      */
};
