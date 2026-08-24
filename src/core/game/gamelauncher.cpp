#include "gamelauncher.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>

namespace {
constexpr const char* STEAM_APP_ID = "413150";
constexpr int POLL_INTERVAL_MS = 3000;
}

GameLauncher::GameLauncher(QObject* parent) : QObject(parent)
{
    m_monitorTimer.setInterval(POLL_INTERVAL_MS);
    connect(&m_monitorTimer, &QTimer::timeout, this, [this]() {
        bool running = isGameRunning();

        if (running) {
            m_wasRunning = true;
        } else if (m_wasRunning) {
            m_monitorTimer.stop();
            m_wasRunning = false;
            emit gameClosed();
        }
    });
}

bool GameLauncher::isGameRunning() const
{
#if defined(Q_OS_WIN)
    QProcess check;
    check.start("tasklist", { "/FI", "IMAGENAME eq StardewModdingAPI.exe" });
    check.waitForFinished(2000);
    QString output = check.readAllStandardOutput();
    if (output.contains("StardewModdingAPI.exe")) return true;

    check.start("tasklist", { "/FI", "IMAGENAME eq Stardew Valley.exe" });
    check.waitForFinished(2000);
    output = check.readAllStandardOutput();
    return output.contains("Stardew Valley.exe");
#else
    int exitCode = QProcess::execute("pgrep", { "-f", "StardewModdingAPI|Stardew Valley" });
    return exitCode == 0;
#endif
}

void GameLauncher::startMonitoring()
{
    m_wasRunning = false;
    QTimer::singleShot(2000, this, [this]() {
        m_monitorTimer.start();
    });
}

bool GameLauncher::isSteamInstall(const QString& gamePath) const
{
    return QFile::exists(gamePath + "/steam_appid.txt")
        || gamePath.contains("/steamapps/common/", Qt::CaseInsensitive)
        || gamePath.contains("\\steamapps\\common\\", Qt::CaseInsensitive);
}

QString GameLauncher::findSmapiExecutable(const QString& gamePath) const
{
    QDir dir(gamePath);
#if defined(Q_OS_WIN)
    QStringList candidates = { "StardewModdingAPI.exe" };
#else
    QStringList candidates = { "StardewModdingAPI" };
#endif
    for (const QString& name : candidates) {
        if (dir.exists(name)) return dir.filePath(name);
    }
    return QString();
}

QString GameLauncher::findGameExecutable(const QString& gamePath) const
{
    QDir dir(gamePath);
    QStringList candidates = { "StardewValley", "Stardew Valley", "Stardew Valley.exe", "StardewValley.exe" };
    for (const QString& name : candidates) {
        if (dir.exists(name)) return dir.filePath(name);
    }
    return QString();
}

bool GameLauncher::launch(const QString& gamePath, QString* errorMessage)
{
    if (gamePath.isEmpty() || !QDir(gamePath).exists()) {
        if (errorMessage) *errorMessage = "Game folder is not set or doesn't exist.";
        return false;
    }

    bool launched = false;

    if (isSteamInstall(gamePath)) {
        launched = QDesktopServices::openUrl(QUrl(QString("steam://rungameid/%1").arg(STEAM_APP_ID)));
        if (!launched && errorMessage) {
            *errorMessage = "Failed to launch via Steam. Is Steam installed and running?";
        }
    } else {
        QString smapiExe = findSmapiExecutable(gamePath);
        if (!smapiExe.isEmpty()) {
            launched = QProcess::startDetached(smapiExe, {}, gamePath);
            if (!launched && errorMessage) *errorMessage = "Failed to start SMAPI executable.";
        } else {
            QString gameExe = findGameExecutable(gamePath);
            if (gameExe.isEmpty()) {
                if (errorMessage) *errorMessage = "Could not find the game executable in the configured folder.";
                return false;
            }
            launched = QProcess::startDetached(gameExe, {}, gamePath);
            if (!launched && errorMessage) *errorMessage = "Failed to start the game executable.";
        }
    }

    if (launched) {
        startMonitoring();
    }

    return launched;
}
