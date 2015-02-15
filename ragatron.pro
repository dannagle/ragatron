#-------------------------------------------------
#
# Project created by QtCreator 2015-01-12T19:32:08
#
#-------------------------------------------------

QT += core gui xml

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
    quazip/JlCompress.cpp \
    quazip/qioapi.cpp \
    quazip/quaadler32.cpp \
    quazip/quacrc32.cpp \
    quazip/quagzipfile.cpp \
    quazip/quaziodevice.cpp \
    quazip/quazip.cpp \
    quazip/quazipdir.cpp \
    quazip/quazipfile.cpp \
    quazip/quazipfileinfo.cpp \
    quazip/quazipnewinfo.cpp \
    quazip/unzip.c \
    quazip/zip.c \
    ragatron/cheatui.cpp

HEADERS  += ragatron/mainwindow.h \
    ragatron/globals.h \
    ragatron/html5game.h \
    quazip/crypt.h \
    quazip/ioapi.h \
    quazip/JlCompress.h \
    quazip/quaadler32.h \
    quazip/quachecksum32.h \
    quazip/quacrc32.h \
    quazip/quagzipfile.h \
    quazip/quaziodevice.h \
    quazip/quazip.h \
    quazip/quazip_global.h \
    quazip/quazipdir.h \
    quazip/quazipfile.h \
    quazip/quazipfileinfo.h \
    quazip/quazipnewinfo.h \
    quazip/unzip.h \
    quazip/zip.h \
    ragatron/cheatui.h \
    ragatron/hackstruct.h

FORMS    += ragatron/mainwindow.ui \
    ragatron/cheatui.ui

OTHER_FILES += \
    ragatron/ragatronicon.rc \
    ragatron/ragatron.css \
    ragatron/Ragatron_instructions.txt

RESOURCES += \
    ragatron/ragatron.qrc

