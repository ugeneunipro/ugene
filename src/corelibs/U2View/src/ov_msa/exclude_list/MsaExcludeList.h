/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_EXCLUDE_LIST_H_
#define _U2_MSA_EXCLUDE_LIST_H_

#include <QSet>
#include <QWidget>

#include <U2Core/DNASequence.h>
#include <U2Core/global.h>

#include <U2Gui/ObjectViewModel.h>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QSplitter;
class QToolButton;

namespace U2 {

class LoadDocumentTask;
class MSAEditor;
class MaModificationInfo;
class MsaExcludeListWidget;
class MultipleAlignment;
class MultipleAlignmentRow;

/** MSA editor built-in support for "MSA Exclude List" widget. */
class MsaExcludeListContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    MsaExcludeListContext(QObject* parent);

    QAction* getMoveMsaSelectionToExcludeListAction(MSAEditor* msaEditor);

protected:
    void initViewContext(GObjectView* view) override;

private:
    void toggleExcludeListView(MSAEditor* msaEditor);

    /** Updates Exclude List actions associated with Msa Editor based on the current editor's state. */
    void updateState(MSAEditor* msaEditor);

    /** Returns active Exclude List widget or nullptr if the widget is not found. */
    MsaExcludeListWidget* findActiveExcludeList(MSAEditor* msaEditor);

    MsaExcludeListWidget* openExcludeList(MSAEditor* msaEditor);

    void updateMsaEditorSplitterStyle(MSAEditor* msaEditor);
};

/** Stores per step (Undo or Redo) information used by Exclude List to keep synchronized state with MSA Editor during Undo/Redo ops. */
struct UndoRedoStep {
    /** Direction of the original move. */
    bool isMoveFromMsaToExcludeList = false;
    /** Affected Exclude List row ids. The row ids keeps the same order as MSA rows being changed during the move. */
    QList<int> excludeListRowIdsDelta;
};

/**
 * Exclude list widget component for MsaEditor.
 * The 'exclude list' renders a list of sequences excluded from the multiple alignment object.
 *
 * The widget contains a data model for Exclude List and a limited Undo/Redo support for MSA:
 * the Undo-Redo is supported only for the life-time of the widget: when Exclude List is closed the Undo/Redo stack of the widget is reset.
 */
class MsaExcludeListWidget : public QWidget {
    Q_OBJECT
public:
    MsaExcludeListWidget(QWidget* parent, MSAEditor* msaEditor, MsaExcludeListContext* viewContext);

    /** Triggers auto-save of the current exclude list state. */
    ~MsaExcludeListWidget();

    /** Moves currently selected MSA rows to the Exclude List. */
    void moveMsaSelectionToExcludeList();

    /** Moves currently selected Exclude List rows to MSA. */
    void moveExcludeListSelectionToMaObject();

    /** Tracks MSA object "Save Document" task and auto-saves Exclude List on successfull taks completion. */
    void trackMsaObjectSaveTask(Task* task);

    /** Returns preferred size hint used on instantiation. This size defines initial widget height in the MSA splitter. */
    QSize sizeHint() const override;

private:
    /** Updates state of all child widgets. */
    void updateState();

    /** Updates currently visible sequence text based on name-list selection. */
    void updateSequenceView();

    /** Moves specified row indexes to exclude list. */
    void moveMsaRowIndexesToExcludeList(const QList<int>& msaRowIndexes);

    /** Adds a minimal support of the synchronized Undo/Redo for the Exclude List and MSA Editor. */
    void handleUndoRedoInMsaEditor(const MultipleAlignment& maBefore, const MaModificationInfo& modInfo);

    /** Adds a name list entry. If excludeListRowId <= 0 auto-assigns a new row id. Returns a valid exclude list row id. */
    int addEntry(const DNASequence& sequence, int excludeListRowId = 0);

    /** Adds a name list entry from MSA row. Fixes non-defined DNA sequence alphabet of MSA row. */
    int addMsaRowEntry(const MultipleAlignmentRow& row, int excludeListRowId = 0);

    /** Remove sequence entries from the Exclude List model. */
    void removeEntries(const QList<QListWidgetItem*>& items);

    /** Triggers context menu for the name list. */
    void showNameListContextMenu();

    /** Triggers loading of new exclude list from the 'excludeListFilePath'. */
    void loadExcludeList(bool create = false);

    /** Unloads current exclude list. Used on the Exclude List file switch. */
    void unloadExcludeList();

    /** Runs save task for Exclude list. */
    Task* runSaveTask(const QString& savePath);

    /** Opens "FileDialog" and selects a new exclude list name to open. */
    void changeExcludeListFile();

    /** Opens "FileDialog" and selects a new exclude list name to save. Saves the current state into the selected file. */
    void saveExcludeFileToNewLocation();

    /** Takes data from the load FASTA document task and updates Exclude List internal model. */
    void handleLoadTaskStateChange();

    /** Returns true if there is an active Exclude List load/save task. */
    bool hasActiveTask() const;

    /** Returns Exclude List row id associated with the given item. */
    int getExcludeListRowId(const QListWidgetItem* item) const;

    /** Returns Exclude List DNA sequence associated with the given item. */
    DNASequence getExcludeListRowSequence(const QListWidgetItem* item) const;

    MSAEditor* msaEditor = nullptr;

    /** A name list widget. Used as a data model for Exclude List: keeps row names & sequences in user data slots. */
    QListWidget* nameListView = nullptr;

    /** Selected sequence viewer. Works in sync with "nameListView". */
    QPlainTextEdit* sequenceView = nullptr;

    QToolButton* selectFileButton = nullptr;

    QToolButton* saveAsButton = nullptr;

    QSplitter* namesAndSequenceSplitter = nullptr;

    QLabel* stateLabel = nullptr;

    QAction* moveToMsaAction = nullptr;

    /** Stores Exclude List state for UNDO ops. */
    QMap<int, UndoRedoStep> trackedUndoMsaVersions;

    /** Stores Exclude List state for REDO ops. */
    QMap<int, UndoRedoStep> trackedRedoMsaVersions;

    /** Used to generate unique row id for each row in Exclude List. */
    int excludeListRowIdGenerator = 1;

    /** Current Exclude List file path. */
    QString excludeListFilePath;

    bool isLoaded = false;

    bool isDirty = false;

    Task* saveTask = nullptr;

    LoadDocumentTask* loadTask = nullptr;

    QHash<int, DNASequence> sequenceByExcludeListRowId;

    /** Keeps MSA row ids need to be moved to the widget after load task is finished. */
    QList<qint64> pendingMoveFromMsaRowIds;
};

}  // namespace U2

#endif  //_U2_MSA_EXCLUDE_LIST_H_
