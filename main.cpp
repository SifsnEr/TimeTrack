#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "TrackerBackend.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QApplication>
#include <QQuickStyle>
#include <QLockFile>
#include <QDir>
#include <QQmlEngine>
#include <QWindow>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);

    QLockFile lock(QDir::temp().absoluteFilePath("TimeTrack.lock"));
    lock.setStaleLockTime(0);
    if (!lock.tryLock()) {
        return 0;
    }

    app.setWindowIcon(QIcon(":/icons/tray.png"));

    QQuickStyle::setStyle("Material");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", QByteArray("Dark"));

    TrackerBackend backend;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);
    engine.addImportPath("D:/cpp/Qt/5.12.12/mingw73_64/qml");
    engine.addImportPath(":/qt-project.org/imports");
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *rootObject = engine.rootObjects().first();
    QWindow *window = qobject_cast<QWindow *>(rootObject);
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/icons/tray.png"));
    QMenu *trayMenu = new QMenu;
    QAction *quitAction = new QAction("Выход");

    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setToolTip("TimeTrack");
    trayIcon->show();

    QObject::connect(quitAction, &QAction::triggered, &app, &QCoreApplication::quit);
    QObject::connect(trayIcon, &QSystemTrayIcon::activated, [&window](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            if (window) {
                window->show();
                window->raise();
                window->requestActivate();
            }
        }
    });

    return app.exec();
}