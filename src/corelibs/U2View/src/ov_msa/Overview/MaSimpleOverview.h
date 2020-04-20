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

#ifndef _U2_MSA_SIMPLE_OVERVIEW_H_
#define _U2_MSA_SIMPLE_OVERVIEW_H_

#include <U2Core/global.h>

#include <QWidget>
#include "MaOverview.h"


namespace U2 {

class MSAEditor;
class MaEditorWgt;
class MSAEditorSequenceArea;
class MsaColorScheme;
class MsaHighlightingScheme;


class U2VIEW_EXPORT MaSimpleOverview : public MaOverview {
    Q_OBJECT
public:
    MaSimpleOverview(MaEditorWgt *ui);
    const static int FIXED_HEIGTH = 70;
    bool isValid() const;
    QPixmap getView();

public slots:
    void sl_selectionChanged();
    void sl_redraw();
    void sl_highlightingChanged();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    void drawOverview(QPainter &p);
    void drawVisibleRange(QPainter &p);
    void drawSelection(QPainter &p);

    void moveVisibleRange(QPoint pos);

    void recalculateSelection();

private:
    mutable QPixmap cachedMSAOverview;

    mutable bool    redrawMsaOverview;
    mutable bool    redrawSelection;
};

} // namespace

#endif // _U2_MSA_SIMPLE_OVERVIEW_H_
