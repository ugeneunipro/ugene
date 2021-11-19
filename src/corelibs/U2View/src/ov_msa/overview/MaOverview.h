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

#ifndef _U2_MA_OVERVIEW_H_
#define _U2_MA_OVERVIEW_H_

#include <QWidget>

#include <U2Core/global.h>

namespace U2 {

class MaEditor;
class MaEditorWgt;
class MaEditorSequenceArea;

#define VISIBLE_RANGE_COLOR QColor(230, 230, 230, 180)
#define SELECTION_COLOR QColor(80, 160, 200, 180)
#define VISIBLE_RANGE_CRITICAL_SIZE 5

class U2VIEW_EXPORT MaOverview : public QWidget {
    Q_OBJECT
public:
    MaOverview(MaEditor *_editor, QWidget *_ui);
    virtual bool isValid() const {
        return false;
    }
    virtual QPixmap getView() {
        return QPixmap();
    }
    MaEditor* getEditor() const;

public slots:
    void sl_visibleRangeChanged();
    virtual void sl_selectionChanged() {
    }
    virtual void sl_redraw();

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    virtual void drawOverview(QPainter&) {
    }
    virtual void drawVisibleRange(QPainter&) {
    }
    virtual void drawSelection(QPainter&) {
    }

    void setVisibleRangeForEmptyAlignment();

    virtual void moveVisibleRange(QPoint) {
    }

    void recalculateScale();

    virtual int getContentWidgetWidth() const;
    virtual int getContentWidgetHeight() const;

    MaEditor *editor;
    QWidget *ui;
    // TODO:ichebyki
    // MaEditorSequenceArea *sequenceArea;

    QPixmap cachedView;
    QRect cachedVisibleRange;

    bool visibleRangeIsMoving;

    double stepX;
    double stepY;
};

}  // namespace U2

#endif  // _U2_MA_OVERVIEW_H_
