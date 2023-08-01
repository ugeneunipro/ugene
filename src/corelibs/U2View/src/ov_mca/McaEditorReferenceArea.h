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

#include <U2View/MaEditorSelection.h>
#include <U2View/PanView.h>

#include "McaReferenceAreaRenderer.h"

namespace U2 {

class McaEditor;
class McaEditorWgt;

class U2VIEW_EXPORT McaEditorReferenceArea : public PanView {
    Q_OBJECT
public:
    McaEditorReferenceArea(McaEditorWgt* ui, SequenceObjectContext* ctx);

    /** Clears reference area selection. */
    void clearSelection();

signals:
    void si_selectionChanged();

public slots:
    void sl_selectMismatch(int pos);

private slots:
    void sl_visibleRangeChanged();
    void sl_selectionChanged(const MaEditorSelection& current, const MaEditorSelection& prev);
    void sl_onSelectionChanged(LRegionsSelection* selection, const QVector<U2Region>& addedRegions, const QVector<U2Region>& removedRegions);
    void sl_fontChanged(const QFont& newFont);

private:
    void setReferenceSelection(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* event);
    void updateScrollBar();

    McaEditor* editor;
    McaEditorWgt* ui;
    McaReferenceAreaRenderer* renderer;
    qint64 firstPressedSelectionPosition = -1;
};

class McaEditorReferenceRenderArea : public PanViewRenderArea {
public:
    McaEditorReferenceRenderArea(McaEditorWgt* ui, PanView* d, PanViewRenderer* renderer);

    qint64 coordToPos(const QPoint& coord) const override;

private:
    McaEditorWgt* const ui;
};

class McaEditorReferenceRenderAreaFactory : public PanViewRenderAreaFactory {
public:
    McaEditorReferenceRenderAreaFactory(McaEditorWgt* _ui, McaEditor* _editor);

    PanViewRenderArea* createRenderArea(PanView* panView) const;

private:
    McaEditorWgt* ui;
    MaEditor* maEditor;
};

}  // namespace U2
