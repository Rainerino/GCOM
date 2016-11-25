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
    modules/uas_message/uas_message_tcp_framer.cpp \
    modules/uas_message/request_message.cpp \
    modules/uas_message/system_info_message.cpp \
    modules/uas_message/command_message.cpp \
    modules/uas_message/response_message.cpp \
    gcom_controller.cpp

HEADERS  += \
    modules/uas_message/uas_message.hpp \
    modules/uas_message/uas_message_tcp_framer.hpp \
    modules/uas_message/request_message.hpp \
    modules/uas_message/system_info_message.hpp \
    modules/uas_message/command_message.hpp \
    modules/uas_message/response_message.hpp \
    gcom_controller.hpp

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14
