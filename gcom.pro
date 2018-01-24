#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport

TARGET = gcom

test {
    TEMPLATE = subdirs
    SUBDIRS += \
        unit_test/test_dcnc
}
else {
    TEMPLATE = app
}

INCLUDEPATH += Mavlink auvsi_suas_interop

CONFIG += c++14

RESOURCES = resources.qrc

SOURCES += main.cpp\
    modules/uas_message/uas_message_tcp_framer.cpp \
    modules/uas_message/request_message.cpp \
    modules/uas_message/system_info_message.cpp \
    modules/uas_message/command_message.cpp \
    modules/uas_message/response_message.cpp \
    gcom_controller.cpp \
    modules/mavlink_relay/mavlink_relay_tcp.cpp \
    modules/uas_dcnc/dcnc.cpp \
    modules/uas_antenna_tracker/antennatracker.cpp \
    modules/uas_message/uas_message_serial_framer.cpp \
    modules/uas_message/gps_message.cpp \
    modules/uas_message/imu_message.cpp \
    modules/uas_message/capabilities_message.cpp \
    modules/uas_message/image_untagged_message.cpp \
    modules/uas_message/image_tagged_message.cpp \
    modules/uas_utility/uas_utility.cpp \
    modules/uas_interop_system/InteropObjects/interop_mission.cpp \
    modules/uas_interop_system/InteropObjects/interop_telemetry.cpp \
    modules/uas_interop_system/InteropObjects/moving_obstacle.cpp \
    modules/uas_interop_system/InteropObjects/stationary_obstacle.cpp \
    modules/uas_interop_system/interop.cpp \
    modules/uas_interop_system/json_interpreter.cpp \
    modules/uas_interop_system/InteropObjects/interop_odlc.cpp \
    modules/uas_cas_quad/main_cas.cpp \
    modules/uas_image_processing/imp_main.cpp \
    modules/uas_image_processing/imp_api.cpp \
    modules/uas_image_processing/imp_object.cpp \
    modules/uas_collision_avoidance/collision_avoidance.cpp


HEADERS  += \
    modules/uas_message/uas_message.hpp \
    modules/mavlink_relay/mavlink_relay_tcp.hpp \
    modules/uas_message/uas_message_tcp_framer.hpp \
    modules/uas_message/system_info_message.hpp \
    modules/uas_message/command_message.hpp \
    modules/uas_message/response_message.hpp \
    gcom_controller.hpp \
    modules/uas_message/uas_message.hpp \
    modules/uas_message/uas_message_tcp_framer.hpp \
    modules/uas_dcnc/dcnc.hpp \
    modules/uas_antenna_tracker/antennatracker.hpp \
    modules/uas_message/uas_message_serial_framer.hpp \
    modules/uas_message/request_message.hpp \
    modules/uas_message/gps_message.hpp \
    modules/uas_message/imu_message.hpp \
    modules/uas_message/capabilities_message.hpp \
    modules/uas_message/image_untagged_message.hpp \
    modules/uas_message/image_tagged_message.hpp \
    modules/uas_utility/uas_utility.h \
    modules/uas_interop_system/InteropObjects/interop_mission.hpp \
    modules/uas_interop_system/InteropObjects/interop_telemetry.hpp \
    modules/uas_interop_system/InteropObjects/moving_obstacle.hpp \
    modules/uas_interop_system/InteropObjects/stationary_obstacle.hpp \
    modules/uas_interop_system/interop.hpp \
    modules/uas_interop_system/json_interpreter.hpp \
    modules/uas_interop_system/InteropObjects/interop_odlc.hpp \
    modules/uas_cas_quad/main_cas.hpp \
    modules/uas_image_processing/imp_main.hpp \
    modules/uas_image_processing/imp_api.hpp \
    modules/uas_image_processing/imp_object.hpp \
    modules/uas_collision_avoidance/collision_avoidance.hpp


FORMS += \
    gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14

RESOURCES = resources.qrc

DISTFILES += \
    ../../2018 Spring/UAS Notes and Information/mapexample
