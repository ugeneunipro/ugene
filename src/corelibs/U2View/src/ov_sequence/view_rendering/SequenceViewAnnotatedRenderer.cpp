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

#include "SequenceViewAnnotatedRenderer.h"

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/SequenceObjectContext.h>


namespace U2 {

SequenceViewAnnotatedRenderer::CutSiteDrawData::CutSiteDrawData()
    : direct(true),
      pos(0) {

}

SequenceViewAnnotatedRenderer::AnnotationViewMetrics::AnnotationViewMetrics()
    : afNormal(QFont("Courier", 10)),
      afSmall(QFont("Arial", 8)),
      afmNormal(QFontMetrics(afNormal)),
      afmSmall(QFontMetrics(afSmall)) {
    afNormalCharWidth  = afmNormal.width('w');
    afSmallCharWidth   = afmSmall.width('w');

    QLinearGradient gradient(0, 0, 0, 1); //vertical
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.00, QColor(255, 255, 255, 120));
    gradient.setColorAt(0.50, QColor( 0,   0,   0,   0));
    gradient.setColorAt(0.70, QColor( 0,   0,   0,   0));
    gradient.setColorAt(1.00, QColor( 0,   0,   0,  70));
    gradientMaskBrush = QBrush(gradient);

}

SequenceViewAnnotatedRenderer::AnnotationDisplaySettings::AnnotationDisplaySettings()
    : displayAnnotationNames(true),
      displayAnnotationArrows(true),
      displayCutSites(true) {
}

/************************************************************************/
/* SequenceViewAnnotatedRenderer */
/************************************************************************/
const int SequenceViewAnnotatedRenderer::MIN_ANNOTATION_WIDTH = 3;
const int SequenceViewAnnotatedRenderer::MIN_ANNOTATION_TEXT_WIDTH = 5;
const int SequenceViewAnnotatedRenderer::MIN_SELECTED_ANNOTATION_WIDTH = 4;
const int SequenceViewAnnotatedRenderer::MIN_WIDTH_TO_DRAW_EXTRA_FEATURES = 10;

const int SequenceViewAnnotatedRenderer::FEATURE_ARROW_HLEN = 3;
const int SequenceViewAnnotatedRenderer::FEATURE_ARROW_VLEN = 3;

const int SequenceViewAnnotatedRenderer::CUT_SITE_HALF_WIDTH = 4;
const int SequenceViewAnnotatedRenderer::CUT_SITE_HALF_HEIGHT = 2;

const int SequenceViewAnnotatedRenderer::MAX_VIRTUAL_RANGE = 10000;

SequenceViewAnnotatedRenderer::SequenceViewAnnotatedRenderer(SequenceObjectContext* ctx)
    : SequenceViewRenderer(ctx) {

}

void SequenceViewAnnotatedRenderer::drawAnnotations(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange, const AnnotationDisplaySettings& displaySettings) {
    foreach (const AnnotationTableObject *ao, ctx->getAnnotationObjects(true)) {
        QList<Annotation *> annotations = ao->getAnnotationsByRegion(visibleRange);
        QList<Annotation *> restrictionSites = ao->getAnnotationsByType(U2FeatureTypes::RestrictionSite);
        foreach(Annotation *a, restrictionSites) {
            CHECK_CONTINUE(!annotations.contains(a));
            annotations.append(a);
        }
        foreach(Annotation *a, annotations) {
            drawAnnotation(p, canvasSize, visibleRange, a, displaySettings);
        }
    }
}

void SequenceViewAnnotatedRenderer::drawAnnotationSelection(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange, const AnnotationDisplaySettings& displaySettings) {
    const AnnotationSelection *annSelection = ctx->getAnnotationsSelection();
    foreach (Annotation* annotation, annSelection->getAnnotations()) {
        AnnotationTableObject *o = annotation->getGObject();
        if (ctx->getAnnotationObjects(true).contains(o)) {
            drawAnnotation(p, canvasSize, visibleRange, annotation, displaySettings, U2Region(), true);
        }
    }
}

void SequenceViewAnnotatedRenderer::drawAnnotation(QPainter &p, const QSize& canvasSize, const U2Region &visibleRange,
                                                   Annotation *a, const AnnotationDisplaySettings& displaySettings,
                                                   const U2Region& predefinedY,
                                                   bool selected, const AnnotationSettings *as) {
    const SharedAnnotationData &aData = a->getData();
    if (as == NULL) {
        AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
        as = asr->getAnnotationSettings(aData);
    }
    CHECK(as->visible, );

    QPen borderPen(Qt::SolidLine);
    borderPen.setWidth(selected ? 2 : 1);

    const bool isRestrictionSite = a->getType() == U2FeatureTypes::RestrictionSite;
    QVector<U2Region> location = aData->getRegions();
    bool simple = location.size() == 1;
    for (int ri = 0, ln = location.size(); ri < ln; ri++) {
        const U2Region &r = location.at(ri);
        if (r.intersects(visibleRange)) {
            const U2Region visibleLocation = r.intersect(visibleRange);
            const U2Region y = predefinedY.isEmpty() ? getAnnotationYRange(a, ri, as, canvasSize, visibleRange) : predefinedY;
            if (y.startPos < 0) {
                continue;
            }

            const int x1 = posToXCoord(visibleLocation.startPos, canvasSize, visibleRange);
            const int x2 = posToXCoord(visibleLocation.endPos(), canvasSize, visibleRange);

            const int rw = qMax(selected ? MIN_SELECTED_ANNOTATION_WIDTH : MIN_ANNOTATION_WIDTH, x2 - x1);
            SAFE_POINT(rw > 0, "Negative length of annotationYRange", );

            // annotation rect setting
            const QRect annotationRect(x1, y.startPos, rw, y.length);
            QPainterPath rectPath;
            rectPath.addRect(x1, y.startPos, rw, y.length);
            // find out if the arrow is needed
            const bool leftTrim = visibleLocation.startPos != r.startPos;
            const bool rightTrim = visibleLocation.endPos() != r.endPos();
            const bool drawArrow = aData->getStrand().isCompementary() ? !leftTrim : !rightTrim;
            if (displaySettings.displayAnnotationArrows && drawArrow) {
                bool isLeft = false;
                if (1 == ri && aData->findFirstQualifierValue("rpt_type") == "inverted") { //temporary solution for drawing inverted repeats correct
                    isLeft = true;
                } else {
                    isLeft = aData->getStrand().isCompementary();
                }
                addArrowPath(rectPath, annotationRect, isLeft);
            }

            rectPath.setFillRule(Qt::WindingFill);
            p.fillPath(rectPath, as->color);
            p.fillPath(rectPath, annMetrics.gradientMaskBrush);

            p.setPen(borderPen);
            if (rw > MIN_ANNOTATION_WIDTH) {
                p.drawPath(rectPath);
                if (displaySettings.displayAnnotationNames && annotationRect.width() >= MIN_ANNOTATION_TEXT_WIDTH) {
                    const QString aText = prepareAnnotationText(aData, as);
                    drawBoundedText(p, annotationRect, aText);
                }
                if (simple &&
                    annotationRect.width() > MIN_WIDTH_TO_DRAW_EXTRA_FEATURES &&
                    displaySettings.displayCutSites) {
                    drawCutSite(p, aData, r, annotationRect, as->color, canvasSize, visibleRange);
                }
            }
            drawAnnotationConnections(p, a, as, displaySettings, canvasSize, visibleRange);
        } else if (isRestrictionSite && simple && displaySettings.displayCutSites) {
            p.setPen(borderPen);
            drawCutSite(p, aData, r, QRect(), as->color, canvasSize, visibleRange);
        }
    }
}

void SequenceViewAnnotatedRenderer::drawBoundedText(QPainter &p, const QRect &r, const QString &text) {
    if (annMetrics.afSmallCharWidth > r.width()) {
        return;
    }
    QFont font = annMetrics.afNormal;
    QFontMetrics fm = annMetrics.afmNormal;
    if (fm.width(text) > r.width()) {
        font = annMetrics.afSmall;
        fm = annMetrics.afmSmall;
    }
    p.setFont(font);

    const int len = text.length();
    int textWidth = 0;
    int prefixLen = 0;
    do {
        int cw = fm.width(text[prefixLen]);
        if (textWidth + cw > r.width()) {
            break;
        }
        textWidth += cw;
    } while (++prefixLen < len);

    if (0 == prefixLen) {
        return;
    }
    p.drawText(r, Qt::TextSingleLine | Qt::AlignCenter, text.left(prefixLen));
}

void SequenceViewAnnotatedRenderer::drawAnnotationConnections(QPainter &p, Annotation *a, const AnnotationSettings *as, const AnnotationDisplaySettings& displaySettings,
                                                              const QSize& canvasSize, const U2Region& visibleRange) {
    const SharedAnnotationData &aData = a->getData();
    CHECK(!aData->location->isSingleRegion(), );

    const qint64 seqLength = ctx->getSequenceLength();
    U2Region sRange(0, seqLength);
    if (U1AnnotationUtils::isSplitted(aData->location, sRange)) {
        return;
    }

    int dx1 = 0;
    int dx2 = 0;
    if (displaySettings.displayAnnotationArrows) {
        if (aData->getStrand().isCompementary()) {
            dx2 = - FEATURE_ARROW_HLEN;
        } else {
            dx1 = FEATURE_ARROW_HLEN;
        }
    }
    QVector<U2Region> location = aData->getRegions();
    for (int ri = 0, ln = location.size(); ri < ln; ri++) {
        const U2Region &r = location.at(ri);
        if (ri > 0) {
            U2Region prev = location.at(ri - 1);
            const int prevPos = prev.endPos();
            const int pos = r.startPos;
            const int min = qMin(prevPos, pos);
            const int max = qMax(prevPos, pos);
            if (visibleRange.intersects(U2Region(min, max - min))) {
                int x1 = posToXCoord(prevPos, canvasSize, visibleRange) + dx1;
                if (x1 == -1 && prevPos > visibleRange.endPos()) {
                    x1 = posToXCoord(visibleRange.endPos() - 1, canvasSize, visibleRange) + dx1;
                }
                int x2 = posToXCoord(pos, canvasSize, visibleRange) + dx2;
                if (x2 == -1 && pos > visibleRange.endPos()) {
                    x2 = posToXCoord(visibleRange.endPos() - 1, canvasSize, visibleRange) + dx2;
                }
                if (qAbs(x2 - x1) > 1) {
                    x1 = qBound( - MAX_VIRTUAL_RANGE, x1, MAX_VIRTUAL_RANGE); //qt4.4 crashes in line clipping alg for extremely large X values
                    x2 = qBound( - MAX_VIRTUAL_RANGE, x2, MAX_VIRTUAL_RANGE);
                    const int midX = (x1 + x2) / 2;
                    const U2Region pyr = getAnnotationYRange(a, ri - 1, as, canvasSize, visibleRange);
                    const U2Region yr = getAnnotationYRange(a, ri, as, canvasSize, visibleRange);
                    const int y1 = pyr.startPos;
                    const int dy1 = pyr.length / 2;
                    const int y2 = yr.startPos;
                    const int dy2 = yr.length / 2;
                    const int midY = qMin(y1, y2);
                    p.drawLine(x1, y1 + dy1, midX, midY);
                    p.drawLine(midX, midY, x2, y2 + dy2);
                }
            }
        }
    }
}

void SequenceViewAnnotatedRenderer::drawCutSite(QPainter &p, const SharedAnnotationData &aData, const U2Region& r,
                                                const QRect& annotationRect, const QColor& color,
                                                const QSize& canvasSize, const U2Region& visibleRange) {
    const QString cutStr = aData->findFirstQualifierValue(GBFeatureUtils::QUALIFIER_CUT);
    bool hasD = false;
    bool hasC = false;
    int cutD = 0;
    int cutC = 0;
    if (!cutStr.isEmpty()) {
        int complSplit = cutStr.indexOf('/');
        if (-1 != complSplit) {
            cutD = cutStr.left(complSplit).toInt(&hasD);
            cutC = cutStr.mid(qMin(cutStr.length(), complSplit + 1))
                    .toInt(&hasC);
        } else {
            cutD = cutStr.toInt(&hasD);
            cutC = cutD;
            hasC = hasD;
        }
    }

    bool isDirectStrand = aData->getStrand().isDirect();
    U2Region cutSiteY = isDirectStrand ? getMirroredYRange(U2Strand(U2Strand::Complementary))
                                       : getMirroredYRange(U2Strand(U2Strand::Direct));
    QRect mirroredAnnotationRect = annotationRect;
    mirroredAnnotationRect.setY(cutSiteY.startPos);
    mirroredAnnotationRect.setHeight(cutSiteY.length);

    CutSiteDrawData toInsert;
    toInsert.color = color;
    if (hasD) {
        toInsert.direct = true;
        toInsert.pos = correctCutPos(isDirectStrand ? r.startPos + cutD : r.startPos + cutC);
        toInsert.r = isDirectStrand ? annotationRect : mirroredAnnotationRect;
        drawCutSite(p, toInsert, canvasSize, visibleRange);
    }
    if (hasC) {
        toInsert.direct = false;
        toInsert.pos = correctCutPos(isDirectStrand ? r.endPos() - cutC : r.endPos() - cutD);
        toInsert.r = !isDirectStrand ? annotationRect : mirroredAnnotationRect;
        drawCutSite(p, toInsert, canvasSize, visibleRange);
    }

}

void SequenceViewAnnotatedRenderer::drawCutSite(QPainter& p, const CutSiteDrawData& cData, const QSize& canvasSize, const U2Region& visibleRange) {
    int xCenter = posToXCoord(cData.pos, canvasSize, visibleRange);
    CHECK(xCenter != -1, );

    int xLeft = xCenter - CUT_SITE_HALF_WIDTH;
    int xRight= xCenter + CUT_SITE_HALF_WIDTH;
    int yFlat = (cData.direct ? cData.r.top() - CUT_SITE_HALF_HEIGHT : cData.r.bottom() + CUT_SITE_HALF_HEIGHT);
    int yPeak = (cData.direct ? cData.r.top() + CUT_SITE_HALF_HEIGHT : cData.r.bottom() - CUT_SITE_HALF_HEIGHT);

    QPolygon triangle;
    triangle << QPoint(xLeft, yFlat) << QPoint(xCenter, yPeak) << QPoint(xRight, yFlat) << QPoint(xLeft, yFlat);

    QPainterPath path;
    path.addPolygon(triangle);

    p.fillPath(path, cData.color);
    p.drawPath(path);
}

QString SequenceViewAnnotatedRenderer::prepareAnnotationText(const SharedAnnotationData &a, const AnnotationSettings *as) const {
    if (!as->showNameQuals || as->nameQuals.isEmpty()) {
        return a->name;
    }
    QVector<U2Qualifier> qs;
    foreach (const QString &qn, as->nameQuals) {
        qs.clear();
        a->findQualifiers(qn, qs);
        if (!qs.isEmpty()) {
            QString res = qs[0].value;
            return res;
        }
    }
    return a->name;
}

void SequenceViewAnnotatedRenderer::addArrowPath(QPainterPath& path, const QRect& rect, bool leftArrow) const {
    if (rect.width() <= FEATURE_ARROW_HLEN || rect.height() <= 0) {
        return;
    }
    int x = leftArrow ? rect.left() : rect.right();
    int dx = leftArrow ? -FEATURE_ARROW_HLEN : FEATURE_ARROW_HLEN;

    QPolygon arr;
    arr << QPoint(x - dx, rect.top()    - FEATURE_ARROW_VLEN);
    arr << QPoint(x + dx, rect.top()    + rect.height() / 2);
    arr << QPoint(x - dx, rect.bottom() + FEATURE_ARROW_VLEN);
    arr << QPoint(x - dx, rect.top()    - FEATURE_ARROW_VLEN);
    QPainterPath arrowPath;
    arrowPath.addPolygon(arr);

    QPainterPath dRectPath;
    dRectPath.addRect(leftArrow ? x : x - (FEATURE_ARROW_HLEN - 1), rect.top(), FEATURE_ARROW_HLEN, rect.height());

    path = path.subtracted(dRectPath);
    path = path.united(arrowPath);
}

qint64 SequenceViewAnnotatedRenderer::correctCutPos(const qint64 pos) const {
    const bool isCircular = ctx->getSequenceObject()->isCircular();
    const qint64 sequenceLength = ctx->getSequenceLength();
    qint64 result = pos;
    if (isCircular) {
        if (result < 0) {
            result = sequenceLength + result;
        } else if (result > sequenceLength) {
            result = result - sequenceLength;
        }
    } else {
        result = qBound((qint64)0, result, sequenceLength);
    }

    return result;
}

} // namespace
