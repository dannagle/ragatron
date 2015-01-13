#-------------------------------------------------
#
# Project created by QtCreator 2015-01-12T19:32:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ragatron
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    html5game.cpp

HEADERS  += mainwindow.h \
    globals.h \
    html5game.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    ragatronicon.rc \
    ragatron.css \
    Ragatron_instructions.txt

RESOURCES += \
    ragatron.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_3_MinGW_32bit-Release/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_3_MinGW_32bit-Release/quazip/debug/ -lquazip

win32:INCLUDEPATH += $$PWD/../quazip-0.7/quazip
win32:DEPENDPATH += $$PWD/../quazip-0.7/quazip


#LIBS += /path/to/your/lib -lquazip

macx:INCLUDEPATH += /Users/dannagle/quazip-0.7/quazip
macx:DEPENDPATH += /Users/dannagle/quazip-0.7/quazip
macx: LIBS += /Users/dannagle/build-quazip-Desktop_Qt_5_3_0_clang_64bit-Release/quazip
