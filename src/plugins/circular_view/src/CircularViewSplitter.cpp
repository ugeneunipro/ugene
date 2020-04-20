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

#include <QApplication>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QScrollArea>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/OrderedToolbar.h>
#include <U2Gui/WidgetWithLocalToolbar.h>

#include "CircularView.h"
#include "CircularViewImageExportTask.h"
#include "CircularViewSplitter.h"
#include "RestrictionMapWidget.h"

namespace U2 {

CircularViewSplitter::CircularViewSplitter(AnnotatedDNAView* view)
    : ADVSplitWidget(view)
{
    zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setIcon(QIcon(":/core/images/zoom_in.png"));
    zoomInAction->setToolTip(tr("Zoom In"));
    zoomInAction->setObjectName("tbZoomIn_" + view->getName());

    zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setIcon(QIcon(":/core/images/zoom_out.png"));
    zoomOutAction->setToolTip(tr("Zoom Out"));

    fitInViewAction = new QAction(tr("Fit To Full View"), this);
    fitInViewAction->setIcon(QIcon(":/core/images/zoom_whole.png"));
    fitInViewAction->setToolTip(tr("Fit To Full View"));

    exportAction = new QAction(tr("Save circular view as image"), this);
    exportAction->setIcon(QIcon(":/core/images/cam2.png"));
    exportAction->setToolTip(tr("Save circular view as image"));

    toggleRestrictionMapAction = new QAction(tr("Show/hide restriction sites map"), this);
    toggleRestrictionMapAction->setIcon(QIcon(":/circular_view/images/side_list.png"));
    toggleRestrictionMapAction->setToolTip(tr("Show/hide restriction sites map"));
    toggleRestrictionMapAction->setCheckable(true);
    toggleRestrictionMapAction->setChecked(true);
    connect(toggleRestrictionMapAction, SIGNAL(triggered(bool)),SLOT(sl_toggleRestrictionMap(bool)));

    connect(exportAction, SIGNAL(triggered()), SLOT(sl_export()));

    splitter = new QSplitter(Qt::Horizontal);

    WidgetWithLocalToolbar* widgetWithToolBar = new WidgetWithLocalToolbar(this);
    widgetWithToolBar->setLocalToolBarObjectName("circular_view_local_toolbar");
    widgetWithToolBar->addActionToLocalToolbar(zoomInAction);
    widgetWithToolBar->addActionToLocalToolbar(zoomOutAction);
    widgetWithToolBar->addActionToLocalToolbar(fitInViewAction);
    widgetWithToolBar->addActionToLocalToolbar(exportAction);
    widgetWithToolBar->addActionToLocalToolbar(toggleRestrictionMapAction);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(splitter);
    widgetWithToolBar->setContentLayout(layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setBaseSize(600,600);
    setAcceptDrops(false);

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setSpacing(0);
    outerLayout->setContentsMargins(0,0,0,0);

    horScroll = new QScrollBar(Qt::Horizontal, this);
    horScroll->setMinimum(0);
    horScroll->setMaximum(360);
    horScroll->setSingleStep(5);
    connect(horScroll, SIGNAL(valueChanged(int)), SLOT(sl_horSliderMoved(int)));

    outerLayout->addWidget(widgetWithToolBar);
    outerLayout->insertWidget(-1, horScroll);
}

void CircularViewSplitter::updateState( const QVariantMap& m) {
    Q_UNUSED(m);
    //TODO:
}

void CircularViewSplitter::saveState( QVariantMap& m ) {
    Q_UNUSED(m);
    //TODO:
}

void CircularViewSplitter::addView(CircularView* view, RestrctionMapWidget* rmapWidget) {
    fitInViewAction->setDisabled(true);
    connect(zoomInAction, SIGNAL(triggered()), view, SLOT(sl_zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered()), view, SLOT(sl_zoomOut()));
    connect(fitInViewAction, SIGNAL(triggered()), view, SLOT(sl_fitInView()));

    connect(view, SIGNAL(si_zoomInDisabled(bool)), SLOT(sl_updateZoomInAction(bool)));
    connect(view, SIGNAL(si_zoomOutDisabled(bool)), SLOT(sl_updateZoomOutAction(bool)));
    connect(view, SIGNAL(si_fitInViewDisabled(bool)), SLOT(sl_updateFitInViewAction(bool)));

    circularViewList.append(view);
    restrictionMapWidgets.append(rmapWidget);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(view);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->addWidget(scrollArea);
    splitter->addWidget(rmapWidget);

    splitter->setStretchFactor(splitter->indexOf(scrollArea), 10);
    splitter->setStretchFactor(splitter->indexOf(rmapWidget), 1);

    connect(view, SIGNAL(si_wheelMoved(int)), SLOT(sl_moveSlider(int)));
}

void CircularViewSplitter::sl_moveSlider(int delta) {
    delta*=-1;
    int oldPos = horScroll->sliderPosition();
    int step = qMin(QApplication::wheelScrollLines() * horScroll->singleStep(), horScroll->pageStep());
    int offset = delta/120 * step;
    if (qAbs(offset)<1) {
        return;
    }
    int newPos = oldPos + offset;
    horScroll->setSliderPosition(newPos);
}

void CircularViewSplitter::removeView(CircularView* view, RestrctionMapWidget* rmapWidget) {
    SAFE_POINT( view != NULL, tr("Circular View is NULL"), );
    QWidget* viewport = view->parentWidget();
    SAFE_POINT( viewport != NULL, tr("Circular View viewport is NULL"), );
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(viewport->parentWidget());
    SAFE_POINT( scrollArea != NULL, tr("Scroll area is NULL"), );
    view->setParent(NULL);
    delete scrollArea;

    circularViewList.removeAll(view);
    restrictionMapWidgets.removeAll(rmapWidget);
}

bool CircularViewSplitter::isEmpty() {
    return circularViewList.isEmpty();
}

bool noValidExtension(const QString& url) {
    QFileInfo fi(url);
    if (fi.suffix().isEmpty()) {
        return true;
    }

    QStringList validExtensions;
    validExtensions << "png" << "bmp" << "jpg" << "jpeg" << "ppm" <<
        "xbm" << "xpm" << "svg" << "pdf" << "ps";

    if (!validExtensions.contains(fi.suffix())) {
        return true;
    }

    return false;
}

void CircularViewSplitter::updateViews() {
    foreach(CircularView* cv, circularViewList) {
        cv->redraw();
    }
}

void CircularViewSplitter::sl_export() {
    CircularView* cvInFocus = NULL;
    foreach(CircularView* cv, circularViewList) {
        if (cv->hasFocus()) {
            cvInFocus = cv;
            break;
        }
    }
    if (cvInFocus == NULL) {
        cvInFocus = circularViewList.last();
    }

    SAFE_POINT(cvInFocus->getSequenceContext() != NULL, tr("Sequence context is NULL"), );
    U2SequenceObject* seqObj = cvInFocus->getSequenceContext()->getSequenceObject();
    SAFE_POINT(seqObj != NULL, tr("Sequence obejct is NULL"), );

    CircularViewImageExportController factory(circularViewList, cvInFocus);

    QString fileName = GUrlUtils::fixFileName(seqObj->getSequenceName());
    QWidget *p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(&factory, ExportImageDialog::CircularView,
                                                                      fileName, ExportImageDialog::SupportScaling, p);
    dialog->exec();
    CHECK(!dialog.isNull(), );
}

void CircularViewSplitter::sl_horSliderMoved(int newVal) {
    foreach(CircularView* cv, circularViewList) {
        cv->setAngle(newVal);
    }
}

void CircularViewSplitter::adaptSize() {

    QWidget* widget = parentWidget();

    Q_ASSERT(widget != NULL);
    QSplitter* parentSplitter = qobject_cast<QSplitter* > (widget);

    int index = parentSplitter->indexOf(this);
    QList<int> sizes = parentSplitter->sizes();

    int splitterSize = 0;

    int psH = parentSplitter->height();
    int psW = parentSplitter->width();

    if (parentSplitter->orientation() == Qt::Horizontal) {
        splitterSize = psH;
    }
    else {
        splitterSize = psW;
    }

    if (!splitterSize) {
        return;
    }

    int midSize = splitterSize;

    if (splitterSize > CircularViewRenderArea::MIDDLE_ELLIPSE_SIZE) {
        sizes[index] = CircularViewRenderArea::MIDDLE_ELLIPSE_SIZE;

        midSize -= CircularViewRenderArea::MIDDLE_ELLIPSE_SIZE;
        if (sizes.count() > 1) {
            midSize /= (sizes.count() - 1);
        }
    }
    else {
        midSize /= sizes.count();
        sizes[index] = midSize;
    }

    for (int i=0; i<sizes.count(); i++) {
        if (i != index) {
            sizes[i] = midSize;
        }
    }
    parentSplitter->setSizes(sizes);
}

void CircularViewSplitter::sl_updateZoomInAction( bool disabled) {
    zoomInAction->setDisabled(disabled);
}

void CircularViewSplitter::sl_updateZoomOutAction( bool disabled) {
    zoomOutAction->setDisabled(disabled);
}

void CircularViewSplitter::sl_updateFitInViewAction( bool disabled) {
    fitInViewAction->setDisabled(disabled);
}

void CircularViewSplitter::sl_toggleRestrictionMap( bool toggle)
{
    foreach (QWidget* w, restrictionMapWidgets) {
        w->setVisible(toggle);
    }
}

} //namespace U2
