/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ov_msa/overview/MaOverview.h"

class QScrollBar;

namespace U2 {

class McaEditor;

class MaSangerOverview : public MaOverview {
    Q_OBJECT
public:
    MaSangerOverview(MaEditor* editor, MaEditorWgt* ui);

    bool isValid() const override;
    QPixmap getView() override;

private slots:
    void sl_updateScrollBar();
    void sl_completeRedraw();
    void sl_resetCaches();
    void sl_screenMoved();
    void sl_colorModeSwitched() override;

private:
    bool eventFilter(QObject* object, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void drawOverview(QPainter& painter) override;
    void drawVisibleRange(QPainter& painter) override;
    void drawReference();
    void drawReads();

    void moveVisibleRange(QPoint pos) override;

    McaEditor* getEditor() const;

    int getContentWidgetWidth() const override;
    int getContentWidgetHeight() const override;
    int getReadsHeight() const;
    int getReferenceHeight() const;
    int getScrollBarValue() const;

    QScrollBar* vScrollBar;
    QWidget* referenceArea;
    QWidget* renderArea;

    QPixmap cachedReadsView;
    QPixmap cachedReferenceView;

    bool completeRedraw;
    int cachedReferenceHeight;

    static const int READ_HEIGHT;
    static const int MINIMUM_HEIGHT;
    static const qreal ARROW_LINE_WIDTH;
    static const qreal ARROW_HEAD_WIDTH;
    static const qreal ARROW_HEAD_LENGTH;
};

}  // namespace U2
