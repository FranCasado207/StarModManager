#include "modlistmanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

ModListManager::ModListManager(QObject* parent) : QAbstractListModel(parent)
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }

    m_filePath = configDir + "/modlists.json";
    load();
}

int ModListManager::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_lists.size();
}

QVariant ModListManager::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_lists.size()) {
        return QVariant();
    }

    const ModList& list = m_lists.at(index.row());

    if (role == NameRole) return list.name;
    if (role == ModCountRole) return list.mods.size();

    return QVariant();
}

QHash<int, QByteArray> ModListManager::roleNames() const
{
    return {
        { NameRole, "name" },
        { ModCountRole, "modCount" }
    };
}

int ModListManager::indexOfList(const QString& name) const
{
    for (int i = 0; i < m_lists.size(); ++i) {
        if (m_lists.at(i).name == name) return i;
    }
    return -1;
}

bool ModListManager::createModList(const QString& name)
{
    QString trimmed = name.trimmed();
    if (trimmed.isEmpty() || indexOfList(trimmed) >= 0) {
        return false;
    }

    beginInsertRows(QModelIndex(), m_lists.size(), m_lists.size());
    ModList list;
    list.name = trimmed;
    m_lists.append(list);
    endInsertRows();

    save();
    return true;
}

bool ModListManager::deleteModList(const QString& name)
{
    int idx = indexOfList(name);
    if (idx < 0) return false;

    beginRemoveRows(QModelIndex(), idx, idx);
    m_lists.removeAt(idx);
    endRemoveRows();

    save();
    return true;
}

bool ModListManager::renameModList(const QString& oldName, const QString& newName)
{
    QString trimmed = newName.trimmed();
    if (trimmed.isEmpty() || indexOfList(trimmed) >= 0) {
        return false;
    }

    int idx = indexOfList(oldName);
    if (idx < 0) return false;

    m_lists[idx].name = trimmed;
    save();

    QModelIndex modelIdx = index(idx);
    emit dataChanged(modelIdx, modelIdx, { NameRole });
    return true;
}

QStringList ModListManager::modsInList(const QString& name) const
{
    int idx = indexOfList(name);
    if (idx < 0) return {};
    return m_lists.at(idx).mods;
}

bool ModListManager::setModsInList(const QString& name, const QStringList& mods)
{
    int idx = indexOfList(name);
    if (idx < 0) return false;

    m_lists[idx].mods = mods;
    save();

    QModelIndex modelIdx = index(idx);
    emit dataChanged(modelIdx, modelIdx, { ModCountRole });
    return true;
}

bool ModListManager::deploy(const QString& name, const QString& modsPath, const QString& gameModsPath)
{
    int idx = indexOfList(name);
    if (idx < 0 || modsPath.isEmpty() || gameModsPath.isEmpty()) {
        return false;
    }

    QDir targetDir(gameModsPath);
    if (!targetDir.exists()) {
        targetDir.mkpath(".");
    }

    const QFileInfoList entries = targetDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo& entry : entries) {
        if (entry.isSymLink()) {
            QFile::remove(entry.absoluteFilePath());
        }
    }

    const ModList& list = m_lists.at(idx);
    bool allOk = true;

    for (const QString& modName : list.mods) {
        QString source = modsPath + "/" + modName;
        QString destination = gameModsPath + "/" + modName;

        if (!QFileInfo::exists(source)) {
            allOk = false;
            continue;
        }

        if (!QFile::link(source, destination)) {
            qWarning() << "Failed to link" << source << "to" << destination;
            allOk = false;
        }
    }

    return allOk;
}

void ModListManager::load()
{
    QFile file(m_filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray raw = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
        return;
    }

    beginResetModel();
    m_lists.clear();
    const QJsonArray arr = doc.array();
    for (const QJsonValue& v : arr) {
        m_lists.append(ModList::fromJson(v.toObject()));
    }
    endResetModel();
}

void ModListManager::save()
{
    QJsonArray arr;
    for (const ModList& list : m_lists) {
        arr.append(list.toJson());
    }

    QJsonDocument doc(arr);
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not write modlists.json";
        return;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}
