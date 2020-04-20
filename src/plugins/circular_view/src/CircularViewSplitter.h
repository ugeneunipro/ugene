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

#ifndef _U2_CIRCULAR_VIEW_SPLITTER_H_
#define _U2_CIRCULAR_VIEW_SPLITTER_H_

#include "CircularView.h"
#include "CircularItems.h"

#include <U2View/ADVSplitWidget.h>

#include <QAction>


class QScrollBar;

namespace U2 {

class CircularViewHeaderWidget;
class RestrctionMapWidget;

class CircularViewSplitter : public ADVSplitWidget {
    Q_OBJECT
public:
    CircularViewSplitter( AnnotatedDNAView* view);
    // there are no special object handling with this view
    // it only shows existing AO only
    virtual bool acceptsGObject(GObject*) {return false;}
    virtual void updateState(const QVariantMap& m);
    virtual void saveState(QVariantMap& m);
    void addView(CircularView* view, RestrctionMapWidget* rmapWidget);
    void adaptSize();
    void removeView(CircularView* view, RestrctionMapWidget* rmapWidget);
    bool isEmpty();
    const QList<CircularView*>& getViewList() const { return circularViewList; }
    void updateViews();
protected slots:
    void sl_export();
    void sl_horSliderMoved(int);
    void sl_moveSlider(int);

    void sl_updateZoomInAction(bool);
    void sl_updateZoomOutAction(bool);
    void sl_updateFitInViewAction(bool);
    void sl_toggleRestrictionMap(bool);
private:
    QSplitter*          splitter;
    QScrollBar*         horScroll;

    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* fitInViewAction;
    QAction* exportAction;
    QAction* toggleRestrictionMapAction;

    QList<CircularView*> circularViewList;
    QList<RestrctionMapWidget*> restrictionMapWidgets;
};

} //namespace U2

#endif //_U2_CIRCULAR_VIEW_SPLITTER_H_
