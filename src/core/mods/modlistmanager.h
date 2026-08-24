#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QUrl>
#include "modlist.h"

class ModListManager : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ModCountRole,
        ThumbnailUrlRole
    };

    explicit ModListManager(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool createModList(const QString& name);
    Q_INVOKABLE bool deleteModList(const QString& name);
    Q_INVOKABLE bool renameModList(const QString& oldName, const QString& newName);

    Q_INVOKABLE QStringList modsInList(const QString& name) const;
    Q_INVOKABLE bool setModsInList(const QString& name, const QStringList& mods);

    Q_INVOKABLE bool setThumbnail(const QString& name, const QUrl& sourceUrl);
    Q_INVOKABLE QString thumbnailUrl(const QString& name) const;

    Q_INVOKABLE QString savesPathOf(const QString& name) const;
    Q_INVOKABLE bool setSavesPath(const QString& name, const QString& path);

    Q_INVOKABLE bool deploy(const QString& name, const QString& modsPath, const QString& gameModsPath);

private:
    void load();
    void save();
    int indexOfList(const QString& name) const;
    QString thumbnailsDir() const;

    QList<ModList> m_lists;
    QString m_filePath;
};
