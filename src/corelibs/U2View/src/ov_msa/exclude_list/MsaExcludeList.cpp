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
#include <QVBoxLayout>

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
MsaExcludeListContext::MsaExcludeListContext(QObject *parent)
    : GObjectViewWindowContext(parent, MsaEditorFactory::ID) {
}

static const char *TOGGLE_EXCLUDE_LIST_ACTION_NAME = "exclude_list_toggle_action";
static const char *MOVE_MSA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME = "exclude_list_move_from_msa_action";

void MsaExcludeListContext::initViewContext(GObjectView *view) {
    auto msaEditor = qobject_cast<MSAEditor *>(view);
    SAFE_POINT(msaEditor != nullptr, "View is not MSAEditor!", );
    CHECK(msaEditor->getMaObject() != nullptr, );
    msaEditor->registerActionProvider(this);

    // Toggles exclude list view in MSA editor. See MsaExcludeList for details.
    auto toggleExcludeListAction = new GObjectViewAction(this, view, tr("Show Exclude List"));
    toggleExcludeListAction->setIcon(QIcon(":core/images/inbox-minus.png"));
    toggleExcludeListAction->setCheckable(true);
    toggleExcludeListAction->setObjectName(TOGGLE_EXCLUDE_LIST_ACTION_NAME);
    toggleExcludeListAction->setToolTip(tr("Show/Hide Exclude List view visibility"));
    connect(toggleExcludeListAction, &QAction::triggered, this, [this, msaEditor] { toggleExcludeListView(msaEditor); });
    connect(view, &GObjectView::si_buildStaticToolbar, this, [toggleExcludeListAction](GObjectView *, QToolBar *toolBar) { toolBar->addAction(toggleExcludeListAction); });
    addViewAction(toggleExcludeListAction);

    auto moveMsaSelectionToExcludeListAction = new GObjectViewAction(this, view, tr("Move to Exclude List"));
    moveMsaSelectionToExcludeListAction->setIcon(QIcon(":core/images/arrow-move-down.png"));
    moveMsaSelectionToExcludeListAction->setObjectName(MOVE_MSA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME);
    moveMsaSelectionToExcludeListAction->setToolTip(tr("Move selected MSA sequences to Exclude List"));
    connect(moveMsaSelectionToExcludeListAction, &QAction::triggered, this, [this, msaEditor]() {
        MsaExcludeList *excludeList = openExcludeList(msaEditor);
        excludeList->moveMsaSelectionToExcludeList();
    });
    connect(msaEditor->getSelectionController(), &MaEditorSelectionController::si_selectionChanged, this, [this, msaEditor]() { updateState(msaEditor); });
    connect(msaEditor->getMaObject(), &GObject::si_lockedStateChanged, this, [this, msaEditor]() { updateState(msaEditor); });
    connect(view, &GObjectView::si_buildMenu, this, [msaEditor, moveMsaSelectionToExcludeListAction](GObjectView *, QMenu *menu) {
        QMenu *copyMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_COPY);
        GUIUtils::insertActionAfter(copyMenu, msaEditor->getUI()->cutSelectionAction, moveMsaSelectionToExcludeListAction);
    });
    addViewAction(moveMsaSelectionToExcludeListAction);

    updateState(msaEditor);
}

MsaExcludeList *MsaExcludeListContext::findActiveExcludeList(MSAEditor *msaEditor) {
    auto mainSplitter = msaEditor->getUI()->getMainSplitter();
    return mainSplitter->findChild<MsaExcludeList *>();
}

MsaExcludeList *MsaExcludeListContext::openExcludeList(MSAEditor *msaEditor) {
    MsaExcludeList *excludeList = findActiveExcludeList(msaEditor);
    CHECK(excludeList == nullptr, excludeList);

    auto mainSplitter = msaEditor->getUI()->getMainSplitter();
    excludeList = new MsaExcludeList(mainSplitter, msaEditor, this);
    mainSplitter->insertWidget(1, excludeList);
    mainSplitter->setCollapsible(1, false);
    updateMsaEditorSplitterStyle(msaEditor);
    return excludeList;
}

void MsaExcludeListContext::updateMsaEditorSplitterStyle(MSAEditor *msaEditor) {
    auto mainSplitter = msaEditor->getUI()->getMainSplitter();
    MaSplitterUtils::updateFixedSizeHandleStyle(mainSplitter);
}

void MsaExcludeListContext::toggleExcludeListView(MSAEditor *msaEditor) {
    auto excludeList = findActiveExcludeList(msaEditor);
    if (excludeList != nullptr) {
        delete excludeList;
    } else {
        openExcludeList(msaEditor);
    }
    updateMsaEditorSplitterStyle(msaEditor);
}

void MsaExcludeListContext::updateState(MSAEditor *msaEditor) {
    auto moveAction = getMoveMsaSelectionToExcludeListAction(msaEditor);
    SAFE_POINT(moveAction != nullptr, "Can't find move action in Msa editor", )
    bool isEnabled = !msaEditor->getMaObject()->isStateLocked() && !msaEditor->getSelection().isEmpty();
    moveAction->setEnabled(isEnabled);
}

QAction *MsaExcludeListContext::getMoveMsaSelectionToExcludeListAction(MSAEditor *msaEditor) {
    auto moveAction = findViewAction(msaEditor, MOVE_MSA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME);
    SAFE_POINT(moveAction != nullptr, "Can't find move action in Msa editor", nullptr)
    return moveAction;
}

////////////////////////////////////
/// MsaExcludeList
///////////////////////////////////

// TODO: document
static constexpr int LIST_ITEM_DATA_ROW_ID = 1000;
static constexpr int LIST_ITEM_DATA_SEQUENCE = 1001;
static const char *PROPERTY_LAST_USED_EXCLUDE_LIST_FILE = "MsaExcludeList_lastUsedFile";
static const char *EXCLUDE_LIST_FILE_SUFFIX = "exclude-list.fasta";

class SequenceNamesListWidget : public QListWidget {
public:
    SequenceNamesListWidget() {
        setObjectName("exclude_list_name_list_widget");
    }

protected:
    QMimeData *mimeData(const QList<QListWidgetItem *> items) const override {
        auto mimeData = QListWidget::mimeData(items);
        SAFE_POINT(items.length() == 1, "Invalid items count", nullptr);
        auto item = items[0];
        QByteArray sequence = item->data(LIST_ITEM_DATA_SEQUENCE).toByteArray();
        mimeData->setText(sequence);
        mimeData->setData(U2Clipboard::UGENE_MIME_TYPE, QString(">" + item->text() + "\n" + sequence).toUtf8());
        return mimeData;
    }
};

MsaExcludeList::MsaExcludeList(QWidget *parent, MSAEditor *_msaEditor, MsaExcludeListContext *viewContext)
    : QWidget(parent), msaEditor(_msaEditor) {
    setObjectName("msa_exclude_list");
    auto layout = new QVBoxLayout(this);
    setLayout(layout);

    auto toolbarLayout = new QHBoxLayout(this);
    layout->addLayout(toolbarLayout);

    toolbarLayout->addWidget(new QLabel("Exclude List file: ", this));
    selectFileButton = new QToolButton(this);
    selectFileButton->setObjectName("exclude_list_select_file_button");
    connect(selectFileButton, &QToolButton::clicked, this, &MsaExcludeList::changeExcludeListFile);
    toolbarLayout->addStrut(10);
    toolbarLayout->addWidget(selectFileButton);

    saveAsButton = new QToolButton(this);
    saveAsButton->setText("Save as ...");
    saveAsButton->setObjectName("exclude_list_save_as_button");
    connect(saveAsButton, &QToolButton::clicked, this, &MsaExcludeList::saveExcludeFileToNewLocation);
    toolbarLayout->addStrut(10);
    toolbarLayout->addWidget(saveAsButton);

    stateLabel = new QLabel(tr("Exclude list file is not loaded"));
    stateLabel->setObjectName("exclude_list_state_label");
    stateLabel->setContentsMargins(0, 20, 0, 20);
    layout->addWidget(stateLabel);

    namesAndSequenceSplitter = new QSplitter(Qt::Horizontal, this);
    namesAndSequenceSplitter->setVisible(false);
    layout->addWidget(namesAndSequenceSplitter);

    nameListView = new SequenceNamesListWidget();
    nameListView->setSelectionMode(QAbstractItemView::SingleSelection);
    nameListView->setAcceptDrops(true);
    nameListView->setDefaultDropAction(Qt::MoveAction);
    nameListView->setDragEnabled(true);
    nameListView->setDropIndicatorShown(true);
    nameListView->setDragDropMode(QAbstractItemView::InternalMove);
    connect(nameListView, &QListWidget::itemSelectionChanged, this, &MsaExcludeList::updateSequenceView);
    connect(nameListView, &QWidget::customContextMenuRequested, this, &MsaExcludeList::showNameListContextMenu);

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

    moveSelectionToMaObjectAction = new QAction(tr("Move to alignment"), this);
    moveSelectionToMaObjectAction->setToolTip(tr("Move selected Exclude List sequences to MSA"));
    moveSelectionToMaObjectAction->setIcon(QIcon(":core/images/arrow-move-up.png"));
    connect(moveSelectionToMaObjectAction, &QAction::triggered, this, &MsaExcludeList::moveExcludeListSelectionToMaObject);
    connect(nameListView, &QListWidget::itemSelectionChanged, this, &MsaExcludeList::updateState);

    auto moveSelectionToMsaButton = new QToolButton();
    moveSelectionToMsaButton->setObjectName("exclude_list_move_to_msa_button");
    moveSelectionToMsaButton->setDefaultAction(moveSelectionToMaObjectAction);
    moveSelectionToMsaButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbarLayout->addWidget(moveSelectionToMsaButton);
    toolbarLayout->addStrut(10);

    auto moveMsaSelectionToExcludeListAction = viewContext->getMoveMsaSelectionToExcludeListAction(msaEditor);
    if (moveMsaSelectionToExcludeListAction != nullptr) {
        auto moveSelectionToExcludeListButton = new QToolButton();
        moveSelectionToExcludeListButton->setObjectName("exclude_list_move_to_exclude_list_button");
        moveSelectionToExcludeListButton->setDefaultAction(moveMsaSelectionToExcludeListAction);
        moveSelectionToExcludeListButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        toolbarLayout->addWidget(moveSelectionToExcludeListButton);
        toolbarLayout->addStrut(10);
    }

    auto msaObject = msaEditor->getMaObject();
    connect(msaObject, &MultipleSequenceAlignmentObject::si_alignmentChanged, this, &MsaExcludeList::handleUndoRedoInMsaEditor);

    excludeListFilePath = msaEditor->property(PROPERTY_LAST_USED_EXCLUDE_LIST_FILE).toString();
    if (excludeListFilePath.isEmpty() || !QFileInfo::exists(excludeListFilePath)) {
        GUrl msaUrl = msaObject->getDocument()->getURL();
        excludeListFilePath = msaUrl.dirPath() + "/" + msaUrl.baseFileName() + "." + EXCLUDE_LIST_FILE_SUFFIX;
    }

    connect(AppContext::getTaskScheduler(), &TaskScheduler::si_stateChanged, this, &MsaExcludeList::trackMsaObjectSaveTask);

    toolbarLayout->addStretch();

    loadExcludeList(true);
}

int MsaExcludeList::addEntry(const QString &name, const QByteArray &sequence, int excludeListRowId) {
    qint64 listRowId = excludeListRowId < 0 ? ++excludeListRowIdGenerator : excludeListRowId;
    auto item = new QListWidgetItem();
    item->setText(name);
    item->setData(LIST_ITEM_DATA_ROW_ID, listRowId);
    item->setData(LIST_ITEM_DATA_SEQUENCE, sequence);
    nameListView->addItem(item);
    isDirty = true;
    return listRowId;
}

void MsaExcludeList::removeEntries(const QList<QListWidgetItem *> &items) {
    CHECK(!items.isEmpty(), );
    qDeleteAll(items);
    isDirty = true;
}

void MsaExcludeList::updateState() {
    selectFileButton->setText(isLoaded ? GUrl(excludeListFilePath).fileName() : tr("<empty>"));
    selectFileButton->setToolTip(isLoaded ? excludeListFilePath : tr("<empty>"));
    saveAsButton->setEnabled(!hasActiveTask() && isLoaded);
    moveSelectionToMaObjectAction->setEnabled(!hasActiveTask() && isLoaded && !nameListView->selectedItems().isEmpty());
    namesAndSequenceSplitter->setVisible(isLoaded);

    if (isLoaded) {
        stateLabel->setText(nameListView->count() == 0 ? tr("Exclude list is empty. Try moving selected sequences to the list using the 'arrow-down' button above")
                                                       : "");
    }
    stateLabel->setVisible(!stateLabel->text().isEmpty());
}

bool MsaExcludeList::hasActiveTask() const {
    return loadTask != nullptr || saveTask != nullptr;
}

void MsaExcludeList::updateSequenceView() {
    QList<QListWidgetItem *> selectedItems = nameListView->selectedItems();
    QByteArray sequence = selectedItems.isEmpty() ? "" : selectedItems.first()->data(LIST_ITEM_DATA_SEQUENCE).toByteArray();
    sequenceView->setPlainText(QString::fromUtf8(sequence));
}

void MsaExcludeList::showNameListContextMenu(const QPoint &) {
    QMenu menu;
    menu.addAction(moveSelectionToMaObjectAction);
    menu.exec(QCursor::pos());
}

void MsaExcludeList::moveMsaSelectionToExcludeList() {
    const MaEditorSelection &selection = msaEditor->getSelection();
    SAFE_POINT(!selection.isEmpty(), "Msa editor selection is empty!", );
    QList<QRect> selectedRects = selection.getRectList();
    QList<int> selectedMsaRowIndexes = msaEditor->getCollapseModel()->getMaRowIndexesFromSelectionRects(selectedRects);
    QList<int> excludeListRowIds;
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
    for (int msaRowIndex : qAsConst(selectedMsaRowIndexes)) {
        MultipleAlignmentRow row = msaObject->getRow(msaRowIndex);
        DNASequence rowSequence = row->getUngappedSequence();
        excludeListRowIds << addEntry(rowSequence.getName(), rowSequence.seq);
    }

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(msaObject->getEntityRef(), os);
    if (!os.hasError()) {
        int versionBefore = msaObject->getObjectVersion();
        msaObject->removeRows(selectedMsaRowIndexes);
        // Exclude list re-uses msa row ids.
        trackedUndoMsaVersions.insert(versionBefore, {true, excludeListRowIds});
        trackedRedoMsaVersions.insert(msaObject->getObjectVersion(), {true, excludeListRowIds});

        // Select the first row before the removed ones.
        if (!msaEditor->isAlignmentEmpty()) {
            int firstSelectedRowIndexBefore = selectedRects.first().top();
            int newSelectedRowIndex = qMin(msaEditor->getCollapseModel()->getViewRowCount() - 1, firstSelectedRowIndexBefore);
            msaEditor->selectRows(newSelectedRowIndex, 1);
        }
    }
    updateState();
}

static DNASequence createDnaSequenceFromListItem(const QListWidgetItem *listItem) {
    auto name = listItem->text();
    auto sequenceData = listItem->data(LIST_ITEM_DATA_SEQUENCE).toByteArray();
    auto alphabet = U2AlphabetUtils::findBestAlphabet(sequenceData);
    return DNASequence(name, sequenceData, alphabet);
}

void MsaExcludeList::moveExcludeListSelectionToMaObject() {
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
    QList<DNASequence> sequences;
    QList<int> excludeListRowIdsMovedToMsa;
    QList<QListWidgetItem *> selectedItems = nameListView->selectedItems();
    for (const QListWidgetItem *listItem : qAsConst(selectedItems)) {
        sequences << createDnaSequenceFromListItem(listItem);
        excludeListRowIdsMovedToMsa << listItem->data(LIST_ITEM_DATA_ROW_ID).toInt();
    }
    qDeleteAll(selectedItems);

    int versionBefore = msaObject->getObjectVersion();
    int insertionIndex = -1;  // Append by default.
    if (!msaEditor->getSelection().isEmpty() && msaEditor->getRowOrderMode() == MaEditorRowOrderMode::Original) {
        insertionIndex = msaEditor->getSelection().getRectList().last().bottom() + 1;
    }
    AddSequenceObjectsToAlignmentTask task(msaObject, sequences, insertionIndex, true);
    task.run();  // TODO: extract as an utility method.
    if (!task.hasError()) {
        trackedUndoMsaVersions.insert(versionBefore, {false, excludeListRowIdsMovedToMsa});
        trackedRedoMsaVersions.insert(msaObject->getObjectVersion(), {false, excludeListRowIdsMovedToMsa});
    }
    updateState();
}

void MsaExcludeList::handleUndoRedoInMsaEditor(const MultipleAlignment &maBefore, const MaModificationInfo &modInfo) {
    auto msaObject = msaEditor->getMaObject();
    int version = msaObject->getObjectVersion();
    if (modInfo.type != MaModificationType_Undo && modInfo.type != MaModificationType_Redo) {
        auto truncateVersionData = [version](QMap<int, UndoRedoContext> &undoRedoMap) {
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

    const UndoRedoContext &undoRedoContext = isRedo ? trackedRedoMsaVersions.value(version) : trackedUndoMsaVersions.value(version);
    bool isAddToExcludeList = (isRedo && undoRedoContext.isMoveFromMsaToExcludeList) || (!isRedo && !undoRedoContext.isMoveFromMsaToExcludeList);
    if (isAddToExcludeList) {  // Add rows removed from MSA to Exclude list
        QList<MultipleAlignmentRow> msaRows;
        QSet<qint64> msaRowIdsAfter = msaObject->getRowIds().toSet();
        for (int i = 0; i < maBefore->getNumRows(); i++) {
            const MultipleAlignmentRow &row = maBefore->getRow(i);
            if (!msaRowIdsAfter.contains(row->getRowId())) {
                msaRows << row;
            }
        }
        SAFE_POINT(msaRows.size() == undoRedoContext.excludeListRowIdsDelta.size(), "Failed to map Exclude List rows to Msa rows", )
        for (int i = 0; i < msaRows.size(); i++) {
            auto &msaRow = msaRows[i];
            int excludeListRowId = undoRedoContext.excludeListRowIdsDelta[i];
            addEntry(msaRow->getName(), msaRow->getUngappedSequence().seq, excludeListRowId);
        }
    } else {  // Remove rows from Exclude List.
        QSet<int> rowIdsToRemove = undoRedoContext.excludeListRowIdsDelta.toSet();
        QList<QListWidgetItem *> listItemsToRemove;
        for (int rowIndex = nameListView->count(); --rowIndex >= 0;) {
            auto listItem = nameListView->item(rowIndex);
            int rowId = listItem->data(LIST_ITEM_DATA_ROW_ID).toInt();
            if (rowIdsToRemove.contains(rowId)) {
                listItemsToRemove << listItem;
            }
        }
        removeEntries(listItemsToRemove);
    }
}

void MsaExcludeList::unloadExcludeList() {
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
    sequenceView->clear();
    stateLabel->clear();
    updateState();
}

void MsaExcludeList::loadExcludeList(bool create) {
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
        SAFE_POINT(loadTask == nullptr, "Duplicate load task!", );
        IOAdapterFactory *ioFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(excludeListFilePath));
        loadTask = new LoadDocumentTask(BaseDocumentFormats::FASTA, excludeListFilePath, ioFactory, {{DocumentReadingMode_AllowEmptyFile, true}});
        // TODO: lock? unique task?
        stateLabel->setText(tr("Loading exclude list file: %1").arg(excludeListFilePath));
        connect(loadTask, &Task::si_stateChanged, this, &MsaExcludeList::handleLoadTaskStateChange);
        AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
    } else {
        stateLabel->setText(tr("File is not found: %1").arg(excludeListFilePath));
    }
    updateState();
}

void MsaExcludeList::changeExcludeListFile() {
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
    excludeListFilePath = newFilePath;
    loadExcludeList();
}

Task *MsaExcludeList::runSaveTask(const QString &savePath) {
    SAFE_POINT(!hasActiveTask(), "Can't save exclude list file when there is an active load/save task. ", nullptr);
    IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA);

    U2OpStatus2Log os;
    Document *document = format->createNewLoadedDocument(ioAdapterFactory, savePath, os);
    for (int i = 0; i < nameListView->count(); i++) {
        QListWidgetItem *listItem = nameListView->item(i);
        DNASequence sequence = createDnaSequenceFromListItem(listItem);
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

void MsaExcludeList::saveExcludeFileToNewLocation() {
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

void MsaExcludeList::handleLoadTaskStateChange() {
    SAFE_POINT(loadTask != nullptr, "Load task is nullptr in handleLoadTaskStateChange", );
    CHECK(loadTask->isFinished(), );

    auto task = loadTask;
    loadTask = nullptr;

    if (task->hasError()) {
        stateLabel->setText(tr("Error loading exclude list file: ").arg(task->getError()));
    } else if (!task->isCanceled()) {
        QList<GObject *> objects = task->getDocument()->findGObjectByType(GObjectTypes::SEQUENCE);
        nameListView->clear();
        U2OpStatus2Log os;
        for (auto object : qAsConst(objects)) {
            auto sequenceObject = qobject_cast<U2SequenceObject *>(object);
            SAFE_POINT(sequenceObject != nullptr, "Not a sequence object: " + object->getGObjectName(), );
            QByteArray sequenceData = sequenceObject->getWholeSequenceData(os);
            SAFE_POINT_OP(os, );
            addEntry(sequenceObject->getSequenceName(), sequenceData);
        }
        isLoaded = true;
        isDirty = false;
    }
    updateState();
}

void MsaExcludeList::trackMsaObjectSaveTask(Task *task) {
    auto saveMsaObjectTask = qobject_cast<SaveDocumentTask *>(task);
    CHECK(saveMsaObjectTask != nullptr && saveMsaObjectTask->getDocument() == msaEditor->getMaObject()->getDocument(), );
    CHECK(saveMsaObjectTask->isFinished() && !saveMsaObjectTask->getStateInfo().isCoR(), );
    runSaveTask(excludeListFilePath);
}

QSize MsaExcludeList::sizeHint() const {
    return {500, 200};
}

}  // namespace U2
