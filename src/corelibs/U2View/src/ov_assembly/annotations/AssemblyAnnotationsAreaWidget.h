/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#ifndef _ASSEMBLY_ANNOTATIONS_AREA_WIDGET_
#define _ASSEMBLY_ANNOTATIONS_AREA_WIDGET_

#include <U2View/PanView.h>

namespace U2 {

class AssemblyBrowser;
class AssemblyBrowserUi;

class AssemblyAnnotationsAreaWidget : public PanView {
    Q_OBJECT
public:
    AssemblyAnnotationsAreaWidget(AssemblyBrowser* browser, AssemblyBrowserUi *ui, SequenceObjectContext* ctx);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;

    int getHorizontalScrollBarPosition() const override;

signals:
    void si_mouseMovedToPos(const QPoint&);

private slots:
    void sl_zoomPerformed();
    void sl_offsetsChanged();

private:
    void connectSlots() const;
    void updateVisibleRange();

    AssemblyBrowser* browser;
    AssemblyBrowserUi* browserUi;
};

} // U2

#endif // _ASSEMBLY_ANNOTATIONS_AREA_WIDGET_
