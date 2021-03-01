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

#include "MSAEditor.h"

#include <QDropEvent>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/GroupHeaderImageWidget.h>
#include <U2Gui/GroupOptionsWidget.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OptionsPanelWidget.h>
#include <U2Gui/ProjectView.h>

#include <U2View/ColorSchemaSettingsController.h>
#include <U2View/FindPatternMsaWidgetFactory.h>

#include "AlignSequencesToAlignment/AlignSequencesToAlignmentTask.h"
#include "Highlighting/MsaSchemesMenuBuilder.h"
#include "MSAEditorOffsetsView.h"
#include "MSAEditorSequenceArea.h"
#include "MaEditorFactory.h"
#include "MaEditorNameList.h"
#include "MaEditorTasks.h"
#include "Overview/MaEditorOverviewArea.h"
#include "RealignSequencesInAlignment/RealignSequencesInAlignmentTask.h"
#include "view_rendering/MaEditorConsensusArea.h"
#include "view_rendering/MaEditorSequenceArea.h"

namespace U2 {

MSAEditor::MSAEditor(const QString &viewName, MultipleSequenceAlignmentObject *obj)
    : MaEditor(MsaEditorFactory::ID, viewName, obj),
      alignSequencesToAlignmentAction(nullptr),
      realignSomeSequenceAction(nullptr),
      treeManager(this) {
    gotoAction = nullptr;
    searchInSequencesAction = nullptr;
    searchInSequenceNamesAction = nullptr;

    sortByNameAscendingAction = new QAction(tr("By name"), this);
    sortByNameAscendingAction->setObjectName("action_sort_by_name");
    sortByNameAscendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by name, ascending"));
    connect(sortByNameAscendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByName()));

    sortByNameDescendingAction = new QAction(tr("By name, descending"), this);
    sortByNameDescendingAction->setObjectName("action_sort_by_name_descending");
    sortByNameAscendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by name, descending"));
    connect(sortByNameDescendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByName()));

    sortByLengthAscendingAction = new QAction(tr("By length"), this);
    sortByLengthAscendingAction->setObjectName("action_sort_by_length");
    sortByLengthAscendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by length, ascending"));
    connect(sortByLengthAscendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByLength()));

    sortByLengthDescendingAction = new QAction(tr("By length, descending"), this);
    sortByLengthDescendingAction->setObjectName("action_sort_by_length_descending");
    sortByLengthDescendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by length, descending"));
    connect(sortByLengthDescendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByLength()));

    sortByLeadingGapAscendingAction = new QAction(tr("By leading gap"), this);
    sortByLeadingGapAscendingAction->setObjectName("action_sort_by_leading_gap");
    sortByLeadingGapAscendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by leading gap, ascending"));
    connect(sortByLeadingGapAscendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByLeadingGap()));

    sortByLeadingGapDescendingAction = new QAction(tr("By leading gap, descending"), this);
    sortByLeadingGapDescendingAction->setObjectName("action_sort_by_leading_gap_descending");
    sortByLeadingGapDescendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by leading gap, descending"));
    connect(sortByLeadingGapDescendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByLeadingGap()));

    openCustomSettingsAction = new QAction(tr("Create new color scheme"), this);
    openCustomSettingsAction->setObjectName("Create new color scheme");
    connect(openCustomSettingsAction, SIGNAL(triggered()), SLOT(sl_showCustomSettings()));

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
    if (maObject->getAlphabet() != nullptr) {
        pairwiseAlignmentWidgetsSettings->customSettings.insert("alphabet", maObject->getAlphabet()->getId());
    }

    convertDnaToRnaAction = new QAction(tr("Convert to RNA alphabet (T->U)"), this);
    convertDnaToRnaAction->setObjectName("convertDnaToRnaAction");
    convertDnaToRnaAction->setToolTip(tr("Convert alignment from DNA to RNA alphabet: replace T with U"));
    connect(convertDnaToRnaAction, SIGNAL(triggered()), SLOT(sl_convertBetweenDnaAndRnaAlphabets()));

    convertRnaToDnaAction = new QAction(tr("Convert to DNA alphabet (U->T)"), this);
    convertRnaToDnaAction->setObjectName("convertRnaToDnaAction");
    convertRnaToDnaAction->setToolTip(tr("Convert alignment from RNA to DNA alphabet: replace U with T"));
    connect(convertRnaToDnaAction, SIGNAL(triggered()), SLOT(sl_convertBetweenDnaAndRnaAlphabets()));

    updateActions();
}

void MSAEditor::updateActions() {
    MaEditor::updateActions();
    bool isReadOnly = maObject->isStateLocked();

    sortByNameAscendingAction->setEnabled(!isReadOnly);
    sortByNameDescendingAction->setEnabled(!isReadOnly);
    sortByLengthAscendingAction->setEnabled(!isReadOnly);
    sortByLengthDescendingAction->setEnabled(!isReadOnly);

    if (alignSequencesToAlignmentAction != nullptr) {
        alignSequencesToAlignmentAction->setEnabled(!isReadOnly);
    }
    buildTreeAction->setEnabled(!isReadOnly && !this->isAlignmentEmpty());
    sl_updateRealignAction();

    auto alphabetId = maObject->getAlphabet()->getId();
    convertDnaToRnaAction->setEnabled(!isReadOnly && alphabetId == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    convertRnaToDnaAction->setEnabled(!isReadOnly && alphabetId == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT());
}

void MSAEditor::sl_buildTree() {
    treeManager.buildTreeWithDialog();
}

bool MSAEditor::onObjectRemoved(GObject *obj) {
    bool result = GObjectView::onObjectRemoved(obj);

    obj->disconnect(ui->getSequenceArea());
    obj->disconnect(ui->getConsensusArea());
    obj->disconnect(ui->getEditorNameList());
    return result;
}

void MSAEditor::onObjectRenamed(GObject *, const QString &) {
    // update title
    OpenMaEditorTask::updateTitle(this);
}

bool MSAEditor::onCloseEvent() {
    if (ui->getOverviewArea() != nullptr) {
        ui->getOverviewArea()->cancelRendering();
    }
    return true;
}

const MultipleSequenceAlignmentRow MSAEditor::getRowByViewRowIndex(int viewRowIndex) const {
    int maRowIndex = ui->getCollapseModel()->getMaRowIndexByViewRowIndex(viewRowIndex);
    return getMaObject()->getMsaRow(maRowIndex);
}

MSAEditor::~MSAEditor() {
    delete pairwiseAlignmentWidgetsSettings;
}

void MSAEditor::buildStaticToolbar(QToolBar *tb) {
    tb->addAction(ui->copyFormattedSelectionAction);

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

void MSAEditor::buildStaticMenu(QMenu *m) {
    addAppearanceMenu(m);

    addNavigationMenu(m);

    addLoadMenu(m);

    addCopyPasteMenu(m);
    addEditMenu(m);
    addSortMenu(m);

    addAlignMenu(m);
    addTreeMenu(m);
    addStatisticsMenu(m);

    addExportMenu(m);

    addAdvancedMenu(m);

    GObjectView::buildStaticMenu(m);

    GUIUtils::disableEmptySubmenus(m);
}

void MSAEditor::addCopyPasteMenu(QMenu *m) {
    MaEditor::addCopyPasteMenu(m);

    QMenu *copyMenu = GUIUtils::findSubMenu(m, MSAE_MENU_COPY);
    SAFE_POINT(copyMenu != nullptr, "copyMenu is null", );

    const MaEditorSelection &selection = getSelection();
    ui->copySelectionAction->setDisabled(selection.isEmpty());

    //TODO:? move the signal emit point to a correct location.
    auto sequenceArea = qobject_cast<MSAEditorSequenceArea *>(ui->getSequenceArea());
    SAFE_POINT(sequenceArea != nullptr, "sequenceArea is null", );
    emit sequenceArea->si_copyFormattedChanging(!selection.isEmpty());

    copyMenu->addAction(ui->copySelectionAction);
    ui->copyFormattedSelectionAction->setDisabled(selection.isEmpty());
    copyMenu->addAction(ui->copyFormattedSelectionAction);
    copyMenu->addAction(copyConsensusAction);
    copyMenu->addAction(copyConsensusWithGapsAction);
    copyMenu->addSeparator();
    copyMenu->addAction(ui->pasteAction);
    copyMenu->addAction(ui->pasteBeforeAction);
    copyMenu->addSeparator();
    copyMenu->addAction(ui->cutSelectionAction);

    auto nameList = qobject_cast<MaEditorNameList *>(ui->getEditorNameList());
    SAFE_POINT(nameList != nullptr, "nameList is null", );
    copyMenu->addSeparator();

    copyMenu->addAction(nameList->copyCurrentSequenceAction);

    //TODO: trigger action update on selection change, not in the menu activation code!
    int selectedMaRowIndex = ui->getCollapseModel()->getMaRowIndexByViewRowIndex(selection.y());
    nameList->copyCurrentSequenceAction->setDisabled(selectedMaRowIndex == -1);
}

void MSAEditor::addEditMenu(QMenu *m) {
    QMenu *menu = m->addMenu(tr("Edit"));
    menu->menuAction()->setObjectName(MSAE_MENU_EDIT);
}

void MSAEditor::addSortMenu(QMenu *m) {
    QMenu *menu = m->addMenu(tr("Sort"));
    menu->menuAction()->setObjectName(MSAE_MENU_SORT);
    menu->addAction(sortByNameAscendingAction);
    menu->addAction(sortByNameDescendingAction);
    menu->addAction(sortByLengthAscendingAction);
    menu->addAction(sortByLengthDescendingAction);
    menu->addAction(sortByLeadingGapAscendingAction);
    menu->addAction(sortByLeadingGapDescendingAction);
}

void MSAEditor::addExportMenu(QMenu *m) {
    MaEditor::addExportMenu(m);
    QMenu *em = GUIUtils::findSubMenu(m, MSAE_MENU_EXPORT);
    SAFE_POINT(em != NULL, "Export menu not found", );
    em->addAction(saveScreenshotAction);
}

void MSAEditor::addAppearanceMenu(QMenu *m) {
    QMenu *appearanceMenu = m->addMenu(tr("Appearance"));
    appearanceMenu->menuAction()->setObjectName(MSAE_MENU_APPEARANCE);

    appearanceMenu->addAction(showOverviewAction);
    auto offsetsController = ui->getOffsetsViewController();
    if (offsetsController != nullptr) {
        appearanceMenu->addAction(offsetsController->toggleColumnsViewAction);
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

void MSAEditor::addColorsMenu(QMenu *m) {
    QMenu *colorsSchemeMenu = m->addMenu(tr("Colors"));
    colorsSchemeMenu->menuAction()->setObjectName("Colors");
    colorsSchemeMenu->setIcon(QIcon(":core/images/color_wheel.png"));
    auto sequenceArea = ui->getSequenceArea();
    foreach (QAction *a, sequenceArea->colorSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, colorsSchemeMenu);
    }
    colorsSchemeMenu->addSeparator();

    QMenu *customColorSchemaMenu = new QMenu(tr("Custom schemes"), colorsSchemeMenu);
    customColorSchemaMenu->menuAction()->setObjectName("Custom schemes");

    foreach (QAction *a, sequenceArea->customColorSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, customColorSchemaMenu);
    }

    if (!sequenceArea->customColorSchemeMenuActions.isEmpty()) {
        customColorSchemaMenu->addSeparator();
    }

    customColorSchemaMenu->addAction(openCustomSettingsAction);

    colorsSchemeMenu->addMenu(customColorSchemaMenu);
    m->insertMenu(GUIUtils::findAction(m->actions(), MSAE_MENU_EDIT), colorsSchemeMenu);
}

void MSAEditor::addHighlightingMenu(QMenu *m) {
    QMenu *highlightSchemeMenu = new QMenu(tr("Highlighting"), NULL);

    highlightSchemeMenu->menuAction()->setObjectName("Highlighting");

    auto sequenceArea = ui->getSequenceArea();
    foreach (QAction *a, sequenceArea->highlightingSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, highlightSchemeMenu);
    }
    highlightSchemeMenu->addSeparator();
    highlightSchemeMenu->addAction(sequenceArea->useDotsAction);
    m->insertMenu(GUIUtils::findAction(m->actions(), MSAE_MENU_EDIT), highlightSchemeMenu);
}

void MSAEditor::addNavigationMenu(QMenu *m) {
    QMenu *navMenu = m->addMenu(tr("Navigation"));
    navMenu->menuAction()->setObjectName(MSAE_MENU_NAVIGATION);
    navMenu->addAction(gotoAction);
    navMenu->addSeparator();
    navMenu->addAction(searchInSequencesAction);
    navMenu->addAction(searchInSequenceNamesAction);
}

void MSAEditor::addTreeMenu(QMenu *m) {
    QMenu *em = m->addMenu(tr("Tree"));
    //em->setIcon(QIcon(":core/images/tree.png"));
    em->menuAction()->setObjectName(MSAE_MENU_TREES);
    em->addAction(buildTreeAction);
}

void MSAEditor::addAdvancedMenu(QMenu *m) {
    QMenu *menu = m->addMenu(tr("Advanced"));
    menu->menuAction()->setObjectName(MSAE_MENU_ADVANCED);

    if (convertDnaToRnaAction->isEnabled()) {
        menu->addAction(convertDnaToRnaAction);
    } else if (convertRnaToDnaAction->isEnabled()) {
        menu->addAction(convertRnaToDnaAction);
    }
}

void MSAEditor::addStatisticsMenu(QMenu *m) {
    QMenu *em = m->addMenu(tr("Statistics"));
    em->setIcon(QIcon(":core/images/chart_bar.png"));
    em->menuAction()->setObjectName(MSAE_MENU_STATISTICS);
}

MsaEditorWgt *MSAEditor::getUI() const {
    return qobject_cast<MsaEditorWgt *>(ui);
}

QWidget *MSAEditor::createWidget() {
    Q_ASSERT(ui == NULL);
    ui = new MsaEditorWgt(this);

    QString objName = "msa_editor_" + maObject->getGObjectName();
    ui->setObjectName(objName);

    initActions();

    connect(ui, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));

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

    searchInSequenceNamesAction = new QAction(QIcon(":core/images/find_dialog.png"), tr("Search in sequence names…"), this);
    searchInSequenceNamesAction->setObjectName("search_in_sequence_names");
    searchInSequenceNamesAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F));
    searchInSequenceNamesAction->setShortcutContext(Qt::WindowShortcut);
    searchInSequenceNamesAction->setToolTip(QString("%1 (%2)").arg(searchInSequenceNamesAction->text()).arg(searchInSequenceNamesAction->shortcut().toString()));
    connect(searchInSequenceNamesAction, SIGNAL(triggered()), this, SLOT(sl_searchInSequenceNames()));

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

    QList<OPFactoryFilterVisitorInterface *> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_AlignmentEditor));

    QList<OPWidgetFactory *> opWidgetFactories = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory *factory, opWidgetFactories) {
        optionsPanel->addGroup(factory);
    }

    connect(realignSomeSequenceAction, SIGNAL(triggered()), this, SLOT(sl_realignSomeSequences()));
    connect(maObject, SIGNAL(si_alphabetChanged(const MaModificationInfo &, const DNAAlphabet *)), SLOT(sl_updateRealignAction()));
    connect(ui->getSequenceArea(), SIGNAL(si_selectionChanged(const MaEditorSelection &, const MaEditorSelection &)), SLOT(sl_updateRealignAction()));

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
    addCopyPasteMenu(&m);
    addEditMenu(&m);
    addSortMenu(&m);
    m.addSeparator();

    addAlignMenu(&m);
    addTreeMenu(&m);
    addStatisticsMenu(&m);
    addExportMenu(&m);
    addAdvancedMenu(&m);

    m.addSeparator();
    snp.clickPoint = QCursor::pos();
    const QPoint nameMapped = ui->getEditorNameList()->mapFromGlobal(snp.clickPoint);
    const qint64 hoverRowId = (0 <= nameMapped.y()) ? ui->getEditorNameList()->sequenceIdAtPos(nameMapped) : U2MsaRow::INVALID_ROW_ID;
    if ((hoverRowId != getReferenceRowId() || U2MsaRow::INVALID_ROW_ID == getReferenceRowId()) && hoverRowId != U2MsaRow::INVALID_ROW_ID) {
        m.addAction(setAsReferenceSequenceAction);
    }
    if (U2MsaRow::INVALID_ROW_ID != getReferenceRowId()) {
        m.addAction(unsetReferenceSequenceAction);
    }
    m.addSeparator();

    emit si_buildPopupMenu(this, &m);

    GUIUtils::disableEmptySubmenus(&m);

    m.exec(QCursor::pos());
}

void MSAEditor::sl_showTreeOP() {
    OptionsPanelWidget *opWidget = dynamic_cast<OptionsPanelWidget *>(optionsPanel->getMainWidget());
    if (opWidget == nullptr) {
        return;
    }

    QWidget *addTreeGroupWidget = opWidget->findOptionsWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if (addTreeGroupWidget != nullptr) {
        addTreeGroupWidget->hide();
        opWidget->closeOptionsPanel();
    }
    QWidget *addTreeHeader = opWidget->findHeaderWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if (addTreeHeader != nullptr) {
        addTreeHeader->hide();
    }

    GroupHeaderImageWidget *header = opWidget->findHeaderWidgetByGroupId("OP_MSA_TREES_WIDGET");
    if (header != nullptr) {
        header->show();
        header->changeState();
    }
}

void MSAEditor::sl_hideTreeOP() {
    OptionsPanelWidget *opWidget = dynamic_cast<OptionsPanelWidget *>(optionsPanel->getMainWidget());
    if (opWidget == nullptr) {
        return;
    }
    GroupHeaderImageWidget *header = opWidget->findHeaderWidgetByGroupId("OP_MSA_TREES_WIDGET");
    QWidget *groupWidget = opWidget->findOptionsWidgetByGroupId("OP_MSA_TREES_WIDGET");
    bool openAddTreeGroup = groupWidget != nullptr;
    header->hide();

    GroupHeaderImageWidget *addTreeHeader = opWidget->findHeaderWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if (addTreeHeader != nullptr) {
        addTreeHeader->show();
        if (openAddTreeGroup) {
            addTreeHeader->changeState();
        }
    }
}

bool MSAEditor::eventFilter(QObject *, QEvent *e) {
    if (e->type() == QEvent::DragEnter || e->type() == QEvent::Drop) {
        QDropEvent *de = (QDropEvent *)e;
        const QMimeData *md = de->mimeData();
        const GObjectMimeData *gomd = qobject_cast<const GObjectMimeData *>(md);
        if (gomd != nullptr) {
            CHECK(!maObject->isStateLocked(), false)
            U2SequenceObject *dnaObj = qobject_cast<U2SequenceObject *>(gomd->objPtr.data());
            if (dnaObj != nullptr) {
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

void MSAEditor::initDragAndDropSupport() {
    SAFE_POINT(ui != nullptr, QString("MSAEditor::ui is not initialized in MSAEditor::initDragAndDropSupport"), );
    ui->setAcceptDrops(true);
    ui->installEventFilter(this);
}

void MSAEditor::sl_align() {
    QMenu m, *mm;

    addLoadMenu(&m);
    addCopyPasteMenu(&m);
    addEditMenu(&m);
    addSortMenu(&m);
    m.addSeparator();

    addAlignMenu(&m);
    addTreeMenu(&m);
    addStatisticsMenu(&m);
    addExportMenu(&m);
    addAdvancedMenu(&m);

    emit si_buildPopupMenu(this, &m);

    GUIUtils::disableEmptySubmenus(&m);

    mm = GUIUtils::findSubMenu(&m, MSAE_MENU_ALIGN);
    SAFE_POINT(mm != nullptr, "mm", );

    mm->exec(QCursor::pos());
}

void MSAEditor::sl_addToAlignment() {
    MultipleSequenceAlignmentObject *msaObject = getMaObject();
    CHECK(!maObject->isStateLocked(), );

    ProjectView *pv = AppContext::getProjectView();
    SAFE_POINT(pv != nullptr, "Project view is null", );

    const GObjectSelection *selection = pv->getGObjectSelection();
    SAFE_POINT(selection != nullptr, "GObjectSelection is null", );

    QList<GObject *> objects = selection->getSelectedObjects();
    bool selectFromProject = !objects.isEmpty();

    foreach (GObject *object, objects) {
        if (object == getMaObject() || (object->getGObjectType() != GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT && object->getGObjectType() != GObjectTypes::SEQUENCE)) {
            selectFromProject = false;
            break;
        }
    }
    if (selectFromProject) {
        alignSequencesFromObjectsToAlignment(objects);
    } else {
        alignSequencesFromFilesToAlignment();
    }
}

void MSAEditor::sl_searchInSequences() {
    auto optionsPanel = getOptionsPanel();
    SAFE_POINT(optionsPanel != nullptr, "Internal error: options panel is NULL"
                                        " when search in sequences was initiated!", );
    QVariantMap options = FindPatternMsaWidgetFactory::getOptionsToActivateSearchInSequences();
    optionsPanel->openGroupById(FindPatternMsaWidgetFactory::getGroupId(), options);
}

void MSAEditor::sl_searchInSequenceNames() {
    auto optionsPanel = getOptionsPanel();
    SAFE_POINT(optionsPanel != nullptr, "Internal error: options panel is NULL"
                                        " when search in sequence names was initiated!", );
    QVariantMap options = FindPatternMsaWidgetFactory::getOptionsToActivateSearchInNames();
    optionsPanel->openGroupById(FindPatternMsaWidgetFactory::getGroupId(), options);
}

void MSAEditor::sl_realignSomeSequences() {
    const MaEditorSelection &selection = getSelection();
    int startSeq = selection.y();
    int endSeq = selection.y() + selection.height() - 1;
    MaCollapseModel *model = ui->getCollapseModel();
    const MultipleAlignment &ma = ui->getEditor()->getMaObject()->getMultipleAlignment();
    QSet<qint64> rowIds;
    for (int i = startSeq; i <= endSeq; i++) {
        rowIds.insert(ma->getRow(model->getMaRowIndexByViewRowIndex(i))->getRowId());
    }
    Task *realignTask = new RealignSequencesInAlignmentTask(getMaObject(), rowIds);
    TaskWatchdog::trackResourceExistence(ui->getEditor()->getMaObject(), realignTask, tr("A problem occurred during realigning sequences. The multiple alignment is no more available."));
    AppContext::getTaskScheduler()->registerTopLevelTask(realignTask);
}

void MSAEditor::alignSequencesFromObjectsToAlignment(const QList<GObject *> &objects) {
    SequenceObjectsExtractor extractor;
    extractor.setAlphabet(maObject->getAlphabet());
    extractor.extractSequencesFromObjects(objects);

    if (!extractor.getSequenceRefs().isEmpty()) {
        AlignSequencesToAlignmentTask *task = new AlignSequencesToAlignmentTask(getMaObject(), extractor);
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
        urls = U2FileDialog::getOpenFileNames(ui, tr("Open file with sequences"), lod.dir, filter, 0, QFileDialog::DontUseNativeDialog);
    } else
#endif
        urls = U2FileDialog::getOpenFileNames(ui, tr("Open file with sequences"), lod.dir, filter);

    if (!urls.isEmpty()) {
        lod.url = urls.first();
        LoadSequencesAndAlignToAlignmentTask *task = new LoadSequencesAndAlignToAlignmentTask(getMaObject(), urls);
        TaskWatchdog::trackResourceExistence(maObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

void MSAEditor::sl_setSeqAsReference() {
    QPoint menuCallPos = snp.clickPoint;
    QPoint nameMapped = ui->getEditorNameList()->mapFromGlobal(menuCallPos);
    if (nameMapped.y() >= 0) {
        qint64 newRowId = ui->getEditorNameList()->sequenceIdAtPos(nameMapped);
        if (U2MsaRow::INVALID_ROW_ID != newRowId && newRowId != snp.seqId) {
            setReference(newRowId);
        }
    }
}

void MSAEditor::sl_unsetReferenceSeq() {
    if (U2MsaRow::INVALID_ROW_ID != getReferenceRowId()) {
        setReference(U2MsaRow::INVALID_ROW_ID);
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
    const MaEditorSelection &selection = getSelection();
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
    return (U2MsaRow::INVALID_ROW_ID != refSeq) ? alignment->getRowNames().at(refSeq) : QString();
}

char MSAEditor::getReferenceCharAt(int pos) const {
    CHECK(getReferenceRowId() != U2MsaRow::INVALID_ROW_ID, '\n');

    U2OpStatusImpl os;
    const int refSeq = maObject->getMultipleAlignment()->getRowIndexByRowId(getReferenceRowId(), os);
    SAFE_POINT_OP(os, '\n');

    return maObject->getMultipleAlignment()->charAt(refSeq, pos);
}

void MSAEditor::sl_showCustomSettings() {
    AppContext::getAppSettingsGUI()->showSettingsDialog(ColorSchemaSettingsPageId);
}

void MSAEditor::sortSequences(const MultipleAlignment::SortType &sortType, const MultipleAlignment::Order &sortOrder) {
    MultipleSequenceAlignmentObject *msaObject = getMaObject();
    CHECK(!msaObject->isStateLocked(), );

    MultipleSequenceAlignment msa = msaObject->getMultipleAlignmentCopy();
    const MaEditorSelection &selection = getSelection();
    U2Region sortRange = selection.height() <= 1 ? U2Region() : U2Region(selection.y(), selection.height());
    msa->sortRows(sortType, sortOrder, sortRange);

    // Switch into 'Original' ordering mode.
    getUI()->getSequenceArea()->sl_toggleSequenceRowOrder(false);

    QStringList rowNames = msa->getRowNames();
    if (rowNames != msaObject->getMultipleAlignment()->getRowNames()) {
        U2OpStatusImpl os;
        msaObject->updateRowsOrder(os, msa->getRowsIds());
    }
}

void MSAEditor::sl_sortSequencesByName() {
    MultipleAlignment::Order order = sender() == sortByNameDescendingAction ? MultipleAlignment::Descending : MultipleAlignment::Ascending;
    sortSequences(MultipleAlignment::SortByName, order);
}

void MSAEditor::sl_sortSequencesByLength() {
    MultipleAlignment::Order order = sender() == sortByLengthDescendingAction ? MultipleAlignment::Descending : MultipleAlignment::Ascending;
    sortSequences(MultipleAlignment::SortByLength, order);
}

void MSAEditor::sl_sortSequencesByLeadingGap() {
    MultipleAlignment::Order order = sender() == sortByLeadingGapDescendingAction ? MultipleAlignment::Descending : MultipleAlignment::Ascending;
    sortSequences(MultipleAlignment::SortByLeadingGap, order);
}

void MSAEditor::sl_convertBetweenDnaAndRnaAlphabets() {
    CHECK(!maObject->isStateLocked(), )

    auto alphabetId = maObject->getAlphabet()->getId();
    bool isDnaAlphabet = alphabetId == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    bool isRnaAlphabet = alphabetId == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT();
    CHECK(isDnaAlphabet || isRnaAlphabet, );

    auto msaObject = getMaObject();
    auto alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(msaObject->getEntityRef(), os);
    auto resultAlphabet = alphabetRegistry->findById(isDnaAlphabet ? BaseDNAAlphabetIds::NUCL_RNA_DEFAULT() : BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    char fromChar = isDnaAlphabet ? 'T' : 'U';
    char toChar = isDnaAlphabet ? 'U' : 'T';
    msaObject->replaceAllCharacters(fromChar, toChar, resultAlphabet);
}

}    // namespace U2
