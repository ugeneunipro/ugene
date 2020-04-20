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

#ifndef _U2_FILES_ITERATOR_
#define _U2_FILES_ITERATOR_

#include <QDir>

#include <U2Core/global.h>

namespace U2 {

class FilesIterator;

class U2CORE_EXPORT FilesIteratorFactory {
public:
    static FilesIterator * createDirectoryScanner(const QStringList &dirs, const QString &includeFilter,
        const QString &excludeFilter, bool recursive);

    static FilesIterator * createFileList(const QStringList &files);
};

class U2CORE_EXPORT FilesIterator {
public:
    FilesIterator() {}
    virtual ~FilesIterator() {}

    /** Returns empty string if there are no more files */
    virtual QString getNextFile() = 0;

    virtual bool hasNext() = 0;
};

/************************************************************************/
/* Iterators */
/************************************************************************/
class U2CORE_EXPORT DirectoryScanner : public FilesIterator {
public:
    DirectoryScanner(const QStringList &dirs, const QString &includeFilter, const QString &excludeFilter, bool recursive);

    virtual QString getNextFile();
    virtual bool hasNext();

private:
    QString includeFilter;
    QString excludeFilter;
    bool recursive;

    QStringList usedDirs;
    QFileInfoList unusedDirs;
    QRegExp incRx;
    QRegExp excRx;

    QStringList readyResults;

private:
    QFileInfoList scanDirectory(const QDir &dir);
    bool isPassedByFilters(const QString &fileName) const;
};

class U2CORE_EXPORT FileList : public FilesIterator {
public:
    FileList(const QStringList &files);

    virtual QString getNextFile();
    virtual bool hasNext();

private:
    QStringList files;
};

} // U2

#endif // _U2_FILES_ITERATOR_
