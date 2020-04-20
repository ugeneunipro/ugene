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

#include <QPushButton>

#include <U2Core/Folder.h>
#include <U2Core/U2SafePoints.h>

#include "FolderNameDialog.h"

namespace U2 {

FolderNameDialog::FolderNameDialog(const QString &name, QWidget *parent)
    : QDialog(parent), okButton(NULL)
{
    setupUi(this);
    setWindowTitle(name.isEmpty() ? tr("Add Folder") : tr("Rename Folder"));

    connect(nameEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_textChanged(const QString &)));

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    nameEdit->setText(name);
    sl_textChanged(name);
}

void FolderNameDialog::sl_textChanged(const QString &text) {
    CHECK(NULL != okButton, );
    okButton->setEnabled(Folder::isCorrectFolderName(text));
}

QString FolderNameDialog::getResult() const {
    return nameEdit->text();
}

} // U2
