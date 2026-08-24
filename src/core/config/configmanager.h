#pragma once

#include <QString>
#include <QJsonObject>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qobject.h>

class ConfigManager : public QObject{
    //QML Properties

    Q_OBJECT

    Q_PROPERTY(QString gamePath READ gamePath WRITE setGamePath NOTIFY gamePathChanged)
    Q_PROPERTY(QString modsPath READ modsPath WRITE setModsPath NOTIFY modsPathChanged)

    public:
        explicit ConfigManager(QObject* parent = nullptr);

        bool isFirstRun() const { return m_firstRun; }
        QString configFilePath() const { return m_configPath; }

        QJsonValue value(const QString& key, const QJsonValue& defaultValue = QJsonValue()) const;
        void setValue(const QString&key, const QJsonValue& value);

        QString gamePath() const;
        void setGamePath(const QString& path);

        QString modsPath() const;
        void setModsPath(const QString& path);

        Q_INVOKABLE void save();

    private:
        void createDefaultConfig();
        void load();

        QString m_configPath;
        bool m_firstRun = false;
        QJsonObject m_data;


    signals:
        void gamePathChanged();
        void modsPathChanged();
};
