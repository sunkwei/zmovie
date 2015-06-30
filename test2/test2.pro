TEMPLATE = app

QT += qml quick widgets multimedia script

DEFINES -= UNICODE

SOURCES += main.cpp \
    myplayer.cpp \
    mediathread.cpp \
    ffmpegwrap.cpp \
    kvconfig2.cpp \
    detect/DetectLoader.cpp \
    cJSON.c \
    ptz/ptz0.cpp \
    ptz.cpp

lupdate_only {
    SOURCES += MainForm.ui.qml
}

RESOURCES += qml.qrc

TRANSLATIONS = test2-zh_CN.ts test2-en_US.ts

mac {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libavcodec libavformat libswscale libswresample libccgnu2 libccext2
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

win32 {
    LIBS += -lavcodec -lavformat -lavutil -lswscale -lswresample -lccgnu2 -lws2_32 libcurldll.a
    QMAKE_LFLAGS += /OPT:NOREF
}

HEADERS += \
    myplayer.h \
    mediathread.h \
    decode_result.h \
    ffmpegwrap.h \
    utils.h \
    circ_buf.h \
    kvconfig2.h \
    detect/DetectLoader.h \
    zifimage.h \
    cJSON.h \
    ptz/ptz0.h \
    ptz.h

DISTFILES += \
    MyLogin.qml \
    MyMain.qml \
    MyPlayers.qml \
    MyPtzControls.qml \
    test.config \
    MyKVPair.qml \
    MyTest.qml \
    MyStudentConfig.qml
