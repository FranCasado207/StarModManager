#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>

struct ModList
{
    QString id;
    QString name;
    QStringList mods;
    QString thumbnailPath;
    QString savesPath;

    QJsonObject toJson() const;
    static ModList fromJson(const QJsonObject& obj);
};
