#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = gcom
TEMPLATE = app


SOURCES += main.cpp\
        gcomcontroller.cpp \
    modules/mavlink_relay/mavlink_relay_tcp.cpp

HEADERS  += gcomcontroller.hpp \
    modules/uas_message/uas_message.hpp \
    modules/mavlink_relay/mavlink_relay_tcp.hpp

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14

