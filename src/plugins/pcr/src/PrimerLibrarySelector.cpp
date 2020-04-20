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

#include <U2Core/Counter.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include "PrimerLibrarySelector.h"

namespace U2 {

PrimerLibrarySelector::PrimerLibrarySelector(QWidget *parent)
: QDialog(parent)
{
    GCOUNTER(cvar, tvar, "PrimerLibrarySelector");
    setupUi(this);
    new HelpButton(this, buttonBox, "24749032");

    connect(primerTable, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(accept()));
    connect(primerTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(sl_selectionChanged()));
    sl_selectionChanged();

    primerTable->setMode(PrimerLibraryTable::Selector);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    CHECK(NULL != okButton, );
    okButton->setText(tr("Choose"));
}

Primer PrimerLibrarySelector::getResult() const {
    QList<Primer> selection = primerTable->getSelection();
    SAFE_POINT(1 == selection.size(), L10N::internalError(), Primer());
    return selection.first();
}

void PrimerLibrarySelector::sl_selectionChanged() {
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    SAFE_POINT(NULL != okButton, L10N::nullPointerError("OK button"), );

    QList<Primer> selection = primerTable->getSelection();
    okButton->setDisabled(selection.isEmpty());
}

} // U2
