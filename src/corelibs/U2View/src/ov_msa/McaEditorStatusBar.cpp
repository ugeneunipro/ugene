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
#include <U2Core/DbiConnection.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/SequenceObjectContext.h>

#include "McaEditor.h"
#include "McaEditorNameList.h"
#include "McaEditorReferenceArea.h"
#include "McaReferenceCharController.h"
#include "mca_reads/McaAlternativeMutationsWidget.h"
#include "view_rendering/MaEditorSequenceArea.h"

namespace U2 {

const QMap<bool, const char *> McaEditorStatusBar::MUTATION_MODE_ON_OFF_STATE_MAP = {{true, QT_TR_NOOP("Mutations mode: alternative")},
                                                                                     {false, QT_TR_NOOP("Mutations mode: normal")}};

McaEditorStatusBar::McaEditorStatusBar(MultipleAlignmentObject *mobj,
                                       MaEditorSequenceArea *seqArea,
                                       McaEditorNameList *nameList,
                                       McaReferenceCharController *refCharController)
    : MaEditorStatusBar(mobj, seqArea),
      refCharController(refCharController),
      nameList(nameList) {
    setObjectName("mca_editor_status_bar");
    setStatusBarStyle();

    mutationsStatus = new QLabel(this);

    columnLabel->setPatterns(tr("RefPos %1 / %2"),
                             tr("Reference position %1 of %2"));
    positionLabel->setPatterns(tr("ReadPos %1 / %2"),
                               tr("Read position %1 of %2"));
    selectionLabel->hide();

    connect(seqArea->getEditor()->getSelectionController(),
            SIGNAL(si_selectionChanged(MaEditorSelection, MaEditorSelection)),
            SLOT(sl_update()));

    connect(refCharController, SIGNAL(si_cacheUpdated()), SLOT(sl_update()));

    updateLabels();
    setupLayout();
}

void McaEditorStatusBar::setMutationStatus(bool isAlternativeMutationsEnabled) {
    mutationsStatus->setText(tr(MUTATION_MODE_ON_OFF_STATE_MAP[isAlternativeMutationsEnabled]));
}

void McaEditorStatusBar::setupLayout() {
    layout->addWidget(mutationsStatus);
    layout->addWidget(lineLabel);
    layout->addWidget(columnLabel);
    layout->addWidget(positionLabel);
    layout->addWidget(lockLabel);
}

void McaEditorStatusBar::updateLabels() {
    updateLineLabel();
    updatePositionLabel();
    updateMutationsLabel();

    McaEditor *editor = qobject_cast<McaEditor *>(seqArea->getEditor());
    SAFE_POINT(editor->getReferenceContext() != nullptr, "Reference context is NULL", );
    DNASequenceSelection *selection = editor->getReferenceContext()->getSequenceSelection();
    SAFE_POINT(selection != nullptr, "Reference selection is NULL", );

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
    int selectionWidth = seqArea->getEditor()->getSelection().toRect().width();
    if (selectionWidth == 1) {
        positions = getGappedPositionInfo();
    } else {
        U2Region rowsSelection = nameList->getSelection();
        if (!rowsSelection.isEmpty()) {
            const MultipleAlignmentRow row = seqArea->getEditor()->getMaObject()->getRow(rowsSelection.startPos);
            const QString rowLength = QString::number(row->getUngappedLength());
            positions = QPair<QString, QString>(NONE_MARK, rowLength);
        }
    }
    positionLabel->update(positions.first, positions.second);
    positionLabel->updateMinWidth(QString::number(aliObj->getLength()));
}

void McaEditorStatusBar::updateMutationsLabel() {
    U2OpStatus2Log os;
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(aliObj->getEntityRef().dbiRef, os));
    CHECK_OP(os, );

    auto attributeDbi = con->dbi->getAttributeDbi();
    SAFE_POINT(attributeDbi != nullptr, "attributeDbi not found", );

    auto attributeId = McaAlternativeMutationsWidget::getAlternativeMutationsCheckedId();
    auto objectAttributes = attributeDbi->getObjectAttributes(aliObj->getEntityRef().entityId, attributeId, os);
    CHECK_OP(os, );
    SAFE_POINT(objectAttributes.size() == 0 || objectAttributes.size() == 1,
               QString("Unexpected %1 objectAttributes size").arg(attributeId), );

    bool alternativeMutationsEnabled = false;
    if (objectAttributes.size() == 1) {
        auto checkedIntAttribute = attributeDbi->getIntegerAttribute(objectAttributes.first(), os);
        CHECK_OP(os, );

        alternativeMutationsEnabled = (bool)checkedIntAttribute.value;
    }

    setMutationStatus(alternativeMutationsEnabled);
}

}    // namespace U2
