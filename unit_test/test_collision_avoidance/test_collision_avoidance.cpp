//===================================================================
// Includes
//===================================================================
// System Includes
#include <QString>
#include <QtTest>

// GCOM Includes
#include "test_collision_avoidance.hpp";

QTEST_MAIN(TestCollisionAvoidance)

void TestCollisionAvoidance::initTestCase()
{
    collisionAvoidanceInstance = new CollisionAvoidance();
    collisionAvoidanceInstance->generateWaypointFile();

}

void TestCollisionAvoidance::cleanupTestCase()
{
    delete collisionAvoidanceInstance;
}

void TestCollisionAvoidance::testCase1()
{
    QVERIFY2(true, "Failure");
}

