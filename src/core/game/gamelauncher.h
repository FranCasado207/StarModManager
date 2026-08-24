#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

class GameLauncher : public QObject
{
    Q_OBJECT

public:
    explicit GameLauncher(QObject* parent = nullptr);

    Q_INVOKABLE bool launch(const QString& gamePath, QString* errorMessage = nullptr);

signals:

    void gameClosed();

private:
    bool isSteamInstall(const QString& gamePath) const;
    QString findSmapiExecutable(const QString& gamePath) const;
    QString findGameExecutable(const QString& gamePath) const;

    bool isGameRunning() const;
    void startMonitoring();

    QTimer m_monitorTimer;
    bool m_wasRunning = false;
};
