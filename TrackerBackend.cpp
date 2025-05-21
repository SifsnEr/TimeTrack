#include "TrackerBackend.h"
#include <windows.h>
#include <psapi.h>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


TrackerBackend::TrackerBackend(QObject *parent) 
    : QObject(parent),
      timer(new QTimer(this))
{
    connect(timer, SIGNAL(timeout()), this, SLOT(updateActiveWindow()));
    timer->start(1000);
}

QString TrackerBackend::currentApp() const {
    return m_currentApp;
}

int TrackerBackend::totalTime() const {
    return m_totalTime;
}

void TrackerBackend::exportToCSV() {
    QFile file("activity_report.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    out << "Application,Time (sec)\n";

    for (auto it = m_appTime.begin(); it != m_appTime.end(); ++it) {
        out << it.key() << "," << it.value() << "\n";
    }

    file.close();
}

void TrackerBackend::updateActiveWindow() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        wchar_t title[256];
        GetWindowTextW(hwnd, title, sizeof(title)/sizeof(wchar_t));
        QString newApp = QString::fromWCharArray(title);

        if (newApp != m_currentApp) {
            m_currentApp = newApp;
            emit currentAppChanged();
        }

        m_appTime[m_currentApp] += 1;
        m_totalTime += 1;
        emit totalTimeChanged();
    }
}
