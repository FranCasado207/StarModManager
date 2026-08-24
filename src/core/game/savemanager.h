#pragma once

#include <QObject>
#include <QString>

class SaveManager : public QObject
{
    Q_OBJECT

public:
    explicit SaveManager(QObject* parent = nullptr);

    Q_INVOKABLE QString defaultSavesPath() const;

    Q_INVOKABLE bool activateCustomSaves(const QString& customPath, QString* errorMessage = nullptr);


    Q_INVOKABLE bool restoreDefaultSaves(QString* errorMessage = nullptr);

    Q_INVOKABLE bool isSwapped() const;

    void recoverFromCrashIfNeeded();

private:
    QString stateFilePath() const;
    QString backupFolderPath() const;

    bool writeState(const QString& originalPath, const QString& backupPath) const;
    bool readState(QString* originalPath, QString* backupPath) const;
    void clearState() const;
};
