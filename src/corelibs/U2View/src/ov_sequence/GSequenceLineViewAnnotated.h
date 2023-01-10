/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GSEQUENCE_LINE_VIEW_ANNOTATED_H_
#define _U2_GSEQUENCE_LINE_VIEW_ANNOTATED_H_

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/AnnotationSelection.h>
#include <U2Core/Task.h>

#include "GSequenceLineView.h"

namespace U2 {

class AnnotationTableObject;
class AnnotationSettings;
class AnnotationModification;
class ClearAnnotationsTask;

class U2VIEW_EXPORT GSequenceLineViewAnnotated : public GSequenceLineView {
    Q_OBJECT
public:
    GSequenceLineViewAnnotated(QWidget* p, SequenceObjectContext* ctx);

    bool isAnnotationVisible(const Annotation* a) const;

    /**
     * Returns list of annotations that hold the on-screen point.
     * The method is delegated to the renderArea. Override the renderArea's variant of the method when needed.
     */
    QList<Annotation*> findAnnotationsByCoord(const QPoint& renderAreaPoint) const;

    static QString prepareAnnotationText(const SharedAnnotationData& a, const AnnotationSettings* as);

    QList<Annotation*> findAnnotationsInRange(const U2Region& range) const;

    bool isAnnotationSelectionInVisibleRange() const;

protected:
    void mousePressEvent(QMouseEvent* e);

    void mouseDoubleClickEvent(QMouseEvent* me);

    virtual bool event(QEvent* e);

    /** Creates a tooltip for the given render area coordinate. */
    virtual QString createToolTip(const QPoint& renderAreaPoint);

    virtual void registerAnnotations(const QList<Annotation*>& l);

    virtual void unregisterAnnotations(const QList<Annotation*>& l);

    virtual void ensureVisible(Annotation* a, int locationIdx);

protected slots:

    virtual void sl_onAnnotationSettingsChanged(const QStringList& changedSettings);

    void sl_onAnnotationObjectAdded(AnnotationTableObject*);

    void sl_onAnnotationObjectRemoved(AnnotationTableObject*);

    void sl_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*);

    void sl_onAnnotationsAdded(const QList<Annotation*>&);

    void sl_onAnnotationsRemoved(const QList<Annotation*>&);

    virtual void sl_onAnnotationsModified(const QList<AnnotationModification>& annotationModifications);

    /* Updates view so annotation becomes visible. */
    virtual void sl_onAnnotationActivated(Annotation* annotation, int regionIndex);

    virtual void sl_onAnnotationSelectionChanged(AnnotationSelection*, const QList<Annotation*>& added, const QList<Annotation*>& removed);

private:
    void connectAnnotationObject(const AnnotationTableObject* ao);

protected:
    friend class ClearAnnotationsTask;
};

class U2VIEW_EXPORT GSequenceLineViewAnnotatedRenderArea : public GSequenceLineViewRenderArea {
    Q_OBJECT
public:
    GSequenceLineViewAnnotatedRenderArea(GSequenceLineViewAnnotated* sequenceLineViewAnnotated);
    ~GSequenceLineViewAnnotatedRenderArea();

    /** Returns all annotations by a coordinate inside render area. */
    virtual QList<Annotation*> findAnnotationsByCoord(const QPoint& coord) const = 0;

protected:
    virtual void drawAll(QPaintDevice* pd) = 0;

    GSequenceLineViewAnnotated* const sequenceLineViewAnnotated;

    //! VIEW_RENDERER_REFACTORING: this parameters are also doubled in SequenceViewAnnotaterRenderer
    // af* == annotation font
    QFont* afNormal;
    QFont* afSmall;

    QFontMetrics* afmNormal;
    QFontMetrics* afmSmall;

    int afNormalCharWidth;
    int afSmallCharWidth;

    QBrush gradientMaskBrush;
};

/**
 * Common base class for all grid-like annotation areas: PanView, DetView.
 * Note: the alternative to grid is a circular render area (see CircularView).
 */
class U2VIEW_EXPORT GSequenceLineViewGridAnnotationRenderArea : public GSequenceLineViewAnnotatedRenderArea {
    Q_OBJECT
public:
    GSequenceLineViewGridAnnotationRenderArea(GSequenceLineViewAnnotated* sequenceLineView);

    /** Returns all annotations by a coordinate inside render area. */
    QList<Annotation*> findAnnotationsByCoord(const QPoint& coord) const override;

    /** Returns true if the given annotation region contains 'y' point. */
    bool checkAnnotationRegionContainsYPoint(int y, Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings) const;

    /**
     * Returns all y-regions covered by the annotation location region.
     * TODO: rework to return list of QRects (may require a bigger refactoring).
     */
    virtual QList<U2Region> getAnnotationYRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings) const = 0;
};

class ClearAnnotationsTask : public Task {
public:
    ClearAnnotationsTask(const QList<Annotation*>& list,
                         GSequenceLineViewAnnotated* view);

    void run();
    Task::ReportResult report();

private:
    QList<Annotation*> l;
    GSequenceLineViewAnnotated* view;
};

}  // namespace U2

#endif
