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

#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>

#include "MaConsensusMismatchController.h"
#include "MaEditorFactory.h"
#include "McaEditor.h"
#include "McaEditorConsensusArea.h"
#include "McaEditorNameList.h"
#include "McaEditorSequenceArea.h"
#include "ExportConsensus/MaExportConsensusTabFactory.h"
#include "General/McaGeneralTabFactory.h"
#include "helpers/MaAmbiguousCharactersController.h"
#include "ov_sequence/SequenceObjectContext.h"
#include "Overview/MaEditorOverviewArea.h"
#include "view_rendering/SequenceWithChromatogramAreaRenderer.h"
#include "MSAEditorOffsetsView.h"

namespace U2 {

McaEditor::McaEditor(const QString &viewName,
                     MultipleChromatogramAlignmentObject *obj)
    : MaEditor(McaEditorFactory::ID, viewName, obj),
      referenceCtx(NULL)
{
    GCOUNTER(cvar, tvar, "Sanger Reads Editor");
    initZoom();
    initFont();

    U2OpStatusImpl os;
    foreach (const MultipleChromatogramAlignmentRow& row, obj->getMca()->getMcaRows()) {
        chromVisibility.insert(obj->getMca()->getRowIndexByRowId(row->getRowId(), os), true);
    }

    U2SequenceObject* referenceObj = obj->getReferenceObj();
    SAFE_POINT(NULL != referenceObj, "Trying to open McaEditor without a reference", );
    referenceCtx = new SequenceObjectContext(referenceObj, this);
}

MultipleChromatogramAlignmentObject *McaEditor::getMaObject() const {
    return qobject_cast<MultipleChromatogramAlignmentObject*>(maObject);
}

McaEditorWgt *McaEditor::getUI() const {
    return qobject_cast<McaEditorWgt *>(ui);
}

void McaEditor::buildStaticToolbar(QToolBar* tb) {
    tb->addAction(showChromatogramsAction);
    tb->addAction(showOverviewAction);
    tb->addSeparator();

    tb->addAction(zoomInAction);
    tb->addAction(zoomOutAction);
    tb->addAction(resetZoomAction);
    tb->addSeparator();

    GObjectView::buildStaticToolbar(tb);
}

void McaEditor::buildStaticMenu(QMenu* menu) {
    addAlignmentMenu(menu);
    addAppearanceMenu(menu);
    addNavigationMenu(menu);
    addEditMenu(menu);
    menu->addSeparator();
    menu->addAction(showConsensusTabAction);
    menu->addSeparator();

    GObjectView::buildStaticMenu(menu);
    GUIUtils::disableEmptySubmenus(menu);
}

int McaEditor::getRowContentIndent(int rowId) const {
    if (isChromVisible(rowId)) {
        return SequenceWithChromatogramAreaRenderer::INDENT_BETWEEN_ROWS / 2;
    }
    return MaEditor::getRowContentIndent(rowId);
}

bool McaEditor::isChromVisible(qint64 rowId) const {
    return isChromVisible(getMaObject()->getRowPosById(rowId));
}

bool McaEditor::isChromVisible(int rowIndex) const {
    return !ui->getCollapseModel()->isGroupWithMaRowIndexCollapsed(rowIndex);
}

bool McaEditor::isChromatogramButtonChecked() const {
    return showChromatogramsAction->isChecked();
}

QString McaEditor::getReferenceRowName() const {
    return getMaObject()->getReferenceObj()->getSequenceName();
}

char McaEditor::getReferenceCharAt(int pos) const {
    U2OpStatus2Log os;
    SAFE_POINT(getMaObject()->getReferenceObj()->getSequenceLength() > pos, "Invalid position", '\n');
    QByteArray seqData = getMaObject()->getReferenceObj()->getSequenceData(U2Region(pos, 1), os);
    CHECK_OP(os, U2Msa::GAP_CHAR);
    return seqData.isEmpty() ? U2Msa::GAP_CHAR : seqData.at(0);
}

SequenceObjectContext* McaEditor::getReferenceContext() const {
    return referenceCtx;
}

void McaEditor::sl_onContextMenuRequested(const QPoint & /*pos*/) {
    QMenu menu;
    buildStaticMenu(&menu);
    emit si_buildPopupMenu(this, &menu);
    menu.exec(QCursor::pos());
}

void McaEditor::sl_showHideChromatograms(bool show) {
    GRUNTIME_NAMED_COUNTER(cvat, tvar, "'Show chromatogram' action triggered", getFactoryId());
    ui->getCollapseModel()->collapseAll(!show);
    sl_saveChromatogramState();
    emit si_completeUpdate();
}

void McaEditor::sl_showGeneralTab() {
    OptionsPanel* optionsPanel = getOptionsPanel();
    SAFE_POINT(NULL != optionsPanel, "Internal error: options panel is NULL"
        " when msageneraltab opening was initiated", );
    optionsPanel->openGroupById(McaGeneralTabFactory::getGroupId());
}

void McaEditor::sl_showConsensusTab() {
    OptionsPanel* optionsPanel = getOptionsPanel();
    SAFE_POINT(NULL != optionsPanel, "Internal error: options panel is NULL"
        " when msaconsensustab opening was initiated", );
    optionsPanel->openGroupById(McaExportConsensusTabFactory::getGroupId());
}

QWidget* McaEditor::createWidget() {
    Q_ASSERT(ui == NULL);
    ui = new McaEditorWgt(this);

    QString objName = "mca_editor_" + maObject->getGObjectName();
    ui->setObjectName(objName);

    connect(ui , SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));

    initActions();

    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry *opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_ChromAlignmentEditor));

    QList<OPWidgetFactory*> opWidgetFactories = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory *factory, opWidgetFactories) {
        optionsPanel->addGroup(factory);
    }

    qDeleteAll(filters);

    updateActions();

    return ui;
}

void McaEditor::initActions() {
    MaEditor::initActions();

    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != NULL, "AppContext::settings is NULL", );

    zoomInAction->setText(tr("Zoom in"));
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    GUIUtils::updateActionToolTip(zoomInAction);
    ui->addAction(zoomInAction);

    zoomOutAction->setText(tr("Zoom out"));
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    GUIUtils::updateActionToolTip(zoomOutAction);
    ui->addAction(zoomOutAction);

    resetZoomAction->setText(tr("Reset zoom"));
    resetZoomAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    GUIUtils::updateActionToolTip(resetZoomAction);
    ui->addAction(resetZoomAction);

    showChromatogramsAction = new QAction(QIcon(":/core/images/graphs.png"), tr("Show chromatograms"), this);
    showChromatogramsAction->setObjectName("chromatograms");
    showChromatogramsAction->setCheckable(true);
    connect(showChromatogramsAction, SIGNAL(triggered(bool)), SLOT(sl_showHideChromatograms(bool)));
    showChromatogramsAction->setChecked(s->getValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_CHROMATOGRAMS, true).toBool());
    ui->addAction(showChromatogramsAction);

    showGeneralTabAction = new QAction(tr("Open \"General\" tab on the options panel"), this);
    connect(showGeneralTabAction, SIGNAL(triggered()), SLOT(sl_showGeneralTab()));
    ui->addAction(showGeneralTabAction);

    showConsensusTabAction = new QAction(tr("Open \"Consensus\" tab on the options panel"), this);
    connect(showConsensusTabAction, SIGNAL(triggered()), SLOT(sl_showConsensusTab()));
    ui->addAction(showConsensusTabAction);

    showOverviewAction->setText(tr("Show overview"));
    showOverviewAction->setObjectName("overview");
    connect(showOverviewAction, SIGNAL(triggered(bool)), SLOT(sl_saveOverviewState()));
    bool overviewVisible = s->getValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_OVERVIEW, true).toBool();
    showOverviewAction->setChecked(overviewVisible);
    ui->getOverviewArea()->setVisible(overviewVisible);
    changeFontAction->setText(tr("Change characters font..."));
    GRUNTIME_NAMED_CONDITION_COUNTER(cvar, tvar, overviewVisible, "'Show overview' is checked on the view opening", getFactoryId());
    GRUNTIME_NAMED_CONDITION_COUNTER(ccvar, ttvar, !overviewVisible, "'Show overview' is unchecked on the view opening", getFactoryId());
}

void McaEditor::sl_saveOverviewState() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != NULL, "AppContext::settings is NULL", );
    GRUNTIME_NAMED_COUNTER(cvat, tvar, "'Show overview' action triggered", getFactoryId());
    s->setValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_OVERVIEW, showOverviewAction->isChecked());
}

void McaEditor::sl_saveChromatogramState() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != NULL, "AppContext::settings is NULL", );
    s->setValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_CHROMATOGRAMS, showChromatogramsAction->isChecked());
}

void McaEditor::addAlignmentMenu(QMenu *menu) {
    QMenu *alignmentMenu = menu->addMenu(tr("Alignment"));
    alignmentMenu->menuAction()->setObjectName(MCAE_MENU_ALIGNMENT);

    alignmentMenu->addAction(showGeneralTabAction);
}

void McaEditor::addAppearanceMenu(QMenu *menu) {
    QMenu* appearanceMenu = menu->addMenu(tr("Appearance"));
    appearanceMenu->menuAction()->setObjectName(MCAE_MENU_APPEARANCE);

    auto ui = getUI();
    auto sequenceArea = ui->getSequenceArea();
    auto offsetsController = ui->getOffsetsViewController();

    appearanceMenu->addAction(showChromatogramsAction);
    appearanceMenu->addMenu(sequenceArea->getTraceActionsMenu());
    appearanceMenu->addAction(showOverviewAction);
    if (offsetsController != NULL) {
        appearanceMenu->addAction(offsetsController->getToggleColumnsViewAction());
    }
    appearanceMenu->addAction(ui->getToggleColumnsAction());
    appearanceMenu->addSeparator();

    appearanceMenu->addAction(zoomInAction);
    appearanceMenu->addAction(zoomOutAction);
    appearanceMenu->addAction(resetZoomAction);
    appearanceMenu->addSeparator();

    appearanceMenu->addAction(sequenceArea->getIncreasePeaksHeightAction());
    appearanceMenu->addAction(sequenceArea->getDecreasePeaksHeightAction());
    appearanceMenu->addSeparator();

    appearanceMenu->addAction(changeFontAction);
    appearanceMenu->addSeparator();

    appearanceMenu->addAction(clearSelectionAction);
}

void McaEditor::addNavigationMenu(QMenu *menu) {
    QMenu *navigationMenu = menu->addMenu(tr("Navigation"));
    navigationMenu->menuAction()->setObjectName(MCAE_MENU_NAVIGATION);

    auto ui = getUI();
    auto ambiguousCharactersController = ui->getSequenceArea()->getAmbiguousCharactersController();
    navigationMenu->addAction(ambiguousCharactersController->getPreviousAction());
    navigationMenu->addAction(ambiguousCharactersController->getNextAction());
    navigationMenu->addSeparator();

    auto mismatchController = ui->getConsensusArea()->getMismatchController();
    navigationMenu->addAction(mismatchController->getPrevMismatchAction());
    navigationMenu->addAction(mismatchController->getNextMismatchAction());
}

void McaEditor::addEditMenu(QMenu* menu) {
    QMenu* editMenu = menu->addMenu(tr("Edit"));
    editMenu->menuAction()->setObjectName(MCAE_MENU_EDIT);

    auto ui = getUI();
    auto sequenceArea = ui->getSequenceArea();

    editMenu->addAction(sequenceArea->getInsertAction());
    editMenu->addAction(sequenceArea->getReplaceCharacterAction());
    editMenu->addAction(ui->getDelSelectionAction());
    editMenu->addSeparator();

    editMenu->addAction(sequenceArea->getInsertGapAction());
    editMenu->addAction(sequenceArea->getRemoveGapBeforeSelectionAction());
    editMenu->addAction(sequenceArea->getRemoveColumnsOfGapsAction());
    editMenu->addSeparator();

    editMenu->addAction(sequenceArea->getTrimLeftEndAction());
    editMenu->addAction(sequenceArea->getTrimRightEndAction());
    editMenu->addSeparator();

    auto editorNameList = ui->getEditorNameList();
    editMenu->addAction(editorNameList->getEditSequenceNameAction());
    editMenu->addAction(editorNameList->getRemoveSequenceAction());
    editMenu->addSeparator();

    editMenu->addAction(ui->getUndoAction());
    editMenu->addAction(ui->getRedoAction());
}

}   // namespace U2
