#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gcom
TEMPLATE = app


SOURCES += main.cpp\
        gcomcontroller.cpp

HEADERS  += gcomcontroller.hpp \
    dcnc.h

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14
