#include <QString>
#include <QtTest>

class test_collision_avoidance : public QObject
{
    Q_OBJECT

public:
    test_collision_avoidance();

private Q_SLOTS:
    void testCase1();
};

test_collision_avoidance::test_collision_avoidance()
{
}

void test_collision_avoidance::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(test_collision_avoidance)

#include "test_collision_avoidance.moc"
