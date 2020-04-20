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

#include "VirtualFileSystem.h"

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2SafePoints.h>

#include <QFile>

#include <cassert>

namespace U2 {

/*******************************************
 * VirtualFileSystem
 *******************************************/

VirtualFileSystem::VirtualFileSystem() {}

VirtualFileSystem::VirtualFileSystem(const QString & fsName) : fileSystemName(fsName) {}

VirtualFileSystem::~VirtualFileSystem() {}

bool VirtualFileSystem::createFile(const QString & filename, const QByteArray & data) {
    if (files.contains(filename)) {
        return false;
    }
    files[filename] = data;
    return true;
}

bool VirtualFileSystem::mapFile(const QString & filename, const QString & filePath) {
    IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filePath));
    SAFE_POINT(iof != NULL, QString("Failed to find IO adapter factory: %1").arg(filePath), false);

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(filePath, IOAdapterMode_Read)) {
        return false;
    }

    QByteArray bytes;
    while (!io->isEof()) {
        QByteArray bytesBlock(READ_BLOCK_SZ, '\0');
        qint64 howMany = io->readBlock(bytesBlock.data(), READ_BLOCK_SZ);
        if (-1 == howMany) {
            return false;
        } else if (0 == howMany) {
            assert(io->isEof());
            continue;
        }
        bytes.append(QByteArray(bytesBlock.data(), howMany));
    }

    modifyFile(filename, bytes);
    return true;
}

bool VirtualFileSystem::mapBack(const QString & filename, const QString & filePath) const {
    if (!files.contains(filename)) {
        return false;
    }

    IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filePath));
    SAFE_POINT(iof != NULL, QString("Failed to find IO adapter factory: %1").arg(filePath), false);

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(filePath, IOAdapterMode_Write)) {
        return false;
    }

    io->writeBlock(files[filename]);
    return true;
}

void VirtualFileSystem::modifyFile(const QString & filename, const QByteArray & data) {
    files[filename] = data;
}

QByteArray VirtualFileSystem::removeFile(const QString & filename) {
    return files.take(filename);
}

void VirtualFileSystem::removeAllFiles() {
    QStringList keys = files.keys();
    foreach(const QString & key, keys) {
        removeFile(key);
    }
}

bool VirtualFileSystem::fileExists(const QString & filename) const {
    return files.contains(filename);
}

QByteArray & VirtualFileSystem::getFileByName(const QString & filename) {
    return files[filename];
}

QByteArray VirtualFileSystem::getFileByName(const QString & filename) const {
    return files.value(filename);
}

void VirtualFileSystem::setId(const QString & id) {
    fileSystemName = id;
}

QString VirtualFileSystem::getId() const {
    return fileSystemName;
}

QStringList VirtualFileSystem::getAllFilenames() const {
    return files.keys();
}

/*******************************************
* VirtualFileSystemRegistry
*******************************************/

VirtualFileSystemRegistry::VirtualFileSystemRegistry() {}

VirtualFileSystemRegistry::~VirtualFileSystemRegistry() {
    qDeleteAll(registry.values());
}

bool VirtualFileSystemRegistry::registerFileSystem(VirtualFileSystem * entry) {
    SAFE_POINT(entry != NULL, "FS is NULL!", false);

    QString id = entry->getId();
    if (registry.contains(id)) {
        return false;
    }
    registry[id] = entry;
    return true;
}

VirtualFileSystem * VirtualFileSystemRegistry::unregisterFileSystem(const QString & id) {
    return registry.take(id);
}

VirtualFileSystem * VirtualFileSystemRegistry::getFileSystemById(const QString & id) const {
    return registry[id];
}

QList< VirtualFileSystem* > VirtualFileSystemRegistry::getAllFileSystems() const {
    return registry.values();
}

QStringList VirtualFileSystemRegistry::getAllIds() const {
    return registry.keys();
}

} // U2
