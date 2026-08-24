#pragma once

#include <QString>

class ArchiveExtractor
{
public:
    static QString extract(const QString& archivePath, const QString& modsPath, QString* errorMessage = nullptr);

private:
    static bool hasCommonRootFolder(const QString& archivePath, QString* rootName);
    static bool extractEntries(const QString& archivePath, const QString& destinationDir, const QString& prefix);
};
