TEMPLATE = app
CONFIG += console
CONFIG -= qt
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_highgui
QMAKE_CXXFLAGS +=  -std=c++0x
SOURCES += main.cpp

