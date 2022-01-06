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

#ifndef _U2_MSA_EXCLUDE_LIST_H_
#define _U2_MSA_EXCLUDE_LIST_H_

#include <QSet>
#include <QWidget>

#include <U2Core/global.h>

#include <U2Gui/ObjectViewModel.h>

class QListWidget;
class QPlainTextEdit;

namespace U2 {

class MSAEditor;
class MaModificationInfo;
class MsaExcludeList;
class MultipleAlignment;

/** MSA editor built-in support for "Align-Sequences-To-Alignment" algorithms. */
class MsaExcludeListContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    MsaExcludeListContext(QObject *parent);

protected:
    void initViewContext(GObjectView *view) override;

private:
    void toggleExcludeListView(MSAEditor *msaEditor);

    void moveMaSelectionToExcludeList(MSAEditor *msaEditor);

    void updateState(MSAEditor *msaEditor);

    MsaExcludeList *findActiveExcludeList(MSAEditor *msaEditor);

    MsaExcludeList *openExcludeList(MSAEditor *msaEditor);

    void updateMsaEditorSplitterStyle(MSAEditor *msaEditor);
};

// todo: document
struct UndoRedoContext {
    bool isMoveFromMsaToExcludeList = false;
    QList<int> excludeListRowIdsDelta;
};

/**
 * Exclude list widget component for MsaEditor.
 * The 'exclude list' renders list of sequences excluded (temporary) from the multiple alignment object.
 * These sequences are stored in a ''
 */
class MsaExcludeList : public QWidget {
    Q_OBJECT
public:
    MsaExcludeList(QWidget *parent, MSAEditor *msaEditor);

    void moveMsaSelectionToExcludeList();
    void moveExcludeListSelectionToMaObject();

private:
    void updateSequenceView();
    void updateState();
    void handleUndoRedoInMsaEditor(const MultipleAlignment &maBefore, const MaModificationInfo &modInfo);
    void addEntry(const QString &name, const QByteArray &sequence, int msaRowId);

    void showNameListContextMenu(const QPoint &pos);

    MSAEditor *msaEditor = nullptr;
    QListWidget *nameListView = nullptr;
    QPlainTextEdit *sequenceView = nullptr;
    QAction *moveSelectionToMaObjectAction = nullptr;
    QMap<int, UndoRedoContext> trackedUndoMsaVersions;
    QMap<int, UndoRedoContext> trackedRedoMsaVersions;
    int excludeListRowIdGenerator = 0;
};

}  // namespace U2

#endif  //_U2_MSA_EXCLUDE_LIST_H_
