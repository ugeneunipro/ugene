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

#include "MessageMetadata.h"

namespace U2 {

const int MessageMetadata::INVALID_ID = -1;

MessageMetadata::MessageMetadata()
: id(INVALID_ID)
{

}

MessageMetadata::MessageMetadata(const QString &datasetName)
: id(nextId()), datasetName(datasetName)
{

}

MessageMetadata::MessageMetadata(const QString &fileUrl, const QString &datasetName)
: id(nextId()), datasetName(datasetName), fileUrl(fileUrl)
{

}

MessageMetadata::MessageMetadata(const QString &databaseUrl, const QString &databaseId, const QString &datasetName)
: id(nextId()), datasetName(datasetName), databaseUrl(databaseUrl), databaseId(databaseId)
{

}

int MessageMetadata::getId() const {
    return id;
}

const QString & MessageMetadata::getDatasetName() const {
    return datasetName;
}

const QString & MessageMetadata::getFileUrl() const {
    return fileUrl;
}

const QString & MessageMetadata::getDatabaseUrl() const {
    return databaseUrl;
}

const QString & MessageMetadata::getDatabaseId() const {
    return databaseId;
}

int MessageMetadata::nextId() {
    static QAtomicInt id(0);
    return id.fetchAndAddRelaxed(1);
}

/************************************************************************/
/* MessageMetadataStorage */
/************************************************************************/
void MessageMetadataStorage::put(const MessageMetadata &value) {
    data[value.getId()] = value;
}

MessageMetadata MessageMetadataStorage::get(int metadataId) const {
    return data.value(metadataId, MessageMetadata());
}

} // U2
