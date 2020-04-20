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

#include <U2Core/Folder.h>
#include <U2Core/U2ObjectDbi.h>

#include <U2Lang/SharedDbUrlUtils.h>

#include "ui_DbFolderOptions.h"
#include "DbFolderItem.h"

namespace U2 {

DbFolderItem::DbFolderItem(const QString &url, QListWidget *parent)
    : UrlItem(url, parent), options(new DbFolderOptions())
{
    connect(options, SIGNAL(si_dataChanged()), SIGNAL(si_dataChanged()));

    QIcon dirIcon = QIcon(QString(":U2Designer/images/database_folder.png"));
    setIcon(dirIcon);

    const QString folderPath = SharedDbUrlUtils::getDbFolderPathByUrl(url);
    setToolTip("<p><b>" + SharedDbUrlUtils::getDbShortNameFromEntityUrl(url) + "</b>: " + folderPath + "</p><p>" + tr("Use <i>right click</i> to set advanced options") + "</p>");

    QString folderName = Folder::getFolderName(folderPath);
    if (folderName.isEmpty()) {
        folderName = U2ObjectDbi::ROOT_FOLDER;
    }

    setText(folderName);
}

DbFolderItem::~DbFolderItem() {
    options->setParent(NULL);
    delete options;
}

QWidget * DbFolderItem::getOptionsWidget() {
    return options;
}

void DbFolderItem::accept(UrlItemVisitor *visitor) {
    visitor->visit(this);
}

void DbFolderItem::setRecursive(bool value) {
    options->setRecursive(value);
}

bool DbFolderItem::isRecursive() const {
    return options->isRecursive();
}

/************************************************************************/
/* DbFolderOptions */
/************************************************************************/
DbFolderOptions::DbFolderOptions(QWidget *parent)
    : QWidget(parent), ui(new Ui_DbFolderOptions)
{
    ui->setupUi(this);
    connect(ui->recursiveBox, SIGNAL(clicked(bool)), SIGNAL(si_dataChanged()));
}

DbFolderOptions::~DbFolderOptions() {
    delete ui;
}

void DbFolderOptions::setRecursive(bool value) {
    ui->recursiveBox->setChecked(value);
}

bool DbFolderOptions::isRecursive() const {
    return ui->recursiveBox->isChecked();
}

} // namespace U2
