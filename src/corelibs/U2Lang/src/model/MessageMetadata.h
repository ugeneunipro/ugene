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

#ifndef _U2_MESSAGE_METADATA_H_
#define _U2_MESSAGE_METADATA_H_

#include <QMap>

#include <U2Core/global.h>

namespace U2 {

class U2LANG_EXPORT MessageMetadata {
public:
    MessageMetadata();
    MessageMetadata(const QString &datasetName);
    MessageMetadata(const QString &fileUrl, const QString &datasetName);
    MessageMetadata(const QString &databaseUrl, const QString &databaseId, const QString &datasetName);

    int getId() const;
    const QString & getDatasetName() const;
    const QString & getFileUrl() const;
    const QString & getDatabaseUrl() const;
    const QString & getDatabaseId() const;

    static const int INVALID_ID;

private:
    static int nextId();

private:
    int id;
    QString datasetName;
    QString fileUrl;
    QString databaseUrl;
    QString databaseId;
};

class U2LANG_EXPORT MessageMetadataStorage {
public:
    void put(const MessageMetadata &value);
    MessageMetadata get(int metadataId) const;

private:
    QMap<int, MessageMetadata> data;
};

} // U2

#endif // _U2_MESSAGE_METADATA_H_
