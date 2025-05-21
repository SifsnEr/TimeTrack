#include "TrackerBackend.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>  
#include <string>  
#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#endif

TrackerBackend::TrackerBackend(QObject *parent) : QObject(parent), totalSeconds(0)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TrackerBackend::checkActiveApp);
    timer->start(1000);
    loadFromJson();
}

QString TrackerBackend::getCurrentApp() const
{
    return currentApp;
}

QString TrackerBackend::getFormattedTime() const
{
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QString getFileDescription(const QString &filePath) 
{
    DWORD dummy;
    DWORD size = GetFileVersionInfoSizeW((LPCWSTR)filePath.utf16(), &dummy);
    if (size == 0) return QFileInfo(filePath).fileName();

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW((LPCWSTR)filePath.utf16(), 0, size, data.data())) {
        return QFileInfo(filePath).fileName();
    }

    void *buffer = nullptr;
    UINT len = 0;
    if (VerQueryValueW(data.data(), L"\\StringFileInfo\\040904b0\\FileDescription", &buffer, &len)) {
        return QString::fromWCharArray((WCHAR*)buffer);
    }

    if (VerQueryValueW(data.data(), L"\\StringFileInfo\\000004b0\\FileDescription", &buffer, &len)) {
        return QString::fromWCharArray((WCHAR*)buffer);
    }

    return QFileInfo(filePath).fileName();
}

/*
QString TrackerBackend::extractReadableName(const QString &path) 
{
    QFileInfo info(path);
    QString baseName = info.completeBaseName();
    return baseName.isEmpty() ? path : baseName;
}
*/

void TrackerBackend::checkActiveApp()
{
    QString newApp = fetchActiveAppName();

    if (newApp.isEmpty())
        return;

    bool appChanged = (newApp != currentApp);

    if (appChanged) {
        currentApp = newApp;
        currentAppName = getFileDescription(currentApp);
        emit currentAppChanged();
        emit currentAppNameChanged();
    }

    appUsage[currentApp]++;
    totalSeconds++;

    emit totalTimeChanged();

    if (appChanged) {
        updateAppStatsCache();
    }

    saveToJson();
}

void TrackerBackend::updateAppStatsCache()
{   
    QVariantList list;
    for (auto it = appUsage.begin(); it != appUsage.end(); ++it) {
        QString path = it.key();
        QVariantMap entry;
        entry["app"] = getFileDescription(path);
        entry["appPath"] = path;
        entry["seconds"] = it.value();
        list.append(entry);
    }
    std::sort(list.begin(), list.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap()["seconds"].toInt() > b.toMap()["seconds"].toInt();
    });
    appStatsCache = list;
    emit appStatsChanged();
}

QVariantList TrackerBackend::getAppUsageStats() const
{
    return appStatsCache;
}
QString TrackerBackend::fetchActiveAppName()
{
#ifdef Q_OS_WIN
    HWND hwnd = GetForegroundWindow();
    if (hwnd == nullptr)
        return QString();

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process) {
        wchar_t exeName[MAX_PATH];
        if (GetModuleFileNameExW(process, nullptr, exeName, MAX_PATH)) {
            CloseHandle(process);
            return QString::fromWCharArray(exeName);
        }
        CloseHandle(process);
    }
#endif
    return QStringLiteral("Unknown");
}

void TrackerBackend::exportToCSV()
{
    QFile file("usage.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "App,Seconds\n";
        for (auto it = appUsage.begin(); it != appUsage.end(); ++it) {
            out << it.key() << "," << it.value() << "\n";
        }
        file.close();
    }
}

void TrackerBackend::saveToJson()
{
    QJsonArray arr;
    for (auto it = appUsage.begin(); it != appUsage.end(); ++it) {
        QJsonObject obj;
        obj["app"] = it.key();
        obj["seconds"] = it.value();
        arr.append(obj);
    }

    QFile file("usage_data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson());
        file.close();
    }
}

void TrackerBackend::loadFromJson()
{
    QFile file("usage_data.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        for (const auto &val : doc.array()) {
            QJsonObject obj = val.toObject();
            QString appName = obj["app"].toString();
            int seconds = obj["seconds"].toInt();
            appUsage[appName] = seconds;
            totalSeconds += seconds;
        }
    }
}

void TrackerBackend::resetStats()
{
    appUsage.clear();
    totalSeconds = 0;
    appStatsCache.clear();
    emit appStatsChanged();
    emit totalTimeChanged();
    saveToJson();
}