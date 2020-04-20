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

#include "GlassView.h"

#include <QEvent>
#include <QResizeEvent>
#include <QPainter>

namespace U2 {

void GlassView::setGlass(GlassPane* glp) {
    glass = glp;
    if (glass) {
        glass->resize(viewport()->size());
    }
    //scene()->update();
    viewport()->update();
}

void GlassView::paintEvent( QPaintEvent * e)
{
    QGraphicsView::paintEvent(e);
    if (glass) {
        QPainter painter;
        painter.begin(viewport());
        //painter.setClipRect(e->rect());
        glass->paint(&painter);
    }
}

void GlassView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx,dy);
    if (glass) {
        viewport()->update();
    }
}

bool GlassView::viewportEvent( QEvent * e )
{
    if (glass) {

        switch (e->type())
        {
        case QEvent::Resize:
            glass->resize(((QResizeEvent*)e)->size());
        case QEvent::Paint:
            return QGraphicsView::viewportEvent(e);

        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Enter :
        case QEvent::Leave:
        case QEvent::Wheel:
        case QEvent::DragEnter:
        case QEvent::DragMove:
        case QEvent::DragLeave:
        case QEvent::Drop:
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::HoverMove:

            if (glass->eventFilter(this, e)) {
                return true;    
            }
            break;
        default: break;
        }
    }
    // else 
    return QGraphicsView::viewportEvent(e);

}

}//namespace
