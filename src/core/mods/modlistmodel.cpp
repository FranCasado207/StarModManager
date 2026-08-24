#include "modlistmodel.h"

#include <QDir>

int ModListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_modNames.size();
}

QVariant ModListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_modNames.size()) {
        return QVariant();
    }

    if (role == NameRole) {
        return m_modNames.at(index.row());
    }

    return QVariant();
}

QHash<int, QByteArray> ModListModel::roleNames() const
{
    return { { NameRole, "name" } };
}

void ModListModel::setModsPath(const QString& path)
{
    if (m_modsPath == path) return;
    m_modsPath = path;
    emit modsPathChanged();
    refresh();
}

void ModListModel::refresh()
{
    beginResetModel();

    m_modNames.clear();

    if (!m_modsPath.isEmpty()) {
        QDir dir(m_modsPath);
        m_modNames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    }

    endResetModel();
    emit countChanged();
}

bool ModListModel::removeMod(const QString& modName)
{
    if (m_modsPath.isEmpty() || modName.isEmpty()) {
        return false;
    }

    QDir modDir(m_modsPath + "/" + modName);
    if (!modDir.exists()) {
        return false;
    }

    bool success = modDir.removeRecursively();

    if (success) {
        refresh();
    }

    return success;
}
