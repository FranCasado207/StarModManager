#include "savemanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

SaveManager::SaveManager(QObject* parent) : QObject(parent)
{
}

QString SaveManager::defaultSavesPath() const
{
#if defined(Q_OS_WIN)
    QString appData = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    return appData + "/StardewValley";
#elif defined(Q_OS_MAC)
    return QDir::homePath() + "/.config/StardewValley";
#else
    QString xdgConfig = qEnvironmentVariable("XDG_CONFIG_HOME");
    if (xdgConfig.isEmpty()) {
        xdgConfig = QDir::homePath() + "/.config";
    }
    return xdgConfig + "/StardewValley";
#endif
}

QString SaveManager::stateFilePath() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return configDir + "/save_swap_state.json";
}

QString SaveManager::backupFolderPath() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return configDir + "/saves_backup";
}

bool SaveManager::writeState(const QString& originalPath, const QString& backupPath) const
{
    QJsonObject obj;
    obj["originalPath"] = originalPath;
    obj["backupPath"] = backupPath;

    QFile file(stateFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool SaveManager::readState(QString* originalPath, QString* backupPath) const
{
    QFile file(stateFilePath());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray raw = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject()) return false;

    QJsonObject obj = doc.object();
    *originalPath = obj.value("originalPath").toString();
    *backupPath = obj.value("backupPath").toString();

    return !originalPath->isEmpty() && !backupPath->isEmpty();
}

void SaveManager::clearState() const
{
    QFile::remove(stateFilePath());
}

bool SaveManager::isSwapped() const
{
    return QFile::exists(stateFilePath());
}

void SaveManager::recoverFromCrashIfNeeded()
{
    if (!isSwapped()) {
        return;
    }

    qWarning() << "Found a pending saves swap from a previous session — restoring automatically.";
    QString errorMessage;
    if (!restoreDefaultSaves(&errorMessage)) {
        qWarning() << "Failed to auto-recover saves:" << errorMessage;
    }
}

bool SaveManager::activateCustomSaves(const QString& customPath, QString* errorMessage)
{
    if (customPath.isEmpty()) {
        if (errorMessage) *errorMessage = "No custom saves path given.";
        return false;
    }

    if (isSwapped()) {
        QString err;
        if (!restoreDefaultSaves(&err)) {
            if (errorMessage) *errorMessage = "Could not clean up a previous saves swap: " + err;
            return false;
        }
    }

    QString defaultPath = defaultSavesPath();
    QDir customDir(customPath);
    if (!customDir.exists()) {
        customDir.mkpath(".");
    }

    QFileInfo defaultInfo(defaultPath);
    QString backupPath = backupFolderPath();
    QDir().mkpath(QFileInfo(backupPath).path());

    if (defaultInfo.exists() && !defaultInfo.isSymLink()) {
        QDir().remove(backupPath);
        if (!QDir().rename(defaultPath, backupPath)) {
            if (errorMessage) *errorMessage = "Failed to back up the default saves folder.";
            return false;
        }
    } else if (defaultInfo.isSymLink()) {
        QFile::remove(defaultPath);
        backupPath = "__none__";
    } else {
        backupPath = "__none__";
    }

    if (!QFile::link(customPath, defaultPath)) {
        if (backupPath != "__none__") {
            QDir().rename(backupPath, defaultPath);
        }
        if (errorMessage) *errorMessage = "Failed to link the custom saves folder.";
        return false;
    }

    writeState(defaultPath, backupPath);
    return true;
}

bool SaveManager::restoreDefaultSaves(QString* errorMessage)
{
    if (!isSwapped()) {
        return true; // nothing to do
    }

    QString originalPath, backupPath;
    if (!readState(&originalPath, &backupPath)) {
        if (errorMessage) *errorMessage = "Could not read saves swap state.";
        return false;
    }

    QFileInfo currentInfo(originalPath);
    if (currentInfo.exists() && currentInfo.isSymLink()) {
        QFile::remove(originalPath);
    }

    if (backupPath != "__none__" && !backupPath.isEmpty()) {
        if (!QDir().rename(backupPath, originalPath)) {
            if (errorMessage) *errorMessage = "Failed to restore the original saves folder.";
            return false;
        }
    }

    clearState();
    return true;
}
