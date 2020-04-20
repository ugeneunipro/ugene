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

#ifndef _U2_GT_UTILS_MCA_EDITOR_H_
#define _U2_GT_UTILS_MCA_EDITOR_H_

class QLabel;
class QScrollBar;

namespace HI{
class GUITestOpStatus;
}

namespace U2 {

class McaEditor;
class McaEditorConsensusArea;
class McaEditorNameList;
class McaEditorReferenceArea;
class McaEditorSequenceArea;
class McaEditorWgt;
class MultipleAlignmentRowData;

class GTUtilsMcaEditor {
public:
    static McaEditor *getEditor(HI::GUITestOpStatus &os);
    static McaEditorWgt *getEditorUi(HI::GUITestOpStatus &os);
    static QLabel *getReferenceLabel(HI::GUITestOpStatus &os);
    static McaEditorNameList *getNameListArea(HI::GUITestOpStatus &os);
    static McaEditorSequenceArea *getSequenceArea(HI::GUITestOpStatus &os);
    static McaEditorConsensusArea* getConsensusArea(HI::GUITestOpStatus &os);
    static McaEditorReferenceArea *getReferenceArea(HI::GUITestOpStatus &os);
    static QScrollBar *getHorizontalScrollBar(HI::GUITestOpStatus &os);
    static QScrollBar *getVerticalScrollBar(HI::GUITestOpStatus &os);

    static MultipleAlignmentRowData* getMcaRow(HI::GUITestOpStatus &os, int rowNum);

    static QAction* getOffsetAction(HI::GUITestOpStatus &os);

    static QString getReferenceLabelText(HI::GUITestOpStatus &os);

    static int getReadsCount(HI::GUITestOpStatus &os);
    static const QStringList getReadsNames(HI::GUITestOpStatus &os);
    static const QStringList getDirectReadsNames(HI::GUITestOpStatus &os);
    static const QStringList getReverseComplementReadsNames(HI::GUITestOpStatus &os);

    static QRect getReadNameRect(HI::GUITestOpStatus &os, const QString &readName);
    static QRect getReadNameRect(HI::GUITestOpStatus &os, int rowNumber);

    static void scrollToRead(HI::GUITestOpStatus &os, const QString &readName);
    static void scrollToRead(HI::GUITestOpStatus &os, int readNumber);
    static void moveToReadName(HI::GUITestOpStatus &os, const QString &readName);
    static void moveToReadName(HI::GUITestOpStatus &os, int readNumber);
    static void clickReadName(HI::GUITestOpStatus &os, const QString &sequenceName, Qt::MouseButton mouseButton = Qt::LeftButton);
    static void clickReadName(HI::GUITestOpStatus &os, int readNumber, Qt::MouseButton mouseButton = Qt::LeftButton);

    static void removeRead(HI::GUITestOpStatus &os, const QString &readName);

    static void undo(HI::GUITestOpStatus &os);
    static void redo(HI::GUITestOpStatus &os);
    static void zoomIn(HI::GUITestOpStatus &os);
    static void zoomOut(HI::GUITestOpStatus &os);
    static void resetZoom(HI::GUITestOpStatus &os);

    static bool isUndoEnabled(HI::GUITestOpStatus &os);
    static bool isRedoEnabled(HI::GUITestOpStatus &os);

    static void toggleShowChromatogramsMode(HI::GUITestOpStatus &os);

private:
    static int readName2readNumber(HI::GUITestOpStatus &os,const QString &readName);
};

}   // namespace U2

#endif // _U2_GT_UTILS_MCA_EDITOR_H_
