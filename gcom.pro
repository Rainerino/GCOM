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
    modules/uas_message/uas_message_tcp_framer.cpp \
    modules/uas_dcnc/dcnc.cpp

HEADERS  += gcomcontroller.hpp \
    modules/uas_message/uas_message.hpp \
    modules/uas_message/uas_message_tcp_framer.hpp \
    modules/uas_dcnc/dcnc.hpp

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14

DISTFILES +=
