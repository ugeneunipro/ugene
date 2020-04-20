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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "SequenceSelectorWidgetController.h"
#include "./view_rendering/MaEditorSelection.h"

const int CURSOR_START_POSITION = 0;

namespace U2 {

SequenceSelectorWidgetController::SequenceSelectorWidgetController(MSAEditor* _msa)
    : msa(_msa), defaultSeqName(""), seqId(U2MsaRow::INVALID_ROW_ID)
{
    setupUi(this);
    filler = new MSACompletionFiller();

    seqLineEdit->setText(msa->getReferenceRowName());
    seqLineEdit->setCursorPosition(CURSOR_START_POSITION);
    seqLineEdit->setObjectName("sequenceLineEdit");
    seqId = msa->getReferenceRowId();
    completer = new BaseCompleter(filler, seqLineEdit);
    updateCompleter();

    connect(addSeq, SIGNAL(clicked()), SLOT(sl_addSeqClicked()));
    connect(deleteSeq, SIGNAL(clicked()), SLOT(sl_deleteSeqClicked()));

    connect(msa->getMaObject(), SIGNAL(si_alignmentChanged(const MultipleAlignment& , const MaModificationInfo&)),
        SLOT(sl_seqLineEditEditingFinished(const MultipleAlignment& , const MaModificationInfo&)));

    connect(completer, SIGNAL(si_editingFinished()), SLOT(sl_seqLineEditEditingFinished()));

    connect(completer, SIGNAL(si_completerClosed()), SLOT(sl_setDefaultLineEditValue()));
}

SequenceSelectorWidgetController::~SequenceSelectorWidgetController() {
    delete completer;
}

QString SequenceSelectorWidgetController::text() const {
    return seqLineEdit->text();
}

void SequenceSelectorWidgetController::setSequenceId(qint64 newId) {
    U2OpStatusImpl os;
    if (newId == U2MsaRow::INVALID_ROW_ID) {
        seqId = newId;
        return;
    }
    const MultipleSequenceAlignmentRow &selectedRow = msa->getMaObject()->getMsa()->getMsaRowByRowId(newId, os);
    CHECK_OP(os, );
    seqId = newId;
    const QString selectedName = selectedRow->getName();
    if (seqLineEdit->text() != selectedName) {
        seqLineEdit->setText(selectedName);
        seqLineEdit->setCursorPosition(CURSOR_START_POSITION);
        defaultSeqName = selectedName;
    }
}

qint64 SequenceSelectorWidgetController::sequenceId( ) const {
    return seqId;
}

void SequenceSelectorWidgetController::updateCompleter() {
    QStringList newNamesList = msa->getMaObject()->getMultipleAlignment()->getRowNames();
    filler->updateSeqList(newNamesList);
    if (!newNamesList.contains(seqLineEdit->text())) {
        sl_seqLineEditEditingFinished();
    }
}

void SequenceSelectorWidgetController::sl_seqLineEditEditingFinished(const MultipleAlignment& , const MaModificationInfo& modInfo){
    if(!modInfo.rowListChanged) {
        return;
    }
    filler->updateSeqList(msa->getMaObject()->getMultipleAlignment()->getRowNames());
    sl_seqLineEditEditingFinished();
}

void SequenceSelectorWidgetController::sl_seqLineEditEditingFinished() {
    const MultipleSequenceAlignment ma = msa->getMaObject()->getMultipleAlignment();
    if (!ma->getRowNames().contains(seqLineEdit->text())) {
        seqLineEdit->setText(defaultSeqName);
    } else {
        const QString selectedSeqName = seqLineEdit->text();
        if (defaultSeqName != selectedSeqName) {
            defaultSeqName = seqLineEdit->text();
            seqLineEdit->setCursorPosition(CURSOR_START_POSITION);
        }
        // index in popup list
        const int sequenceIndex = completer->getLastChosenItemIndex();
        if ( completer == QObject::sender( ) && -1 != sequenceIndex ) {
            const QStringList rowNames = ma->getRowNames( );
            SAFE_POINT( rowNames.contains( selectedSeqName ), "Unexpected sequence name is selected", );
            if ( 1 < rowNames.count( selectedSeqName ) ) { // case when there are sequences with identical names
                int selectedRowIndex = -1;
                // search for chosen row in the msa
                for ( int sameNameCounter = 0; sameNameCounter <= sequenceIndex; ++sameNameCounter ) {
                    selectedRowIndex = rowNames.indexOf( selectedSeqName, selectedRowIndex + 1 );
                }
                seqId = ma->getMsaRow( selectedRowIndex )->getRowId( );
            } else { // case when chosen name is unique in the msa
                seqId = ma->getMsaRow( selectedSeqName )->getRowId( );
            }
        }
    }
    emit si_selectionChanged();
}

void SequenceSelectorWidgetController::sl_addSeqClicked() {
    if (msa->isAlignmentEmpty()) {
        return;
    }

    const MultipleSequenceAlignmentRow selectedRow = msa->getRowByLineNumber(msa->getSelection().y());
    setSequenceId(selectedRow->getRowId());
    emit si_selectionChanged();
}

void SequenceSelectorWidgetController::sl_deleteSeqClicked() {
    seqLineEdit->setText("");
    defaultSeqName = "";
    setSequenceId(U2MsaRow::INVALID_ROW_ID);
    emit si_selectionChanged();
}

void SequenceSelectorWidgetController::sl_setDefaultLineEditValue() {
    seqLineEdit->setText(defaultSeqName);
    seqLineEdit->clearFocus();
}

}
