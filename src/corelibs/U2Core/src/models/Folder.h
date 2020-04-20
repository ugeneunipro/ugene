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

#ifndef _U2_FOLDER_H_
#define _U2_FOLDER_H_

#include <U2Core/DocumentModel.h>

namespace U2 {

class U2CORE_EXPORT Folder : public QObject {
    Q_OBJECT
public:
    Folder();
    Folder(Document *doc, const QString &folderPath);
    Folder(const Folder &other);
    Folder & operator=(const Folder &other);

    Document * getDocument() const;
    const QString & getFolderPath() const;
    QString getParentPath() const;
    QString getFolderName() const;

    void setFolderPath(const QString &newPath);

    bool operator ==(const Folder &other) const;

    static bool isCorrectFolderName(const QString &name);
    static QString createPath(const QString &parentFolder, const QString &folderName);
    static QString getFolderParentPath(const QString &path);
    static QString getFolderName(const QString &path);
    static bool isSubFolder(const QString &path, const QString &expectedSubpath);
    static bool folderNameLessThan(const QString &first, const QString &second);

private:
    QPointer<Document> doc;
    QString folderPath;
};

class U2CORE_EXPORT FolderMimeData : public QMimeData {
    Q_OBJECT
public:
    FolderMimeData(const Folder &folder);

    Folder folder;

    // QMimeData
    bool hasFormat(const QString &mimeType) const;
    QStringList formats() const;

    static const QString MIME_TYPE;
};

} // U2

#endif // _U2_FOLDER_H_
