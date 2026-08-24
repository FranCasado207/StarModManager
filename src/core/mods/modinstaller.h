#pragma once

#include <QObject>
#include <QUrl>

class ModInstaller : public QObject
{
    Q_OBJECT

public:
    explicit ModInstaller(QObject* parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void installMod(const QUrl& archiveUrl, const QString& modsPath);

signals:
    void modInstalled(const QString& modName);
    void installFailed(const QString& fileName, const QString& reason);
};
