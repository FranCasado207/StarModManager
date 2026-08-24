#include "archiveextractor.h"

#include <QDir>
#include <QFileInfo>
#include <QSet>

#include <archive.h>
#include <archive_entry.h>

namespace {

int copyData(struct archive* reader, struct archive* writer)
{
    const void* buffer;
    size_t size;
    la_int64_t offset;

    for (;;) {
        int result = archive_read_data_block(reader, &buffer, &size, &offset);
        if (result == ARCHIVE_EOF) return ARCHIVE_OK;
        if (result < ARCHIVE_OK) return result;

        result = archive_write_data_block(writer, buffer, size, offset);
        if (result < ARCHIVE_OK) return result;
    }
}

}

bool ArchiveExtractor::hasCommonRootFolder(const QString& archivePath, QString* rootName)
{
    struct archive* reader = archive_read_new();
    archive_read_support_filter_all(reader);
    archive_read_support_format_all(reader);

    if (archive_read_open_filename(reader, archivePath.toLocal8Bit().constData(), 10240) != ARCHIVE_OK) {
        archive_read_free(reader);
        return false;
    }

    QSet<QString> topLevelNames;
    struct archive_entry* entry;

    while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
        QString path = QString::fromUtf8(archive_entry_pathname(entry));
        int slashIndex = path.indexOf('/');
        QString top = (slashIndex >= 0) ? path.left(slashIndex) : path;
        topLevelNames.insert(top);
        archive_read_data_skip(reader);
    }

    archive_read_free(reader);

    if (topLevelNames.size() == 1) {
        *rootName = *topLevelNames.begin();
        return true;
    }

    return false;
}

bool ArchiveExtractor::extractEntries(const QString& archivePath, const QString& destinationDir, const QString& prefix)
{
    struct archive* reader = archive_read_new();
    archive_read_support_filter_all(reader);
    archive_read_support_format_all(reader);

    if (archive_read_open_filename(reader, archivePath.toLocal8Bit().constData(), 10240) != ARCHIVE_OK) {
        archive_read_free(reader);
        return false;
    }

    struct archive* writer = archive_write_disk_new();
    archive_write_disk_set_options(writer,
        ARCHIVE_EXTRACT_TIME |
        ARCHIVE_EXTRACT_PERM |
        ARCHIVE_EXTRACT_ACL |
        ARCHIVE_EXTRACT_FFLAGS
    );

    struct archive_entry* entry;
    bool success = true;

    while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
        QString originalPath = QString::fromUtf8(archive_entry_pathname(entry));
        QString finalPath = destinationDir + "/" + prefix + originalPath;

        archive_entry_set_pathname(entry, finalPath.toLocal8Bit().constData());

        if (archive_write_header(writer, entry) != ARCHIVE_OK) {
            success = false;
            break;
        }

        if (archive_entry_size(entry) > 0) {
            if (copyData(reader, writer) != ARCHIVE_OK) {
                success = false;
                break;
            }
        }

        if (archive_write_finish_entry(writer) != ARCHIVE_OK) {
            success = false;
            break;
        }
    }

    archive_read_free(reader);
    archive_write_free(writer);

    return success;
}

QString ArchiveExtractor::extract(const QString& archivePath, const QString& modsPath, QString* errorMessage)
{
    QDir modsDir(modsPath);
    if (!modsDir.exists()) {
        modsDir.mkpath(".");
    }

    QString rootName;
    bool commonRoot = hasCommonRootFolder(archivePath, &rootName);

    QString prefix;
    QString resultFolderName;

    if (commonRoot && !rootName.isEmpty()) {
        prefix = "";
        resultFolderName = rootName;
    } else {
        QString baseName = QFileInfo(archivePath).completeBaseName();

        if (baseName.endsWith(".tar", Qt::CaseInsensitive)) {
            baseName.chop(4);
        }

        prefix = baseName + "/";
        resultFolderName = baseName;
    }

    bool success = extractEntries(archivePath, modsPath, prefix);

    if (!success) {
        if (errorMessage) *errorMessage = "Failed to extract archive.";
        return QString();
    }

    return resultFolderName;
}
