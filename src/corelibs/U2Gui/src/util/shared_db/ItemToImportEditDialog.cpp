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

#include <U2Gui/HelpButton.h>

#include "ImportOptionsWidget.h"
#include "ItemToImportEditDialog.h"
#include "ui_ItemToImportEditDialog.h"

namespace U2 {

ItemToImportEditDialog::ItemToImportEditDialog(const QString &item, const QString &folder, const ImportToDatabaseOptions& options, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_ItemToImportEditDialog)
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24749086");
    init(item, folder, options);
}

ItemToImportEditDialog::~ItemToImportEditDialog() {
    delete ui;
}

QString ItemToImportEditDialog::getFolder() const {
    return ui->optionsWidget->getFolder();
}

ImportToDatabaseOptions ItemToImportEditDialog::getOptions() const {
    return ui->optionsWidget->getOptions();
}

void ItemToImportEditDialog::init(const QString &item, const QString &folder, const ImportToDatabaseOptions& options) {
    ui->optionsWidget->init(folder, options);
    ui->leItem->setText(item);
}

}   // namespace U2
