TEMPLATE = app
TARGET = TimeTrack
QT += qml quick gui
CONFIG += c++17

SOURCES += main.cpp \
           TrackerBackend.cpp

HEADERS += TrackerBackend.h

RESOURCES += qml_qmlcache.qrc

win32 {
    LIBS += -lpsapi
    INCLUDEPATH += $$(WINDOWSSDKDIR)\Include\$$(WINDOWSSDKVERSION)\um
}

QT_INSTALL_PREFIX = D:/Qt/6.9.0/msvc2019_64