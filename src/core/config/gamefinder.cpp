#include "gamefinder.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#ifdef Q_OS_WIN
#include <QSettings>
#endif

namespace {
constexpr const char* STEAM_APP_ID = "413150";
}

QString GameFinder::findGameFolder()
{
    QStringList paths = customInstallPaths() + steamInstallCandidates() + defaultInstallPaths();

    for (const QString& path : paths) {
        QString normalized = QDir(path).absolutePath();
        if (isValidGameFolder(normalized)) {
            return normalized;
        }
    }

    return QString();
}

bool GameFinder::isValidGameFolder(const QString& path)
{
    if (path.isEmpty() || !QDir(path).exists()) {
        return false;
    }
    return QDir(path).exists("Stardew Valley.dll");
}

QString GameFinder::findSteamRootPath()
{
    QString home = QDir::homePath();

    QStringList candidates;

#if defined(Q_OS_LINUX)
    candidates << home + "/.steam/steam";
    candidates << home + "/.local/share/Steam";
    candidates << home + "/.var/app/com.valvesoftware.Steam/data/Steam"; // Flatpak
#elif defined(Q_OS_MAC)
    candidates << home + "/Library/Application Support/Steam";
#elif defined(Q_OS_WIN)
    QSettings steamPathKey("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
    QString regPath = steamPathKey.value("SteamPath").toString();
    if (!regPath.isEmpty()) {
        candidates << regPath;
    }
    candidates << "C:/Program Files (x86)/Steam";
    candidates << "C:/Program Files/Steam";
#endif

    for (const QString& candidate : candidates) {
        if (QDir(candidate).exists("steamapps")) {
            return candidate;
        }
    }

    return QString();
}


QStringList GameFinder::parseLibraryFoldersVdf(const QString& steamRootPath)
{
    QStringList result;

    QFile file(steamRootPath + "/steamapps/libraryfolders.vdf");
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return result;
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    QRegularExpression blockRe(R"re("\d+"\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\})re");
    QRegularExpression pathRe(R"re("path"\s*"([^"]+)")re");
    QRegularExpression appIdRe("\"" + QString(STEAM_APP_ID) + "\"");

    QRegularExpressionMatchIterator blockMatches = blockRe.globalMatch(content);
    while (blockMatches.hasNext()) {
        QString block = blockMatches.next().captured(1);

        if (!block.contains(appIdRe)) {
            continue;
        }

        QRegularExpressionMatch pathMatch = pathRe.match(block);
        if (pathMatch.hasMatch()) {
            QString libPath = pathMatch.captured(1);
            libPath.replace("\\\\", "/");
            result << libPath + "/steamapps/common/Stardew Valley";
        }
    }

    return result;
}

QStringList GameFinder::steamInstallCandidates()
{
    QString steamRoot = findSteamRootPath();
    if (steamRoot.isEmpty()) {
        return {};
    }

    QStringList result;

    result << steamRoot + "/steamapps/common/Stardew Valley";

    result << parseLibraryFoldersVdf(steamRoot);

    return result;
}

QStringList GameFinder::defaultInstallPaths()
{
    QStringList paths;
    QString home = QDir::homePath();

#if defined(Q_OS_LINUX)
    paths << home + "/GOG Games/Stardew Valley/game";

#elif defined(Q_OS_MAC)
    paths << "/Applications/Stardew Valley.app/Contents/MacOS";

#elif defined(Q_OS_WIN)
    paths << windowsRegistryPaths();

    for (const QString& programFiles : {QString("C:/Program Files"), QString("C:/Program Files (x86)")}) {
        paths << programFiles + "/GalaxyClient/Games/Stardew Valley";
        paths << programFiles + "/GOG Galaxy/Games/Stardew Valley";
        paths << programFiles + "/GOG Games/Stardew Valley";
    }

    for (char drive = 'C'; drive <= 'H'; ++drive) {
        paths << QString("%1:/Program Files/ModifiableWindowsApps/Stardew Valley").arg(drive);
    }
#endif

    return paths;
}

#ifdef Q_OS_WIN
QStringList GameFinder::windowsRegistryPaths()
{
    QStringList paths;

    QSettings gogKey(
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\GOG.com\\Games\\1453375253",
        QSettings::NativeFormat
    );
    QString gogPath = gogKey.value("PATH").toString();
    if (!gogPath.isEmpty()) {
        paths << gogPath;
    }

    return paths;
}
#endif

QStringList GameFinder::customInstallPaths()
{
    QStringList result;
    QString home = QDir::homePath();
    QFile targetsFile(home + "/stardewvalley.targets");

    if (!targetsFile.exists() || !targetsFile.open(QIODevice::ReadOnly)) {
        return result;
    }

    QTextStream stream(&targetsFile);
    QString content = stream.readAll();
    targetsFile.close();

    QRegularExpression re("<GamePath>(.*?)</GamePath>");
    QRegularExpressionMatch match = re.match(content);
    if (match.hasMatch()) {
        QString path = match.captured(1).trimmed();
        if (!path.isEmpty()) {
            result << path;
        }
    }

    return result;
}
