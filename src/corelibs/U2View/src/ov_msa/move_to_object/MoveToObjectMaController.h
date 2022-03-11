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

#ifndef _U2_MOVETOOBJECTMAMENUCONTROLLER_H_
#define _U2_MOVETOOBJECTMAMENUCONTROLLER_H_

#include <QAction>
#include <QMenu>

#include <U2Core/Task.h>

#include <U2View/MaEditorContext.h>

namespace U2 {

class MaEditorContext;
class GObjectView;

/** Implements set of actions to move data between different MA objects in project. */
class MoveToObjectMaController : public QObject, public MaEditorContext {
    Q_OBJECT
public:
    MoveToObjectMaController(MaEditor *maEditor, QWidget *wgt);

private slots:
    /** Shows moveSelectionToAnotherObject at cursor position. */
    void showMoveSelectedRowsToAnotherObjectMenu();

    /** Shows new MSA file selection dialog, creates a new file with an alignment and runs a task to move selected sequences into that alignment. */
    void runMoveSelectedRowsToNewFileDialog();

    /** Adds 'moveSelectionToAnotherObjectAction' to the export menu. */
    void buildMenu(GObjectView* view, QMenu* menu, const QString& menuType);

    /** Updates all  si_updateActions from MaEditor. */
    void updateActions();

private:
    /** Builds a new 'Move selection to another object' sub-menu. */
    QMenu* buildMoveSelectionToAnotherObjectMenu() const;

    /** Moves selected rows into another MSA object. */
    QAction* moveSelectionToAnotherObjectAction = nullptr;

    /** Moves selected rows into a new file. Opens file selector dialog to select the file to move. */
    QAction* moveSelectionToNewFileAction = nullptr;
};

/** Removes set of rows from the MSA object. */
class RemoveRowsFromMaObjectTask : public Task {
    Q_OBJECT
public:
    RemoveRowsFromMaObjectTask(MaEditor* maEditor, const QList<qint64>& rowIds);

    void run() override;

private:
    const QPointer<MaEditor> maEditor;
    const QList<qint64> rowIds;
};

}  // namespace U2

#endif  // _U2_MOVETOOBJECTMAMENUCONTROLLER_H_
