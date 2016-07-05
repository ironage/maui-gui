TEMPLATE = app

# This project expects your computer to set OPENCV_INCLUDE
# and MATLAB_INCLUDE environment variables to include director
# of the respective installations on your computer

OPENCV_INCLUDE_PATH = $$(OPENCV_INCLUDE)
OPENCV_LIBS = $$(OPENCV_DIR)\build\x64\vc12\lib
MATLAB_INCLUDE_PATH = "$$(MATLAB_INCLUDE)"
#MATLAB_LIBS = $$(_PRO_FILE_PWD_)\libs
MATLAB_LIBS = "$$PWD/libs/"

SOURCES += main.cpp \
    mcvplayer.cpp \
    mcamerathread.cpp \
    mvideocapture.cpp
#    util/MxArray.cpp

QT += qml quick multimedia
CONFIG += c++11

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += $$OPENCV_INCLUDE_PATH
INCLUDEPATH += "$$MATLAB_INCLUDE_PATH"
INCLUDEPATH += "$$MATLAB_INCLUDE_PATH"/../../bin/win64

LIBS += -L"$$OPENCV_LIBS"
LIBS += -lopencv_core2413 -lopencv_highgui2413 -lopencv_imgproc2413 -lopencv_video2413

LIBS += -L"$$MATLAB_LIBS"
LIBS += -L$$PWD/libs/
LIBS += -l$$PWD/libs/libAutoInit -lmclmcrrt -lmclmcr
LIBS += -llibmat -llibmx -lmclbase -llibmwservices -lmclcommain  -lmclxlmain

LIBS += -lkernel32 -luser32 -lgdi32 -lwinspool
LIBS += -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32

message($$LIBS)

HEADERS += \
    mcvplayer.h \
    mcamerathread.h \
    mvideocapture.h \
    libAutoInit.h
#    util/MxArray.hpp
