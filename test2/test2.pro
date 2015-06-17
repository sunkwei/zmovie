TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    myplayer.cpp \
    mediathread.cpp \
    ffmpegwrap.cpp \
    test_ffmpeg.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

win32: LIBS += -lavcodec -lavformat -lavutil -lswscale -lccgnu2

HEADERS += \
    myplayer.h \
    mediathread.h \
    decode_result.h \
    ffmpegwrap.h \
    test_ffmpeg.h \
    utils.h

macx: LIBS += -L/opt/local/lib/ -lavcodec -lavformat -lswscale -lavutil

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include
