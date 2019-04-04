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

#include <U2Core/Counter.h>

#include <U2Gui/GScrollBar.h>

#include "../AssemblyBrowser.h"
#include "AssemblyAnnotationsArea.h"
#include "AssemblyAnnotationsAreaWidget.h"
#include "AssemblyAnnotationsRenderAreaFactory.h"

namespace U2 {

AssemblyAnnotationsAreaWidget::AssemblyAnnotationsAreaWidget
                    (AssemblyBrowser* _browser,
                     AssemblyBrowserUi* _ui,
                     SequenceObjectContext* _ctx)
                        : PanView(_ui,
                                  _ctx,
                                  AssemblyAnnotationsRenderAreaFactory(_ui,
                                                                       _browser)),
                          browser(_browser),
                          browserUi(_ui) {
    GCOUNTER(c2, t2, "AssemblyReadsArea::annotationWigetWasCreaated");

    setLocalToolbarVisible(false);
    sl_toggleMainRulerVisibility(false);
    sl_toggleCustomRulersVisibility(false);
    scrollBar->hide();
    connectSlots();
    updateVisibleRange();
    update();
}

void AssemblyAnnotationsAreaWidget::mouseMoveEvent(QMouseEvent *e) {
    emit si_mouseMovedToPos(e->pos());
    PanView::mouseMoveEvent(e);
}

int AssemblyAnnotationsAreaWidget::getHorizontalScrollBarPosition() const {
    return scrollBar->sliderPosition();
}

void AssemblyAnnotationsAreaWidget::sl_zoomPerformed() {
    updateVisibleRange();
    update();
}

void AssemblyAnnotationsAreaWidget::sl_offsetsChanged() {
    updateVisibleRange();
    update();
}

void AssemblyAnnotationsAreaWidget::connectSlots() const {
    connect(this, SIGNAL(si_mouseMovedToPos(const QPoint &)),
            browserUi->getAnnotationsArea(), SIGNAL(si_mouseMovedToPos(const QPoint &)));
    connect(browser, SIGNAL(si_zoomOperationPerformed()), SLOT(sl_zoomPerformed()));
    connect(browser, SIGNAL(si_offsetsChanged()), SLOT(sl_offsetsChanged()));
}

void AssemblyAnnotationsAreaWidget::updateVisibleRange() {
    U2Region visibleBaseReg = browser->getVisibleBasesRegion();
    if (visibleBaseReg != getVisibleRange()) {
        setVisibleRange(visibleBaseReg);
        addUpdateFlags(GSLV_UF_VisibleRangeChanged);
    }
}

} // U2
