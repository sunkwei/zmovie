TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    piechart.cpp \
    MainThread.cpp \
    MediaThread.cpp \
    WorkingThread.cpp \
    player.cpp \
    zkrender.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    piechart.h \
    DecodeResult.h \
    MainThread.h \
    MediaThread.h \
    WorkingThread.h \
    zkrender.h \
    player.h

LIBS += -lavformat -lavcodec -lswscale -lavutil -lSDL2 -lccgnu2
