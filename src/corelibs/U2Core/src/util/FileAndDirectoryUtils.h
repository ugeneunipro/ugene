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

#ifndef _U2_FILE_AND_DIRECTORY_UTILS_H_
#define _U2_FILE_AND_DIRECTORY_UTILS_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>

namespace U2 {

class U2CORE_EXPORT FileAndDirectoryUtils {
public:
    enum OutDirectory {
        FILE_DIRECTORY = 0,
        WORKFLOW_INTERNAL,
        CUSTOM,
        WORKFLOW_INTERNAL_CUSTOM
    };

    static QString getWorkingDir(const QString& fileUrl, int dirMode, const QString& customDir, const QString& workingDir);
    static QString createWorkingDir(const QString& fileUrl, int dirMode, const QString& customDir, const QString& workingDir);
    static QString detectFormat(const QString &url);
    static bool isFileEmpty(const QString& url);
    static void dumpStringToFile(QFile *f, QString &str); //Be aware: string will be cleared after dumping
    static QString getAbsolutePath(const QString& filePath);


private:
    static QString getFormatId(const FormatDetectionResult &r);

    static int MIN_LENGTH_TO_WRITE;
    static const QString HOME_DIR_IDENTIFIER;
};

} // U2

#endif // _U2_FILE_AND_DIRECTORY_UTILS_H_
