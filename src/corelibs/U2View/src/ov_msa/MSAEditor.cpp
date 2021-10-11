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

#include "MSAEditor.h"

#include <QGroupBox>
#include <QVBoxLayout>

#include <QDropEvent>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/GroupHeaderImageWidget.h>
#include <U2Gui/GroupOptionsWidget.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OptionsPanelWidget.h>
#include <U2Gui/ProjectView.h>

#include <U2View/ColorSchemaSettingsController.h>
#include <U2View/FindPatternMsaWidgetFactory.h>

#include "MSAEditorOffsetsView.h"
#include "MSAEditorOverviewArea.h"
#include "MSAEditorSequenceArea.h"
#include "MaEditorConsensusArea.h"
#include "MaEditorFactory.h"
#include "MaEditorNameList.h"
#include "MaEditorSelection.h"
#include "MaEditorSequenceArea.h"
#include "MaEditorTasks.h"
#include "align_to_alignment/RealignSequencesInAlignmentTask.h"
#include "export_image/MSAImageExportTask.h"
#include "highlighting/MsaSchemesMenuBuilder.h"
#include "move_to_object/MoveToObjectMaController.h"
#include "overview/MaEditorOverviewArea.h"

namespace U2 {

const QString MsaEditorMenuType::ALIGN("msa_editor_menu_align");
const QString MsaEditorMenuType::ALIGN_NEW_SEQUENCES_TO_ALIGNMENT("msa_editor_menu_align_new_sequences_to_alignment");
const QString MsaEditorMenuType::ALIGN_NEW_ALIGNMENT_TO_ALIGNMENT("msa_editor_menu_align_new_alignment_to_alignment");
const QString MsaEditorMenuType::ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT("msa_editor_menu_align_selected_sequences_to_alignment");

MSAEditor::MSAEditor(const QString& viewName, MultipleSequenceAlignmentObject* obj)
    : MaEditor(MsaEditorFactory::ID, viewName, obj),
      treeManager(this) {
    selectionController = new MaEditorSelectionController(this);
    connect(maObject, SIGNAL(si_rowsRemoved(const QList<qint64>&)), SLOT(sl_rowsRemoved(const QList<qint64>&)));

    searchInSequencesAction = nullptr;
    searchInSequenceNamesAction = nullptr;

    sortByNameAscendingAction = new QAction(tr("By name"), this);
    sortByNameAscendingAction->setObjectName("action_sort_by_name");
    sortByNameAscendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by name, ascending"));
    connect(sortByNameAscendingAction, SIGNAL(triggered()), SLOT(sl_sortSequencesByName()));

    sortByNameDescendingAction = new QAction(tr("By name, descending"), this);
    sortByNameDescendingAction->setObjectName("action_sort_by_name_descending");
    sortByNameDescendingAction->setToolTip(tr("Sort selected sequences range or the whole alignment by name, descending"));
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

    sortGroupsBySizeAscendingAction = new QAction(tr("Sort groups, small first"), this);
    sortGroupsBySizeAscendingAction->setObjectName("action_sort_groups_by_size_ascending");
    sortGroupsBySizeAscendingAction->setToolTip(tr("Sort groups by number of sequences in the group, ascending"));
    connect(sortGroupsBySizeAscendingAction, SIGNAL(triggered()), SLOT(sl_sortGroupsBySize()));

    sortGroupsBySizeDescendingAction = new QAction(tr("Sort groups, large first"), this);
    sortGroupsBySizeDescendingAction->setObjectName("action_sort_groups_by_size_descending");
    sortGroupsBySizeDescendingAction->setToolTip(tr("Sort groups by number of sequences in the group, descending"));
    connect(sortGroupsBySizeDescendingAction, SIGNAL(triggered()), SLOT(sl_sortGroupsBySize()));

    saveScreenshotAction = new QAction(QIcon(":/core/images/cam2.png"), tr("Export as image"), this);
    saveScreenshotAction->setObjectName("export_msa_as_image_action");
    connect(saveScreenshotAction, &QAction::triggered, this, &MSAEditor::sl_exportImage);

    buildTreeAction = new QAction(QIcon(":/core/images/phylip.png"), tr("Build Tree"), this);
    buildTreeAction->setObjectName("Build Tree");
    connect(buildTreeAction, SIGNAL(triggered()), SLOT(sl_buildTree()));

    alignSelectedSequencesToAlignmentAction = new QAction(QIcon(":/core/images/realign_some_sequences.png"), tr("Realign sequence(s) to other sequences"), this);
    alignSelectedSequencesToAlignmentAction->setObjectName("align_selected_sequences_to_alignment");

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

    convertRawToDnaAction = new QAction(tr("Convert RAW to DNA alphabet"), this);
    convertRawToDnaAction->setObjectName("convertRawToDnaAction");
    convertRawToDnaAction->setToolTip(tr("Convert alignment from RAW to DNA alphabet: use N for unknown symbols"));
    connect(convertRawToDnaAction, SIGNAL(triggered()), SLOT(sl_convertRawToDnaAlphabet()));

    convertRawToAminoAction = new QAction(tr("Convert RAW to Amino alphabet"), this);
    convertRawToAminoAction->setObjectName("convertRawToAminoAction");
    convertRawToAminoAction->setToolTip(tr("Convert alignment from RAW to Amino alphabet: use X for unknown symbols"));
    connect(convertRawToAminoAction, SIGNAL(triggered()), SLOT(sl_convertRawToAminoAlphabet()));

    gotoAction = new QAction(QIcon(":core/images/goto.png"), tr("Go to position…"), this);
    gotoAction->setObjectName("action_go_to_position");
    gotoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    gotoAction->setShortcutContext(Qt::WindowShortcut);
    gotoAction->setToolTip(QString("%1 (%2)").arg(gotoAction->text()).arg(gotoAction->shortcut().toString()));

    initZoom();
    initFont();
}

void MSAEditor::updateActions() {
    MaEditor::updateActions();
    bool isReadOnly = maObject->isStateLocked();

    sortByNameAscendingAction->setEnabled(!isReadOnly);
    sortByNameDescendingAction->setEnabled(!isReadOnly);
    sortByLengthAscendingAction->setEnabled(!isReadOnly);
    sortByLengthDescendingAction->setEnabled(!isReadOnly);

    alignNewSequencesToAlignmentAction->setEnabled(!isReadOnly);

    buildTreeAction->setEnabled(maObject->getRowCount() >= 3);

    sl_updateRealignAction();

    auto alphabetId = maObject->getAlphabet()->getId();
    convertDnaToRnaAction->setEnabled(!isReadOnly && alphabetId == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    convertRnaToDnaAction->setEnabled(!isReadOnly && alphabetId == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT());
    convertRawToDnaAction->setEnabled(!isReadOnly && alphabetId == BaseDNAAlphabetIds::RAW());
    convertRawToAminoAction->setEnabled(!isReadOnly && alphabetId == BaseDNAAlphabetIds::RAW());

    // Sorting of groups is enabled only on "group by content" mode.
    // This 'virtual' mode is 100% managed by MSA Editor and is not saved to file.
    bool isGroupBySequenceContent = getRowOrderMode() == MaEditorRowOrderMode::Sequence;
    sortGroupsBySizeAscendingAction->setEnabled(isGroupBySequenceContent);
    sortGroupsBySizeDescendingAction->setEnabled(isGroupBySequenceContent);
}

void MSAEditor::sl_buildTree() {
    treeManager.buildTreeWithDialog();
}

bool MSAEditor::onObjectRemoved(GObject* obj) {
    bool result = GObjectView::onObjectRemoved(obj);

    obj->disconnect(getUI()->getSequenceArea());
    obj->disconnect(getUI()->getConsensusArea());
    obj->disconnect(getUI()->getEditorNameList());
    return result;
}

void MSAEditor::onObjectRenamed(GObject*, const QString&) {
    // update title
    OpenMaEditorTask::updateTitle(this);
}

MultipleSequenceAlignmentRow MSAEditor::getRowByViewRowIndex(int viewRowIndex) const {
    int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
    return getMaObject()->getMsaRow(maRowIndex);
}

MSAEditor::~MSAEditor() {
    delete pairwiseAlignmentWidgetsSettings;
}

void MSAEditor::buildStaticToolbar(QToolBar *tb) {
    tb->addAction(getUI()->copyFormattedSelectionAction);

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
    tb->addAction(alignNewSequencesToAlignmentAction);
    tb->addAction(alignSelectedSequencesToAlignmentAction);

    GObjectView::buildStaticToolbar(tb);
}

void MSAEditor::buildMenu(QMenu* m, const QString& type) {
    if (type != MsaEditorMenuType::STATIC) {
        GObjectView::buildMenu(m, type);
        return;
    }
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

    GObjectView::buildMenu(m, type);

    GUIUtils::disableEmptySubmenus(m);
}

void MSAEditor::addCopyPasteMenu(QMenu* m) {
    MaEditor::addCopyPasteMenu(m);

    QMenu* copyMenu = GUIUtils::findSubMenu(m, MSAE_MENU_COPY);
    SAFE_POINT(copyMenu != nullptr, "copyMenu is null", );

    const MaEditorSelection &selection = getSelection();
    getUI()->copySelectionAction->setDisabled(selection.isEmpty());

    // TODO:? move the signal emit point to a correct location.
    auto sequenceArea = qobject_cast<MSAEditorSequenceArea *>(getUI()->getSequenceArea());
    SAFE_POINT(sequenceArea != nullptr, "sequenceArea is null", );
    emit sequenceArea->si_copyFormattedChanging(!selection.isEmpty());

    copyMenu->addAction(getUI()->copySelectionAction);
    getUI()->copyFormattedSelectionAction->setDisabled(selection.isEmpty());
    copyMenu->addAction(getUI()->copyFormattedSelectionAction);
    copyMenu->addAction(copyConsensusAction);
    copyMenu->addAction(copyConsensusWithGapsAction);
    copyMenu->addSeparator();
    copyMenu->addAction(getUI()->pasteAction);
    copyMenu->addAction(getUI()->pasteBeforeAction);
    copyMenu->addSeparator();
    copyMenu->addAction(getUI()->cutSelectionAction);

    copyMenu->addSeparator();
    MaEditorNameList *nameList = getUI()->getEditorNameList();
    copyMenu->addAction(nameList->copyWholeRowAction);
}

void MSAEditor::addEditMenu(QMenu* m) {
    QMenu* menu = m->addMenu(tr("Edit"));
    menu->menuAction()->setObjectName(MSAE_MENU_EDIT);
}

void MSAEditor::addSortMenu(QMenu* m) {
    QMenu* menu = m->addMenu(tr("Sort"));
    menu->menuAction()->setObjectName(MSAE_MENU_SORT);
    menu->addAction(sortByNameAscendingAction);
    menu->addAction(sortByNameDescendingAction);
    menu->addAction(sortByLengthAscendingAction);
    menu->addAction(sortByLengthDescendingAction);
    menu->addAction(sortByLeadingGapAscendingAction);
    menu->addAction(sortByLeadingGapDescendingAction);

    if (getRowOrderMode() == MaEditorRowOrderMode::Sequence) {
        menu->addSeparator();
        menu->addAction(sortGroupsBySizeDescendingAction);
        menu->addAction(sortGroupsBySizeAscendingAction);
    }
}

void MSAEditor::addAlignMenu(QMenu* m) {
    QMenu* alignMenu = m->addMenu(tr("Align"));
    alignMenu->setIcon(QIcon(":core/images/align.png"));
    alignMenu->menuAction()->setObjectName(MSAE_MENU_ALIGN);

    buildActionMenu(alignMenu,
                    {
                        MsaEditorMenuType::ALIGN,
                        MsaEditorMenuType::ALIGN_NEW_SEQUENCES_TO_ALIGNMENT,
                        MsaEditorMenuType::ALIGN_NEW_ALIGNMENT_TO_ALIGNMENT,
                        MsaEditorMenuType::ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT,
                    });
}

void MSAEditor::addExportMenu(QMenu* m) {
    MaEditor::addExportMenu(m);
    QMenu* em = GUIUtils::findSubMenu(m, MSAE_MENU_EXPORT);
    SAFE_POINT(em != nullptr, "Export menu not found", );
    em->addAction(saveScreenshotAction);
}

void MSAEditor::addAppearanceMenu(QMenu* m) {
    QMenu* appearanceMenu = m->addMenu(tr("Appearance"));
    appearanceMenu->menuAction()->setObjectName(MSAE_MENU_APPEARANCE);

    appearanceMenu->addAction(showOverviewAction);
    auto offsetsController = getUI()->getOffsetsViewController();
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

void MSAEditor::addColorsMenu(QMenu* m) {
    QMenu* colorsSchemeMenu = m->addMenu(tr("Colors"));
    colorsSchemeMenu->menuAction()->setObjectName("Colors");
    colorsSchemeMenu->setIcon(QIcon(":core/images/color_wheel.png"));
    auto sequenceArea = getUI()->getSequenceArea();
    foreach (QAction *a, sequenceArea->colorSchemeMenuActions) {
        MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(a, colorsSchemeMenu);
    }
    colorsSchemeMenu->addSeparator();

    QMenu* customColorSchemaMenu = new QMenu(tr("Custom schemes"), colorsSchemeMenu);
    customColorSchemaMenu->menuAction()->setObjectName("Custom schemes");

    foreach (QAction* a, sequenceArea->customColorSchemeMenuActions) {
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
    QMenu* highlightSchemeMenu = new QMenu(tr("Highlighting"), nullptr);

    highlightSchemeMenu->menuAction()->setObjectName("Highlighting");

    auto sequenceArea = getUI()->getSequenceArea();
    foreach (QAction *a, sequenceArea->highlightingSchemeMenuActions) {
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
    navMenu->addAction(searchInSequenceNamesAction);
}

void MSAEditor::addTreeMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Tree"));
    // em->setIcon(QIcon(":core/images/tree.png"));
    em->menuAction()->setObjectName(MSAE_MENU_TREES);
    em->addAction(buildTreeAction);
}

void MSAEditor::addAdvancedMenu(QMenu* m) {
    QMenu* menu = m->addMenu(tr("Advanced"));
    menu->menuAction()->setObjectName(MSAE_MENU_ADVANCED);

    if (convertDnaToRnaAction->isEnabled()) {
        menu->addAction(convertDnaToRnaAction);
    } else if (convertRnaToDnaAction->isEnabled()) {
        menu->addAction(convertRnaToDnaAction);
    }
}

void MSAEditor::addStatisticsMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Statistics"));
    em->setIcon(QIcon(":core/images/chart_bar.png"));
    em->menuAction()->setObjectName(MSAE_MENU_STATISTICS);
}

MsaEditorWgt *MSAEditor::getUI(uint index) const {
    return uiChild == nullptr ? nullptr : qobject_cast<MsaEditorWgt *>(uiChild[index]);
}

QWidget *MSAEditor::createWidget()
{
    Q_ASSERT(ui == nullptr && uiChild == nullptr && uiChildCount == 0 && uiChildLength == 0);

    MsaEditorMultilineWgt *uiX = new MsaEditorMultilineWgt(this);
    uiX->setObjectName("msa_editor_vertical_childs_layout_" + maObject->getGObjectName());

    ui = new QGroupBox(tr("MSA vertical child layout"));
    ui->setObjectName("msa_editor_vertical_childs_layout_" + maObject->getGObjectName());
    QVBoxLayout *layout = new QVBoxLayout;
    ui->setLayout(layout);

    uiChildLength = 2;
    uiChild = new MaEditorWgt *[2];
    MSAEditorOverviewArea *overviewArea = nullptr;

    for (int i = 0; i < 2; i++) {
        uiChild[i] = nullptr;
        MsaEditorWgt *child = createChildWidget(i);
        Q_ASSERT(child != nullptr);
        layout->addWidget(child);
        uiChildCount++;
    }
    layout->addWidget(overviewArea);

    return uiX;
}

MsaEditorWgt *MSAEditor::createChildWidget(uint index,
                                           MaEditorOverviewArea *overview,
                                           MaEditorStatusBar *statusbar) {
    Q_ASSERT(uiChild[index] == nullptr);
    uiChild[index] = new MsaEditorWgt(this, overview, statusbar);


    QString objName = QString("msa_editor_" + maObject->getGObjectName() + "%1").arg(index);
    uiChild[index]->setObjectName(objName);

    initActions(index);

    connect(uiChild[index], SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));

    gotoAction = new QAction(QIcon(":core/images/goto.png"), tr("Go to position…"), this);
    gotoAction->setObjectName("action_go_to_position");
    gotoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    gotoAction->setShortcutContext(Qt::WindowShortcut);
    gotoAction->setToolTip(QString("%1 (%2)").arg(gotoAction->text()).arg(gotoAction->shortcut().toString()));
    connect(gotoAction, SIGNAL(triggered()), uiChild[index]->getSequenceArea(), SLOT(sl_goto()));

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

    alignNewSequencesToAlignmentAction = new QAction(QIcon(":/core/images/add_to_alignment.png"), tr("Align sequence(s) to this alignment"), this);
    alignNewSequencesToAlignmentAction->setObjectName("align_new_sequences_to_alignment_action");
    connect(alignNewSequencesToAlignmentAction, &QAction::triggered, this, &MSAEditor::sl_alignNewSequencesToAlignment);

    setAsReferenceSequenceAction = new QAction(tr("Set this sequence as reference"), this);
    setAsReferenceSequenceAction->setObjectName("set_seq_as_reference");
    connect(setAsReferenceSequenceAction, SIGNAL(triggered()), SLOT(sl_setSeqAsReference()));

    unsetReferenceSequenceAction = new QAction(tr("Unset reference sequence"), this);
    unsetReferenceSequenceAction->setObjectName("unset_reference");
    connect(unsetReferenceSequenceAction, SIGNAL(triggered()), SLOT(sl_unsetReferenceSeq()));

    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_AlignmentEditor));

    QList<OPWidgetFactory*> opWidgetFactories = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory* factory, opWidgetFactories) {
        optionsPanel->addGroup(factory);
    }

    connect(alignSelectedSequencesToAlignmentAction, &QAction::triggered, this, &MSAEditor::sl_alignSelectedSequencesToAlignment);

    connect(maObject, SIGNAL(si_alphabetChanged(const MaModificationInfo&, const DNAAlphabet*)), SLOT(sl_updateRealignAction()));
    connect(getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_updateRealignAction()));

    qDeleteAll(filters);

    connect(uiChild[index], SIGNAL(si_showTreeOP()), SLOT(sl_showTreeOP()));
    connect(uiChild[index], SIGNAL(si_hideTreeOP()), SLOT(sl_hideTreeOP()));
    sl_hideTreeOP();

    treeManager.loadRelatedTrees();

    new MoveToObjectMaController(this);

    initDragAndDropSupport();
    updateActions();
    return (MsaEditorWgt *)uiChild[index];
}

void MSAEditor::sl_onContextMenuRequested(const QPoint& /*pos*/) {
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
    const QPoint nameMapped = getUI()->getEditorNameList()->mapFromGlobal(snp.clickPoint);
    const qint64 hoverRowId = (0 <= nameMapped.y()) ? getUI()->getEditorNameList()->sequenceIdAtPos(nameMapped) : U2MsaRow::INVALID_ROW_ID;
    if ((hoverRowId != getReferenceRowId() || U2MsaRow::INVALID_ROW_ID == getReferenceRowId()) && hoverRowId != U2MsaRow::INVALID_ROW_ID) {
        m.addAction(setAsReferenceSequenceAction);
    }
    if (U2MsaRow::INVALID_ROW_ID != getReferenceRowId()) {
        m.addAction(unsetReferenceSequenceAction);
    }
    m.addSeparator();

    emit si_buildMenu(this, &m, MsaEditorMenuType::CONTEXT);

    GUIUtils::disableEmptySubmenus(&m);

    m.exec(QCursor::pos());
}

void MSAEditor::sl_showTreeOP() {
    OptionsPanelWidget* opWidget = dynamic_cast<OptionsPanelWidget*>(optionsPanel->getMainWidget());
    if (opWidget == nullptr) {
        return;
    }

    QWidget* addTreeGroupWidget = opWidget->findOptionsWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if (addTreeGroupWidget != nullptr) {
        addTreeGroupWidget->hide();
        opWidget->closeOptionsPanel();
    }
    QWidget* addTreeHeader = opWidget->findHeaderWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if (addTreeHeader != nullptr) {
        addTreeHeader->hide();
    }

    GroupHeaderImageWidget* header = opWidget->findHeaderWidgetByGroupId("OP_MSA_TREES_WIDGET");
    if (header != nullptr) {
        header->show();
        header->changeState();
    }
}

void MSAEditor::sl_hideTreeOP() {
    OptionsPanelWidget* opWidget = dynamic_cast<OptionsPanelWidget*>(optionsPanel->getMainWidget());
    if (opWidget == nullptr) {
        return;
    }
    GroupHeaderImageWidget* header = opWidget->findHeaderWidgetByGroupId("OP_MSA_TREES_WIDGET");
    QWidget* groupWidget = opWidget->findOptionsWidgetByGroupId("OP_MSA_TREES_WIDGET");
    bool openAddTreeGroup = groupWidget != nullptr;
    header->hide();

    GroupHeaderImageWidget* addTreeHeader = opWidget->findHeaderWidgetByGroupId("OP_MSA_ADD_TREE_WIDGET");
    if (addTreeHeader != nullptr) {
        addTreeHeader->show();
        if (openAddTreeGroup) {
            addTreeHeader->changeState();
        }
    }
}

bool MSAEditor::eventFilter(QObject*, QEvent* e) {
    if (e->type() == QEvent::DragEnter || e->type() == QEvent::Drop) {
        QDropEvent* de = (QDropEvent*)e;
        const QMimeData* md = de->mimeData();
        const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
        if (gomd != nullptr) {
            CHECK(!maObject->isStateLocked(), false)
            U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(gomd->objPtr.data());
            if (dnaObj != nullptr) {
                if (U2AlphabetUtils::deriveCommonAlphabet(dnaObj->getAlphabet(), maObject->getAlphabet()) == nullptr) {
                    return false;
                }
                if (e->type() == QEvent::DragEnter) {
                    de->acceptProposedAction();
                } else {
                    U2OpStatusImpl os;
                    DNASequence seq = dnaObj->getWholeSequence(os);
                    seq.alphabet = dnaObj->getAlphabet();
                    Task* task = new AddSequenceObjectsToAlignmentTask(getMaObject(), QList<DNASequence>() << seq);
                    TaskWatchdog::trackResourceExistence(maObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
                    AppContext::getTaskScheduler()->registerTopLevelTask(task);
                }
            }
        }
    }

    return false;
}

void MSAEditor::initDragAndDropSupport() {
    SAFE_POINT(getUI() != nullptr, QString("MSAEditor::ui is not initialized in MSAEditor::initDragAndDropSupport"), );
    getUI()->setAcceptDrops(true);
    getUI()->installEventFilter(this);
}

void MSAEditor::sl_align() {
    QMenu menu;
    buildActionMenu(&menu, MsaEditorMenuType::ALIGN);
    menu.exec(QCursor::pos());
}

void MSAEditor::sl_alignNewSequencesToAlignment() {
    QMenu menu;
    buildActionMenu(&menu, {MsaEditorMenuType::ALIGN_NEW_SEQUENCES_TO_ALIGNMENT, MsaEditorMenuType::ALIGN_NEW_ALIGNMENT_TO_ALIGNMENT});
    menu.exec(QCursor::pos());
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

void MSAEditor::sl_alignSelectedSequencesToAlignment() {
    QMenu menu;
    buildActionMenu(&menu, {MsaEditorMenuType::ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT});
    menu.exec(QCursor::pos());
}

void MSAEditor::sl_setSeqAsReference() {
    QPoint menuCallPos = snp.clickPoint;
    QPoint nameMapped = getUI()->getEditorNameList()->mapFromGlobal(menuCallPos);
    if (nameMapped.y() >= 0) {
        qint64 newRowId = getUI()->getEditorNameList()->sequenceIdAtPos(nameMapped);
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

void MSAEditor::sl_rowsRemoved(const QList<qint64>& rowIds) {
    foreach (qint64 rowId, rowIds) {
        if (getReferenceRowId() == rowId) {
            sl_unsetReferenceSeq();
            break;
        }
    }
}

void MSAEditor::sl_updateRealignAction() {
    if (maObject->isStateLocked() || maObject->getAlphabet()->isRaw() || ui == nullptr) {
        alignSelectedSequencesToAlignmentAction->setDisabled(true);
        return;
    }
    const MaEditorSelection& selection = getSelection();
    int selectionWidth = selection.getWidth();
    int selectedRowsCount = selection.getCountOfSelectedRows();
    bool isWholeSequenceSelection = selectionWidth == maObject->getLength() && selectedRowsCount >= 1;
    bool isAllRowsSelection = selectedRowsCount == collapseModel->getViewRowCount();
    alignSelectedSequencesToAlignmentAction->setEnabled(isWholeSequenceSelection && !isAllRowsSelection);
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

void MSAEditor::sortSequences(const MultipleAlignment::SortType& sortType, const MultipleAlignment::Order& sortOrder) {
    MultipleSequenceAlignmentObject* msaObject = getMaObject();
    CHECK(!msaObject->isStateLocked(), );

    MultipleSequenceAlignment msa = msaObject->getMultipleAlignmentCopy();
    const MaEditorSelection& selection = getSelection();
    QRect selectionRect = selection.toRect();
    U2Region sortRange = selectionRect.height() <= 1 ? U2Region() : U2Region(selectionRect.y(), selectionRect.height());
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

void MSAEditor::sl_convertRawToDnaAlphabet() {
    CHECK(!maObject->isStateLocked(), )

    auto alphabetId = maObject->getAlphabet()->getId();
    CHECK(alphabetId == BaseDNAAlphabetIds::RAW(), );

    auto msaObject = getMaObject();
    auto alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(msaObject->getEntityRef(), os);
    auto resultAlphabet = alphabetRegistry->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    QByteArray replacementMap(256, '\0');
    replacementMap['U'] = 'T';
    msaObject->morphAlphabet(resultAlphabet, replacementMap);
}

void MSAEditor::sl_convertRawToAminoAlphabet() {
    CHECK(!maObject->isStateLocked(), )

    auto alphabetId = maObject->getAlphabet()->getId();
    CHECK(alphabetId == BaseDNAAlphabetIds::RAW(), );

    auto msaObject = getMaObject();
    auto alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(msaObject->getEntityRef(), os);
    auto resultAlphabet = alphabetRegistry->findById(BaseDNAAlphabetIds::AMINO_DEFAULT());
    msaObject->morphAlphabet(resultAlphabet);
}

void MSAEditor::sl_sortGroupsBySize() {
    groupsSortOrder = sender() == sortGroupsBySizeAscendingAction ? GroupsSortOrder::Ascending : GroupsSortOrder::Descending;
    updateCollapseModel();
}

// TODO: move this function into MSA?
/* Groups rows by similarity. Two rows are considered equal if their sequences are equal with ignoring of gaps. */
static QList<QList<int>> groupRowsBySimilarity(const QList<MultipleAlignmentRow>& msaRows) {
    QList<QList<int>> rowGroups;
    QSet<int> mappedRows;  // contains indexes of the already processed rows.
    for (int i = 0; i < msaRows.size(); i++) {
        if (mappedRows.contains(i)) {
            continue;
        }
        const MultipleAlignmentRow& row = msaRows[i];
        QList<int> rowGroup;
        rowGroup << i;
        for (int j = i + 1; j < msaRows.size(); j++) {
            const MultipleAlignmentRow& next = msaRows[j];
            if (!mappedRows.contains(j) && MultipleAlignmentRowData::isEqualIgnoreGaps(next.data(), row.data())) {
                rowGroup << j;
                mappedRows.insert(j);
            }
        }
        rowGroups << rowGroup;
    }
    return rowGroups;
}

void MSAEditor::updateCollapseModel() {
    if (rowOrderMode == MaEditorRowOrderMode::Original) {
        // Synchronize collapsible model with a current alignment.
        collapseModel->reset(getMaRowIds());
        return;
    } else if (rowOrderMode == MaEditorRowOrderMode::Free) {
        // Check if the modification is compatible with the current view state: all rows have view properties assigned. Reset to the Original order if not.
        QSet<qint64> maRowIds = getMaRowIds().toSet();
        QSet<qint64> viewModelRowIds = collapseModel->getAllRowIds();
        if (viewModelRowIds != maRowIds) {
            rowOrderMode = MaEditorRowOrderMode::Original;
            collapseModel->reset(getMaRowIds());
        }
        return;
    }

    SAFE_POINT(rowOrderMode == MaEditorRowOrderMode::Sequence, "Unexpected row order mode", );

    // Order and group rows by sequence content.
    MultipleSequenceAlignmentObject* msaObject = getMaObject();
    QList<QList<int>> rowGroups = groupRowsBySimilarity(msaObject->getRows());
    QVector<MaCollapsibleGroup> newCollapseGroups;

    QSet<qint64> maRowIdsOfNonCollapsedRowsBefore;
    for (int i = 0; i < collapseModel->getGroupCount(); i++) {
        const MaCollapsibleGroup* group = collapseModel->getCollapsibleGroup(i);
        if (!group->isCollapsed) {
            maRowIdsOfNonCollapsedRowsBefore += group->maRowIds.toSet();
        }
    }
    for (int i = 0; i < rowGroups.size(); i++) {
        const QList<int>& maRowsInGroup = rowGroups[i];
        QList<qint64> maRowIdsInGroup = msaObject->getMultipleAlignment()->getRowIdsByRowIndexes(maRowsInGroup);
        bool isCollapsed = !maRowIdsOfNonCollapsedRowsBefore.contains(maRowIdsInGroup[0]);
        newCollapseGroups << MaCollapsibleGroup(maRowsInGroup, maRowIdsInGroup, isCollapsed);
    }
    if (groupsSortOrder != GroupsSortOrder::Original) {
        std::stable_sort(newCollapseGroups.begin(), newCollapseGroups.end(), [&](const MaCollapsibleGroup& g1, const MaCollapsibleGroup& g2) {
            int size1 = g1.maRowIds.size();
            int size2 = g2.maRowIds.size();
            return groupsSortOrder == GroupsSortOrder::Ascending ? size1 < size2 : size2 < size1;
        });
    }
    collapseModel->update(newCollapseGroups);
}

void MSAEditor::setRowOrderMode(MaEditorRowOrderMode mode) {
    if (mode == rowOrderMode) {
        return;
    }
    MaEditor::setRowOrderMode(mode);
    freeModeMasterMarkersSet.clear();
    updateCollapseModel();
    updateActions();
}

const QSet<QObject*>& MSAEditor::getFreeModeMasterMarkersSet() const {
    return freeModeMasterMarkersSet;
}

void MSAEditor::addFreeModeMasterMarker(QObject* marker) {
    freeModeMasterMarkersSet.insert(marker);
}

void MSAEditor::removeFreeModeMasterMarker(QObject* marker) {
    freeModeMasterMarkersSet.remove(marker);
}

MaEditorSelectionController* MSAEditor::getSelectionController() const {
    return selectionController;
}

void MSAEditor::sl_exportImage() {
    MSAImageExportController controller(getUI());
    QWidget *parentWidget = (QWidget *)AppContext::getMainWindow()->getQMainWindow();
    QString fileName = GUrlUtils::fixFileName(maObject->getGObjectName());
    QObjectScopedPointer<ExportImageDialog> dlg = new ExportImageDialog(&controller,
                                                                        ExportImageDialog::MSA,
                                                                        fileName,
                                                                        ExportImageDialog::NoScaling,
                                                                        parentWidget);
    dlg->exec();
}

}  // namespace U2
