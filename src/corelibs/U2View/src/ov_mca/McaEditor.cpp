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

#include "McaEditor.h"

#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>

#include "McaEditorConsensusArea.h"
#include "McaEditorNameList.h"
#include "McaEditorSequenceArea.h"
#include "general_tab/McaGeneralTabFactory.h"
#include "ov_mca/MaConsensusMismatchController.h"
#include "ov_mca/SequenceWithChromatogramAreaRenderer.h"
#include "ov_msa/MSAEditorOffsetsView.h"
#include "ov_msa/MaAmbiguousCharactersController.h"
#include "ov_msa/MaEditorFactory.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/export_consensus/MaExportConsensusTabFactory.h"
#include "ov_msa/overview/MaEditorOverviewArea.h"
#include "ov_sequence/SequenceObjectContext.h"

namespace U2 {

McaEditor::McaEditor(const QString& viewName,
                     MultipleChromatogramAlignmentObject* obj)
    : MaEditor(McaEditorFactory::ID, viewName, obj),
      showChromatogramsAction(nullptr), showGeneralTabAction(nullptr), showConsensusTabAction(nullptr), referenceCtx(nullptr) {
    selectionController = new McaEditorSelectionController(this);
    initZoom();
    initFont();

    U2OpStatusImpl os;
    foreach (const MultipleChromatogramAlignmentRow& row, obj->getMca()->getMcaRows()) {
        chromVisibility.insert(obj->getMca()->getRowIndexByRowId(row->getRowId(), os), true);
    }

    U2SequenceObject* referenceObj = obj->getReferenceObj();
    SAFE_POINT(nullptr != referenceObj, "Trying to open McaEditor without a reference", );
    referenceCtx = new SequenceObjectContext(referenceObj, this);
}

MultipleChromatogramAlignmentObject* McaEditor::getMaObject() const {
    return qobject_cast<MultipleChromatogramAlignmentObject*>(maObject);
}

McaEditorWgt *McaEditor::getUI() const {
    return qobject_cast<McaEditorWgt *>(ui);
}

void McaEditor::buildStaticToolbar(QToolBar *tb) {
    tb->addAction(showChromatogramsAction);
    tb->addAction(showOverviewAction);
    tb->addSeparator();

    tb->addAction(zoomInAction);
    tb->addAction(zoomOutAction);
    tb->addAction(resetZoomAction);
    tb->addSeparator();

    GObjectView::buildStaticToolbar(tb);
}

void McaEditor::buildMenu(QMenu* menu, const QString& type) {
    if (type != MsaEditorMenuType::STATIC) {
        GObjectView::buildMenu(menu, type);
        return;
    }
    addAlignmentMenu(menu);
    addAppearanceMenu(menu);
    addNavigationMenu(menu);
    addEditMenu(menu);
    menu->addSeparator();
    menu->addAction(showConsensusTabAction);
    menu->addSeparator();

    GObjectView::buildMenu(menu, type);
    GUIUtils::disableEmptySubmenus(menu);
}

int McaEditor::getRowContentIndent(int rowId) const {
    if (isChromatogramRowExpanded(rowId)) {
        return SequenceWithChromatogramAreaRenderer::INDENT_BETWEEN_ROWS / 2;
    }
    return MaEditor::getRowContentIndent(rowId);
}

bool McaEditor::isChromatogramRowExpanded(int rowIndex) const {
    return !collapseModel->isGroupWithMaRowIndexCollapsed(rowIndex);
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

void McaEditor::sl_onContextMenuRequested(const QPoint& /*pos*/) {
    QMenu menu;
    buildMenu(&menu, MsaEditorMenuType::STATIC);  // TODO: this call triggers extra signal for static menu.
    emit si_buildMenu(this, &menu, MsaEditorMenuType::CONTEXT);
    menu.exec(QCursor::pos());
}

void McaEditor::sl_showHideChromatograms(bool show) {
    GCOUNTER(cvar, "Show/hide chromatogram in MCA");
    collapseModel->collapseAll(!show);
    sl_saveChromatogramState();
    emit si_completeUpdate();
}

void McaEditor::sl_showGeneralTab() {
    OptionsPanel* optionsPanel = getOptionsPanel();
    SAFE_POINT(optionsPanel != nullptr, "Internal error: options panel is NULL"
                                        " when msageneraltab opening was initiated", );
    optionsPanel->openGroupById(McaGeneralTabFactory::getGroupId());
}

void McaEditor::sl_showConsensusTab() {
    OptionsPanel* optionsPanel = getOptionsPanel();
    SAFE_POINT(nullptr != optionsPanel, "Internal error: options panel is NULL"
                                        " when msaconsensustab opening was initiated", );
    optionsPanel->openGroupById(McaExportConsensusTabFactory::getGroupId());
}

QWidget *McaEditor::createWidget() {
    Q_ASSERT(ui == nullptr);
    ui = new McaEditorWgt(this);

    collapseModel->reset(getMaRowIds());

    bool showChromatograms = AppContext::getSettings()->getValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_CHROMATOGRAMS, true).toBool();
    collapseModel->collapseAll(!showChromatograms);
    GCounter::increment(QString("'Show chromatograms' is %1 on MCA open").arg(showChromatograms ? "ON" : "OFF"));

    QString objName = "mca_editor_" + maObject->getGObjectName();
    ui->setObjectName(objName);

    connect(ui, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));

    initActions();

    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_ChromAlignmentEditor));

    QList<OPWidgetFactory*> opWidgetFactories = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory* factory, opWidgetFactories) {
        optionsPanel->addGroup(factory);
    }

    qDeleteAll(filters);

    updateActions();

    return ui;
}

void McaEditor::initActions() {
    MaEditor::initActions();

    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != nullptr, "AppContext::settings is NULL", );

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
    getUI()->getOverviewArea()->setVisible(overviewVisible);
    changeFontAction->setText(tr("Change characters font..."));

    GCounter::increment(QString("'Show overview' is %1 on MCA open").arg(overviewVisible ? "ON" : "OFF"));
}

void McaEditor::sl_saveOverviewState() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != nullptr, "AppContext::settings is NULL", );
    s->setValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_OVERVIEW, showOverviewAction->isChecked());
}

void McaEditor::sl_saveChromatogramState() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != nullptr, "AppContext::settings is NULL", );
    s->setValue(getSettingsRoot() + MCAE_SETTINGS_SHOW_CHROMATOGRAMS, showChromatogramsAction->isChecked());
}

void McaEditor::addAlignmentMenu(QMenu* menu) {
    QMenu* alignmentMenu = menu->addMenu(tr("Alignment"));
    alignmentMenu->menuAction()->setObjectName(MCAE_MENU_ALIGNMENT);

    alignmentMenu->addAction(showGeneralTabAction);
}

void McaEditor::addAppearanceMenu(QMenu* menu) {
    QMenu* appearanceMenu = menu->addMenu(tr("Appearance"));
    appearanceMenu->menuAction()->setObjectName(MCAE_MENU_APPEARANCE);

    auto ui = getUI();
    auto sequenceArea = ui->getSequenceArea();
    auto offsetsController = ui->getOffsetsViewController();

    appearanceMenu->addAction(showChromatogramsAction);
    appearanceMenu->addMenu(sequenceArea->getTraceActionsMenu());
    appearanceMenu->addAction(showOverviewAction);
    if (offsetsController != nullptr) {
        appearanceMenu->addAction(offsetsController->toggleColumnsViewAction);
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

void McaEditor::addNavigationMenu(QMenu* menu) {
    QMenu* navigationMenu = menu->addMenu(tr("Navigation"));
    navigationMenu->menuAction()->setObjectName(MCAE_MENU_NAVIGATION);

    auto ui = getUI();
    navigationMenu->addAction(gotoSelectedReadAction);

    auto ambiguousCharactersController = ui->getSequenceArea()->getAmbiguousCharactersController();
    navigationMenu->addAction(ambiguousCharactersController->getPreviousAction());
    navigationMenu->addAction(ambiguousCharactersController->getNextAction());
    navigationMenu->addSeparator();

    auto mismatchController = ui->getConsensusArea()->getMismatchController();
    navigationMenu->addAction(mismatchController->getPrevMismatchAction());
    navigationMenu->addAction(mismatchController->getNextMismatchAction());
}

void McaEditor::addEditMenu(QMenu *menu) {
    QMenu *editMenu = menu->addMenu(tr("Edit"));
    editMenu->menuAction()->setObjectName(MCAE_MENU_EDIT);

    auto ui = getUI();
    auto sequenceArea = ui->getSequenceArea();

    editMenu->addAction(sequenceArea->getInsertAction());
    editMenu->addAction(sequenceArea->getReplaceCharacterAction());
    editMenu->addAction(ui->delSelectionAction);
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

    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
}

MaEditorSelectionController *McaEditor::getSelectionController() const {
    return selectionController;
}
}  // namespace U2
