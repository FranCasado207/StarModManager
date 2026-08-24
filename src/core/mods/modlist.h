#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>

struct ModList
{
    QString name;
    QStringList mods;

    QJsonObject toJson() const;
    static ModList fromJson(const QJsonObject& obj);
};
