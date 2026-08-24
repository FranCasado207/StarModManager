#pragma once

#include <QString>
#include <QStringList>

class GameFinder
{
public:
    static QString findGameFolder();

private:
    static QStringList defaultInstallPaths();
    static QStringList customInstallPaths();

    static QStringList steamInstallCandidates();
    static QString findSteamRootPath();
    static QStringList parseLibraryFoldersVdf(const QString& steamRootPath);

#ifdef Q_OS_WIN
    static QStringList windowsRegistryPaths();
#endif

    static bool isValidGameFolder(const QString& path);
};
