#include "modlist.h"
#include <QJsonArray>

QJsonObject ModList::toJson() const
{
    QJsonObject obj;
    obj["name"] = name;

    QJsonArray arr;
    for (const QString& m : mods) arr.append(m);
    obj["mods"] = arr;

    return obj;
}

ModList ModList::fromJson(const QJsonObject& obj)
{
    ModList list;
    list.name = obj.value("name").toString();

    QJsonArray arr = obj.value("mods").toArray();
    for (const QJsonValue& v : arr) list.mods << v.toString();

    return list;
}
