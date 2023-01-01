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

#ifndef _U2_SEQUENCE_VIEW_ANNOTATED_RENDERER_H_
#define _U2_SEQUENCE_VIEW_ANNOTATED_RENDERER_H_

#include <QPainter>

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationSelection.h>

#include "SequenceViewRenderer.h"

namespace U2 {

class AnnotationSettings;

/**
 * SequenceViewRenderer with annotations rendering support.
 *
 * TODO: this class is stateful and contains some layout information (visible det-view lines, fonts),
 *  but most of its methods accept other partial state data like canvasSize, visibleRange, etc.
 *  We should cache/update other layout state related fields like 'canvasSize', 'visibleRange' inside the renderer and simplify method signatures
 *  In this case a good idea could be is to create a Renderer only for a time-range of re-paint (like a QPainter)
 *  Or, as an alternative, we can make the renderer 100% stateless and have a layout context with as a single parameter into all methods.
 */
class SequenceViewAnnotatedRenderer : public SequenceViewRenderer {
protected:
    struct CutSiteDrawData {
        CutSiteDrawData();

        QRect r;
        QColor color;
        bool direct;
        int pos;
    };

    struct AnnotationViewMetrics {
        AnnotationViewMetrics();

        QFont afNormal;
        QFont afSmall;

        QFontMetrics afmNormal;
        QFontMetrics afmSmall;

        int afNormalCharWidth;
        int afSmallCharWidth;

        QBrush gradientMaskBrush;
    };

    struct AnnotationDisplaySettings {
        AnnotationDisplaySettings();

        bool displayAnnotationNames;
        bool displayAnnotationArrows;
        bool displayCutSites;
    };

public:
    SequenceViewAnnotatedRenderer(SequenceObjectContext* ctx);

    virtual qint64 coordToPos(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const = 0;

    virtual double getCurrentScale() const = 0;

    /** Returns Y range of the cut-site for an annotation on the given strand. */
    virtual U2Region getCutSiteYRange(const U2Strand& mStrand, int availableHeight) const = 0;

    virtual int getMinimumHeight() const = 0;

    virtual void drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) = 0;
    virtual void drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) = 0;

    virtual void drawAnnotations(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, const AnnotationDisplaySettings& displaySettings);
    virtual void drawAnnotationSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, const AnnotationDisplaySettings& displaySettings);

protected:
    virtual void drawAnnotation(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, Annotation* a, const AnnotationDisplaySettings& displaySettings, bool selected = false, const AnnotationSettings* as = nullptr);

    /**
     * Returns visible Y range of the annotation region.
     *
     * The 'availableHeight' parameter may be used to allow the view to center inner content if extra space is available
     *  (see non-translation mode in single-line DetView).
     *
     * TODO: there is a design flow with this method: multi-line views may have multiple y-regions per single annotation region.
     *  When GSequenceLineViewGridAnnotationRenderArea will be able to return QRects instead of Regions this annotation rendering logic can be re-written
     *  and this method is removed.
     */
    virtual U2Region getAnnotationYRange(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, int availableHeight) const = 0;

    void drawBoundedText(QPainter& p, const QRect& r, const QString& text);

    void drawAnnotationConnections(QPainter& p, Annotation* a, const AnnotationSettings* as, const AnnotationDisplaySettings& drawSettings, const QSize& canvasSize, const U2Region& visibleRange);

    void drawCutSite(QPainter& p, const SharedAnnotationData& aData, const U2Region& r, const QRect& annotationRect, const QColor& color, const QSize& canvasSize, const U2Region& visibleRange);
    void drawCutSite(QPainter& p, const CutSiteDrawData& cData, const QSize& canvasSize, const U2Region& visibleRange);

    QString prepareAnnotationText(const SharedAnnotationData& a, const AnnotationSettings* as) const;

    void addArrowPath(QPainterPath& path, const QRect& rect, bool leftArrow) const;

    qint64 correctCutPos(qint64 pos) const;

    AnnotationViewMetrics annMetrics;

    static const int MIN_ANNOTATION_WIDTH;
    static const int MIN_ANNOTATION_TEXT_WIDTH;
    static const int MIN_SELECTED_ANNOTATION_WIDTH;
    static const int MIN_WIDTH_TO_DRAW_EXTRA_FEATURES;

    static const int FEATURE_ARROW_HLEN;
    static const int FEATURE_ARROW_VLEN;

    static const int CUT_SITE_HALF_WIDTH;
    static const int CUT_SITE_HALF_HEIGHT;

    static const int MAX_VIRTUAL_RANGE;
};

}  // namespace U2

#endif  // _U2_SEQUENCE_VIEW_ANNOTATED_RENDERER_H_
