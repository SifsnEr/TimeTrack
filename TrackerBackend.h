#ifndef TRACKERBACKEND_H
#define TRACKERBACKEND_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QString>
#include <QVariantList>

class TrackerBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentApp    READ getCurrentApp    NOTIFY currentAppChanged)
    Q_PROPERTY(QString formattedTime READ getFormattedTime NOTIFY totalTimeChanged)
    Q_PROPERTY(QVariantList appStats READ getAppUsageStats NOTIFY appStatsChanged)
    Q_PROPERTY(QString currentAppName READ getCurrentAppName NOTIFY currentAppNameChanged)

public:
    explicit TrackerBackend(QObject *parent = nullptr);

    Q_INVOKABLE QString getCurrentAppName() const { return currentAppName; }
    Q_INVOKABLE QString getCurrentApp() const;
    Q_INVOKABLE QString getFormattedTime() const;
    Q_INVOKABLE void exportToCSV();
    Q_INVOKABLE QVariantList getAppUsageStats() const;
    Q_INVOKABLE void resetStats();

signals:
    void currentAppChanged();
    void currentAppNameChanged();
    void totalTimeChanged();
    void appStatsChanged();

private:
    QTimer *timer;
    QString extractReadableName(const QString &path);
    QString currentApp;
    QString currentAppName;
    QMap<QString, int> appUsage;
    int totalSeconds;

    QVariantList appStatsCache;

    void checkActiveApp();
    void updateAppStatsCache();
    QString fetchActiveAppName();
    void saveToJson();
    void loadFromJson();
};

#endif 
