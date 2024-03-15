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

#pragma once

#include <QStringList>

#include <U2Core/FilesIterator.h>
#include <U2Core/global.h>

#include <U2Lang/SupportClass.h>

namespace U2 {

class URLContainerVisitor;

class U2LANG_EXPORT URLContainer {
public:
    URLContainer(const QString& url, bool convertUrlToAbsolute = true);
    virtual ~URLContainer();

    const QString& getUrl() const;

    virtual FilesIterator* getFileUrls() = 0;
    virtual URLContainer* clone() = 0;
    virtual void accept(URLContainerVisitor* visitor) = 0;

    /** Validate the URL is present and readable */
    virtual bool validateUrl(NotificationsList& notificationList) = 0;

protected:
    QString url;
};

class U2LANG_EXPORT URLContainerFactory {
public:
    static URLContainer* createUrlContainer(const QString& url);
};

////////////////////////////////////////////////////////////////////////////////
class U2LANG_EXPORT FileUrlContainer : public URLContainer {
public:
    FileUrlContainer(const QString& url);

    FilesIterator* getFileUrls() override;
    URLContainer* clone() override;
    void accept(URLContainerVisitor* visitor) override;

    bool validateUrl(NotificationsList& notificationList) override;
};

class U2LANG_EXPORT DbObjUrlContainer : public URLContainer {
public:
    DbObjUrlContainer(const QString& url);

    FilesIterator* getFileUrls() override;
    URLContainer* clone() override;
    void accept(URLContainerVisitor* visitor) override;

    bool validateUrl(NotificationsList& notificationList) override;
};

class U2LANG_EXPORT DirUrlContainer : public URLContainer {
public:
    DirUrlContainer(const QString& url);
    DirUrlContainer(const QString& url, const QString& incFilter, const QString& excFilter, bool recursive);

    FilesIterator* getFileUrls() override;
    URLContainer* clone() override;
    void accept(URLContainerVisitor* visitor) override;

    /**
     * Validates filtered files inside the folder
     * are present and accessible for reading.
     */
    bool validateUrl(NotificationsList& notificationList) override;

    const QString& getIncludeFilter() const;
    const QString& getExcludeFilter() const;
    bool isRecursive() const;

    void setIncludeFilter(const QString& value);
    void setExcludeFilter(const QString& value);
    void setRecursive(bool value);

private:
    QString incFilter;
    QString excFilter;
    bool recursive;
};

class U2LANG_EXPORT DbFolderUrlContainer : public URLContainer {
public:
    DbFolderUrlContainer(const QString& url);
    DbFolderUrlContainer(const QString& url, const QString& accFilter, const QString& objNameFilter, bool recursive);

    FilesIterator* getFileUrls() override;
    URLContainer* clone() override;
    void accept(URLContainerVisitor* visitor) override;

    bool validateUrl(NotificationsList& notificationList) override;

    void setSequenceAccFilter(const QString& acc);
    void setObjNameFilter(const QString& name);

    const QString& getSequenceAccFilter() const;
    const QString& getObjNameFilter() const;

    bool isRecursive() const;
    void setRecursive(bool value);

private:
    QString accFilter;
    QString objNameFilter;
    bool recursive;
};

////////////////////////////////////////////////////////////////////////////////
class U2LANG_EXPORT URLContainerVisitor {
public:
    virtual ~URLContainerVisitor();

    virtual void visit(FileUrlContainer*) = 0;
    virtual void visit(DirUrlContainer*) = 0;
    virtual void visit(DbObjUrlContainer*) = 0;
    virtual void visit(DbFolderUrlContainer*) = 0;
};

}  // namespace U2
