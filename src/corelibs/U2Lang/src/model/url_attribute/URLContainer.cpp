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

#include <U2Core/U2SafePoints.h>

#include <U2Lang/DbFolderScanner.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/WorkflowUtils.h>

#include "URLContainer.h"

namespace U2 {

static QString toUniversal(const QString &url) {
    return QFileInfo(url).absoluteFilePath();
}

URLContainer::URLContainer(const QString &_url, bool convertUrlToAbsolute)
    : url(convertUrlToAbsolute ? toUniversal(_url) : _url)
{

}

URLContainer::~URLContainer() {

}

const QString & URLContainer::getUrl() const {
    return url;
}

//////////////////////////////////////////////////////////////////////////

URLContainer * URLContainerFactory::createUrlContainer(const QString &url) {
    if (SharedDbUrlUtils::isDbObjectUrl(url)) {
        return new DbObjUrlContainer(url);
    } else if (SharedDbUrlUtils::isDbFolderUrl(url)) {
        return new DbFolderUrlContainer(url);
    }

    QFileInfo info(url);
    if (!info.exists()) {
        return NULL;
    }

    if (info.isDir()) {
        return new DirUrlContainer(url);
    } else {
        return new FileUrlContainer(url);
    }
}

//////////////////////////////////////////////////////////////////////////

FileUrlContainer::FileUrlContainer(const QString &url)
    : URLContainer(url)
{

}

FilesIterator * FileUrlContainer::getFileUrls() {
    return FilesIteratorFactory::createFileList(QStringList() << url);
}

URLContainer * FileUrlContainer::clone() {
    return new FileUrlContainer(url);
}

void FileUrlContainer::accept(URLContainerVisitor *visitor) {
    visitor->visit(this);
}

bool FileUrlContainer::validateUrl(NotificationsList &notificationList) {
    return WorkflowUtils::validateInputFiles(url, notificationList);
}

//////////////////////////////////////////////////////////////////////////

DbObjUrlContainer::DbObjUrlContainer(const QString &url)
    : URLContainer(url, false)
{

}

FilesIterator * DbObjUrlContainer::getFileUrls() {
    return FilesIteratorFactory::createFileList(QStringList() << url);
}

URLContainer * DbObjUrlContainer::clone() {
    return new DbObjUrlContainer(url);
}

void DbObjUrlContainer::accept(URLContainerVisitor *visitor) {
    visitor->visit(this);
}

bool DbObjUrlContainer::validateUrl(NotificationsList &notificationList) {
    return WorkflowUtils::validateInputDbObject(url, notificationList);
}

//////////////////////////////////////////////////////////////////////////

DirUrlContainer::DirUrlContainer(const QString &url)
    : URLContainer(url), recursive(false)
{

}

DirUrlContainer::DirUrlContainer(const QString &url, const QString &_incFilter, const QString &_excFilter, bool _recursive)
    : URLContainer(url), incFilter(_incFilter), excFilter(_excFilter), recursive(_recursive)
{

}

FilesIterator * DirUrlContainer::getFileUrls() {
    return FilesIteratorFactory::createDirectoryScanner(QStringList() << url, incFilter, excFilter, recursive);
}

URLContainer * DirUrlContainer::clone() {
    DirUrlContainer *cloned = new DirUrlContainer(url);
    cloned->incFilter = incFilter;
    cloned->excFilter = excFilter;
    cloned->recursive = recursive;
    return cloned;
}

void DirUrlContainer::accept(URLContainerVisitor *visitor) {
    visitor->visit(this);
}

bool DirUrlContainer::validateUrl(NotificationsList &notificationList) {
    bool res = WorkflowUtils::validateInputDirs(url, notificationList);
    if (!res) {
        return false;
    }

    FilesIterator *it = getFileUrls();
    SAFE_POINT(NULL != it, "NULL fileIterator!", false);
    while (it->hasNext()) {
        QString fileUrl = it->getNextFile();
        bool urlIsValid = WorkflowUtils::validateInputFiles(fileUrl, notificationList);
        res = res && urlIsValid;
    }
    return res;
}

const QString & DirUrlContainer::getIncludeFilter() const {
    return incFilter;
}

const QString & DirUrlContainer::getExcludeFilter() const {
    return excFilter;
}

bool DirUrlContainer::isRecursive() const {
    return recursive;
}

void DirUrlContainer::setIncludeFilter(const QString &value) {
    incFilter = value;
}

void DirUrlContainer::setExcludeFilter(const QString &value) {
    excFilter = value;
}

void DirUrlContainer::setRecursive(bool value) {
    recursive = value;
}

//////////////////////////////////////////////////////////////////////////

DbFolderUrlContainer::DbFolderUrlContainer(const QString &url)
    : URLContainer(url, false), recursive(false)
{

}

DbFolderUrlContainer::DbFolderUrlContainer(const QString &url, const QString &accFilter, const QString &objNameFilter, bool recursive)
    : URLContainer(url, false), accFilter(accFilter), objNameFilter(objNameFilter), recursive(recursive)
{

}

FilesIterator * DbFolderUrlContainer::getFileUrls() {
    return new DbFolderScanner(url, accFilter, objNameFilter, recursive);
}

URLContainer * DbFolderUrlContainer::clone() {
    return new DbFolderUrlContainer(url, accFilter, objNameFilter, recursive);
}

void DbFolderUrlContainer::accept(URLContainerVisitor *visitor) {
    visitor->visit(this);
}

bool DbFolderUrlContainer::validateUrl(NotificationsList &notificationList) {
    bool res = WorkflowUtils::validateInputDbFolders(url, notificationList);
    if (!res) {
        return false;
    }

    FilesIterator *it = getFileUrls();
    SAFE_POINT(NULL != it, "Invalid DB object iterator", false);
    while (it->hasNext()) {
        QString fileUrl = it->getNextFile();
        bool urlIsValid = WorkflowUtils::validateInputDbObject(fileUrl, notificationList);
        res = res && urlIsValid;
    }
    return res;
}

const QString & DbFolderUrlContainer::getSequenceAccFilter() const {
    return accFilter;
}

const QString & DbFolderUrlContainer::getObjNameFilter() const {
    return objNameFilter;
}

void DbFolderUrlContainer::setSequenceAccFilter(const QString &acc) {
    accFilter = acc;
}

void DbFolderUrlContainer::setObjNameFilter(const QString &name) {
    objNameFilter = name;
}

bool DbFolderUrlContainer::isRecursive() const {
    return recursive;
}

void DbFolderUrlContainer::setRecursive(bool value) {
    recursive = value;
}

URLContainerVisitor::~URLContainerVisitor() {
    // do nothing
}

} // U2
