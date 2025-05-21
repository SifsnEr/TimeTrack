#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include "TrackerBackend.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<TrackerBackend>("Tracker", 1, 0, "TrackerBackend");

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML file";
        return -1;
    }

    return app.exec();
}
