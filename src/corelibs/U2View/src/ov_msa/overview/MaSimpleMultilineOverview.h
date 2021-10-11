/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_SIMPLE_MULTILINE_OVERVIEW_H_
#define _U2_MSA_SIMPLE_MULTILINE_OVERVIEW_H_

#include <QWidget>

#include <U2Core/global.h>

#include "MaMultilineOverview.h"

namespace U2 {

class U2VIEW_EXPORT MaSimpleMultilineOverview : public MaMultilineOverview {
    Q_OBJECT
public:
    MaSimpleMultilineOverview(MaEditorMultilineWgt *ui);

    /** Height of the overview. */
    const static int FIXED_HEIGTH = 70;

    bool isValid() const override;

    QPixmap getView() override;

public slots:
    void sl_selectionChanged() override;
    void sl_redraw() override;
    void sl_highlightingChanged();

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    void drawOverview(QPainter &p) override;
    void drawVisibleRange(QPainter &p) override;
    void drawSelection(QPainter &p) override;

    void moveVisibleRange(QPoint pos) override;

private:
    QPixmap cachedMSAOverview;

    bool redrawMsaOverview = true;
    bool redrawSelection = true;
};

}  // namespace U2

#endif  // _U2_MSA_SIMPLE_MULTILINE_OVERVIEW_H_
