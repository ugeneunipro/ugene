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

#include "FilesIterator.h"

namespace U2 {

FilesIterator * FilesIteratorFactory::createDirectoryScanner(const QStringList &dirs, const QString &includeFilter,
    const QString &excludeFilter, bool recursive) {
    return new DirectoryScanner(dirs, includeFilter, excludeFilter, recursive);
}

FilesIterator * FilesIteratorFactory::createFileList(const QStringList &files) {
    return new FileList(files);
}

/************************************************************************/
/* DirectoryScanner */
/************************************************************************/
DirectoryScanner::DirectoryScanner(const QStringList &dirs, const QString &_includeFilter, const QString &_excludeFilter, bool _recursive)
: FilesIterator(), includeFilter(_includeFilter), excludeFilter(_excludeFilter), recursive(_recursive),
incRx(includeFilter), excRx(excludeFilter)
{
    foreach (const QString &dirPath, dirs) {
        unusedDirs << QFileInfo(dirPath);
    }
    incRx.setPatternSyntax(QRegExp::Wildcard);
    excRx.setPatternSyntax(QRegExp::Wildcard);
}

QString DirectoryScanner::getNextFile() {
    while (readyResults.isEmpty() && !unusedDirs.isEmpty()) {
        QFileInfo entry = unusedDirs.takeFirst();
        if (usedDirs.contains(entry.absoluteFilePath())) {
            continue;
        }

        QDir dir(entry.absoluteFilePath());
        QFileInfoList files = scanDirectory(dir);
        foreach (const QFileInfo &path, files) {
            if (isPassedByFilters(path.fileName())) {
                readyResults << path.absoluteFilePath();
            }
        }
        usedDirs << dir.absolutePath();
    }

    if (!readyResults.isEmpty()) {
        return readyResults.takeFirst();
    } else {
        return "";
    }
}

bool DirectoryScanner::isPassedByFilters(const QString &fileName) const {
    bool passed = true;
    if (!includeFilter.isEmpty()) {
        passed = incRx.exactMatch(fileName);
    }
    if (!excludeFilter.isEmpty()) {
        passed = passed && !excRx.exactMatch(fileName);
    }

    return passed;
}

bool DirectoryScanner::hasNext() {
    QString next = getNextFile();
    if (next.isEmpty()) {
        return false;
    } else {
        readyResults.insert(0, next);
        return true;
    }
}

QFileInfoList DirectoryScanner::scanDirectory(const QDir &dir) {
    QFileInfoList result;
    if (!dir.exists()) {
        return result;
    }

    QFileInfoList nestedDirs;
    QFileInfoList entries = dir.entryInfoList();
    foreach (const QFileInfo &entry, entries) {
        if (entry.isDir()) {
            if ("." == entry.fileName() || ".." == entry.fileName()) {
                continue;
            }
            if (entry.isSymLink()) {
                nestedDirs << QFileInfo(entry.symLinkTarget());
            } else {
                nestedDirs << entry;
            }
        } else if (entry.isFile()) {
            result << entry;
        }
    }
    if (recursive) {
        unusedDirs << nestedDirs;
    }

    return result;
}

/************************************************************************/
/* FileList */
/************************************************************************/
FileList::FileList(const QStringList &_files)
: FilesIterator(), files(_files)
{

}

QString FileList::getNextFile() {
    if (files.isEmpty()) {
        return "";
    }
    return files.takeFirst();
}

bool FileList::hasNext() {
    return !files.isEmpty();
}

} // U2
