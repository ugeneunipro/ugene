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

#include <QCheckBox>
#include <QMessageBox>

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include "MaEditor.h"
#include "./view_rendering/MaEditorSelection.h"
#include "MSASelectSubalignmentDialog.h"

namespace U2 {


SelectSubalignmentDialog::SelectSubalignmentDialog(MaEditor *editor, const U2Region &region, const QList<int> &_selectedIndexes, QWidget *p)
    : QDialog(p),
      editor(editor),
      window(region),
      selectedIndexes(_selectedIndexes) {
    SAFE_POINT(editor != NULL, L10N::nullPointerError("MaEditor"), );

    if (region.isEmpty() && selectedIndexes.isEmpty()) {
        int startSeq = -1;
        int endSeq = -1;
        int startPos = -1;
        int endPos = -1;
        const MaEditorSelection& selection = editor->getSelection();
        if (selection.isEmpty()) {
            startPos = 0;
            endPos = editor->getAlignmentLen();
            startSeq = 0;
            endSeq = editor->getNumSequences();
        } else {
            startSeq = selection.y();
            endSeq = selection.y() + selection.height();
            startPos = selection.x();
            endPos = selection.x() + selection.width();
        }
        window = U2Region(startPos, endPos - startPos);
        for (int i = startSeq; i <= endSeq; i++) {
            selectedIndexes << i;
        }
    }
    init();
}

void SelectSubalignmentDialog::accept() {
    int start = startLineEdit->value() - 1;
    int end = endLineEdit->value() - 1;
    int seqLen = editor->getAlignmentLen();

    CHECK_EXT( start <= end,
               QMessageBox::critical(this, windowTitle(), tr("Start position must be less than end position!")), );

    U2Region region(start, end - start + 1);
    U2Region sequence(0, seqLen);
    CHECK_EXT( sequence.contains(region),
               QMessageBox::critical(this, windowTitle(), tr("Entered region not contained in current sequence")), );

    selectedNames.clear();
    selectedIndexes.clear();
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        if(cb->isChecked()){
            selectedNames.append(cb->text());
            selectedIndexes.append(i);
        }
    }
    CHECK_EXT( !selectedIndexes.isEmpty(),
               QMessageBox::critical(this, windowTitle(), tr("No sequences selected")), );

    window = region;
    QDialog::accept();
}

void SelectSubalignmentDialog::sl_allButtonClicked(){
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        cb->setChecked(true);
    }
}

void SelectSubalignmentDialog::sl_invertButtonClicked(){
    for (int i=0; i<sequencesTableWidget->rowCount(); i++) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        cb->setChecked(!cb->isChecked());
    }
}

void SelectSubalignmentDialog::sl_noneButtonClicked(){
    for (int i=0; i<sequencesTableWidget->rowCount(); i++) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        cb->setChecked(false);
    }
}

void SelectSubalignmentDialog::init() {
    SAFE_POINT(editor != NULL, tr("Ma Editor is NULL"), );

    setupUi(this);
    new HelpButton(this, buttonBox, "24748869");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Select"));

    connect(allButton, SIGNAL(clicked()), SLOT(sl_allButtonClicked()));
    connect(noneButton, SIGNAL(clicked()), SLOT(sl_noneButtonClicked()));
    connect(invertButton, SIGNAL(clicked()), SLOT(sl_invertButtonClicked()));

    MultipleAlignmentObject *mobj = editor->getMaObject();
    SAFE_POINT(mobj != NULL, tr("MSA Object is NULL"), );

    int rowNumber = mobj->getNumRows();
    int alignLength = mobj->getLength();

    sequencesTableWidget->clearContents();
    sequencesTableWidget->setRowCount(rowNumber);
    sequencesTableWidget->setColumnCount(1);
    sequencesTableWidget->verticalHeader()->setHidden( true );
    sequencesTableWidget->horizontalHeader()->setHidden( true );
    sequencesTableWidget->setShowGrid(false);
    sequencesTableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );

    startLineEdit->setMaximum(alignLength);
    endLineEdit->setMaximum(alignLength);

    startLineEdit->setValue(window.startPos + 1);
    endLineEdit->setValue(window.endPos());

    for (int i = 0; i < rowNumber; i++) {
        QCheckBox *cb = new QCheckBox(mobj->getMultipleAlignment()->getRow(i)->getName(), this);
        cb->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        if (selectedIndexes.contains(i)) {
            cb->setChecked(true);
        }
        sequencesTableWidget->setCellWidget(i, 0, cb);
        sequencesTableWidget->setRowHeight(i, 15);
    }
}


} // namespace
