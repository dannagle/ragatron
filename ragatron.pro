#-------------------------------------------------
#
# Project created by QtCreator 2015-01-12T19:32:08
#
#-------------------------------------------------

QT       += core gui
QT += xml


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ragatron
TEMPLATE = app

DEFINES +=QUAZIP_STATIC="1"

macx:LIBS += -lz


SOURCES += main.cpp\
        mainwindow.cpp \
    html5game.cpp \
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
    quazip/zip.c

HEADERS  += mainwindow.h \
    globals.h \
    html5game.h \
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
    quazip/zip.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    ragatronicon.rc \
    ragatron.css \
    Ragatron_instructions.txt

RESOURCES += \
    ragatron.qrc
