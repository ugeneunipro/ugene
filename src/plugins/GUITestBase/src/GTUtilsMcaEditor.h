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

#pragma once

#include <QLabel>
#include <QScrollBar>
#include <QWidget>

namespace HI {
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
    /** Returns active MCA editor window or fails. */
    static QWidget* getActiveMcaEditorWindow();

    /** Checks if editor window is active or fails. */
    static void checkMcaEditorWindowIsActive();

    static McaEditor* getEditor();
    static McaEditorWgt* getEditorUi();
    static QLabel* getReferenceLabel();
    static McaEditorNameList* getNameListArea();
    static McaEditorSequenceArea* getSequenceArea();
    static McaEditorConsensusArea* getConsensusArea();
    static McaEditorReferenceArea* getReferenceArea();
    static QScrollBar* getHorizontalScrollBar();
    static QScrollBar* getVerticalScrollBar();

    static MultipleAlignmentRowData* getMcaRow(int rowNum);

    static QAction* getOffsetAction();

    static QString getReferenceLabelText();

    static int getReadsCount();
    static const QStringList getReadsNames();
    static const QStringList getDirectReadsNames();
    static const QStringList getReverseComplementReadsNames();

    static QRect getReadNameRect(const QString& readName);
    static QRect getReadNameRect(int rowNumber);

    static void scrollToRead(const QString& readName);
    static void scrollToRead(int readNumber);
    static void moveToReadName(const QString& readName);
    static void moveToReadName(int readNumber);

    static void clickReadName(
        const QString& sequenceName,
        Qt::MouseButton mouseButton = Qt::LeftButton,
        const Qt::KeyboardModifiers& modifiers = Qt::NoModifier);

    static void clickReadName(
        int readNumber,
        Qt::MouseButton mouseButton = Qt::LeftButton,
        const Qt::KeyboardModifiers& modifiers = Qt::NoModifier);

    /** Double clicks read with the given index in the name list. */
    static void doubleClickReadName(int readIndex);

    static void removeRead(const QString& readName);

    static void undo();
    static void redo();
    static void zoomIn();
    static void zoomOut();
    static void resetZoom();

    static bool isUndoEnabled();
    static bool isRedoEnabled();

    static void toggleShowChromatogramsMode();

    /** Selects reads in the name list by name using Ctrl + Mouse click. Fails if any of the rows is not found. */
    static void selectReadsByName(const QStringList& rowNames);

private:
    static int readName2readNumber(const QString& readName);
};

}  // namespace U2
