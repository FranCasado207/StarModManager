#include "configmanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <qdir.h>
#include <qfiledevice.h>
#include <qjsondocument.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qstandardpaths.h>
#include <qstringview.h>
#include "gamefinder.h"

ConfigManager::ConfigManager(QObject* parent) : QObject(parent){

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QDir dir;

    if(!dir.exists(configDir)){
        dir.mkpath(configDir);
        qDebug() << "Creating config folder";
    }

    m_configPath = configDir + "/config.json";
    m_firstRun = !QFile::exists(m_configPath);

    if(m_firstRun){
        qDebug() << "First execution, creating config file";
        createDefaultConfig();
        save();
    } else {
        load();
    }
}

void ConfigManager::createDefaultConfig(){
    QString detectedPath = GameFinder::findGameFolder();
    m_data["game_path"] = "";
    m_data["mods_path"] = "";

    if(!detectedPath.isEmpty()){
        m_data["game_path"] = detectedPath;
    }
}

QJsonValue ConfigManager::value(const QString& key, const QJsonValue& defaultValue) const {
    if (m_data.contains(key)){
        return m_data[key];
    }
    return defaultValue;
}

void ConfigManager::setValue(const QString& key, const QJsonValue& value) {
    m_data[key] = value;
}

void ConfigManager::save(){
    QJsonDocument doc(m_data);

    QFile file(m_configPath);
    if(!file.open(QIODevice::WriteOnly)){
        qWarning() << "Excuse me wtf";
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

void ConfigManager::load(){
    QFile file(m_configPath);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning() << "Cannot read";
        return;
    }

    QByteArray raw = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);

    if(parseError.error != QJsonParseError::NoError){
        qWarning() << parseError.errorString();
        return;
    }

    if(!doc.isObject()) {
        qWarning() << "config is fucked";
        return;
    }

    m_data = doc.object();
}

QString ConfigManager::gamePath() const{
    return value("game_path", QJsonValue::Null).toString();
}

void ConfigManager::setGamePath(const QString& path){
    if (gamePath() == path) return;
    m_data["game_path"] = path;
    emit gamePathChanged();
    save();
}

QString ConfigManager::modsPath() const{
    return value("mods_path", QJsonValue::Null).toString();
}

void ConfigManager::setModsPath(const QString& path){
    if (modsPath() == path) return;
    m_data["mods_path"] = path;
    emit modsPathChanged();
    save();
}
