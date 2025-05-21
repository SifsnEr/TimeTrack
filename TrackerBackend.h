#ifndef TRACKERBACKEND_H
#define TRACKERBACKEND_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QString>

class TrackerBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentApp READ currentApp NOTIFY currentAppChanged)
    Q_PROPERTY(int totalTime READ totalTime NOTIFY totalTimeChanged)
    
public:
    explicit TrackerBackend(QObject *parent = nullptr);
    ~TrackerBackend() override = default;

    QString currentApp() const;
    int totalTime() const;
    Q_INVOKABLE void exportToCSV();

signals:
    void currentAppChanged();
    void totalTimeChanged();

private slots:
    void updateActiveWindow();

private:
    QTimer *timer;
    QString m_currentApp;
    int m_totalTime = 0;
    QMap<QString, int> m_appTime;
};

#endif