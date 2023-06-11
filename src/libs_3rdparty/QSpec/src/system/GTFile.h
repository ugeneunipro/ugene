/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once
#include <QFile>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTFile {
public:
    static void setReadWrite(const QString& path, bool recursive = false);
    static void setReadOnly(const QString& path, bool recursive = false);

    // @simplified - remove all spacing characters (\t, \n, \r, \v and \f) before equaling
    static bool equals(const QString&, const QString&, bool simplified = false);
    static bool equals(const QString&);

    static qint64 getSize(const QString&);

    // backup file to the file with backupPostfix. fail the given file can't be opened
    static void backup(const QString&);

    // backup directory to the file with backupPostfix
    static void backupDir(const QString&);

    // copy file removing target file if exist
    static void copy(const QString& from, const QString& to);

    // copy directory entries removing target file if exist
    static void copyDir(const QString& dirToCopy, const QString& dirToPaste);

    /** Removes the dir if exists. */
    static void removeDir(const QString& dirName);

    // restores file and deletes backup. fail if there is no backup or can't write to the given file
    static void restore(const QString&);

    // restores file and deletes backup. fail if there is no backup or can't write to the given file
    static void restoreDir(const QString&);

    // checks if file exists
    static bool check(const QString&);

    /** Waits for the file to exist. Fails if file is not exist after 'timeout'. */
    static void waitForFile(const QString& path, int timeout = 10000);

    // creates a new empty file, rewrite the file if it already exists
    static void create(const QString& filePath);

    static QByteArray readAll(const QString& filePath);

    static const QString backupPostfix;
};

}  // namespace HI
