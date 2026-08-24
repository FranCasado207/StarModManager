#include "modinstaller.h"
#include "archiveextractor.h"

#include <QFileInfo>

void ModInstaller::installMod(const QUrl& archiveUrl, const QString& modsPath)
{
    QString archivePath = archiveUrl.toLocalFile();
    QFileInfo info(archivePath);

    if (!info.exists() || !info.isFile()) {
        emit installFailed(info.fileName(), "File not found.");
        return;
    }

    if (modsPath.isEmpty()) {
        emit installFailed(info.fileName(), "Mods folder is not set.");
        return;
    }

    QString errorMessage;
    QString resultFolder = ArchiveExtractor::extract(archivePath, modsPath, &errorMessage);

    if (resultFolder.isEmpty()) {
        emit installFailed(info.fileName(), errorMessage);
        return;
    }

    emit modInstalled(resultFolder);
}
