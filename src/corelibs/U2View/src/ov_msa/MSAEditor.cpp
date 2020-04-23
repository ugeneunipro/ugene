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

#include <QDropEvent>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/GroupHeaderImageWidget.h>
#include <U2Gui/GroupOptionsWidget.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OptionsPanelWidget.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/ProjectView.h>
#include <U2View/FindPatternMsaWidgetFactory.h>
#include <U2View/ColorSchemaSettingsController.h>

#include "AlignSequencesToAlignment/AlignSequencesToAlignmentTask.h"
#include "MSAEditor.h"
#include "MaEditorFactory.h"
#include "MaEditorNameList.h"
#include "MaEditorTasks.h"
#include "Overview/MaEditorOverviewArea.h"
#include "RealignSequencesInAlignment/RealignSequencesInAlignmentTask.h"
#include "view_rendering/MaEditorConsensusArea.h"
#include "view_rendering/MaEditorSequenceArea.h"
#include "MSAEditorOffsetsView.h"
#include "Highlighting/MsaSchemesMenuBuilder.h"

namespace U2 {

MSAEditor::MSAEditor(const QString& viewName, MultipleSequenceAlignmentObject* obj)
    : MaEditor(MsaEditorFactory::ID, viewName, obj),
      alignSequencesToAlignmentAction(nullptr),
      realignSomeSequenceAction(nullptr),
      treeManager(this)
{
    gotoAction = NULL;
    searchInSequencesAction = NULL;

    initZoom();
    initFont();

    buildTreeAction = new QAction(QIcon(":/core/images/phylip.png"), tr("Build Tree"), this);
    buildTreeAction->setObjectName("Build Tree");
    buildTreeAction->setEnabled(!isAlignmentEmpty());
    connect(maObject, SIGNAL(si_rowsRemoved(const QList<qint64> &)), SLOT(sl_rowsRemoved(const QList<qint64> &)));
    connect(buildTreeAction, SIGNAL(triggered()), SLOT(sl_buildTree()));

    realignSomeSequenceAction = new QAction(QIcon(":/core/images/realign_some_sequences.png"), tr("Realign sequence(s) to other sequences"), this);
    realignSomeSequenceAction->setObjectName("Realign sequence(s) to other sequences");

    pairwiseAlignmentWidgetsSettings = new PairwiseAlignmentWidgetsSettings;
    if (maObject->getAlphabet() != NULL) {
        pairwiseAlignmentWidgetsSettings->customSettings.insert("alphabet", maObject->getAlphabet()->getId());
    }

    openCustomSettingsAction = new QAction(tr("Create new color scheme"), this);
    openCustomSettingsAction->setObjectName("Create new color scheme");
    connect(openCustomSettingsAction, SIGNAL(triggered()), SLOT(sl_showCustomSettings()));

    updateActions();
}

void MSAEditor::sl_buildTree() {
    treeManager.buildTreeWithDialog();
}

bool MSAEditor::onObjectRemoved(GObject* obj) {
    bool result = GObjectView::onObjectRemoved(obj);

    obj->disconnect(ui->getSequenceArea());
    obj->disconnect(ui->getConsensusArea());
    obj->disconnect(ui->getEditorNameList());
    return result;
}

void MSAEditor::onObjectRenamed(GObject*, const QString&) {
    // update title
    OpenMaEditorTask::updateTitle(this);
}

bool MSAEditor::onCloseEvent() {
    if (ui->getOverviewArea() != NULL) {
        ui->getOverviewArea()->cancelRendering();
    }
    return true;
}

const MultipleSequenceAlignmentRow MSAEditor::getRowByLineNumber(int lineNumber) const {
    if (ui->isCollapsibleMode()) {
        lineNumber = ui->getCollapseModel()->getMaRowIndexByViewRowIndex(lineNumber);
    }
    return getMaObject()->getMsaRow(lineNumber);
}

MSAEditor::~MSAEditor() {
    delete pairwiseAlignmentWidgetsSettings;
}

void MSAEditor::buildStaticToolbar(QToolBar* tb) {
    tb->addAction(ui->getCopyFormattedSelectionAction());

    tb->addAction(saveAlignmentAction);
    tb->addAction(saveAlignmentAsAction);

    tb->addAction(zoomInAction);
    tb->addAction(zoomOutAction);
    tb->addAction(zoomToSelectionAction);
    tb->addAction(resetZoomAction);

    tb->addAction(showOverviewAction);
    tb->addAction(changeFontAction);

    tb->addAction(saveScreenshotAction);
    tb->addAction(buildTreeAction);
    tb->addAction(alignAction);
    tb->addAction(alignSequencesToAlignmentAction);
    tb->addAction(realignSomeSequenceAction);

    GObjectView::buildStaticToolbar(tb);
}

void MSAEditor::buildStaticMenu(QMenu* m) {
    addAppearanceMenu(m);

    addNavigationMenu(m);

    addLoadMenu(m);

    addCopyMenu(m);
    addEditMenu(m);

    addAlignMenu(m);
    addTreeMenu(m);
    addStatisticsMenu(m);

    addSortMenu(m);
    addExportMenu(m);

    addAdvancedMenu(m);

    GObjectView::buildStaticMenu(m);

    GUIUtils::disableEmptySubmenus(m);
}

void MSAEditor::addExportMenu(QMenu* m) {
    MaEditor::addExportMenu(m);
    QMenu* em = GUIUtils::findSubMenu(m, MSAE_MENU_EXPORT);
    SAFE_POINT(em != NULL, "Export menu not found", );
    em->addAction(saveScreenshotAction);
}

void MSAEditor::addAppearanceMenu(QMenu* m) {
    QMenu* appearanceMenu = m->addMenu(tr("Appearance"));
    appearanceMenu->menuAction()->setObjectName(MSAE_MENU_APPEARANCE);

    appearanceMenu->addAction(showOverviewAction);
    auto offsetsController = ui->getOffsetsViewController();
    if (offsetsController != NULL) {
        appearanceMenu->addAction(offsetsController->getToggleColumnsViewAction());
    }
    appearanceMenu->addSeparator();
    appearanceMenu->addAction(zoomInAction);
    appearanceMenu->addAction(zoomOutAction);
    appearanceMenu->addAction(zoomToSelectionAction);
    appearanceMenu->addAction(resetZoomAction);
    appearanceMenu->addSeparator();

    addColorsMenu(appearanceMenu);
    addHighlightingMenu(appearanceMenu);
    appearanceMenu->addSeparator();

    appearanceMenu->addAction(changeFontAction);
    appearanceMenu->addSeparator();
    appearanceMenu->addAction(clearSelectionAction);
}

void MSAEditor::addColorsMenu(QMenu* m) {
    QMenu* colorsSchemeMenu = m->addMenu(tr("Colors"));
    colorsSchemeMenu->menuAction()->setObjectName("Colors");
    colorsSchemeMenu->setIcon(QIcon(":core/images/color_wheel.png"));
    auto sequenceArea = ui->getSequenceArea();
    foreach(QAction* a, sequenceArea->colorSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, colorsSchemeMenu);
    }
    colorsSchemeMenu->addSeparator();

    QMenu* customColorSchemaMenu = new QMenu(tr("Custom schemes"), colorsSchemeMenu);
    customColorSchemaMenu->menuAction()->setObjectName("Custom schemes");

    foreach(QAction* a, sequenceArea->customColorSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, customColorSchemaMenu);
    }

    if (!sequenceArea->customColorSchemeMenuActions.isEmpty()) {
        customColorSchemaMenu->addSeparator();
    }

    customColorSchemaMenu->addAction(openCustomSettingsAction);

    colorsSchemeMenu->addMenu(customColorSchemaMenu);
    m->insertMenu(GUIUtils::findAction(m->actions(), MSAE_MENU_EDIT), colorsSchemeMenu);
}

void MSAEditor::addHighlightingMenu(QMenu* m) {
    QMenu* highlightSchemeMenu = new QMenu(tr("Highlighting"), NULL);

    highlightSchemeMenu->menuAction()->setObjectName("Highlighting");

    auto sequenceArea= ui->getSequenceArea();
    foreach(QAction* a, sequenceArea->highlightingSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, highlightSchemeMenu);
    }
    highlightSchemeMenu->addSeparator();
    highlightSchemeMenu->addAction(sequenceArea->useDotsAction);
    m->insertMenu(GUIUtils::findAction(m->actions(), MSAE_MENU_EDIT), highlightSchemeMenu);
}

void MSAEditor::addNavigationMenu(QMenu* m) {
    QMenu* navMenu = m->addMenu(tr("Navigation"));
    navMenu->menuAction()->setObjectName(MSAE_MENU_NAVIGATION);
    navMenu->addAction(gotoAction);
    navMenu->addSeparator();
    navMenu->addAction(searchInSequencesAction);
}

void MSAEditor::addTreeMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Tree"));
    //em->setIcon(QIcon(":core/images/tree.png"));
    em->menuAction()->setObjectName(MSAE_MENU_TREES);
    em->addAction(buildTreeAction);
}

void MSAEditor::addAdvancedMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Advanced"));
    em->menuAction()->setObjectName(MSAE_MENU_ADVANCED);
}

void MSAEditor::addStatisticsMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Statistics"));
    em->setIcon(QIcon(":core/images/chart_bar.png"));
    em->menuAction()->setObjectName(MSAE_MENU_STATISTICS);
}

MsaEditorWgt *MSAEditor::getUI() const {
    return qobject_cast<MsaEditorWgt *>(ui);
}

QWidget* MSAEditor::createWidget() {
    Q_ASSERT(ui == NULL);
    ui = new MsaEditorWgt(this);

    QString objName = "msa_editor_" + maObject->getGObjectName();
    ui->setObjectName(objName);

    initActions();

    connect(ui , SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));

    gotoAction = new QAction(QIcon(":core/images/goto.png"), tr("Go to position…"), this);
    gotoAction->setObjectName("action_go_to_position");
    gotoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    gotoAction->setShortcutContext(Qt::WindowShortcut);
    gotoAction->setToolTip(QString("%1 (%2)").arg(gotoAction->text()).arg(gotoAction->shortcut().toString()));
    connect(gotoAction, SIGNAL(triggered()), ui->getSequenceArea(), SLOT(sl_goto()));

    searchInSequencesAction = new QAction(QIcon(":core/images/find_dialog.png"), tr("Search in sequences…"), this);
    searchInSequencesAction->setObjectName("search_in_sequences");
    searchInSequencesAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    searchInSequencesAction->setShortcutContext(Qt::WindowShortcut);
    searchInSequencesAction->setToolTip(QString("%1 (%2)").arg(searchInSequencesAction->text()).arg(searchInSequencesAction->shortcut().toString()));
    connect(searchInSequencesAction, SIGNAL(triggered()), this, SLOT(sl_searchInSequences()));

    alignAction = new QAction(QIcon(":core/images/align.png"), tr("Align"), this);
    alignAction->setObjectName("Align");
    connect(alignAction, SIGNAL(triggered()), this, SLOT(sl_align()));

    alignSequencesToAlignmentAction = new QAction(QIcon(":/core/images/add_to_alignment.png"), tr("Align sequence(s) to this alignment"), this);
    alignSequencesToAlignmentAction->setObjectName("Align sequence(s) to this alignment");
    connect(alignSequencesToAlignmentAction, SIGNAL(triggered()), this, SLOT(sl_addToAlignment()));

    setAsReferenceSequenceAction = new QAction(tr("Set this sequence as reference"), this);
    setAsReferenceSequenceAction->setObjectName("set_seq_as_reference");
    connect(setAsReferenceSequenceAction, SIGNAL(triggered()), SLOT(sl_setSeqAsReference()));

    unsetReferenceSequenceAction = new QAction(tr("Unset reference sequence"), this);
    unsetReferenceSequenceAction->setObjectName("unset_reference");
    connect(unsetReferenceSequenceAction, SIGNAL(triggered()), SLOT(sl_unsetReferenceSeq()));

    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry *opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_AlignmentEditor));

    QList<OPWidgetFactory*> opWidgetFactories = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory *factory, opWidgetFactories) {
        optionsPanel->addGroup(factory);
    }

    connect(realignSomeSequenceAction, SIGNAL(triggered()), this, SLOT(sl_realignSomeSequences()));
    connect(maObject, SIGNAL(si_alphabetChanged(const MaModificationInfo&, const DNAAlphabet*)), SLOT(sl_updateRealignAction()));
    connect(ui->getSequenceArea(), SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)), SLOT(sl_updateRealignAction()));

    qDeleteAll(filters);

    connect(ui, SIGNAL(si_showTreeOP()), SLOT(sl_showTreeOP()));
    connect(ui, SIGNAL(si_hideTreeOP()), SLOT(sl_hideTreeOP()));
    sl_hideTreeOP();

    treeManager.loadRelatedTrees();

    initDragAndDropSupport();
    updateActions();
    return ui;
}

void MSAEditor::sl_onContextMenuRequested(const QPoint & /*pos*/) {
    QMenu m;

    addAppearanceMenu(&m);
    addNavigationMenu(&m);
    addLoadMenu(&m);
    addCopyMenu(&m);
    addEditMenu(&m);
    addAlignMenu(&m);
    addTreeMenu(&m);
    addStatisticsMenu(&m);
    addSortMenu(&m);
    addExportMenu(&m);
    addAdvancedMenu(&m);

    m.addSeparator();
    snp.clickPoint = QCursor::pos( );
    const QPoint nameMapped = ui->getEditorNameList()->mapFromGlobal( snp.clickPoint );
    const qint64 hoverRowId = ( 0 <= nameMapped.y( ) )
        ? ui->getEditorNameList()->sequenceIdAtPos( nameMapped ) : U2MsaRow::INVALID_ROW_ID;
    if ( ( hoverRowId != getReferenceRowId( )
        || U2MsaRow::INVALID_ROW_ID == getReferenceRowId( ) )
        && hoverRowId != U2MsaRow::INVALID_ROW_ID )
    {
        m.addAction( setAsReferenceSequenceAction );
    }
    if ( U2MsaRow::INVALID_ROW_ID != getReferenceRowId( ) ) {
        m.addAction( unsetReferenceSequenceAction );
    }
    m.addSeparator();

    emit si_buildPopupMenu(this, &m);

    GUIUtils::disableEmptySubmenus(&m);

    m.exec(QCursor::pos());
}

void MSAEditor::updateActions() {
    MaEditor::updateActions();
    if(alignSequencesToAlignmentAction != NULL) {
        alignSequencesToAlignmentAction->setEnabled(!maObject->isStateLocked());
    }
    buildTreeAction->setEnabled(!maObject->isStateLocked() && !this->isAlignmentEmpty());
    sl_updateRealignAction();
}

void MSAEditor::sl_onSeqOrderChanged(const QStringList& order ){
    if(!maObject->isStateLocked()) {
        maObject->sortRowsByList(order);
    }
}

void MSAEditor::sl_showTreeOP() {
    OptionsPanelWidget* opWidget = dynamic_cast<OptionsPanelWidget*>(optionsPanel->getMainWidget());
    if(NULL == opWidget) {
        return;
    }

    QWidget* addTreeGroupWidget = opWidget->findOptionsWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if(NULL != addTreeGroupWidget) {
        addTreeGroupWidget->hide();
        opWidget->closeOptionsPanel();
    }
    QWidget* addTreeHeader = opWidget->findHeaderWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if(NULL != addTreeHeader) {
        addTreeHeader->hide();
    }

    GroupHeaderImageWidget* header = opWidget->findHeaderWidgetByGroupId("OP_MSA_TREES_WIDGET");
    if(NULL != header) {
        header->show();
        header->changeState();
    }
}

void MSAEditor::sl_hideTreeOP() {
    OptionsPanelWidget* opWidget = dynamic_cast<OptionsPanelWidget*>(optionsPanel->getMainWidget());
    if(NULL == opWidget) {
        return;
    }
    GroupHeaderImageWidget* header = opWidget->findHeaderWidgetByGroupId("OP_MSA_TREES_WIDGET");
    QWidget* groupWidget = opWidget->findOptionsWidgetByGroupId("OP_MSA_TREES_WIDGET");
    bool openAddTreeGroup = (NULL != groupWidget);

    header->hide();

    GroupHeaderImageWidget* addTreeHeader = opWidget->findHeaderWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if(NULL != addTreeHeader) {
        addTreeHeader->show();
        if(openAddTreeGroup) {
            addTreeHeader->changeState();
        }
    }
}


bool MSAEditor::eventFilter(QObject*, QEvent* e) {
    if (e->type() == QEvent::DragEnter || e->type() == QEvent::Drop) {
        QDropEvent* de = (QDropEvent*)e;
        const QMimeData* md = de->mimeData();
        const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
        if (gomd != NULL) {
            if (maObject->isStateLocked()) {
                return false;
            }
            U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*> (gomd->objPtr.data());
            if (dnaObj != NULL ) {
                if (U2AlphabetUtils::deriveCommonAlphabet(dnaObj->getAlphabet(), maObject->getAlphabet()) == NULL) {
                    return false;
                }
                if (e->type() == QEvent::DragEnter) {
                    de->acceptProposedAction();
                } else {
                    U2OpStatusImpl os;
                    DNASequence seq = dnaObj->getWholeSequence(os);
                    seq.alphabet = dnaObj->getAlphabet();
                    Task *task = new AddSequenceObjectsToAlignmentTask(getMaObject(), QList<DNASequence>() << seq);
                    TaskWatchdog::trackResourceExistence(maObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
                    AppContext::getTaskScheduler()->registerTopLevelTask(task);
                }
            }
        }
    }

    return false;
}

void MSAEditor::initDragAndDropSupport()
{
    SAFE_POINT(ui!= NULL, QString("MSAEditor::ui is not initialized in MSAEditor::initDragAndDropSupport"),);
    ui->setAcceptDrops(true);
    ui->installEventFilter(this);
}

void MSAEditor::sl_align(){
    QMenu m, *mm;

    addLoadMenu(&m);
    addCopyMenu(&m);
    addEditMenu(&m);
    addAlignMenu(&m);
    addTreeMenu(&m);
    addStatisticsMenu(&m);
    addSortMenu(&m);
    addExportMenu(&m);
    addAdvancedMenu(&m);

    emit si_buildPopupMenu(this, &m);

    GUIUtils::disableEmptySubmenus(&m);

    mm = GUIUtils::findSubMenu(&m, MSAE_MENU_ALIGN);
    SAFE_POINT(mm != NULL, "mm", );

    mm->exec(QCursor::pos());
}

void MSAEditor::sl_addToAlignment() {
    MultipleSequenceAlignmentObject* msaObject = getMaObject();
    if (msaObject->isStateLocked()) {
        return;
    }

    ProjectView* pv = AppContext::getProjectView();
    SAFE_POINT(pv != NULL, "Project view is null",);

    const GObjectSelection* selection = pv->getGObjectSelection();
    SAFE_POINT(selection  != NULL, "GObjectSelection is null",);

    QList<GObject*> objects = selection->getSelectedObjects();
    bool selectFromProject = !objects.isEmpty();

    foreach(GObject* object, objects) {
        if(object == getMaObject() || (object->getGObjectType() != GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT && object->getGObjectType() != GObjectTypes::SEQUENCE)) {
            selectFromProject = false;
            break;
        }
    }
    if(selectFromProject ) {
        alignSequencesFromObjectsToAlignment(objects);
    } else {
        alignSequencesFromFilesToAlignment();
    }
}

void MSAEditor::sl_searchInSequences() {
    auto optionsPanel = getOptionsPanel();
    SAFE_POINT(optionsPanel != NULL, "Internal error: options panel is NULL"
                                     " when search in sequences has been initiated!",);
    optionsPanel->openGroupById(FindPatternMsaWidgetFactory::getGroupId());
}

void MSAEditor::sl_realignSomeSequences() {
    const MaEditorSelection& selection = getSelection();
    int startSeq = selection.y();
    int endSeq = selection.y() + selection.height() - 1;
    MaCollapseModel* model = ui->getCollapseModel();
    const MultipleAlignment& ma = ui->getEditor()->getMaObject()->getMultipleAlignment();
    QSet<qint64> rowIds;
    for (int i = startSeq; i <= endSeq; i++) {
        rowIds.insert(ma->getRow(model->getMaRowIndexByViewRowIndex(i))->getRowId());
    }
    Task* realignTask = new RealignSequencesInAlignmentTask(getMaObject(), rowIds);
    TaskWatchdog::trackResourceExistence(ui->getEditor()->getMaObject(), realignTask, tr("A problem occurred during realigning sequences. The multiple alignment is no more available."));
    AppContext::getTaskScheduler()->registerTopLevelTask(realignTask);
}

void MSAEditor::alignSequencesFromObjectsToAlignment(const QList<GObject*>& objects) {
    SequenceObjectsExtractor extractor;
    extractor.setAlphabet(maObject->getAlphabet());
    extractor.extractSequencesFromObjects(objects);

    if(!extractor.getSequenceRefs().isEmpty()) {
        AlignSequencesToAlignmentTask* task = new AlignSequencesToAlignmentTask(getMaObject(), extractor);
        TaskWatchdog::trackResourceExistence(maObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

void MSAEditor::alignSequencesFromFilesToAlignment() {
    QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);

    LastUsedDirHelper lod;
    QStringList urls;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        urls = U2FileDialog::getOpenFileNames(ui, tr("Open file with sequences"), lod.dir, filter, 0, QFileDialog::DontUseNativeDialog );
    } else
#endif
        urls = U2FileDialog::getOpenFileNames(ui, tr("Open file with sequences"), lod.dir, filter);

    if (!urls.isEmpty()) {
        lod.url = urls.first();
        LoadSequencesAndAlignToAlignmentTask * task = new LoadSequencesAndAlignToAlignmentTask(getMaObject(), urls);
        TaskWatchdog::trackResourceExistence(maObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

void MSAEditor::sl_setSeqAsReference(){
    QPoint menuCallPos = snp.clickPoint;
    QPoint nameMapped = ui->getEditorNameList()->mapFromGlobal(menuCallPos);
    if ( nameMapped.y() >= 0 ) {
        qint64 newRowId = ui->getEditorNameList()->sequenceIdAtPos(nameMapped);
        if (U2MsaRow::INVALID_ROW_ID != newRowId && newRowId != snp.seqId) {
            setReference(newRowId);
        }
    }
}

void MSAEditor::sl_unsetReferenceSeq( ) {
    if ( U2MsaRow::INVALID_ROW_ID != getReferenceRowId( ) ) {
        setReference( U2MsaRow::INVALID_ROW_ID );
    }
}

void MSAEditor::sl_rowsRemoved(const QList<qint64> &rowIds) {
    foreach (qint64 rowId, rowIds) {
        if (getReferenceRowId() == rowId) {
            sl_unsetReferenceSeq();
            break;
        }
    }
}

void MSAEditor::sl_updateRealignAction() {
    if (maObject->isStateLocked() || maObject->getAlphabet()->isRaw() || ui == nullptr) {
        realignSomeSequenceAction->setDisabled(true);
        return;
    }
    const MaEditorSelection& selection = getSelection();
    bool isWholeSequenceSelection = selection.width() == maObject->getLength() && selection.height() >= 1;
    bool isAllRowsSelection = selection.height() == ui->getCollapseModel()->getViewRowCount();
    realignSomeSequenceAction->setEnabled(isWholeSequenceSelection && !isAllRowsSelection);
}

void MSAEditor::buildTree() {
    sl_buildTree();
}

QString MSAEditor::getReferenceRowName() const {
    const MultipleAlignment alignment = getMaObject()->getMultipleAlignment();
    U2OpStatusImpl os;
    const int refSeq = alignment->getRowIndexByRowId(getReferenceRowId(), os);
    return (U2MsaRow::INVALID_ROW_ID != refSeq) ? alignment->getRowNames().at(refSeq)
                                                : QString();
}

char MSAEditor::getReferenceCharAt(int pos) const {
    CHECK(getReferenceRowId() != U2MsaRow::INVALID_ROW_ID, '\n');

    U2OpStatusImpl os;
    const int refSeq = maObject->getMultipleAlignment()->getRowIndexByRowId(getReferenceRowId(), os);
    SAFE_POINT_OP(os, '\n');

    return maObject->getMultipleAlignment()->charAt(refSeq, pos);
}

void MSAEditor::sl_showCustomSettings(){
    AppContext::getAppSettingsGUI()->showSettingsDialog(ColorSchemaSettingsPageId);
}


}   // namespace U2
