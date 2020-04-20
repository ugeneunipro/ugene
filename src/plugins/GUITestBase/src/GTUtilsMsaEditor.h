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

#ifndef _U2_GT_UTILS_MSA_EDITOR_H_
#define _U2_GT_UTILS_MSA_EDITOR_H_

#include <QColor>
#include <QRect>
#include <GTGlobals.h>

namespace U2 {

class MSAEditor;
class MSAEditorConsensusArea;
class MaEditorNameList;
class MSAEditorSequenceArea;
class MSAEditorTreeViewerUI;
class MsaEditorWgt;
class MaGraphOverview;
class MaSimpleOverview;

// If you can't find an appropriate method check the GTUtilsMsaEditorSequenceArea class
class GTUtilsMsaEditor {
public:
    static QColor getGraphOverviewPixelColor(HI::GUITestOpStatus &os, const QPoint &point);
    static QColor getSimpleOverviewPixelColor(HI::GUITestOpStatus &os, const QPoint &point);

    static MSAEditor * getEditor(HI::GUITestOpStatus &os);
    static MsaEditorWgt * getEditorUi(HI::GUITestOpStatus &os);
    static MaGraphOverview * getGraphOverview(HI::GUITestOpStatus &os);
    static MaSimpleOverview * getSimpleOverview(HI::GUITestOpStatus &os);
    static MSAEditorTreeViewerUI * getTreeView(HI::GUITestOpStatus &os);
    static MaEditorNameList * getNameListArea(HI::GUITestOpStatus &os);
    static MSAEditorConsensusArea * getConsensusArea(HI::GUITestOpStatus &os);
    static MSAEditorSequenceArea * getSequenceArea(HI::GUITestOpStatus &os);

    static QRect getSequenceNameRect(HI::GUITestOpStatus &os, const QString &sequenceName);
    static QRect getSequenceNameRect(HI::GUITestOpStatus &os, int rowNumber);
    static QRect getColumnHeaderRect(HI::GUITestOpStatus &os, int column);

    static void replaceSequence(HI::GUITestOpStatus &os, const QString &sequenceToReplace, int targetPosition);
    static void replaceSequence(HI::GUITestOpStatus &os, int rowNumber, int targetPosition);
    static void removeColumn(HI::GUITestOpStatus &os, int column);
    static void removeRows(HI::GUITestOpStatus &os, int firstRowNumber, int lastRowNumber);

    static void moveToSequence(HI::GUITestOpStatus &os, int rowNumber);
    static void moveToSequenceName(HI::GUITestOpStatus &os, const QString &sequenceName);
    static void clickSequence(HI::GUITestOpStatus &os, int rowNumber, Qt::MouseButton mouseButton = Qt::LeftButton);
    static void clickSequenceName(HI::GUITestOpStatus &os, const QString &sequenceName, Qt::MouseButton mouseButton = Qt::LeftButton);
    static void moveToColumn(HI::GUITestOpStatus &os, int column);
    static void clickColumn(HI::GUITestOpStatus &os, int column, Qt::MouseButton mouseButton = Qt::LeftButton);

    static void selectRows(HI::GUITestOpStatus &os, int firstRowNumber, int lastRowNumber, GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void selectColumns(HI::GUITestOpStatus &os, int firstColumnNumber, int lastColumnNumber, GTGlobals::UseMethod method = GTGlobals::UseKey);

    static void clearSelection(HI::GUITestOpStatus &os);

    static QString getReferenceSequenceName(HI::GUITestOpStatus &os);
    static void setReference(HI::GUITestOpStatus &os, const QString &sequenceName);

    static void toggleCollapsingMode(HI::GUITestOpStatus &os);
    static void toggleCollapsingGroup(HI::GUITestOpStatus &os, const QString &groupName);
    static bool isSequenceCollapsed(HI::GUITestOpStatus &os, const QString &seqName);

    static int getSequencesCount(HI::GUITestOpStatus &os);
    static QStringList getWholeData(HI::GUITestOpStatus &os);

    static void undo(HI::GUITestOpStatus &os);
    static void redo(HI::GUITestOpStatus &os);

    static bool isUndoEnabled(HI::GUITestOpStatus &os);
    static bool isRedoEnabled(HI::GUITestOpStatus &os);

    static void buildPhylogeneticTree(HI::GUITestOpStatus &os, const QString &pathToSave);

    static void dragAndDropSequenceFromProject(HI::GUITestOpStatus &os, const QStringList &pathToSequence);
};

}   // namespace U2

#endif // _U2_GT_UTILS_MSA_EDITOR_H_
