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

#include "Folder.h"

#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

Folder::Folder()
    : QObject() {
}

Folder::Folder(Document* doc, const QString& folderPath)
    : QObject(), doc(doc), folderPath(folderPath) {
}

Folder::Folder(const Folder& other)
    : QObject(), doc(other.doc), folderPath(other.folderPath) {
}

Folder& Folder::operator=(const Folder& other) {
    doc = other.doc;
    folderPath = other.folderPath;
    return *this;
}

Document* Folder::getDocument() const {
    return doc.data();
}

const QString& Folder::getFolderPath() const {
    return folderPath;
}

QString Folder::getParentPath() const {
    return getFolderParentPath(folderPath);
}

QString Folder::getFolderName() const {
    return getFolderName(folderPath);
}

bool Folder::operator==(const Folder& other) const {
    return other.getDocument() == doc && other.getFolderPath() == folderPath;
}

QString Folder::createPath(const QString& parentFolder, const QString& folderName) {
    QString path = parentFolder;
    if (U2ObjectDbi::ROOT_FOLDER != path) {
        path += U2ObjectDbi::PATH_SEP;
    }
    return path + folderName;
}

QString Folder::getFolderParentPath(const QString& path) {
    QString name = getFolderName(path);
    CHECK(!name.isEmpty(), "");

    QString result = path.left(path.size() - name.size());
    if (U2ObjectDbi::ROOT_FOLDER != result) {
        result = result.left(result.size() - 1);
    }
    return result;
}

QString Folder::getFolderName(const QString& path) {
    SAFE_POINT(!path.isEmpty(), "Can't extract the folder name from the empty path", "");
    QStringList pathList = path.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
    if (pathList.isEmpty()) {
        return U2ObjectDbi::ROOT_FOLDER;
    }
    return pathList.last();
}

bool Folder::isSubFolder(const QString& path, const QString& expectedSubpath) {
    return expectedSubpath.startsWith(path + U2ObjectDbi::PATH_SEP);
}

bool Folder::folderNameLessThan(const QString& first, const QString& second) {
    return QString::compare(first, second, Qt::CaseInsensitive) < 0;
}

/************************************************************************/
/* FolderMimeData */
/************************************************************************/
const QString FolderMimeData::MIME_TYPE("application/x-ugene-folder-mime");

FolderMimeData::FolderMimeData(const Folder& folder)
    : QMimeData(), folder(folder) {
}

bool FolderMimeData::hasFormat(const QString& mimeType) const {
    return mimeType == MIME_TYPE;
}

QStringList FolderMimeData::formats() const {
    return QStringList(MIME_TYPE);
}

}  // namespace U2
