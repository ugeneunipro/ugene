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

#include "ui_NewBreakpointDialog.h"
#include "NewBreakpointDialog.h"
#include <U2Gui/HelpButton.h>
#include <QPushButton>

namespace U2 {

NewBreakpointDialog::NewBreakpointDialog(const QStringList &elementsNames, QWidget *parent,
    Qt::WindowFlags f) : QDialog(parent, f), ui(new Ui_NewBreakpointDialog())
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "28967089");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    ui->elementCombo->addItems(elementsNames);

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    connect(okButton, SIGNAL(clicked()), SLOT(sl_announceNewBreakpoint()));

}

NewBreakpointDialog::~NewBreakpointDialog() {
    delete ui;
}

void NewBreakpointDialog::sl_announceNewBreakpoint() {
    emit si_newBreakpointCreated(ui->elementCombo->currentText());
}

} // namespace U2
