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

#include <U2Gui/ScaleBar.h>

#include "../ov_msa/MaEditorSequenceArea.h"
#include "McaEditor.h"

namespace U2 {

class MaAmbiguousCharactersController;
class McaEditor;

class ChromatogramViewSettings {
public:
    ChromatogramViewSettings() {
        drawTraceA = true;
        drawTraceC = true;
        drawTraceG = true;
        drawTraceT = true;
    }
    bool drawTraceA;
    bool drawTraceC;
    bool drawTraceG;
    bool drawTraceT;
};

class U2VIEW_EXPORT McaEditorSequenceArea : public MaEditorSequenceArea {
    Q_OBJECT
public:
    McaEditorSequenceArea(McaEditorWgt* ui, GScrollBar* hb, GScrollBar* vb);

    McaEditor* getEditor() const {
        return qobject_cast<McaEditor*>(editor);
    }

    const ChromatogramViewSettings& getSettings() const {
        return settings;
    }
    bool getShowQA() const {
        return showQVAction->isChecked();
    }

    void moveSelection(int dx, int dy, bool allowSelectionResize = false);

    virtual void adjustReferenceLength(U2OpStatus& os);

    MaAmbiguousCharactersController* getAmbiguousCharactersController() const;

    QMenu* getTraceActionsMenu() const;
    QAction* getIncreasePeaksHeightAction() const;
    QAction* getDecreasePeaksHeightAction() const;
    QAction* getInsertAction() const;
    QAction* getInsertGapAction() const;
    QAction* getRemoveGapBeforeSelectionAction() const;
    QAction* getRemoveColumnsOfGapsAction() const;
    QAction* getTrimLeftEndAction() const;
    QAction* getTrimRightEndAction() const;

public slots:
    void sl_backgroundSelectionChanged();

private slots:
    void sl_showHideTrace();
    void sl_showAllTraces();
    void sl_setRenderAreaHeight(int k);

    void sl_buildStaticToolbar(GObjectViewController* v, QToolBar* t);

    void sl_addInsertion();
    void sl_removeGapBeforeSelection();
    void sl_removeColumnsOfGaps();
    void sl_trimLeftEnd();
    void sl_trimRightEnd();

    void sl_updateActions();

private:
    void initRenderer();
    void drawBackground(QPainter& p);

    void updateCollapseModel(const MaModificationInfo& modInfo) override;

    void getColorAndHighlightingIds(QString& csid, QString& hsid);

    QAction* createToggleTraceAction(const QString& actionName);

    void insertChar(char newCharacter);
    bool isCharacterAcceptable(const QString& text) const;
    const QString& getInacceptableCharacterErrorMessage() const;

    McaEditorWgt* getMcaEditorWgt() const;

    void trimRowEnd(MultipleChromatogramAlignmentObject::TrimEdge edge);

    void updateTrimActions(bool isEnabled);

    ChromatogramViewSettings settings;
    MaAmbiguousCharactersController* ambiguousCharactersController;

    QAction* showQVAction;
    QAction* showAllTraces;
    QMenu* traceActionsMenu;
    ScaleBar* scaleBar;
    QAction* scaleAction;

    QAction* insertAction;
    QAction* removeGapBeforeSelectionAction;
    QAction* removeColumnsOfGapsAction;
    QAction* trimLeftEndAction;
    QAction* trimRightEndAction;
};

}  // namespace U2
