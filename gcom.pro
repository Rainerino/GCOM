#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport

TARGET = gcom
TEMPLATE = app

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
    modules/uas_message/image_message.cpp \
    modules/uas_interop_system/interop_interface.cpp \
    modules/uas_interop_system/moving_obstacle.cpp \
    modules/uas_interop_system/stationary_obstacle.cpp

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
    modules/uas_message/image_message.hpp \
    modules/uas_interop_system/interop_interface.hpp \
    modules/uas_interop_system/moving_obstacle.hpp \
    modules/uas_interop_system/stationary_obstacle.hpp

FORMS    += gcomcontroller.ui
