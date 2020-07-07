/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "FileAndDirectoryUtils.h"

#include <QDir>

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

static const QString OUTPUT_SUBDIR = "run";

namespace U2 {

int FileAndDirectoryUtils::MIN_LENGTH_TO_WRITE = 32768;

#if defined(Q_OS_LINUX) | defined(Q_OS_MAC)
const QString FileAndDirectoryUtils::HOME_DIR_IDENTIFIER = "~/";
#else
const QString FileAndDirectoryUtils::HOME_DIR_IDENTIFIER = "%UserProfile%/";
#endif

QString FileAndDirectoryUtils::getFormatId(const FormatDetectionResult &r) {
    if (NULL != r.format) {
        return r.format->getFormatId();
    }
    if (NULL != r.importer) {
        return r.importer->getId();
    }
    return "";
}

QString FileAndDirectoryUtils::getWorkingDir(const QString &fileUrl, int dirMode, const QString &customDir, const QString &workingDir) {
    QString result;

    bool useInternal = false;

    if (dirMode == FILE_DIRECTORY) {
        result = GUrl(fileUrl).dirPath() + "/";
    } else if (dirMode == CUSTOM) {
        if (!customDir.isEmpty()) {
            result = customDir;
            if (!result.endsWith("/")) {
                result += "/";
            }
        } else {
            algoLog.error("Result folder is empty, default workflow folder is used");
            useInternal = true;
        }
    } else {
        useInternal = true;
    }

    if (useInternal) {
        result = workingDir;
        if (!result.endsWith("/")) {
            result += "/";
        }
        result += OUTPUT_SUBDIR;
        if (!result.endsWith("/")) {
            result += "/";
        }
        if (dirMode == WORKFLOW_INTERNAL_CUSTOM) {
            if (!customDir.isEmpty()) {
                result += customDir;
            }
            if (!result.endsWith("/")) {
                result += "/";
            }
        }
    }
    return result;
}

QString FileAndDirectoryUtils::createWorkingDir(const QString &fileUrl, int dirMode, const QString &customDir, const QString &workingDir) {
    const QString result = getWorkingDir(fileUrl, dirMode, customDir, workingDir);
    QDir dir(result);
    if (!dir.exists(result)) {
        dir.mkdir(result);
    }
    return result;
}

QString FileAndDirectoryUtils::detectFormat(const QString &url) {
    FormatDetectionConfig cfg;
    cfg.bestMatchesOnly = false;
    cfg.useImporters = true;
    cfg.excludeHiddenFormats = false;

    const QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, cfg);
    if (formats.empty()) {
        return "";
    }

    return getFormatId(formats.first());
}

bool FileAndDirectoryUtils::isFileEmpty(const QString &url) {
    QFile file(url);
    if (!file.exists()) {
        return true;
    }
    if (file.size() == 0) {
        return true;
    }
    return false;
}

void FileAndDirectoryUtils::dumpStringToFile(QFile *f, QString &str) {
    if (Q_LIKELY(f == NULL || str.length() <= MIN_LENGTH_TO_WRITE)) {
        return;
    }
    f->write(str.toLocal8Bit());
    str.clear();
}

QString FileAndDirectoryUtils::getAbsolutePath(const QString &filePath) {
    CHECK(!filePath.isEmpty(), filePath);
    QString result = QDir::fromNativeSeparators(filePath);
    if (result.startsWith(HOME_DIR_IDENTIFIER, Qt::CaseInsensitive)) {
        result.remove(0, HOME_DIR_IDENTIFIER.length() - 1);
        result.prepend(QDir::homePath());
    }
    return QFileInfo(result).absoluteFilePath();
}

bool FileAndDirectoryUtils::isFilepathCorrect(const QString &filePath) {
#ifdef Q_OS_WIN
    QString path = filePath;
    // Anything following the raw filename prefix should be legal.
    if (path.left(4) == "\\\\?\\")
        return true;

    // Windows filenames are not case sensitive.
    path = path.toUpper();

    // Trim the drive letter off
    if (path[1] == ':' && (path[0] >= 'A' && path[0] <= 'Z'))
        path = path.right(path.length() - 2);

    QString illegal = "<>:\"|?*";

    foreach (const QChar &c, path) {
        // Check for control characters
        if (c.toLatin1() > 0 && c.toLatin1() < 32)
            return false;

        // Check for illegal characters
        if (illegal.contains(c))
            return false;
    }

    // Check for device names in filenames
    static QStringList devices;

    if (!devices.count())
        devices << "CON"
                << "PRN"
                << "AUX"
                << "NUL"
                << "COM0"
                << "COM1"
                << "COM2"
                << "COM3"
                << "COM4"
                << "COM5"
                << "COM6"
                << "COM7"
                << "COM8"
                << "COM9"
                << "LPT0"
                << "LPT1"
                << "LPT2"
                << "LPT3"
                << "LPT4"
                << "LPT5"
                << "LPT6"
                << "LPT7"
                << "LPT8"
                << "LPT9";

    const QFileInfo fi(path);
    const QString basename = fi.baseName();

    foreach (const QString &s, devices)
        if (basename == s)
            return false;

    // Check for trailing periods or spaces
    if (path.right(1) == "." || path.right(1) == " ")
        return false;

    // Check for pathnames that are too long (disregarding raw pathnames)
    if (path.length() > 260)
        return false;

    // Exclude raw device names
    if (path.left(4) == "\\\\.\\")
        return false;

    // Since we are checking for a filename, it mustn't be a directory
    if (path.right(1) == "\\")
        return false;

    return true;
#else
    return QRegExp("'^[^*&%\s]+$'").exactMatch(filePath);
#endif    // Q_OS_WIN
}

}    // namespace U2
