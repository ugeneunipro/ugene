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

#include "MsaExcludeList.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/FileFilters.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Gui/GUIUtils.h>

#include "ov_msa/MSAEditor.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditorFactory.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorSplitters.h"

namespace U2 {
////////////////////////////////////
/// MsaExcludeListContext
///////////////////////////////////
MsaExcludeListContext::MsaExcludeListContext(QObject* parent)
    : GObjectViewWindowContext(parent, MsaEditorFactory::ID) {
}

static const char* TOGGLE_EXCLUDE_LIST_ACTION_NAME = "exclude_list_toggle_action";
static const char* MOVE_MSA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME = "exclude_list_move_from_msa_action";

void MsaExcludeListContext::initViewContext(GObjectView* view) {
    auto msaEditor = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "View is not MSAEditor!", );
    msaEditor->registerActionProvider(this);

    // Toggles exclude list view in MSA editor. See MsaExcludeList for details.
    auto toggleExcludeListAction = new GObjectViewAction(this, view, tr("Show Exclude List"));
    toggleExcludeListAction->setIcon(QIcon(":core/images/inbox-minus.png"));
    toggleExcludeListAction->setCheckable(true);
    toggleExcludeListAction->setObjectName(TOGGLE_EXCLUDE_LIST_ACTION_NAME);
    toggleExcludeListAction->setToolTip(tr("Show/Hide Exclude List view visibility"));
    connect(toggleExcludeListAction, &QAction::triggered, this, [this, msaEditor] { toggleExcludeListView(msaEditor); });
    connect(view, &GObjectView::si_buildStaticToolbar, this, [toggleExcludeListAction](GObjectView*, QToolBar* toolBar) { toolBar->addAction(toggleExcludeListAction); });
    addViewAction(toggleExcludeListAction);

    auto moveFromMsaAction = new GObjectViewAction(this, view, tr("Move to Exclude List"));
    moveFromMsaAction->setIcon(QIcon(":core/images/arrow-move-down.png"));
    moveFromMsaAction->setObjectName(MOVE_MSA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME);
    moveFromMsaAction->setToolTip(tr("Move selected MSA sequences to Exclude List"));
    connect(moveFromMsaAction, &QAction::triggered, this, [this, msaEditor, toggleExcludeListAction]() {
        auto excludeListWidget = findActiveExcludeList(msaEditor);
        if (excludeListWidget == nullptr) {
            toggleExcludeListAction->trigger();
            excludeListWidget = findActiveExcludeList(msaEditor);
            CHECK(excludeListWidget != nullptr, );
        }
        excludeListWidget->moveMsaSelectionToExcludeList();
    });
    connect(msaEditor->getSelectionController(), &MaEditorSelectionController::si_selectionChanged, this, [this, msaEditor]() { updateState(msaEditor); });

    QPointer<MultipleSequenceAlignmentObject> msaObjectPtr = msaEditor->getMaObject();
    QPointer<MSAEditor> msaEditorPtr = msaEditor;
    connect(msaObjectPtr, &GObject::si_lockedStateChanged, this, [this, msaEditorPtr]() {
        CHECK(!msaEditorPtr.isNull(), );
        updateState(msaEditorPtr);
    });
    connect(msaEditor, &GObject::destroyed, this, [this, msaObjectPtr]() {
        // MSA object may be destroyed before MSA Editor when the object is deleted from the Project Tree View with an opened MSA editor.
        // This is a bug in UGENE: views must be closed before object is destroyed.
        CHECK(!msaObjectPtr.isNull(), );
        msaObjectPtr->disconnect(this);
    });
    connect(view, &GObjectView::si_buildMenu, this, [msaEditor, moveFromMsaAction](GObjectView *, QMenu *menu) {
        QMenu *copyMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_COPY);
            GUIUtils::insertActionAfter(copyMenu, msaEditor->getUI()->getUI(0)->cutSelectionAction, moveFromMsaAction);
    });
    addViewAction(moveFromMsaAction);

    updateState(msaEditor);
}

MsaExcludeListWidget *MsaExcludeListContext::findActiveExcludeList(MSAEditor *msaEditor) {
    auto mainSplitter = msaEditor->getUI()->getUI(0)->getMainSplitter();
    return mainSplitter->findChild<MsaExcludeListWidget *>();
}

MsaExcludeListWidget* MsaExcludeListContext::openExcludeList(MSAEditor* msaEditor) {
    MsaExcludeListWidget* excludeList = findActiveExcludeList(msaEditor);
    CHECK(excludeList == nullptr, excludeList);
    GCOUNTER(cvar, "MsaExcludeListWidget");

    auto mainSplitter = msaEditor->getUI()->getUI(0)->getMainSplitter();
    excludeList = new MsaExcludeListWidget(mainSplitter, msaEditor, this);
    mainSplitter->insertWidget(1, excludeList);
    mainSplitter->setCollapsible(1, false);
    updateMsaEditorSplitterStyle(msaEditor);
    return excludeList;
}

void MsaExcludeListContext::updateMsaEditorSplitterStyle(MSAEditor *msaEditor) {
    auto mainSplitter = msaEditor->getUI()->getUI(0)->getMainSplitter();
    MaSplitterUtils::updateFixedSizeHandleStyle(mainSplitter);
}

void MsaExcludeListContext::toggleExcludeListView(MSAEditor* msaEditor) {
    auto excludeList = findActiveExcludeList(msaEditor);
    if (excludeList != nullptr) {
        delete excludeList;
    } else {
        openExcludeList(msaEditor);
    }
    updateMsaEditorSplitterStyle(msaEditor);
}

void MsaExcludeListContext::updateState(MSAEditor* msaEditor) {
    bool isRegisteredView = viewResources.contains(msaEditor);
    CHECK(isRegisteredView, );
    // MSA editor emits signals during the destruction process (TreeWidget resets rows ordering and updates collapse model & selection).
    // We need to check if the view is still registered first.
    auto moveAction = getMoveMsaSelectionToExcludeListAction(msaEditor);
    SAFE_POINT(moveAction != nullptr, "Can't find move action in Msa editor", )
    bool isEnabled = !msaEditor->getMaObject()->isStateLocked() && !msaEditor->getSelection().isEmpty();
    moveAction->setEnabled(isEnabled);
}

QAction* MsaExcludeListContext::getMoveMsaSelectionToExcludeListAction(MSAEditor* msaEditor) {
    auto moveAction = findViewAction(msaEditor, MOVE_MSA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME);
    SAFE_POINT(moveAction != nullptr, "Can't find move action in Msa editor", nullptr)
    return moveAction;
}

////////////////////////////////////
/// MsaExcludeList
///////////////////////////////////

/** Data used by the Exclude List is stored directly in the name list items. */
static constexpr int LIST_ITEM_DATA_ROW_ID = 1000;

/** Property key for the last used exclude list file name. Stored in MsaEditor memory. */
static const char* PROPERTY_LAST_USED_EXCLUDE_LIST_FILE = "MsaExcludeList_lastUsedFile";

/** A constant for Exclude List file. An Exclude List file name for "file.aln" is constructed like  "file." + EXCLUDE_LIST_FILE_SUFFIX. */
static const char* EXCLUDE_LIST_FILE_SUFFIX = "exclude-list.fasta";

MsaExcludeListWidget::MsaExcludeListWidget(QWidget* parent, MSAEditor* _msaEditor, MsaExcludeListContext* viewContext)
    : QWidget(parent), msaEditor(_msaEditor) {
    setObjectName("msa_exclude_list");
    auto layout = new QVBoxLayout(this);
    setLayout(layout);

    auto toolbarLayout = new QHBoxLayout(this);
    layout->addLayout(toolbarLayout);

    toolbarLayout->addWidget(new QLabel("Exclude List file: ", this));
    selectFileButton = new QToolButton(this);
    selectFileButton->setObjectName("exclude_list_select_file_button");
    connect(selectFileButton, &QToolButton::clicked, this, &MsaExcludeListWidget::changeExcludeListFile);
    toolbarLayout->addStrut(10);
    toolbarLayout->addWidget(selectFileButton);

    saveAsButton = new QToolButton(this);
    saveAsButton->setText("Save as ...");
    saveAsButton->setObjectName("exclude_list_save_as_button");
    connect(saveAsButton, &QToolButton::clicked, this, &MsaExcludeListWidget::saveExcludeFileToNewLocation);
    toolbarLayout->addStrut(10);
    toolbarLayout->addWidget(saveAsButton);

    stateLabel = new QLabel(tr("Exclude list file is not loaded"));
    stateLabel->setObjectName("exclude_list_state_label");
    stateLabel->setContentsMargins(0, 20, 0, 20);
    layout->addWidget(stateLabel);

    namesAndSequenceSplitter = new QSplitter(Qt::Horizontal, this);
    namesAndSequenceSplitter->setVisible(false);
    layout->addWidget(namesAndSequenceSplitter);

    nameListView = new QListWidget();
    nameListView->setObjectName("exclude_list_name_list_widget");
    nameListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    nameListView->setAcceptDrops(true);
    nameListView->setDefaultDropAction(Qt::MoveAction);
    nameListView->setDragEnabled(true);
    nameListView->setDropIndicatorShown(true);
    nameListView->setDragDropMode(QAbstractItemView::InternalMove);
    connect(nameListView, &QListWidget::itemSelectionChanged, this, &MsaExcludeListWidget::updateState);
    connect(nameListView, &QWidget::customContextMenuRequested, this, &MsaExcludeListWidget::showNameListContextMenu);

    sequenceView = new QPlainTextEdit(this);
    sequenceView->setObjectName("exclude_list_sequence_view");
    sequenceView->setReadOnly(true);
    sequenceView->setTextInteractionFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);

    namesAndSequenceSplitter->addWidget(nameListView);
    namesAndSequenceSplitter->setStretchFactor(0, 1);
    namesAndSequenceSplitter->setCollapsible(0, false);
    namesAndSequenceSplitter->addWidget(sequenceView);
    namesAndSequenceSplitter->setStretchFactor(1, 3);
    namesAndSequenceSplitter->setCollapsible(1, false);

    this->setContextMenuPolicy(Qt::PreventContextMenu);
    nameListView->setContextMenuPolicy(Qt::CustomContextMenu);

    moveToMsaAction = new QAction(tr("Move to alignment"), this);
    moveToMsaAction->setToolTip(tr("Move selected Exclude List sequences to MSA"));
    moveToMsaAction->setIcon(QIcon(":core/images/arrow-move-up.png"));
    connect(moveToMsaAction, &QAction::triggered, this, &MsaExcludeListWidget::moveExcludeListSelectionToMaObject);

    auto moveToMsaButton = new QToolButton();
    moveToMsaButton->setObjectName("exclude_list_move_to_msa_button");
    moveToMsaButton->setDefaultAction(moveToMsaAction);
    moveToMsaButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbarLayout->addWidget(moveToMsaButton);
    toolbarLayout->addStrut(10);

    auto moveFromMsaAction = viewContext->getMoveMsaSelectionToExcludeListAction(msaEditor);
    if (moveFromMsaAction != nullptr) {
        auto moveFromMsaButton = new QToolButton();
        moveFromMsaButton->setObjectName("exclude_list_move_from_msa_button");
        moveFromMsaButton->setDefaultAction(moveFromMsaAction);
        moveFromMsaButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        toolbarLayout->addWidget(moveFromMsaButton);
        toolbarLayout->addStrut(10);
    }

    auto msaObject = msaEditor->getMaObject();
    connect(msaObject, &MultipleSequenceAlignmentObject::si_alignmentChanged, this, &MsaExcludeListWidget::handleUndoRedoInMsaEditor);
    connect(msaObject, &MultipleSequenceAlignmentObject::si_lockedStateChanged, this, &MsaExcludeListWidget::updateState);

    excludeListFilePath = msaEditor->property(PROPERTY_LAST_USED_EXCLUDE_LIST_FILE).toString();
    if (excludeListFilePath.isEmpty() || !QFileInfo::exists(excludeListFilePath)) {
        GUrl msaUrl = msaObject->getDocument()->getURL();
        excludeListFilePath = msaUrl.dirPath() + "/" + msaUrl.baseFileName() + "." + EXCLUDE_LIST_FILE_SUFFIX;
        if (GUrl(excludeListFilePath) == msaUrl) {
            excludeListFilePath = msaUrl.getURLString() + "." + EXCLUDE_LIST_FILE_SUFFIX;
        }
    }

    connect(AppContext::getTaskScheduler(), &TaskScheduler::si_stateChanged, this, &MsaExcludeListWidget::trackMsaObjectSaveTask);

    toolbarLayout->addStretch();

    loadExcludeList(true);
}

MsaExcludeListWidget::~MsaExcludeListWidget() {
    if (isDirty) {
        runSaveTask(excludeListFilePath);
    }
}

int MsaExcludeListWidget::addEntry(const DNASequence& sequence, int excludeListRowId) {
    SAFE_POINT(sequence.alphabet != nullptr, "Sequence must be fully defined!", 0);  // By default MSA row sequences have no alphabet. Catch this kind of error ASAP.
    int computedExcludeListRowId = excludeListRowId <= 0 ? ++excludeListRowIdGenerator : excludeListRowId;
    auto item = new QListWidgetItem();
    item->setText(sequence.getName());
    item->setData(LIST_ITEM_DATA_ROW_ID, computedExcludeListRowId);
    nameListView->addItem(item);
    sequenceByExcludeListRowId[computedExcludeListRowId] = sequence;
    isDirty = true;
    return computedExcludeListRowId;
}

int MsaExcludeListWidget::addMsaRowEntry(const MultipleAlignmentRow& row, int excludeListRowId) {
    DNASequence sequence = row->getUngappedSequence();
    sequence.alphabet = msaEditor->getMaObject()->getAlphabet();  // TODO: MSA row must return a fully defined sequence with a correct alphabet.
    return addEntry(sequence, excludeListRowId);
}

void MsaExcludeListWidget::removeEntries(const QList<QListWidgetItem*>& items) {
    CHECK(!items.isEmpty(), );
    bool hasSelectionBefore = !nameListView->selectedItems().isEmpty();
    int firstRemovedIndex = -1;
    QHash<QListWidgetItem*, int> indexByItem;
    for (int i = 0; i < nameListView->count(); i++) {
        indexByItem[nameListView->item(i)] = i;
    }
    for (auto item : qAsConst(items)) {
        sequenceByExcludeListRowId.remove(item->data(LIST_ITEM_DATA_ROW_ID).toInt());
        int itemIndex = indexByItem.value(item, INT_MAX);
        firstRemovedIndex = firstRemovedIndex == -1 ? itemIndex : qMin(itemIndex, firstRemovedIndex);
    }
    qDeleteAll(items);
    bool hasSelectionAfter = !nameListView->selectedItems().isEmpty();
    if (hasSelectionBefore && !hasSelectionAfter && firstRemovedIndex >= 0 && nameListView->count() > 0) {
        int newSelectedItemIndex = qMin(firstRemovedIndex, nameListView->count() - 1);
        nameListView->item(newSelectedItemIndex)->setSelected(true);
    }
    isDirty = true;
}

void MsaExcludeListWidget::updateState() {
    selectFileButton->setText(isLoaded ? GUrl(excludeListFilePath).fileName() : tr("<empty>"));
    selectFileButton->setToolTip(isLoaded ? excludeListFilePath : tr("<empty>"));
    saveAsButton->setEnabled(!hasActiveTask() && isLoaded);
    moveToMsaAction->setEnabled(!hasActiveTask() && isLoaded && !nameListView->selectedItems().isEmpty() && !msaEditor->getMaObject()->isStateLocked());
    namesAndSequenceSplitter->setVisible(isLoaded);

    if (isLoaded) {
        stateLabel->setText(nameListView->count() == 0 ? tr("Exclude list is empty. Try moving selected sequences to the list using the 'Arrow Down' button above")
                                                       : "");
    }
    stateLabel->setVisible(!stateLabel->text().isEmpty());
    updateSequenceView();
}

bool MsaExcludeListWidget::hasActiveTask() const {
    return loadTask != nullptr || saveTask != nullptr;
}

int MsaExcludeListWidget::getExcludeListRowId(const QListWidgetItem* item) const {
    SAFE_POINT(item != nullptr, "Exclude list item is null!", 0);
    int id = item->data(LIST_ITEM_DATA_ROW_ID).toInt();
    SAFE_POINT(id > 0, "Invalid exclude list row id: " + QString::number(id), id);
    return id;
}

DNASequence MsaExcludeListWidget::getExcludeListRowSequence(const QListWidgetItem* item) const {
    int id = getExcludeListRowId(item);
    SAFE_POINT(sequenceByExcludeListRowId.contains(id), "Sequence not found: exclude list row id: " + QString::number(id), {});
    return sequenceByExcludeListRowId[id];
}

void MsaExcludeListWidget::updateSequenceView() {
    QList<QListWidgetItem*> selectedItems = nameListView->selectedItems();
    sequenceView->setEnabled(selectedItems.count() == 1);
    if (selectedItems.isEmpty()) {
        sequenceView->clear();
        return;
    } else if (selectedItems.length() > 1) {
        sequenceView->setPlainText(tr("%1 sequences selected").arg(selectedItems.length()));
        return;
    }
    int excludeListRowId = getExcludeListRowId(selectedItems.first());
    DNASequence sequence = sequenceByExcludeListRowId[excludeListRowId];
    sequenceView->setPlainText(QString::fromUtf8(sequence.seq));
}

void MsaExcludeListWidget::showNameListContextMenu() {
    QMenu menu;
    menu.addAction(moveToMsaAction);
    menu.exec(QCursor::pos());
}

void MsaExcludeListWidget::moveMsaSelectionToExcludeList() {
    const MaEditorSelection& selection = msaEditor->getSelection();
    SAFE_POINT(!selection.isEmpty(), "Msa editor selection is empty!", );
    QList<QRect> selectedRects = selection.getRectList();
    QList<int> selectedMsaRowIndexes = msaEditor->getCollapseModel()->getMaRowIndexesFromSelectionRects(selectedRects, true);
    if (loadTask == nullptr) {
        GCOUNTER(cvar, "MsaExcludeListWidget::moveFromMsa");
        moveMsaRowIndexesToExcludeList(selectedMsaRowIndexes);
        return;
    }
    // Make the move later, after the task is finished.
    // This is possible when user moves & opens Exclude List from the single action in MSA Editor: the widget is opened
    // and starts loading at the same time.
    QList<qint64> selectedMsaRowIds = msaEditor->getMaObject()->getRowIdsByRowIndexes(selectedMsaRowIndexes);
    for (qint64 msaRowId : qAsConst(selectedMsaRowIds)) {
        if (!pendingMoveFromMsaRowIds.contains(msaRowId)) {
            pendingMoveFromMsaRowIds << msaRowId;
        }
    }
}

void MsaExcludeListWidget::moveMsaRowIndexesToExcludeList(const QList<int>& msaRowIndexes) {
    CHECK(!msaRowIndexes.isEmpty(), )
    SAFE_POINT(loadTask == nullptr, "Can't add rows with an active load task!", )

    QList<int> excludeListRowIds;
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    if (msaObject->getRowCount() == msaRowIndexes.count()) {
        // TODO: support empty MSA for all file formats.
        QMessageBox::critical(this, L10N::warningTitle(), tr("Multiple alignment must keep at least one row"));
        return;
    }
    for (int msaRowIndex : qAsConst(msaRowIndexes)) {
        excludeListRowIds << addMsaRowEntry(msaObject->getRow(msaRowIndex));
    }

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(msaObject->getEntityRef(), os);
    if (!os.hasError()) {
        auto collapseModel = msaEditor->getCollapseModel();
        int firstSelectedMsaRowBefore = msaEditor->getSelection().getFirstSelectedRowIndex();
        int versionBefore = msaObject->getObjectVersion();
        msaObject->removeRows(msaRowIndexes);
        // Exclude list re-uses msa row ids.
        trackedUndoMsaVersions.insert(versionBefore, {true, excludeListRowIds});
        trackedRedoMsaVersions.insert(msaObject->getObjectVersion(), {true, excludeListRowIds});

        // Select the first row in the position of the first removed one.
        if (!msaEditor->isAlignmentEmpty() && msaEditor->getSelection().isEmpty() && firstSelectedMsaRowBefore >= 0) {
            int newSelectedViewRowIndex = qMin(collapseModel->getViewRowCount() - 1, firstSelectedMsaRowBefore);
            msaEditor->selectRows(newSelectedViewRowIndex, 1);
        }
    }
    updateState();
}

void MsaExcludeListWidget::moveExcludeListSelectionToMaObject() {
    GCOUNTER(cvar, "MsaExcludeListWidget::moveToMsa");
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    QList<DNASequence> sequences;
    QList<int> excludeListRowIdsMovedToMsa;
    QList<QListWidgetItem*> selectedItems = nameListView->selectedItems();
    for (const QListWidgetItem* listItem : qAsConst(selectedItems)) {
        sequences << getExcludeListRowSequence(listItem);
        excludeListRowIdsMovedToMsa << getExcludeListRowId(listItem);
    }
    removeEntries(selectedItems);

    int versionBefore = msaObject->getObjectVersion();
    int insertionIndex = -1;  // Append by default.
    if (!msaEditor->getSelection().isEmpty() && msaEditor->getRowOrderMode() == MaEditorRowOrderMode::Original) {
        insertionIndex = msaEditor->getSelection().getRectList().last().bottom() + 1;
    }
    U2OpStatus2Log os;
    AddSequenceObjectsToAlignmentUtils::addObjectsToAlignment(os, msaObject, sequences, insertionIndex, true);
    if (!os.hasError()) {
        trackedUndoMsaVersions.insert(versionBefore, {false, excludeListRowIdsMovedToMsa});
        trackedRedoMsaVersions.insert(msaObject->getObjectVersion(), {false, excludeListRowIdsMovedToMsa});
    }
    updateState();
}

void MsaExcludeListWidget::handleUndoRedoInMsaEditor(const MultipleAlignment& maBefore, const MaModificationInfo& modInfo) {
    auto msaObject = msaEditor->getMaObject();
    int version = msaObject->getObjectVersion();
    if (modInfo.type != MaModificationType_Undo && modInfo.type != MaModificationType_Redo) {
        auto truncateVersionData = [version](QMap<int, UndoRedoStep>& undoRedoMap) {
            CHECK(!undoRedoMap.isEmpty() && version <= undoRedoMap.lastKey(), );
            QList<int> allUndoRedoVersions = undoRedoMap.keys();
            for (int undoRedoVersion : qAsConst(allUndoRedoVersions)) {
                if (undoRedoVersion >= version) {
                    undoRedoMap.remove(undoRedoVersion);
                }
            }
        };
        truncateVersionData(trackedUndoMsaVersions);
        truncateVersionData(trackedRedoMsaVersions);
        return;
    }
    bool isRedo = modInfo.type == MaModificationType_Redo;
    CHECK((isRedo && trackedRedoMsaVersions.contains(version)) || (!isRedo && trackedUndoMsaVersions.contains(version)), );

    const UndoRedoStep& undoRedoContext = isRedo ? trackedRedoMsaVersions.value(version) : trackedUndoMsaVersions.value(version);
    bool isAddToExcludeList = (isRedo && undoRedoContext.isMoveFromMsaToExcludeList) || (!isRedo && !undoRedoContext.isMoveFromMsaToExcludeList);
    if (isAddToExcludeList) {  // Add rows removed from MSA to Exclude list
        QList<MultipleAlignmentRow> msaRows;
        QSet<qint64> msaRowIdsAfter = msaObject->getRowIds().toSet();
        for (int i = 0; i < maBefore->getRowCount(); i++) {
            const MultipleAlignmentRow& row = maBefore->getRow(i);
            if (!msaRowIdsAfter.contains(row->getRowId())) {
                msaRows << row;
            }
        }
        SAFE_POINT(msaRows.size() == undoRedoContext.excludeListRowIdsDelta.size(), "Failed to map Exclude List rows to Msa rows", )
        for (int i = 0; i < msaRows.size(); i++) {
            const auto& msaRow = msaRows[i];
            int excludeListRowId = undoRedoContext.excludeListRowIdsDelta[i];
            addMsaRowEntry(msaRow, excludeListRowId);
        }
    } else {  // Remove rows from Exclude List.
        QSet<int> rowIdsToRemove = undoRedoContext.excludeListRowIdsDelta.toSet();
        QList<QListWidgetItem*> listItemsToRemove;
        for (int rowIndex = nameListView->count(); --rowIndex >= 0;) {
            auto listItem = nameListView->item(rowIndex);
            int rowId = listItem->data(LIST_ITEM_DATA_ROW_ID).toInt();
            if (rowIdsToRemove.contains(rowId)) {
                listItemsToRemove << listItem;
            }
        }
        removeEntries(listItemsToRemove);
    }
    updateState();
}

void MsaExcludeListWidget::unloadExcludeList() {
    if (isDirty) {
        SAFE_POINT(saveTask == nullptr, "Found active save task!", );
        if (QMessageBox::question(this, tr("Question"), tr("Save current exclude list first?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if (auto task = runSaveTask(excludeListFilePath)) {
                connect(new TaskSignalMapper(task), &TaskSignalMapper::si_taskSucceeded, this, [this] { unloadExcludeList(); });
            }
        }
    }
    isDirty = false;
    isLoaded = false;
    nameListView->clear();
    stateLabel->clear();
    updateState();
}

void MsaExcludeListWidget::loadExcludeList(bool create) {
    unloadExcludeList();
    SAFE_POINT(!hasActiveTask(), "Can't load a new exclude list file when there is an active load/save task. ", );
    bool exists = QFileInfo::exists(excludeListFilePath);
    if (!exists && create) {
        if (FileAndDirectoryUtils::canWriteToPath(excludeListFilePath)) {
            isLoaded = true;
        } else {
            stateLabel->setText(tr("Failed to write into exclude list file: %1").arg(excludeListFilePath));
        }
    } else if (exists) {
        CHECK(loadTask == nullptr, );
        IOAdapterFactory* ioFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(excludeListFilePath));
        loadTask = new LoadDocumentTask(BaseDocumentFormats::FASTA, excludeListFilePath, ioFactory, {{DocumentReadingMode_AllowEmptyFile, true}});
        stateLabel->setText(tr("Loading exclude list file: %1").arg(excludeListFilePath));
        connect(loadTask, &Task::si_stateChanged, this, &MsaExcludeListWidget::handleLoadTaskStateChange);
        AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
    } else {
        stateLabel->setText(tr("File is not found: %1").arg(excludeListFilePath));
    }
    updateState();
}

void MsaExcludeListWidget::changeExcludeListFile() {
    QString dir = GUrl(excludeListFilePath).dirPath();
    QString newFilePath = U2FileDialog::getOpenFileName(this,
                                                        tr("Select exclude list file"),
                                                        dir,
                                                        FileFilters::createFileFilter(tr("Exclude list FASTA file"), {EXCLUDE_LIST_FILE_SUFFIX}));
    CHECK(!newFilePath.isEmpty() && newFilePath != excludeListFilePath, );
    if (!FileAndDirectoryUtils::canWriteToPath(newFilePath)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File is not writable: %1").arg(newFilePath));
        return;
    }
    if (GUrl(newFilePath) == msaEditor->getMaObject()->getDocument()->getURL()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Currently opened object file can't be set as exclude list : %1").arg(newFilePath));
        return;
    }
    excludeListFilePath = newFilePath;
    loadExcludeList();
}

Task* MsaExcludeListWidget::runSaveTask(const QString& savePath) {
    SAFE_POINT(!hasActiveTask(), "Can't save exclude list file when there is an active load/save task. ", nullptr);
    IOAdapterFactory* ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA);

    U2OpStatus2Log os;
    Document* document = format->createNewLoadedDocument(ioAdapterFactory, savePath, os);
    for (int i = 0; i < nameListView->count(); i++) {
        QListWidgetItem* listItem = nameListView->item(i);
        int excludeListRowId = listItem->data(LIST_ITEM_DATA_ROW_ID).toInt();
        DNASequence sequence = sequenceByExcludeListRowId[excludeListRowId];
        U2EntityRef ref = U2SequenceUtils::import(os, document->getDbiRef(), U2ObjectDbi::ROOT_FOLDER, sequence);
        CHECK_OP(os, nullptr);
        document->addObject(new U2SequenceObject(sequence.getName(), ref));
    }

    SAFE_POINT_OP(os, nullptr);
    saveTask = new SaveDocumentTask(document);
    connect(new TaskSignalMapper(saveTask), &TaskSignalMapper::si_taskFinished, this, [this] { saveTask = nullptr; });
    AppContext::getTaskScheduler()->registerTopLevelTask(saveTask);
    updateState();
    return saveTask;
}

void MsaExcludeListWidget::saveExcludeFileToNewLocation() {
    QString dir = GUrl(excludeListFilePath).dirPath();
    QString newFilePath = U2FileDialog::getSaveFileName(this,
                                                        tr("Select new exclude list file name"),
                                                        dir,
                                                        FileFilters::createFileFilter(tr("Exclude list FASTA file"), {EXCLUDE_LIST_FILE_SUFFIX}));
    CHECK(!newFilePath.isEmpty() && newFilePath != excludeListFilePath, );
    if (!newFilePath.endsWith(EXCLUDE_LIST_FILE_SUFFIX)) {
        newFilePath = newFilePath + "." + EXCLUDE_LIST_FILE_SUFFIX;
    }
    if (!FileAndDirectoryUtils::canWriteToPath(newFilePath)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File is not writable: %1").arg(newFilePath));
        return;
    };
    if (auto task = runSaveTask(newFilePath)) {
        connect(new TaskSignalMapper(task), &TaskSignalMapper::si_taskSucceeded, this, [this, newFilePath] {
            excludeListFilePath = newFilePath;
            isDirty = false;
        });
    }
}

void MsaExcludeListWidget::handleLoadTaskStateChange() {
    SAFE_POINT(loadTask != nullptr, "Load task is nullptr in handleLoadTaskStateChange", );
    CHECK(loadTask->isFinished(), );

    auto task = loadTask;
    loadTask = nullptr;

    if (task->hasError()) {
        stateLabel->setText(tr("Error loading exclude list file: ").arg(task->getError()));
    } else if (!task->isCanceled()) {
        QList<GObject*> objects = task->getDocument()->findGObjectByType(GObjectTypes::SEQUENCE);
        nameListView->clear();
        U2OpStatus2Log os;
        for (auto object : qAsConst(objects)) {
            auto sequenceObject = qobject_cast<U2SequenceObject*>(object);
            SAFE_POINT(sequenceObject != nullptr, "Not a sequence object: " + object->getGObjectName(), );
            DNASequence sequence = sequenceObject->getWholeSequence(os);
            SAFE_POINT_OP(os, );
            addEntry(sequence);
        }
        if (!pendingMoveFromMsaRowIds.isEmpty()) {
            QList<int> pendingMsaRowIndexes;
            QList<qint64> allMsaRowIds = msaEditor->getMaRowIds();
            for (qint64 msaRowId : qAsConst(pendingMoveFromMsaRowIds)) {
                int msaRowIndex = allMsaRowIds.indexOf(msaRowId);
                if (msaRowIndex >= 0) {
                    pendingMsaRowIndexes << msaRowIndex;
                }
            }
            moveMsaRowIndexesToExcludeList(pendingMsaRowIndexes);
        }
        isLoaded = true;
        isDirty = false;
    }
    updateState();
}

void MsaExcludeListWidget::trackMsaObjectSaveTask(Task* task) {
    auto saveMsaObjectTask = qobject_cast<SaveDocumentTask*>(task);
    CHECK(saveMsaObjectTask != nullptr && saveMsaObjectTask->getDocument() == msaEditor->getMaObject()->getDocument(), );
    CHECK(saveMsaObjectTask->isFinished() && !saveMsaObjectTask->getStateInfo().isCoR(), );
    runSaveTask(excludeListFilePath);
}

QSize MsaExcludeListWidget::sizeHint() const {
    return {500, 200};
}

}  // namespace U2
