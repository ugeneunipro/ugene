/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "VFSAdapter.h"

#include <U2Core/AppContext.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/VirtualFileSystem.h>


namespace U2 {

VFSAdapterFactory::VFSAdapterFactory(QObject* o) : IOAdapterFactory(o) {
    name = tr("Memory buffer");
}

IOAdapter* VFSAdapterFactory::createIOAdapter() {
    return new VFSAdapter(this);
}

VFSAdapter::VFSAdapter(VFSAdapterFactory* factory, QObject* o) : IOAdapter(factory, o), url("", GUrl_VFSFile), buffer(NULL) {
}


bool VFSAdapter::open(const GUrl& _url, IOAdapterMode m) {
    SAFE_POINT(!isOpen(), "Adapter is already opened!", false);
    SAFE_POINT(buffer == NULL, "Buffers is not null!", false);

    QString vfsPrefix = U2_VFS_URL_PREFIX;
    // assume that all membuf adapters work with files in some vfs
    if( !_url.getURLString().startsWith(vfsPrefix) ) {
        return false; // not a file in vfs
    }
    VirtualFileSystemRegistry * vfsReg = AppContext::getVirtualFileSystemRegistry();
    SAFE_POINT(vfsReg != NULL, "VirtualFileSystemRegistry not found!", false);

    QStringList urlArgs = _url.getURLString().mid(vfsPrefix.size()).split(U2_VFS_FILE_SEPARATOR, QString::SkipEmptyParts);
    if ( 2 != urlArgs.size() ) { // urlArgs - vfsname and filename
        return false;
    }
    VirtualFileSystem * vfs = vfsReg->getFileSystemById( urlArgs[0] );
    if( NULL == vfs ) {
        return false; // no such vfs registered
    }

    if( !vfs->fileExists( urlArgs[1] ) ) {
        if( IOAdapterMode_Read == m ) {
            return false;
        } else {
            vfs->createFile( urlArgs[1], QByteArray() );
        }
    }

    buffer = new QBuffer( &vfs->getFileByName( urlArgs[1] ) );
    QIODevice::OpenMode iomode = m == IOAdapterMode_Read ? QIODevice::ReadOnly : QIODevice::WriteOnly | QIODevice::Truncate;
    if (!buffer->open(iomode)) {
        return false;
    }
    url = _url;
    return true;
}

void VFSAdapter::close() {
    SAFE_POINT(isOpen(), "Adapter is not opened!",);

    delete buffer;
    buffer = NULL;
    url = GUrl("", GUrl_VFSFile);
}

qint64 VFSAdapter::readBlock(char* data, qint64 size) {
    qint64 l = buffer->read(data, size);
    if (formatMode == TextMode) {
        l = TextUtils::cutByteOrderMarks(data, l);
    }
    return l;
}

qint64 VFSAdapter::writeBlock(const char* data, qint64 size) {
    qint64 l = buffer->write(data, size);
    return l;
}

bool VFSAdapter::skip(qint64 nBytes) {
    SAFE_POINT(isOpen(), "Adapter is not opened!", false);

    qint64 p = buffer->pos();
    return buffer->seek(p+nBytes);
}

qint64 VFSAdapter::left() const {
    SAFE_POINT(isOpen(), "Adapter is not opened!", -1);

    qint64 p = buffer->pos();
    qint64 len = buffer->size();
    return len - p;
}

int VFSAdapter::getProgress() const {
    SAFE_POINT(isOpen(), "Adapter is not opened!", -1);
    return int(100 * float(buffer->pos()) / buffer->size());
}

qint64 VFSAdapter::bytesRead() const {
    return buffer->pos();
}

QString VFSAdapter::errorString() const{
    return buffer->errorString();
}


};//namespace
