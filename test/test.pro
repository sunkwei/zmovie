TEMPLATE = app

QT += qml quick widgets

SOURCES += \
    MediaThread.cpp \
    WorkingThread.cpp \
    player.cpp \
    main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    DecodeResult.h \
    MediaThread.h \
    WorkingThread.h \
    zkrender.h \
    player.h

linux: LIBS += -lavformat -lavcodec -lswscale -lavutil -lSDL2 -lccgnu2

win32: LIBS += avformat.lib avcodec.lib swscale.lib avutil.lib ccgnu2.lib sdl2.lib
