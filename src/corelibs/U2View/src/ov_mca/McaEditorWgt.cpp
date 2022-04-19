/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "McaEditorWgt.h"

#include <QApplication>

#include <U2Algorithm/MSAConsensusAlgorithm.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>

#include "MaConsensusMismatchController.h"
#include "McaEditor.h"
#include "McaEditorConsensusArea.h"
#include "McaEditorNameList.h"
#include "McaEditorOverviewArea.h"
#include "McaEditorReferenceArea.h"
#include "McaEditorSequenceArea.h"
#include "McaEditorStatusBar.h"
#include "McaReferenceCharController.h"
#include "McaRowHeightController.h"
#include "ov_msa/MSAEditorOffsetsView.h"
#include "ov_sequence/SequenceObjectContext.h"

namespace U2 {

#define TOP_INDENT 10

McaEditorWgt::McaEditorWgt(McaEditor* editor)
    : MaEditorWgt(editor) {
    rowHeightController = new McaRowHeightController(this);
    refCharController = new McaReferenceCharController(this, editor);

    initActions();
    initWidgets();

    refArea = new McaEditorReferenceArea(this, getEditor()->getReferenceContext());
    connect(refArea, SIGNAL(si_selectionChanged()), statusBar, SLOT(sl_updateStatusBar()));
    seqAreaHeaderLayout->insertWidget(0, refArea);

    MaEditorConsensusAreaSettings consSettings;
    consSettings.visibleElements = MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_RULER;
    consSettings.highlightMismatches = true;
    consensusArea->setDrawSettings(consSettings);

    QString name = getEditor()->getReferenceContext()->getSequenceObject()->getSequenceName();
    QWidget* refName = createHeaderLabelWidget(tr("Reference %1:").arg(name),
                                               Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter),
                                               refArea);
    refName->setObjectName("reference label container widget");

    nameAreaLayout->insertWidget(0, refName);
    nameAreaLayout->setContentsMargins(0, TOP_INDENT, 0, 0);

    enableCollapsingOfSingleRowGroups = true;

    McaEditorConsensusArea* mcaConsArea = qobject_cast<McaEditorConsensusArea*>(consensusArea);
    SAFE_POINT(mcaConsArea != nullptr, "Failed to cast consensus area to MCA consensus area", );
    seqAreaHeaderLayout->setContentsMargins(0, TOP_INDENT, 0, 0);
    seqAreaHeader->setStyleSheet("background-color: white;");
    connect(mcaConsArea->getMismatchController(), SIGNAL(si_selectMismatch(int)), refArea, SLOT(sl_selectMismatch(int)));
}

McaEditor* McaEditorWgt::getEditor() const {
    return qobject_cast<McaEditor*>(editor);
}

McaEditorConsensusArea* McaEditorWgt::getConsensusArea() const {
    return qobject_cast<McaEditorConsensusArea*>(consensusArea);
}

McaEditorNameList* McaEditorWgt::getEditorNameList() const {
    return qobject_cast<McaEditorNameList*>(nameList);
}

McaEditorSequenceArea* McaEditorWgt::getSequenceArea() const {
    return qobject_cast<McaEditorSequenceArea*>(sequenceArea);
}

McaReferenceCharController* McaEditorWgt::getRefCharController() const {
    return refCharController;
}

QAction* McaEditorWgt::getToggleColumnsAction() const {
    SAFE_POINT(offsetsViewController != nullptr, "Offset controller is NULL", nullptr);
    return offsetsViewController->toggleColumnsViewAction;
}

void McaEditorWgt::initActions() {
    MaEditorWgt::initActions();

    delSelectionAction->setText(tr("Remove selection"));
}

void McaEditorWgt::initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) {
    sequenceArea = new McaEditorSequenceArea(this, shBar, cvBar);
}

void McaEditorWgt::initOverviewArea(MaEditorOverviewArea *_overviewArea) {
    if (_overviewArea == nullptr) {
        overviewArea = new McaEditorOverviewArea(this);
    } else {
        overviewArea = _overviewArea;
    }
}

void McaEditorWgt::initNameList(QScrollBar* nhBar) {
    nameList = new McaEditorNameList(this, nhBar);
}

void McaEditorWgt::initConsensusArea() {
    consensusArea = new McaEditorConsensusArea(this);
}

void McaEditorWgt::initStatusBar(MaEditorStatusBar *statusbar)
{
    if (statusbar == nullptr) {
        statusBar = new McaEditorStatusBar(getEditor(), refCharController);
    } else {
        statusBar = statusbar;
    }
}

McaEditorReferenceArea* McaEditorWgt::getReferenceArea() const {
    return refArea;
}

}  // namespace U2
