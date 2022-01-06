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
#include <QPlainTextEdit>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>

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

static const char *TOGGLE_EXCLUDE_LIST_ACTION_NAME = "toggle_exclude_list_action";
static const char *MOVE_MA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME = "move_ma_selection_to_exclude_list_action";

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
    connect(toggleExcludeListAction, &QAction::triggered, this, [this, msaEditor]() { toggleExcludeListView(msaEditor); });
    connect(view, &GObjectView::si_buildStaticToolbar, this, [toggleExcludeListAction](GObjectView *, QToolBar *toolBar) { toolBar->addAction(toggleExcludeListAction); });
    addViewAction(toggleExcludeListAction);

    auto moveMaSelectionToExcludeListAction = new GObjectViewAction(this, view, tr("Move to Exclude List"));
    moveMaSelectionToExcludeListAction->setObjectName(MOVE_MA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME);
    moveMaSelectionToExcludeListAction->setToolTip(tr("Move selected sequences to Exclude List"));
    connect(moveMaSelectionToExcludeListAction, &QAction::triggered, this, [this, msaEditor]() {
        MsaExcludeList *excludeList = openExcludeList(msaEditor);
        excludeList->moveMsaSelectionToExcludeList();
    });
    connect(msaEditor->getSelectionController(), &MaEditorSelectionController::si_selectionChanged, this, [this, msaEditor]() { updateState(msaEditor); });
    connect(msaEditor->getMaObject(), &GObject::si_lockedStateChanged, this, [this, msaEditor]() { updateState(msaEditor); });
    connect(view, &GObjectView::si_buildMenu, this, [msaEditor, moveMaSelectionToExcludeListAction](GObjectView *, QMenu *menu) {
        QMenu *copyMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_COPY);
        GUIUtils::insertActionAfter(copyMenu, msaEditor->getUI()->cutSelectionAction, moveMaSelectionToExcludeListAction);
    });
    addViewAction(moveMaSelectionToExcludeListAction);

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
    excludeList = new MsaExcludeList(mainSplitter, msaEditor);
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
    auto moveAction = findViewAction(msaEditor, MOVE_MA_SELECTION_TO_EXCLUDE_LIST_ACTION_NAME);
    SAFE_POINT(moveAction != nullptr, "Can't find move action in Msa editor", )
    bool isEnabled = !msaEditor->getMaObject()->isStateLocked() && !msaEditor->getSelection().isEmpty();
    moveAction->setEnabled(isEnabled);
}

////////////////////////////////////
/// MsaExcludeList
///////////////////////////////////

// TODO: document
static constexpr int LIST_ITEM_DATA_ROW_ID = 1000;
static constexpr int LIST_ITEM_DATA_SEQUENCE = 1001;

class SequenceNamesListWidget : public QListWidget {
public:
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

MsaExcludeList::MsaExcludeList(QWidget *parent, MSAEditor *_msaEditor)
    : QWidget(parent), msaEditor(_msaEditor) {
    auto layout = new QVBoxLayout(this);
    setLayout(layout);

    auto toolbarLayout = new QHBoxLayout(this);
    layout->addLayout(toolbarLayout);

    toolbarLayout->addWidget(new QLabel("Exclude List file: ", this));
    auto selectFileButton = new QToolButton(this);
    selectFileButton->setText("COI.aln.exclude-list.fasta");
    toolbarLayout->addStrut(10);
    toolbarLayout->addWidget(selectFileButton);

    auto saveAsButton = new QToolButton(this);
    saveAsButton->setText("Save as ...");
    toolbarLayout->addStrut(10);
    toolbarLayout->addWidget(saveAsButton);

    toolbarLayout->addStretch();

    auto namesAndSequenceSplitter = new QSplitter(Qt::Horizontal, this);
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
    connect(moveSelectionToMaObjectAction, &QAction::triggered, this, &MsaExcludeList::moveExcludeListSelectionToMaObject);
    connect(nameListView, &QListWidget::itemSelectionChanged, this, &MsaExcludeList::updateState);

    connect(msaEditor->getMaObject(), &MultipleSequenceAlignmentObject::si_alignmentChanged, this, &MsaExcludeList::handleUndoRedoInMsaEditor);
}

void MsaExcludeList::addEntry(const QString &name, const QByteArray &sequence, int excludeListRowId) {
    auto item = new QListWidgetItem();
    item->setText(name);
    item->setData(LIST_ITEM_DATA_ROW_ID, excludeListRowId);
    item->setData(LIST_ITEM_DATA_SEQUENCE, sequence);
    nameListView->addItem(item);
}

void MsaExcludeList::updateState() {
    moveSelectionToMaObjectAction->setEnabled(!nameListView->selectedItems().isEmpty());
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
    QList<int> selectedMsaRowIndexes = msaEditor->getCollapseModel()->getMaRowIndexesFromSelectionRects(selection.getRectList());
    QList<int> excludeListRowIds;
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
    for (int msaRowIndex : qAsConst(selectedMsaRowIndexes)) {
        MultipleAlignmentRow row = msaObject->getRow(msaRowIndex);
        DNASequence rowSequence = row->getUngappedSequence();
        qint64 listRowId = ++excludeListRowIdGenerator;
        excludeListRowIds << listRowId;
        addEntry(rowSequence.getName(), rowSequence.seq, listRowId);
    }

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(msaObject->getEntityRef(), os);
    SAFE_POINT_OP(os, );
    int versionBefore = msaObject->getObjectVersion();
    msaObject->removeRows(selectedMsaRowIndexes);
    // Exclude list re-uses msa row ids.
    trackedUndoMsaVersions.insert(versionBefore, {true, excludeListRowIds});
    trackedRedoMsaVersions.insert(msaObject->getObjectVersion(), {true, excludeListRowIds});
}

void MsaExcludeList::moveExcludeListSelectionToMaObject() {
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
    QList<DNASequence> sequences;
    QList<int> excludeListRowIdsMovedToMsa;
    QList<QListWidgetItem *> selectedItems = nameListView->selectedItems();
    for (const QListWidgetItem *listItem : qAsConst(selectedItems)) {
        auto name = listItem->text();
        auto sequenceData = listItem->data(LIST_ITEM_DATA_SEQUENCE).toByteArray();
        auto alphabet = U2AlphabetUtils::findBestAlphabet(sequenceData);
        sequences << DNASequence(name, sequenceData, alphabet);
        excludeListRowIdsMovedToMsa << listItem->data(LIST_ITEM_DATA_ROW_ID).toInt();
    }
    qDeleteAll(selectedItems);

    int versionBefore = msaObject->getObjectVersion();
    AddSequenceObjectsToAlignmentTask task(msaObject, sequences, -1, true);
    task.run();  // TODO: extract as an utility method.
    if (!task.hasError()) {
        trackedUndoMsaVersions.insert(versionBefore, {false, excludeListRowIdsMovedToMsa});
        trackedRedoMsaVersions.insert(msaObject->getObjectVersion(), {false, excludeListRowIdsMovedToMsa});
    }
}

void MsaExcludeList::handleUndoRedoInMsaEditor(const MultipleAlignment &maBefore, const MaModificationInfo &modInfo) {
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
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
        for (int rowIndex = nameListView->count(); --rowIndex >= 0;) {
            auto listItem = nameListView->item(rowIndex);
            int rowId = listItem->data(LIST_ITEM_DATA_ROW_ID).toInt();
            if (rowIdsToRemove.contains(rowId)) {
                delete listItem;
            }
        }
    }
}

}  // namespace U2
