#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gcom
TEMPLATE = app


SOURCES += main.cpp\
        gcomcontroller.cpp \
    modules/mavlink_relay/missionplannertcp.cpp

HEADERS  += gcomcontroller.hpp \
    modules/uas_message/uas_message.hpp \
    modules/mavlink_relay/missionplannertcp.hpp

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14
