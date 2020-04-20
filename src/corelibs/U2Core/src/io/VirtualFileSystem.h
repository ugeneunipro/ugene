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


#ifndef _U2_VIRTUAL_FILE_SYSTEM_H_
#define _U2_VIRTUAL_FILE_SYSTEM_H_

#include <U2Core/global.h>


#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>

namespace U2 {

/*
 * VirtualFileSystem represents files mapped to memory
 * usage: map files to memory then use MemBufAdapter to read them
 */
class U2CORE_EXPORT VirtualFileSystem {
public:
    // if you want MemBufAdapter reading-writing to your vfs: make url of your file as U2_VFS_URL_PREFIX+vfsname+U2_VFS_FILE_SEPARATOR+filename
    static const int        READ_BLOCK_SZ = 4096;

public:
    VirtualFileSystem();
    VirtualFileSystem( const QString & fsName );
    ~VirtualFileSystem();

    // if file exist in vfs: return false, do nothing
    bool createFile( const QString & filename, const QByteArray & data );

    // maps filePath file from disk to memory and adds it to vfs
    // if filename exists in vfs: replace prev
    bool mapFile( const QString & filename, const QString & filePath );
    // maps back to disk (from memory)
    bool mapBack( const QString & filename, const QString & filePath ) const;
    // if file does not exist: create new file
    void modifyFile( const QString & filename, const QByteArray & data );

    QByteArray removeFile(  const QString & filename );
    void removeAllFiles();

    bool fileExists( const QString & filename ) const;
    QByteArray & getFileByName( const QString & filename );
    QByteArray getFileByName( const QString & filename ) const;

    void setId( const QString & id );
    QString getId() const;
    QStringList getAllFilenames() const;

private:
    QString fileSystemName; // unique id of fs
    QMap< QString, QByteArray > files;

}; // VirtualFileSystem

class U2CORE_EXPORT VirtualFileSystemRegistry {
public:
    VirtualFileSystemRegistry();
    ~VirtualFileSystemRegistry();

    bool registerFileSystem( VirtualFileSystem * entry );
    VirtualFileSystem * unregisterFileSystem( const QString & id );

    VirtualFileSystem * getFileSystemById( const QString & id ) const;
    QList< VirtualFileSystem* > getAllFileSystems() const;
    QStringList getAllIds() const;

private:
    QMap< QString, VirtualFileSystem* > registry;

}; // VirtualFileSystemRegistry

} // U2

#endif // _U2_VIRTUAL_FILE_SYSTEM_H_
