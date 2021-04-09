/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "McaEditorStatusBar.h"

#include <QHBoxLayout>

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/SequenceObjectContext.h>

#include "McaEditor.h"
#include "McaEditorNameList.h"
#include "McaEditorReferenceArea.h"
#include "McaReferenceCharController.h"
#include "view_rendering/MaEditorSequenceArea.h"

namespace U2 {

McaEditorStatusBar::McaEditorStatusBar(MultipleAlignmentObject *mobj,
                                       MaEditorSequenceArea *seqArea,
                                       McaEditorNameList *nameList,
                                       McaReferenceCharController *refCharController)
    : MaEditorStatusBar(mobj, seqArea),
      refCharController(refCharController),
      nameList(nameList) {
    setObjectName("mca_editor_status_bar");
    setStatusBarStyle();

    columnLabel->setPatterns(tr("RefPos %1 / %2"),
                             tr("Reference position %1 of %2"));
    positionLabel->setPatterns(tr("ReadPos %1 / %2"),
                               tr("Read position %1 of %2"));
    selectionLabel->hide();

    connect(nameList, SIGNAL(si_selectionChanged()), SLOT(sl_update()));
    connect(refCharController, SIGNAL(si_cacheUpdated()), SLOT(sl_update()));

    updateLabels();
    setupLayout();
}

void McaEditorStatusBar::setupLayout() {
    layout->addWidget(lineLabel);
    layout->addWidget(columnLabel);
    layout->addWidget(positionLabel);
    layout->addWidget(lockLabel);
}

void McaEditorStatusBar::updateLabels() {
    updateLineLabel();
    updatePositionLabel();

    McaEditor *editor = qobject_cast<McaEditor *>(seqArea->getEditor());
    SAFE_POINT(editor->getReferenceContext() != NULL, "Reference context is NULL", );
    DNASequenceSelection *selection = editor->getReferenceContext()->getSequenceSelection();
    SAFE_POINT(selection != NULL, "Reference selection is NULL", );

    QString ungappedRefLen = QString::number(refCharController->getUngappedLength());
    if (selection->isEmpty()) {
        columnLabel->update(NONE_MARK, ungappedRefLen);
    } else {
        int startSelection = selection->getSelectedRegions().first().startPos;
        int refPos = refCharController->getUngappedPosition(startSelection);
        columnLabel->update(refPos == -1 ? GAP_MARK : QString::number(refPos + 1), ungappedRefLen);
    }
}

void McaEditorStatusBar::updateLineLabel() {
    const U2Region selection = nameList->getSelection();
    lineLabel->update(selection.isEmpty() ? MaEditorStatusBar::NONE_MARK : QString::number(selection.startPos + 1),
                      QString::number(aliObj->getNumRows()));
}

void McaEditorStatusBar::updatePositionLabel() {
    QPair<QString, QString> positions = QPair<QString, QString>(NONE_MARK, NONE_MARK);
    if (!seqArea->getSelection().isEmpty()) {
        positions = getGappedPositionInfo();
    } else {
        const U2Region rowsSelection = nameList->getSelection();
        if (!rowsSelection.isEmpty()) {
            const MultipleAlignmentRow row = seqArea->getEditor()->getMaObject()->getRow(rowsSelection.startPos);
            const QString rowLength = QString::number(row->getUngappedLength());
            positions = QPair<QString, QString>(NONE_MARK, rowLength);
        }
    }
    positionLabel->update(positions.first, positions.second);
    positionLabel->updateMinWidth(QString::number(aliObj->getLength()));
}

}    // namespace U2
