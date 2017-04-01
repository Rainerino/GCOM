#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T08:23:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = gcom
TEMPLATE = app


SOURCES += main.cpp\
    modules/uas_message/uas_message_tcp_framer.cpp \
    modules/uas_message/request_message.cpp \
    modules/uas_message/system_info_message.cpp \
    modules/uas_message/command_message.cpp \
    modules/uas_message/response_message.cpp \
    gcom_controller.cpp \
    modules/mavlink_relay/mavlink_relay_tcp.cpp \
    modules/uas_dcnc/dcnc.cpp \
    modules/uas_message/image_tagger_message.cpp \
    modules/uas_image_tagger/image_tagger.cpp

HEADERS  += \
    modules/uas_message/uas_message.hpp \
    modules/mavlink_relay/mavlink_relay_tcp.hpp \
    modules/uas_message/request_message.hpp \
    modules/uas_message/system_info_message.hpp \
    modules/uas_message/command_message.hpp \
    modules/uas_message/response_message.hpp \
    gcom_controller.hpp \
    modules/uas_dcnc/dcnc.hpp \
    modules/uas_message/image_tagger_message.hpp \
    modules/uas_image_tagger/image_tagger.hpp

FORMS    += gcomcontroller.ui

INCLUDEPATH += Mavlink

CONFIG += c++14

RESOURCES = resources.qrc
