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

#ifndef _U2_MA_SANGER_OVERVIEW_H_
#define _U2_MA_SANGER_OVERVIEW_H_

#include "MaOverview.h"

class QScrollBar;

namespace U2 {

class McaEditor;

class MaSangerOverview : public MaOverview {
    Q_OBJECT
public:
    MaSangerOverview(MaEditorWgt *ui);

    bool isValid() const;
    QPixmap getView();

private slots:
    void sl_updateScrollBar();
    void sl_completeRedraw();
    void sl_resetCaches();
    void sl_screenMoved();

private:
    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent(QResizeEvent *event);

    void drawOverview(QPainter &painter);
    void drawVisibleRange(QPainter &painter);
    void drawReference();
    void drawReads();

    void moveVisibleRange(QPoint pos);

    McaEditor* getEditor() const;

    int getContentWidgetWidth() const;
    int getContentWidgetHeight() const;
    int getReadsHeight() const;
    int getReferenceHeight() const;
    int getScrollBarValue() const;

    QScrollBar *vScrollBar;
    QWidget *referenceArea;
    QWidget *renderArea;

    QPixmap cachedReadsView;
    QPixmap cachedReferenceView;

    bool completeRedraw;
    int cachedReferenceHeight;

    static const int READ_HEIGHT;
    static const int MINIMUM_HEIGHT;
    static const qreal ARROW_LINE_WIDTH;
    static const qreal ARROW_HEAD_WIDTH;
    static const qreal ARROW_HEAD_LENGTH;
    static const QColor ARROW_DIRECT_COLOR;
    static const QColor ARROW_REVERSE_COLOR;
};

}   // namespace U2

#endif // _U2_MA_SANGER_OVERVIEW_H_
