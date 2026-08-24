#pragma once

#include <QAbstractListModel>
#include <QStringList>

class ModListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString modsPath READ modsPath WRITE setModsPath NOTIFY modsPathChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole + 1
    };

    explicit ModListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString modsPath() const { return m_modsPath; }
    void setModsPath(const QString& path);

    int count() const { return m_modNames.size(); }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool removeMod(const QString& modName);

signals:
    void modsPathChanged();
    void countChanged();

private:
    QString m_modsPath;
    QStringList m_modNames;
};
