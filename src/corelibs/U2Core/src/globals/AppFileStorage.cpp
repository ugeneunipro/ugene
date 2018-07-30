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

#include <QApplication>
#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AppFileStorage.h"

namespace U2 {

const QString StorageRoles::SORTED_BAM("SORTED_BAM");
const QString StorageRoles::IMPORTED_BAM("IMPORTED_BAM");
const QString StorageRoles::HASH("HASH");
const QString StorageRoles::SAM_TO_BAM("SAM_TO_BAM");
const QString StorageRoles::CUSTOM_FILE_TO_FILE("CUSTOM_FILE_TO_FILE");

static const QString DB_FILE_NAME("fileinfo.ugenedb");
static const QString WD_DIR_NAME("workflow_data");

namespace FileStorage {
/************************************************************************/
/* FileInfo */
/************************************************************************/
FileInfo::FileInfo(const QString &url, const QString &role, const QString &info)
    : U2Triplet(url, role, info)
{

}

FileInfo::FileInfo(const U2Triplet &triplet)
    : U2Triplet(triplet)
{

}

QString FileInfo::getFile() const {
    return this->getKey();
}

QString FileInfo::getInfo() const {
    return this->getValue();
}

bool FileInfo::isFileToFileInfo() const {
    CHECK(StorageRoles::CUSTOM_FILE_TO_FILE != getRole(), true);
    CHECK(StorageRoles::SORTED_BAM != getRole(), true);
    CHECK(StorageRoles::SAM_TO_BAM != getRole(), true);
    CHECK(StorageRoles::IMPORTED_BAM != getRole(), true);
    return false;
}

/************************************************************************/
/* WorkflowProcess */
/************************************************************************/
WorkflowProcess::WorkflowProcess(const QString &_id)
: id(_id)
{

}

WorkflowProcess::~WorkflowProcess() {
    this->unuseFiles();
}

QString WorkflowProcess::getId() const {
    return id;
}

QString WorkflowProcess::getTempDirectory() const {
    QDir dir(tempDirectory);
    if (!dir.exists()) {
        bool created = dir.mkpath(tempDirectory);
        if (!created) {
            coreLog.error(QString("Can not create a folder: %1").arg(tempDirectory));
        }
    }
    return tempDirectory;
}

void WorkflowProcess::addFile(const QString &url) {
    QFile *f = new QFile(url);
    bool opened = f->open(QIODevice::ReadOnly);
    if (!opened) {
        delete f;
        return;
    }
    usedFiles << f;
}

void WorkflowProcess::unuseFiles() {
    foreach (QFile *f, usedFiles) {
        f->close();
        delete f;
    }
    usedFiles.clear();
}

} // FileStorage

/************************************************************************/
/* AppFileStorage */
/************************************************************************/
AppFileStorage::AppFileStorage()
: storage(NULL)
{

}

void AppFileStorage::init(U2OpStatus &os) {
    UserAppsSettings *settings = AppContext::getAppSettings()->getUserAppsSettings();
    CHECK_EXT(NULL != settings, os.setError("NULL user application settings"), );

    storageDir = settings->getFileStorageDir();

    QDir dir(storageDir);
    if (!dir.exists()) {
        bool created = dir.mkpath(storageDir);
        CHECK_EXT(created, os.setError(QString("Can not create a folder: %1").arg(storageDir)), );
    }
    QString storageUrl = storageDir + "/" + DB_FILE_NAME;

    storage = new U2SQLiteTripleStore();
    storage->init(storageUrl, os);
}

AppFileStorage::~AppFileStorage() {
    if (NULL != storage) {
        U2OpStatusImpl os;
        storage->shutdown(os);
        if (os.isCoR()) {
            coreLog.error(os.getError());
        }
        delete storage;
    }
}

QString AppFileStorage::getStorageDir() const {
    return storageDir;
}

void AppFileStorage::addFileInfo(const FileStorage::FileInfo &info, FileStorage::WorkflowProcess &process, U2OpStatus &os) {
    storage->addValue(info, os);
    CHECK_OP(os, );

    if (info.isFileToFileInfo()) {
        process.addFile(info.getInfo());
    }
}

bool AppFileStorage::contains(const QString &url, const QString &role, U2OpStatus &os) const {
    return storage->contains(url, role, os);
}

QString AppFileStorage::getFileInfo(const QString &url, const QString &role, FileStorage::WorkflowProcess &process, U2OpStatus &os) const {
    QString info = storage->getValue(url, role, os);
    if (!info.isEmpty()) {
        FileStorage::FileInfo i(url, role, info);
        if (i.isFileToFileInfo()) {
            process.addFile(info);
        }
    }
    return info;
}

void AppFileStorage::addFileOwner(const FileStorage::FileInfo &info, FileStorage::WorkflowProcess &process, U2OpStatus &os) {
    bool exists = storage->contains(info, os);
    CHECK_OP(os, );
    if (exists) {
        if (info.isFileToFileInfo()) {
            process.addFile(info.getInfo());
        }
    } else {
        os.setError("The file info is not exists. The owner is not added");
    }
}

void AppFileStorage::registerWorkflowProcess(FileStorage::WorkflowProcess &process, U2OpStatus &os) {
    QString wdDirPath = storageDir + "/" + WD_DIR_NAME + "/" + process.getId();
    QDir wdDir(wdDirPath);
    bool created = wdDir.mkpath(wdDirPath);
    if (!created) {
        os.setError(QString("Can not create a folder: %1").arg(wdDirPath));
        return;
    }

    process.tempDirectory = wdDirPath;
}

bool removeFile(const QString &url) {
    if (!QFile::exists(url)) {
        return true;
    }
    bool ok = QFile::remove(url);
    if (!ok) {
        coreLog.error(QString("Can not remove a file: %1").arg(url));
    }
    return ok;
}

void removeDirIfEmpty(const QString &url) {
    QDir dir(url);
    if (dir.exists()) {
        QStringList subFiles = dir.entryList();
        subFiles.removeOne(".");
        subFiles.removeOne("..");
        if (0 == subFiles.size()) {
            dir.rmdir(url);
        }
    }
}

void AppFileStorage::unregisterWorkflowProcess(FileStorage::WorkflowProcess &process, U2OpStatus & /*os*/) {
    process.unuseFiles();

    removeDirIfEmpty(process.tempDirectory);
}

void AppFileStorage::cleanup(U2OpStatus &os) {
    QMutexLocker lock(&cleanupMutex);
    //1. Find data
    QList<U2Triplet> data = storage->getTriplets(os);
    CHECK_OP(os, );

    QList<U2Triplet> newData;
    QStringList unremovedFiles;
    //2. Remove triplets' files
    foreach (const U2Triplet &triplet, data) {
        FileStorage::FileInfo info(triplet);
        if (info.isFileToFileInfo()) {
            QString url = info.getValue();
            bool inTheStorage = url.startsWith(storageDir);
            bool removed = removeFile(url);
            if (removed) {
                removeFile(url + ".bai");
            }
            if (removed || !inTheStorage) {
                U2OpStatus2Log logOs;
                storage->removeValue(info, logOs);
            } else {
                unremovedFiles << url;
                unremovedFiles << info.getKey(); // source url for hash
            }
        } else {
            newData << triplet;
        }
    }

    //3. Remove triplets' data
    foreach (const U2Triplet &t, newData) {
        if (unremovedFiles.contains(t.getKey())) {
            continue;
        }
        U2OpStatus2Log logOs;
        storage->removeValue(t, logOs);
    }

    //4. Remove empty directories
    QDir stDir(storageDir + "/" + WD_DIR_NAME);
    foreach (const QFileInfo &info, stDir.entryInfoList()) {
        if (info.isDir()) {
            QString name = info.fileName();
            if ("." == name || ".." == name) {
                continue;
            }
            removeDirIfEmpty(info.absoluteFilePath());
        }
    }
}

QString AppFileStorage::createDirectory() const {
    QDir storageRoot(storageDir + "/" + WD_DIR_NAME);
    if (!storageRoot.exists()) {
        bool created = storageRoot.mkpath(storageRoot.path());
        SAFE_POINT(created, QString("Can not create a folder: %1").arg(storageRoot.path()), "");
    }
    uint time = QDateTime::currentDateTime().toTime_t();
    QString baseDirName = QByteArray::number(time);

    int idx = 0;
    QString result;
    bool created = false;
    do {
        result = baseDirName + "_" + QByteArray::number(idx);
        created = storageRoot.mkdir(result);
        idx++;
    } while (!created);

    return storageRoot.path() + "/" + result;
}

} // U2
