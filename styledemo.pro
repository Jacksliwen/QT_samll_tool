QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = SmallTools
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
OBJECTS_DIR = temp/obj

DESTDIR     = $$PWD/bin/SmallTools

CONFIG      += warn_off

UI_DIR      =  temp/ui

SOURCES     += src/main.cpp \
    src/base64.cpp \
    src/frmmain.cpp \

HEADERS     += include/frmmain.h \
    include/base64.h \
    include/head.h \
    include/common/log.h

FORMS       += ui/frmmain.ui
RESOURCES   += other/qss.qrc
RESOURCES   += other/main.qrc

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/common
INCLUDEPATH += $$PWD/include/vlc
include ($$PWD/include/vlc/vlc.pri)
