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

#ifndef _U2_URL_ITEM_H_
#define _U2_URL_ITEM_H_

#include <QListWidgetItem>

namespace U2 {

class UrlItemVisitor;

class UrlItem : public QObject, public QListWidgetItem {
    Q_OBJECT
public:
    UrlItem(const QString &url, QListWidget *parent = NULL);
    virtual ~UrlItem() {}

    virtual void accept(UrlItemVisitor *visitor) = 0;
    virtual QWidget * getOptionsWidget();

signals:
    void si_dataChanged();
};

////////////////////////////////////////////////////////////////////////////////

class DbFolderItem;
class DbObjectItem;
class DirectoryItem;
class FileItem;

class UrlItemVisitor {
public:
    virtual void visit(DirectoryItem *item) = 0;
    virtual void visit(FileItem *item) = 0;
    virtual void visit(DbObjectItem *item) = 0;
    virtual void visit(DbFolderItem *item) = 0;
};

} // U2

#endif // _U2_URL_ITEM_H_
