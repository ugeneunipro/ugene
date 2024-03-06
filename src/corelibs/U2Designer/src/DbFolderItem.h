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

#include "UrlItem.h"

class Ui_DbFolderOptions;

namespace U2 {

class DbFolderOptions;

class DbFolderItem : public UrlItem {
    Q_OBJECT
public:
    DbFolderItem(const QString& url, QListWidget* parent = nullptr);
    ~DbFolderItem();

    void accept(UrlItemVisitor* visitor) override;
    QWidget* getOptionsWidget() override;

    void setRecursive(bool value);
    bool isRecursive() const;

private:
    DbFolderOptions* options;
};

class DbFolderOptions : public QWidget {
    Q_OBJECT
public:
    DbFolderOptions(QWidget* parent = nullptr);
    ~DbFolderOptions();

    void setRecursive(bool value);
    bool isRecursive() const;

signals:
    void si_dataChanged();

private:
    Ui_DbFolderOptions* ui;
};

}  // namespace U2
