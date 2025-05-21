TEMPLATE = app
TARGET = TimeTrack
QT += qml quick gui quickcontrols2 widgets
CONFIG += c++17
LIBS += -lpsapi -lVersion

SOURCES += main.cpp \
           TrackerBackend.cpp

HEADERS += TrackerBackend.h

RESOURCES += resources.qrc

win32: LIBS += -lpsapi
