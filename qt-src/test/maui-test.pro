QT += core qml quick multimedia network testlib
QT -= gui

CONFIG += c++11

# This project expects your computer to set OPENCV_INCLUDE
# and MATLAB_INCLUDE environment variables to include director
# of the respective installations on your computer

MAUI_SRC = "$$PWD/.."
#message($$MAUI_SRC)

OPENCV_INCLUDE_PATH = "$$(OPENCV_INCLUDE)"
OPENCV_LIBS = "$$(OPENCV_DIR)\build\x64\vc14\lib"
MATLAB_INCLUDE_PATH = "$$(MATLAB_INCLUDE)"
MATLAB_LIBS = "$$MAUI_SRC/libs/"


CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_maui.cpp

# MAUI CONFIG BELOW
LIBS += -L"$$OPENCV_LIBS"
LIBS += -lopencv_core2413 -lopencv_highgui2413 -lopencv_imgproc2413 -lopencv_video2413

LIBS += -L"$$MATLAB_LIBS"
LIBS += -L"$$MAUI_SRC"/libs/
LIBS += -l"$$MAUI_SRC"/libs/MAUIVelocityDllWithAutoInit -lmclmcrrt -lmclmcr
LIBS += -llibmat -llibmx -lmclbase -llibmwservices -lmclcommain  -lmclxlmain

LIBS += -lkernel32 -luser32 -lgdi32 -lwinspool
LIBS += -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32

#message($$LIBS)

INCLUDEPATH += "$$OPENCV_INCLUDE_PATH"
INCLUDEPATH += "$$MATLAB_INCLUDE_PATH"
INCLUDEPATH += "$$MATLAB_INCLUDE_PATH"/../../bin/win64
INCLUDEPATH += "$$MAUI_SRC"

HEADERS +=  "$$MAUI_SRC/mcvplayer.h" \
            "$$MAUI_SRC/mcamerathread.h" \
            "$$MAUI_SRC/mvideocapture.h" \
            "$$MAUI_SRC/mpoint.h" \
            "$$MAUI_SRC/mdatalog.h" \
            "$$MAUI_SRC/mlogmetadata.h" \
            "$$MAUI_SRC/minitthread.h" \
            "$$MAUI_SRC/mremoteinterface.h" \
            "$$MAUI_SRC/msettings.h" \
            "$$MAUI_SRC/qblowfish.h" \
            "$$MAUI_SRC/qblowfish_p.h" \
            "$$MAUI_SRC/MAUIVelocityDllWithAutoInit.h" \
            "$$MAUI_SRC/mresultswriter.h" \
            "$$MAUI_SRC/mvideoinfo.h"

SOURCES +=  "$$MAUI_SRC/mcvplayer.cpp" \
            "$$MAUI_SRC/mcamerathread.cpp" \
            "$$MAUI_SRC/mvideocapture.cpp" \
            "$$MAUI_SRC/mpoint.cpp" \
            "$$MAUI_SRC/mdatalog.cpp" \
            "$$MAUI_SRC/mlogmetadata.cpp" \
            "$$MAUI_SRC/minitthread.cpp" \
            "$$MAUI_SRC/mremoteinterface.cpp" \
            "$$MAUI_SRC/msettings.cpp" \
            "$$MAUI_SRC/qblowfish.cpp" \
            "$$MAUI_SRC/mresultswriter.cpp" \
            "$$MAUI_SRC/mvideoinfo.cpp"

