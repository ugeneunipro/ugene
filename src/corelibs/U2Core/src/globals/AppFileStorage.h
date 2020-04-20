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

#ifndef _U2_APP_FILE_STORAGE_
#define _U2_APP_FILE_STORAGE_

#include <QMutex>
#include <QFile>

#include <U2Core/global.h>
#include <U2Core/U2OpStatus.h>
#include "../dbi/U2SQLiteTripleStore.h"

namespace U2 {

class AppFileStorage;

namespace FileStorage {

class U2CORE_EXPORT FileInfo : public U2Triplet {
public:
    FileInfo(const QString &url, const QString &role, const QString &info);
    FileInfo(const U2Triplet &triplet);

    QString getFile() const;
    QString getInfo() const;

    bool isFileToFileInfo() const;
};

class U2CORE_EXPORT WorkflowProcess {
    friend class U2::AppFileStorage;
public:
    WorkflowProcess(const QString &id);
    virtual ~WorkflowProcess();

    QString getId() const;
    QString getTempDirectory() const;

private:
    QString id;
    QString tempDirectory;
    QList<QFile*> usedFiles;

    void addFile(const QString &url);
    void unuseFiles();
};

} // FileStorage

/** A triplestore for information about files produced by UGENE */
class U2CORE_EXPORT AppFileStorage {
public:
    AppFileStorage();
    virtual ~AppFileStorage();

    void init(U2OpStatus &os);

    /**
     * A file is used by some processes (e.g. workflows). These processes are the owners of the file.
     * It is impossible to delete file and corresponding data if it is used by some owner.
     * One file could have several owners (see addFileOwner)
     */
    void addFileInfo(const FileStorage::FileInfo &info, FileStorage::WorkflowProcess &process, U2OpStatus &os);
    void addFileOwner(const FileStorage::FileInfo &info, FileStorage::WorkflowProcess &process, U2OpStatus &os);

    bool contains(const QString &url, const QString &role, U2OpStatus &os) const;
    QString getFileInfo(const QString &url, const QString &role, FileStorage::WorkflowProcess &process, U2OpStatus &os) const;

    QString getStorageDir() const;

    /** Sets tempDirectory for the @process*/
    void registerWorkflowProcess(FileStorage::WorkflowProcess &process, U2OpStatus &os);
    void unregisterWorkflowProcess(FileStorage::WorkflowProcess &process, U2OpStatus &os);

    /**
     * Removes all files from the storage folder and all records from the storage
     * which are not used by registered workflow processes (active owners)
     * These active owners could be created not only by current process,
     * but by some other UGENE processes
     */
    void cleanup(U2OpStatus &os);

    /**
     * Returns the absolute path to the new folder in the storage folder.
     * The result path does not contain "/" symbol in the end
     */
    QString createDirectory() const;

private:
    U2SQLiteTripleStore *storage;
    QString storageDir;

    QMutex cleanupMutex;
}; // AppFileStorage

/** Describes role types for the storage */
class U2CORE_EXPORT StorageRoles {
public:
    /** Source BAM/SAM file url - SORTED - Sorted BAM file url */
    static const QString SORTED_BAM;

    /** Source BAM/SAM file url - IMPORTED - ugenedb url */
    static const QString IMPORTED_BAM;

    /** Source file url - HASH - hash string */
    static const QString HASH;

    /** Source SAM file url - SAM_TO_BAM - destination BAM file url */
    static const QString SAM_TO_BAM;

    /** Source custom file url - CUSTOM_FILE_TO_FILE_ROLE - destination custom file url */
    static const QString CUSTOM_FILE_TO_FILE;
}; // StorageRoles

} // U2

#endif // _U2_APP_FILE_STORAGE_
