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

                                            GSequenceLineViewAnnotated(QWidget *p, SequenceObjectContext *ctx);

    bool                                    isAnnotationVisible(Annotation *a) const;

    virtual QList<AnnotationSelectionData>  selectAnnotationByCoord(const QPoint &coord) const;

    static QString                          prepareAnnotationText(const SharedAnnotationData &a, const AnnotationSettings *as);

    QList<Annotation *>                     findAnnotationsInRange(const U2Region &range) const;

    bool                                    isAnnotationSelectionInVisibleRange() const;


protected:
    void                                    mousePressEvent(QMouseEvent *e);
    void                                    mouseDoubleClickEvent(QMouseEvent* me);

    virtual bool                            event(QEvent *e);
    virtual QString                         createToolTip(QHelpEvent *e);

    virtual void                            registerAnnotations(const QList<Annotation *> &l);
    virtual void                            unregisterAnnotations(const QList<Annotation *> &l);
    virtual void                            ensureVisible(Annotation *a, int locationIdx);

protected slots:
    virtual void                            sl_onAnnotationSettingsChanged(const QStringList &changedSettings);

    void                                    sl_onAnnotationObjectAdded(AnnotationTableObject *);
    void                                    sl_onAnnotationObjectRemoved(AnnotationTableObject *);
    void                                    sl_onAnnotationsInGroupRemoved(const QList<Annotation *> &, AnnotationGroup *);
    void                                    sl_onAnnotationsAdded(const QList<Annotation *> &);
    void                                    sl_onAnnotationsRemoved(const QList<Annotation *> &);
    virtual void                            sl_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications);
    virtual void                            sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &added,
                                                const QList<Annotation *> &removed);

private:
    void                                    connectAnnotationObject(const AnnotationTableObject *ao);

protected:
    friend class ClearAnnotationsTask;
};

class U2VIEW_EXPORT GSequenceLineViewAnnotatedRenderArea : public GSequenceLineViewRenderArea {
public:
    GSequenceLineViewAnnotatedRenderArea(GSequenceLineViewAnnotated *d);
    ~GSequenceLineViewAnnotatedRenderArea();

    //! VIEW_RENDERER_REFACTORING: only the second method should be available, because it is more common
    virtual U2Region                getAnnotationYRange(Annotation *a, int region, const AnnotationSettings *as) const = 0;
    virtual bool                    isPosOnAnnotationYRange(const QPoint& p, Annotation *a, int region, const AnnotationSettings* as) const;

    GSequenceLineViewAnnotated *    getGSequenceLineViewAnnotated() const;

protected:
    virtual void drawAll(QPaintDevice* pd) = 0;

    //! VIEW_RENDERER_REFACTORING: should be removed, currenlty is used in CircularView
    enum DrawAnnotationPass {
        DrawAnnotationPass_DrawFill,
        DrawAnnotationPass_DrawBorder
    };

    //! VIEW_RENDERER_REFACTORING: this parameters are also doubled in SequenceViewAnnotaterRenderer
    //af* == annotation font
    QFont *afNormal;
    QFont *afSmall;

    QFontMetrics *afmNormal;
    QFontMetrics *afmSmall;

    int afNormalCharWidth;
    int afSmallCharWidth;

    QBrush gradientMaskBrush;
};

class ClearAnnotationsTask : public Task {
public:
                                    ClearAnnotationsTask(const QList<Annotation *> &    list,
                                                          GSequenceLineViewAnnotated *  view);

    void                            run();
    Task::ReportResult              report();

private:
    QList<Annotation *>             l;
    GSequenceLineViewAnnotated *    view;
};

} // namespace U2

#endif
