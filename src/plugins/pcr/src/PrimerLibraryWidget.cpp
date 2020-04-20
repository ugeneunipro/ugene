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

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>

#include "EditPrimerDialog.h"
#include "PrimerLibrary.h"
#include "PrimerLibraryTable.h"
#include "PrimerLibraryTableController.h"
#include "PrimerLibraryWidget.h"
#include "export/ExportPrimersDialog.h"
#include "import/ImportPrimersDialog.h"
#include "import/ImportPrimersMultiTask.h"

#define CHECK_OP_UI(os, result) \
    if (os.hasError()) { \
    QMessageBox::warning(this, QCoreApplication::translate("Global", "Error"), os.getError()); \
    } \
    CHECK_OP(os, result);

namespace U2 {

PrimerLibraryWidget::PrimerLibraryWidget(QWidget *parent)
: QWidget(parent), editPrimerButton(NULL), removePrimersButton(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749035");

    QPushButton *newPrimerButton = buttonBox->addButton(tr("New primer"), QDialogButtonBox::ActionRole);
    connect(newPrimerButton, SIGNAL(clicked()), SLOT(sl_newPrimer()));

    editPrimerButton = buttonBox->addButton(tr("Edit primer"), QDialogButtonBox::ActionRole);
    connect(editPrimerButton, SIGNAL(clicked()), SLOT(sl_editPrimer()));

    removePrimersButton = buttonBox->addButton(tr("Remove primer(s)"), QDialogButtonBox::ActionRole);
    connect(removePrimersButton, SIGNAL(clicked()), SLOT(sl_removePrimers()));

    QPushButton *importPrimersButton = buttonBox->addButton(tr("Import primer(s)"), QDialogButtonBox::ActionRole);
    connect(importPrimersButton, SIGNAL(clicked()), SLOT(sl_importPrimers()));

    exportPrimersButton = buttonBox->addButton(tr("Export primer(s)"), QDialogButtonBox::ActionRole);
    connect(exportPrimersButton, SIGNAL(clicked()), SLOT(sl_exportPrimers()));

    connect(buttonBox, SIGNAL(rejected()), SIGNAL(si_close()));

    new PrimerLibraryTableController(this, primerTable);
    connect(primerTable, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(sl_editPrimer()));
    connect(primerTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(sl_selectionChanged()));
    sl_selectionChanged();
}

void PrimerLibraryWidget::sl_newPrimer() {
    QObjectScopedPointer<EditPrimerDialog> dlg = new EditPrimerDialog(this);
    const int result = dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(QDialog::Accepted == result, );

    U2OpStatusImpl os;
    PrimerLibrary *library = PrimerLibrary::getInstance(os);
    CHECK_OP_UI(os, );

    Primer primer = dlg->getPrimer();
    library->addRawPrimer(primer, os);
    CHECK_OP_UI(os, );
}

void PrimerLibraryWidget::sl_editPrimer() {
    QList<Primer> selection = primerTable->getSelection();
    CHECK(1 == selection.size(), );
    Primer primerToEdit = selection.first();
    QObjectScopedPointer<EditPrimerDialog> dlg = new EditPrimerDialog(this, primerToEdit);
    const int result = dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(QDialog::Accepted == result, );

    U2OpStatusImpl os;
    PrimerLibrary *library = PrimerLibrary::getInstance(os);
    CHECK_OP_UI(os, );

    Primer primer = dlg->getPrimer();
    primer.id = primerToEdit.id;
    library->updateRawPrimer(primer, os);
    CHECK_OP_UI(os, );
}

void PrimerLibraryWidget::sl_removePrimers() {
    const QList<Primer> primers = primerTable->getSelection();

    U2OpStatusImpl os;
    PrimerLibrary *library = PrimerLibrary::getInstance(os);
    CHECK_OP_UI(os, );

    foreach (const Primer &primer, primers) {
        library->removePrimer(primer, os);
        CHECK_OP_UI(os, );
    }
}

void PrimerLibraryWidget::sl_importPrimers() {
    QObjectScopedPointer<ImportPrimersDialog> importDialog = new ImportPrimersDialog(this);
    importDialog->exec();
}

void PrimerLibraryWidget::sl_exportPrimers() {
    const QList<Primer> selection = primerTable->getSelection();
    SAFE_POINT(!selection.isEmpty(), L10N::nullPointerError("Selection"), );
    QObjectScopedPointer<ExportPrimersDialog> exportDialog = new ExportPrimersDialog(selection, this);
    exportDialog->exec();
}

void PrimerLibraryWidget::sl_selectionChanged() {
    QList<Primer> selection = primerTable->getSelection();
    editPrimerButton->setEnabled(1 == selection.size());
    removePrimersButton->setDisabled(selection.isEmpty());
    exportPrimersButton->setDisabled(selection.isEmpty());
}

} // U2
