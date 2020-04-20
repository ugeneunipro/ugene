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

#ifndef _U2_DIRECTORY_ITEM_H_
#define _U2_DIRECTORY_ITEM_H_

#include "UrlItem.h"

#include "ui_DirectoryOptions.h"

namespace U2 {

class DirectoryOptions;

class DirectoryItem : public UrlItem {
    Q_OBJECT
public:
    DirectoryItem(const QString &url, QListWidget *parent = NULL);
    virtual ~DirectoryItem();

    virtual void accept(UrlItemVisitor *visitor);
    virtual QWidget * getOptionsWidget();

    void setIncludeFilter(const QString &value);
    void setExcludeFilter(const QString &value);
    void setRecursive(bool value);

    QString getIncludeFilter() const;
    QString getExcludeFilter() const;
    bool isRecursive() const;

private:
    DirectoryOptions *options;
};

class DirectoryOptions : public QWidget, public Ui_DirectoryOptions {
    Q_OBJECT
public:
    DirectoryOptions(QWidget *parent = NULL);

    void setIncludeFilter(const QString &value);
    void setExcludeFilter(const QString &value);
    void setRecursive(bool value);

    QString getIncludeFilter() const;
    QString getExcludeFilter() const;
    bool isRecursive() const;

signals:
    void si_dataChanged();
};

} // U2

#endif // _U2_DIRECTORY_ITEM_H_
