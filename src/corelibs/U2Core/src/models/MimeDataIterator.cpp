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

#include "MimeDataIterator.h"

#include <U2Core/BunchMimeData.h>
#include <U2Core/GObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

/************************************************************************/
/* MimeDataIterator */
/************************************************************************/
MimeDataIterator::MimeDataIterator(const QMimeData* md)
    : docIdx(0), objectIdx(0), folderIdx(0) {
    auto dmd = dynamic_cast<const DocumentMimeData*>(md);
    if (dmd != nullptr) {
        docs << dmd->objPtr;
    }

    auto gomd = dynamic_cast<const GObjectMimeData*>(md);
    if (gomd != nullptr) {
        objects << gomd->objPtr;
    }

    auto fmd = dynamic_cast<const FolderMimeData*>(md);
    if (fmd != nullptr) {
        folders << fmd->folder;
    }

    auto bmd = dynamic_cast<const BunchMimeData*>(md);
    if (bmd != nullptr) {
        docs << bmd->docs;
        objects << bmd->objects;
        folders << bmd->folders;
    }
}

bool MimeDataIterator::hasNextDocument() const {
    return (docIdx < docs.size());
}

Document* MimeDataIterator::nextDocument() {
    CHECK(hasNextDocument(), nullptr);
    docIdx++;
    return docs[docIdx - 1];
}

bool MimeDataIterator::hasNextObject() const {
    return (objectIdx < objects.size());
}

GObject* MimeDataIterator::nextObject() {
    CHECK(hasNextObject(), nullptr);
    objectIdx++;
    return objects[objectIdx - 1];
}

bool MimeDataIterator::hasNextFolder() const {
    return (folderIdx < folders.size());
}

Folder MimeDataIterator::nextFolder() {
    CHECK(hasNextFolder(), Folder());
    folderIdx++;
    return folders[folderIdx - 1];
}

}  // namespace U2
