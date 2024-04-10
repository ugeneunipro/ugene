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

#pragma once

#include <QToolButton>

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>

#include "ADVSequenceObjectContext.h"
#include "GSequenceLineView.h"

namespace U2 {

class PanView;
class DetView;
class OverviewRenderArea;
class AnnotationModification;
class AnnotationTableObject;
class ADVSingleSequenceWidget;

class U2VIEW_EXPORT Overview : public GSequenceLineView {
    Q_OBJECT
public:
    Overview(ADVSingleSequenceWidget* p, ADVSequenceObjectContext* ctx);

protected slots:
    void sl_visibleRangeChanged();
    void sl_graphActionTriggered();
    void sl_annotationsAdded(const QList<Annotation*>& a);
    void sl_annotationsRemoved(const QList<Annotation*>& a);
    void sl_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*);
    void sl_annotationsModified(const QList<AnnotationModification>& annotationModifications);
    void sl_onAnnotationSettingsChanged(const QStringList& changedSettings);
    void sl_annotationObjectAdded(AnnotationTableObject* obj);
    void sl_annotationObjectRemoved(AnnotationTableObject* obj);
    void sl_sequenceChanged() override;

protected:
    void pack() override;
    bool event(QEvent* e) override;
    void mousePressEvent(QMouseEvent* me) override;
    void mouseMoveEvent(QMouseEvent* me) override;
    void mouseDoubleClickEvent(QMouseEvent* me) override;
    void mouseReleaseEvent(QMouseEvent* me) override;
    void wheelEvent(QWheelEvent* we) override;

    /** Creates a tool tip string for the given 'X' offset in the render area. */
    QString createToolTip(int renderAreaXOffset);
    PanView* getPan() const;
    DetView* getDet() const;

    bool panSliderClicked;
    bool detSliderClicked;
    bool panSliderMovedRight;
    bool panSliderMovedLeft;

    qint64 offset;

private:
    void connectAnnotationTableObject(AnnotationTableObject* object);
    void setGraphActionVisible(const bool setVisible);

    PanView* panView;
    DetView* detView;
    QPoint mousePosToSlider;
    ADVSingleSequenceWidget* seqWidget;

    /** A renderArea from the base class with a correct type. Used to avoid casts in the code. */
    OverviewRenderArea* overviewRenderArea;

    static const QString ANNOTATION_GRAPH_STATE;

    friend class OverviewRenderArea;
};

class OverviewRenderArea : public GSequenceLineViewRenderArea {
    Q_OBJECT
public:
    OverviewRenderArea(Overview* p);

    const QRectF getPanSlider() const;
    const QRectF getDetSlider() const;

    int getAnnotationDensity(int pos) const;

    void setGraphVisibility(const bool isVisible);
    bool isGraphVisible() const;

    /**
     * Returns sequence position corresponding to the given on-screen X coordinate.
     * OverviewRenderArea is a true single-line-view, so the result does not depend on Y coordinate.
     */
    qint64 coordXToPos(int coordX) const {
        return coordToPos(QPoint(coordX, 0));
    }

protected:
    void drawAll(QPaintDevice* pd) override;

private:
    void drawRuler(QPainter& p);
    void drawSelection(QPainter& p);
    void drawSlider(QPainter& p, QRectF rect, QColor col);
    void drawArrow(QPainter& p, QRectF rect, QColor col);
    void setAnnotationsOnPos();
    void drawGraph(QPainter& p);
    QColor getUnitColor(int count);

    QRectF panSlider;
    QRectF detSlider;
    QBrush gradientMaskBrush;
    QVector<int> annotationsOnPos;
    bool graphVisible;
};

}  // namespace U2
