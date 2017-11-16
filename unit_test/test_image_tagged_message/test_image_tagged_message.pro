######################################################################
# Automatically generated by qmake (3.1) Sat Nov 11 16:37:46 2017
######################################################################

QT += testlib network
TEMPLATE = app
TARGET = test_image_tagged_message
INCLUDEPATH += ../../

CONFIG += c++14 \
        resources_big

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
         QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += test_image_tagged_message.hpp \
        ../../modules/uas_dcnc/dcnc.hpp \
        ../../modules/uas_message/uas_message.hpp \
        ../../modules/uas_message/uas_message_tcp_framer.hpp \
        ../../modules/uas_message/capabilities_message.hpp \
        ../../modules/uas_message/response_message.hpp \
        ../../modules/uas_message/command_message.hpp \
        ../../modules/uas_message/request_message.hpp \
        ../../modules/uas_message/system_info_message.hpp \
        ../../modules/uas_message/image_untagged_message.hpp \
        ../../modules/uas_message/image_tagged_message.hpp \

SOURCES += test_image_tagged_message.cpp \
        ../../modules/uas_dcnc/dcnc.cpp \
        ../../modules/uas_message/uas_message_tcp_framer.cpp \
        ../../modules/uas_message/capabilities_message.cpp \
        ../../modules/uas_message/response_message.cpp \
        ../../modules/uas_message/command_message.cpp \
        ../../modules/uas_message/request_message.cpp \
        ../../modules/uas_message/system_info_message.cpp \
        ../../modules/uas_message/image_untagged_message.cpp \
        ../../modules/uas_message/image_tagged_message.cpp \
