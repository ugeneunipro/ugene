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

#ifndef _U2_MCA_EDITOR_NAME_LIST_H_
#define _U2_MCA_EDITOR_NAME_LIST_H_

#include "MaEditorNameList.h"

namespace U2 {

class McaEditor;
class McaEditorWgt;

class U2VIEW_EXPORT McaEditorNameList : public MaEditorNameList {
    Q_OBJECT
public:
    McaEditorNameList(McaEditorWgt* ui, QScrollBar* nhBar);

protected slots:
    void sl_selectionChanged(const MaEditorSelection& current, const MaEditorSelection &oldSelection);

private slots:
    void sl_updateActions();

signals:
    void si_selectionChanged();

protected:
    void drawCollapsibleSequenceItem(QPainter &painter, int rowIndex, const QString &name, const QRect &rect,
                                     bool isSelected, bool isCollapsed, bool isReference) override;

    void setSelection(int startSeq, int count) override;

private:
    McaEditor* getEditor() const;
    bool isRowReversed(int rowIndex) const;
    void drawText(QPainter &painter, const QString &text, const QRect &rect, bool selected) override;
    void drawArrow(QPainter &painter, bool isReversed, const QRectF &arrowRect);
    QRectF calculateArrowRect(const U2Region &yRange) const;

    int getAvailableWidth() const;
    int getMinimumWidgetWidth() const;

    int getIconColumnWidth() const;

    static const int MARGIN_ARROW_LEFT;
    static const int MARGIN_ARROW_RIGHT;
    static const qreal ARROW_LINE_WIDTH;
    static const qreal ARROW_LENGTH;
    static const qreal ARROW_HEAD_WIDTH;
    static const qreal ARROW_HEAD_LENGTH;
    static const QColor ARROW_DIRECT_COLOR;
    static const QColor ARROW_REVERSE_COLOR;
};

}   // namespace U2

#endif // _U2_MCA_EDITOR_NAME_LIST_H_
