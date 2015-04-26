#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T08:31:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zmovie
TEMPLATE = app
INCLUDEPATH += /usr/include/ffmpeg/ /usr/include
LIBS += -L/usr/lib64/ -lavformat -lavcodec -lavutil

SOURCES += main.cpp\
        mainwindow.cpp \
    dialoglogger.cpp \
    mediasource.cpp \
    libavinit.cpp \
    log.cpp

HEADERS  += mainwindow.h \
    dialoglogger.h \
    mediasource.h \
    libavinit.h \
    log.h

FORMS    += mainwindow.ui \
    dialoglogger.ui
