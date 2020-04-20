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

#ifndef _U2_PROJECT_UTILS_H_
#define _U2_PROJECT_UTILS_H_

#include <U2Core/global.h>

namespace U2 {

class Document;
class Folder;
class LoadUnloadedDocumentTask;

class U2GUI_EXPORT ProjectUtils {
public:
    /** Returns true if the document is a shared database */
    static QList<Document *> getConnectedSharedDatabases();
    static bool areSharedDatabasesAvailable();
    static bool isConnectedDatabaseDoc(const Document *doc);
    static bool isDatabaseDoc(const Document *doc);

    static bool isFolderInRecycleBin(const QString &folderPath);
    static bool isFolderInRecycleBinSubtree(const QString &folderPath);

    static bool isSystemFolder(const QString &folderPath);

    static Document * findDocument(const QString &url);
    static bool hasLoadedDocument(const QString &url);
    static bool hasUnloadedDocument(const QString &url);
    static LoadUnloadedDocumentTask * findLoadTask(const QString &url);

    static const int MAX_OBJS_TO_SHOW_LOAD_PROGRESS;
    static const QString RECYCLE_BIN_FOLDER_PATH;
};

} // U2

#endif // _U2_PROJECTUTILS_H_
