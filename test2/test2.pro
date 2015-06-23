TEMPLATE = app

QT += qml quick widgets multimedia

SOURCES += main.cpp \
    myplayer.cpp \
    mediathread.cpp \
    ffmpegwrap.cpp \
    test_ffmpeg.cpp \
    kvconfig.cpp

RESOURCES += qml.qrc

mac {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libavcodec libavformat libswscale libswresample
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

win32: LIBS += -lavcodec -lavformat -lavutil -lswscale -lswresample -lccgnu2

HEADERS += \
    myplayer.h \
    mediathread.h \
    decode_result.h \
    ffmpegwrap.h \
    test_ffmpeg.h \
    utils.h \
    circ_buf.h \
    kvconfig.h

# macx: LIBS += -L/opt/local/lib/ -lavcodec -lavformat -lswscale -lavutil -lswresample

#INCLUDEPATH += /opt/local/include
#DEPENDPATH += /opt/local/include
