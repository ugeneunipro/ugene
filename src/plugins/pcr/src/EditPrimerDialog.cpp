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
#include <QValidator>

#include <U2Core/Counter.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include "EditPrimerDialog.h"

namespace U2 {

EditPrimerDialog::EditPrimerDialog(QWidget *parent)
: QDialog(parent)
{
    init();
}

EditPrimerDialog::EditPrimerDialog(QWidget *parent, const Primer &editToPrimer)
: QDialog(parent)
{
    init();
    setWindowTitle(tr("Edit Primer"));
    primerEdit->setText(editToPrimer.sequence);
    nameEdit->setText(editToPrimer.name);
}

void EditPrimerDialog::init() {
    GCOUNTER(cvar, tvar, "Add primer in library");
    setupUi(this);
    new HelpButton(this, buttonBox, "24749035");

    primerEdit->setValidator(new PrimerValidator(this));

    connect(primerEdit, SIGNAL(textEdited(const QString &)), SLOT(sl_onPrimerChanged(const QString &)));

    connect(primerEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_validate()));
    connect(nameEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_validate()));

    sl_validate();
}

Primer EditPrimerDialog::getPrimer() const {
    Primer result;
    result.name = nameEdit->text();
    result.sequence = primerEdit->text();
    return result;
}

void EditPrimerDialog::sl_onPrimerChanged(const QString &primerSequence) {
    int curPos = primerEdit->cursorPosition();
    primerEdit->setText(primerSequence.toUpper());
    primerEdit->setCursorPosition(curPos);
}

void EditPrimerDialog::sl_validate() {
    CHECK_EXT(!primerEdit->text().isEmpty(), validate(false), );
    CHECK_EXT(!nameEdit->text().isEmpty(), validate(false), );
    validate(true);
}

void EditPrimerDialog::validate(bool isValid) {
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    SAFE_POINT(NULL != okButton, L10N::nullPointerError("OK button"), );
    okButton->setEnabled(isValid);
}

} // U2
