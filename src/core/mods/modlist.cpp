#include "modlist.h"
#include <QJsonArray>
#include <QUuid>

QJsonObject ModList::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["thumbnailPath"] = thumbnailPath;
    obj["savesPath"] = savesPath;

    QJsonArray arr;
    for (const QString& m : mods) arr.append(m);
    obj["mods"] = arr;

    return obj;
}

ModList ModList::fromJson(const QJsonObject& obj)
{
    ModList list;
    list.id = obj.value("id").toString();
    if (list.id.isEmpty()) {
        list.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    list.name = obj.value("name").toString();
    list.thumbnailPath = obj.value("thumbnailPath").toString();
    list.savesPath = obj.value("savesPath").toString();

    QJsonArray arr = obj.value("mods").toArray();
    for (const QJsonValue& v : arr) list.mods << v.toString();

    return list;
}
