TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    myplayer.cpp \
    mediathread.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

win32: LIBS += -lavcodec -lavformat -lavutil -lswscale

HEADERS += \
    myplayer.h \
    mediathread.h \
    decode_result.h

macx: LIBS += -L/opt/local/lib/ -lavcodec -lavformat -lswscale -lavutil

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include
