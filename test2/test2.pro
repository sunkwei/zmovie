TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    myplayer.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

win32: LIBS += -lavcodec -lavformat -lavutil -lswscale

HEADERS += \
    myplayer.h
