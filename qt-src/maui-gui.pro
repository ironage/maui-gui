TEMPLATE = app

OPENCV_INCLUDE_PATH = $$(OPENCV_INCLUDE)
OPENCV_LIBS = $$(OPENCV_DIR)\build\x64\vc12\lib

SOURCES += main.cpp \
    mmediaplayer.cpp \
    mcvsource.cpp

QT += qml quick multimedia
CONFIG += c++11

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += $$OPENCV_INCLUDE_PATH
LIBS += -L"$$OPENCV_LIBS"
LIBS += -lopencv_core2413 -lopencv_highgui2413 -lopencv_imgproc2413 -lopencv_video2413

HEADERS += \
    mmediaplayer.h \
    mcvsource.h

message("libs= $$LIBS")
