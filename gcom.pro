#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gcom
TEMPLATE = app


SOURCES += main.cpp\
        gcomcontroller.cpp

HEADERS  += gcomcontroller.hpp

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14
