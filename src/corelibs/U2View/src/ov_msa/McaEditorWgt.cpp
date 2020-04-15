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

#include <QApplication>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>
#include <U2Algorithm/MSAConsensusAlgorithm.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Settings.h>

#include "MaConsensusMismatchController.h"
#include "McaEditor.h"
#include "McaEditorConsensusArea.h"
#include "McaEditorNameList.h"
#include "McaEditorOverviewArea.h"
#include "McaEditorReferenceArea.h"
#include "McaEditorSequenceArea.h"
#include "McaEditorStatusBar.h"
#include "McaEditorWgt.h"
#include "McaReferenceCharController.h"
#include "MSAEditorOffsetsView.h"
#include "helpers/McaRowHeightController.h"
#include "ov_sequence/SequenceObjectContext.h"

namespace U2 {

#define TOP_INDENT 10

McaEditorWgt::McaEditorWgt(McaEditor *editor)
    : MaEditorWgt(editor)
{
    rowHeightController = new McaRowHeightController(this);
    refCharController = new McaReferenceCharController(this, editor);

    initActions();
    initWidgets();

    refArea = new McaEditorReferenceArea(this, getEditor()->getReferenceContext());
    connect(refArea, SIGNAL(si_selectionChanged()), statusBar, SLOT(sl_update()));
    seqAreaHeaderLayout->insertWidget(0, refArea);

    MaEditorConsensusAreaSettings consSettings;
    consSettings.visibleElements = MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_RULER;
    consSettings.highlightMismatches = true;
    consArea->setDrawSettings(consSettings);

    QString name = getEditor()->getReferenceContext()->getSequenceObject()->getSequenceName();
    QWidget *refName = createHeaderLabelWidget(tr("Reference %1:").arg(name),
                                               Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter), refArea);
    refName->setObjectName("reference label container widget");

    nameAreaLayout->insertWidget(0, refName);
    nameAreaLayout->setContentsMargins(0, TOP_INDENT, 0, 0);

    // MCA editor has "always ON" collapsible mode.
    collapsibleMode = true;
    enableCollapsingOfSingleRowGroups = true;
    collapseModel->reset(editor->getMaRowIds());

    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != NULL, "AppContext::settings is NULL", );
    bool showChromatograms = s->getValue(editor->getSettingsRoot() + MCAE_SETTINGS_SHOW_CHROMATOGRAMS, true).toBool();
    collapseModel->collapseAll(!showChromatograms);
    GRUNTIME_NAMED_CONDITION_COUNTER(cvar, tvar, showChromatograms, "'Show chromatograms' is checked on the view opening", editor->getFactoryId());
    GRUNTIME_NAMED_CONDITION_COUNTER(ccvar, ttvar, !showChromatograms, "'Show chromatograms' is unchecked on the view opening", editor->getFactoryId());

    McaEditorConsensusArea* mcaConsArea = qobject_cast<McaEditorConsensusArea*>(consArea);
    SAFE_POINT(mcaConsArea != NULL, "Failed to cast consensus area to MCA consensus area", );
    seqAreaHeaderLayout->setContentsMargins(0, TOP_INDENT, 0, 0);
    seqAreaHeader->setStyleSheet("background-color: white;");
    connect(mcaConsArea->getMismatchController(), SIGNAL(si_selectMismatch(int)), refArea, SLOT(sl_selectMismatch(int)));
}

McaEditor *McaEditorWgt::getEditor() const {
    return qobject_cast<McaEditor *>(editor);
}

McaEditorConsensusArea *McaEditorWgt::getConsensusArea() const {
    return qobject_cast<McaEditorConsensusArea *>(consArea);
}

McaEditorNameList *McaEditorWgt::getEditorNameList() const {
    return qobject_cast<McaEditorNameList *>(nameList);
}

McaEditorSequenceArea* McaEditorWgt::getSequenceArea() const {
    return qobject_cast<McaEditorSequenceArea*>(seqArea);
}

McaReferenceCharController* McaEditorWgt::getRefCharController() const {
    return refCharController;
}

QAction* McaEditorWgt::getToggleColumnsAction() const {
    SAFE_POINT(offsetsView != NULL, "Offset controller is NULL", NULL);
    return offsetsView->getToggleColumnsViewAction();
}

void McaEditorWgt::initActions() {
    MaEditorWgt::initActions();

    delSelectionAction->setText(tr("Remove character/gap"));
}

void McaEditorWgt::initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) {
    seqArea = new McaEditorSequenceArea(this, shBar, cvBar);
}

void McaEditorWgt::initOverviewArea() {
    overviewArea = new McaEditorOverviewArea(this);
}

void McaEditorWgt::initNameList(QScrollBar* nhBar) {
    nameList = new McaEditorNameList(this, nhBar);
}

void McaEditorWgt::initConsensusArea() {
    consArea = new McaEditorConsensusArea(this);
}

void McaEditorWgt::initStatusBar() {
    statusBar = new McaEditorStatusBar(editor->getMaObject(), seqArea, getEditorNameList(), refCharController);
}

}   // namespace U2
