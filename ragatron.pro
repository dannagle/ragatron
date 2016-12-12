#-------------------------------------------------
#
# Project created by QtCreator 2015-01-12T19:32:08
#
# This code is copyright Dan Nagle and licensed GPL v2
#-------------------------------------------------

QT += core gui xml network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ragatron
TEMPLATE = app

DEFINES +=QUAZIP_STATIC="1"

macx:LIBS += -lz


macx:ICON = ragatron/dannagle_logo.icns


win32:RC_FILE = ragatron/ragatronicon.rc




SOURCES += ragatron/main.cpp\
        ragatron/mainwindow.cpp \
    ragatron/html5game.cpp \
    ragatron/cheatui.cpp \
    ragatron/filedownloader.cpp

HEADERS  += ragatron/mainwindow.h \
    ragatron/globals.h \
    ragatron/html5game.h \
    ragatron/cheatui.h \
    ragatron/hackstruct.h \
    ragatron/filedownloader.h

FORMS    += ragatron/mainwindow.ui \
    ragatron/cheatui.ui

OTHER_FILES += \
    ragatron/ragatronicon.rc \
    ragatron/ragatron.css \
    ragatron/Ragatron_instructions.txt

RESOURCES += \
    ragatron/ragatron.qrc

