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

#include "SequenceViewAnnotatedRenderer.h"

#include <QPainterPath>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/Timer.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>

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
    afNormalCharWidth = afmNormal.width('w');
    afSmallCharWidth = afmSmall.width('w');

    QLinearGradient gradient(0, 0, 0, 1);  // vertical
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.00, QColor(255, 255, 255, 120));
    gradient.setColorAt(0.50, QColor(0, 0, 0, 0));
    gradient.setColorAt(0.70, QColor(0, 0, 0, 0));
    gradient.setColorAt(1.00, QColor(0, 0, 0, 70));
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

// Maximum reasonable distance of cut-site from the annotation to draw it in UGENE (det-view)
// in the meaningful (related to the annotation) way.

#define MAX_CUTSITE_DISTANCE 100

void SequenceViewAnnotatedRenderer::drawAnnotations(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, const AnnotationDisplaySettings& displaySettings) {
    GTIMER(c1, t1, "SequenceViewAnnotatedRenderer::drawAnnotations");
    U2Region annotationsRange = visibleRange;
    QList<U2Region> extraAnnotationRegions;
    qint64 sequenceLength = ctx->getSequenceLength();
    if (displaySettings.displayCutSites) {
        annotationsRange = U2Region(annotationsRange.startPos - MAX_CUTSITE_DISTANCE, annotationsRange.length + 2 * MAX_CUTSITE_DISTANCE);
        if (annotationsRange.startPos < 0) {
            qint64 tailLength = -annotationsRange.startPos;
            extraAnnotationRegions << U2Region(sequenceLength - tailLength, tailLength);
        }
        if (annotationsRange.endPos() > sequenceLength) {
            qint64 headLength = annotationsRange.endPos() - sequenceLength;
            extraAnnotationRegions << U2Region(0, headLength);
        }
    }
    QSet<AnnotationTableObject*> annotationObjectSet = ctx->getAnnotationObjects(true);
    for (const AnnotationTableObject* annotationObject : qAsConst(annotationObjectSet)) {
        const QList<Annotation*>& annotations = annotationObject->getAnnotations();
        for (Annotation* annotation : qAsConst(annotations)) {
            bool isVisible = annotationsRange.intersects(annotation->getRegions());
            for (int i = 0; i < extraAnnotationRegions.size() && !isVisible; i++) {
                isVisible = extraAnnotationRegions[i].intersects(annotation->getRegions());
            }
            if (isVisible) {
                drawAnnotation(p, canvasSize, visibleRange, annotation, displaySettings);
            }
        }
    }
}

void SequenceViewAnnotatedRenderer::drawAnnotationSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, const AnnotationDisplaySettings& displaySettings) {
    const AnnotationSelection* selection = ctx->getAnnotationsSelection();
    for (Annotation* annotation : qAsConst(selection->getAnnotations())) {
        AnnotationTableObject* o = annotation->getGObject();
        if (ctx->getAnnotationObjects(true).contains(o)) {
            drawAnnotation(p, canvasSize, visibleRange, annotation, displaySettings, true);
        }
    }
}

void SequenceViewAnnotatedRenderer::drawAnnotation(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, Annotation* a, const AnnotationDisplaySettings& displaySettings, bool selected, const AnnotationSettings* as) {
    const SharedAnnotationData& aData = a->getData();
    SAFE_POINT_NN(aData, )
    if (as == nullptr) {
        AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
        as = asr->getAnnotationSettings(aData);
    }
    CHECK(as->visible, );

    QPen borderPen(Qt::SolidLine);
    borderPen.setWidth(selected ? 2 : 1);

    const bool isRestrictionSite = a->getType() == U2FeatureTypes::RestrictionSite;
    QVector<U2Region> location = aData->getRegions();
    for (int ri = 0, ln = location.size(); ri < ln; ri++) {
        const U2Region& r = location.at(ri);
        if (r.intersects(visibleRange)) {
            const QRect annotationRect = getAnnotationRect(r, canvasSize, visibleRange, selected, a, as);
            CHECK_CONTINUE(!annotationRect.isEmpty());

            QPainterPath rectPath;
            rectPath.addRect(annotationRect.x(), annotationRect.y(), annotationRect.width(), annotationRect.height());
            // find out if the arrow is needed
            const U2Region visibleLocation = r.intersect(visibleRange);
            const bool leftTrim = visibleLocation.startPos != r.startPos;
            const bool rightTrim = visibleLocation.endPos() != r.endPos();
            const bool drawArrow = aData->getStrand().isComplementary() ? !leftTrim : !rightTrim;
            if (displaySettings.displayAnnotationArrows && drawArrow) {
                bool isLeft = false;
                if (1 == ri && aData->findFirstQualifierValue("rpt_type") == "inverted") {  // temporary solution for drawing inverted repeats correct
                    isLeft = true;
                } else {
                    isLeft = aData->getStrand().isComplementary();
                }
                addArrowPath(rectPath, annotationRect, isLeft);
            }

            rectPath.setFillRule(Qt::WindingFill);
            p.fillPath(rectPath, as->color);
            p.fillPath(rectPath, annMetrics.gradientMaskBrush);

            p.setPen(borderPen);
            if (annotationRect.width() > MIN_ANNOTATION_WIDTH) {
                p.drawPath(rectPath);
                if (displaySettings.displayAnnotationNames && annotationRect.width() >= MIN_ANNOTATION_TEXT_WIDTH) {
                    const QString aText = prepareAnnotationText(aData, as);
                    drawBoundedText(p, annotationRect, aText);
                }
            }
            drawAnnotationConnections(p, a, as, displaySettings, canvasSize, visibleRange);
        }
    }
    // draw restriction sites
    if (isRestrictionSite && displaySettings.displayCutSites) {
        p.setPen(borderPen);
        const bool isCircular = ctx->getSequenceObject()->isCircular();
        qint64 seqLength = ctx->getSequenceLength();
        bool isDirectStrand = aData->getStrand().isDirect();
        const auto& qual = aData->findFirstQualifierValue(GBFeatureUtils::QUALIFIER_CUT);
        if (isCircular && U1AnnotationUtils::isAnnotationContainsJunctionPoint(a, seqLength)) {
            auto merged = U1AnnotationUtils::mergeAnnotatedRegionsAroundJunctionPoint(location, seqLength);
            auto regionPair = merged.front();
            // draw cuts separately,
            // because they are located one on the 3' end of the direct sequence,
            // and the other one on the 3' end of reverse-complementary sequence
            // draw direct cut
            auto cutPosDirect = getCutPosition(regionPair.first, qual, isDirectStrand, true);
            if (cutPosDirect != INCORRECT_CUT_POSITION) {
                auto annRect = getAnnotationRect(regionPair.first, canvasSize, visibleRange, selected, a, as);
                drawCutSite(p, aData, cutPosDirect, annRect, as->color, canvasSize, visibleRange, true);
            }
            // draw complement cut
            auto cutPosComplement = getCutPosition(regionPair.second, qual, isDirectStrand, false);
            if (cutPosComplement != INCORRECT_CUT_POSITION) {
                auto annRect = getAnnotationRect(regionPair.second, canvasSize, visibleRange, selected, a, as);
                drawCutSite(p, aData, cutPosComplement, annRect, as->color, canvasSize, visibleRange, false);
            }
        } else if (location.size() == 1) {
            const auto& reg = location.first();
            QRect annRect;
            // draw direct cut
            auto cutPosDirect = getCutPosition(reg, qual, isDirectStrand, true);
            if (cutPosDirect != INCORRECT_CUT_POSITION) {
                annRect = getAnnotationRect(reg, canvasSize, visibleRange, selected, a, as);
                drawCutSite(p, aData, cutPosDirect, annRect, as->color, canvasSize, visibleRange, true);
            }
            // draw complement cut
            auto cutPosComplement = getCutPosition(reg, qual, isDirectStrand, false);
            if (cutPosComplement != INCORRECT_CUT_POSITION) {
                if (annRect.isEmpty()) {
                    annRect = getAnnotationRect(reg, canvasSize, visibleRange, selected, a, as);
                }
                drawCutSite(p, aData, cutPosComplement, annRect, as->color, canvasSize, visibleRange, false);
            }
        }
    }
}

U2Region SequenceViewAnnotatedRenderer::getAnnotationXRange(const U2Region& annotationRegion, const U2Region& visibleRange, const QSize& canvasSize, bool selected) const {
    const U2Region visibleLocation = annotationRegion.intersect(visibleRange);
    CHECK(!visibleLocation.isEmpty(), U2Region());

    const int visibleLocationStartCoord = posToXCoord(visibleLocation.startPos, canvasSize, visibleRange);
    const int visibleLocationEndCoord = posToXCoord(visibleLocation.endPos(), canvasSize, visibleRange);

    const int visibleRegionWidth = qMax(selected ? MIN_SELECTED_ANNOTATION_WIDTH : MIN_ANNOTATION_WIDTH, visibleLocationEndCoord - visibleLocationStartCoord);
    SAFE_POINT(visibleRegionWidth > 0, "Negative length of annotationXRange", U2Region());

    return U2Region(visibleLocationStartCoord, visibleRegionWidth);
}

void SequenceViewAnnotatedRenderer::drawBoundedText(QPainter& p, const QRect& r, const QString& text) {
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

int SequenceViewAnnotatedRenderer::getCutPosition(const U2Region& r, const QString& cutQual, bool isAnnotationDirect, bool isCutDirect) const {
    bool hasDirect = false;
    bool hasComplement = false;
    int cutDirectShift = 0;
    int cutComplementShift = 0;
    if (!cutQual.isEmpty()) {
        int complSplit = cutQual.indexOf('/');
        if (complSplit != -1) {
            cutDirectShift = cutQual.left(complSplit).toInt(&hasDirect);
            cutComplementShift = cutQual.mid(complSplit + 1).toInt(&hasComplement);
        } else {
            cutDirectShift = cutQual.toInt(&hasDirect);
            cutComplementShift = cutQual.toInt(&hasComplement);
        }
    }

    int cutPosition = INCORRECT_CUT_POSITION;
    if (isCutDirect && hasDirect) {
        cutPosition = correctCutPos(isAnnotationDirect ? r.startPos + cutDirectShift : r.startPos + cutComplementShift);
    } else if (!isCutDirect && hasComplement) {
        cutPosition = correctCutPos(isAnnotationDirect ? r.endPos() - cutComplementShift : r.endPos() - cutDirectShift);
    }

    return cutPosition;
}

QRect SequenceViewAnnotatedRenderer::getAnnotationRect(const U2Region& reg, const QSize& canvasSize, const U2Region& visibleRange, bool selected, Annotation* a, const AnnotationSettings* as) const {
    QRect res;
    const U2Region y = getAnnotationYRange(a, 0, as, canvasSize.height());
    CHECK(y.startPos >= 0, res);

    if (reg.intersects(visibleRange)) {
        const U2Region x = getAnnotationXRange(reg, visibleRange, canvasSize, selected);
        CHECK(!x.isEmpty(), res);

        res = QRect(x.startPos, y.startPos, x.length, y.length);
    } else {
        res = QRect(0, y.startPos, 0, y.length);
    }

    return res;
}

void SequenceViewAnnotatedRenderer::drawAnnotationConnections(QPainter& p, Annotation* a, const AnnotationSettings* as, const AnnotationDisplaySettings& displaySettings, const QSize& canvasSize, const U2Region& visibleRange) {
    const SharedAnnotationData& aData = a->getData();
    CHECK(!aData->location->isSingleRegion(), );

    const qint64 seqLength = ctx->getSequenceLength();
    U2Region sRange(0, seqLength);
    if (U1AnnotationUtils::isSplitted(aData->location, sRange)) {
        return;
    }

    int dx1 = 0;
    int dx2 = 0;
    if (displaySettings.displayAnnotationArrows) {
        if (aData->getStrand().isComplementary()) {
            dx2 = -FEATURE_ARROW_HLEN;
        } else {
            dx1 = FEATURE_ARROW_HLEN;
        }
    }
    int availableHeight = canvasSize.height();
    QVector<U2Region> location = aData->getRegions();
    for (int ri = 0, ln = location.size(); ri < ln; ri++) {
        const U2Region& r = location.at(ri);
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
                    x1 = qBound(-MAX_VIRTUAL_RANGE, x1, MAX_VIRTUAL_RANGE);  // qt4.4 crashes in line clipping alg for extremely large X values
                    x2 = qBound(-MAX_VIRTUAL_RANGE, x2, MAX_VIRTUAL_RANGE);
                    const int midX = (x1 + x2) / 2;
                    const U2Region pyr = getAnnotationYRange(a, ri - 1, as, availableHeight);
                    const U2Region yr = getAnnotationYRange(a, ri, as, availableHeight);
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

void SequenceViewAnnotatedRenderer::drawCutSite(QPainter& p, const SharedAnnotationData& aData, int cutPos, const QRect& annotationRect, const QColor& color, const QSize& canvasSize, const U2Region& visibleRange, bool isDirectCut) {
    bool isDirectStrand = aData->getStrand().isDirect();
    int availableHeight = canvasSize.height();
    U2Region cutSiteY = getCutSiteYRange(U2Strand(isDirectStrand ? U2Strand::Complementary : U2Strand::Direct), availableHeight);
    QRect mirroredAnnotationRect = annotationRect;
    mirroredAnnotationRect.setY(cutSiteY.startPos);
    mirroredAnnotationRect.setHeight(cutSiteY.length);

    CutSiteDrawData toInsert;
    toInsert.color = color;
    toInsert.pos = cutPos;
    if (isDirectCut) {
        toInsert.direct = true;
        toInsert.r = isDirectStrand ? annotationRect : mirroredAnnotationRect;
        drawCutSite(p, toInsert, canvasSize, visibleRange);
    } else {
        toInsert.direct = false;
        toInsert.r = !isDirectStrand ? annotationRect : mirroredAnnotationRect;
        drawCutSite(p, toInsert, canvasSize, visibleRange);
    }
}

void SequenceViewAnnotatedRenderer::drawCutSite(QPainter& p, const CutSiteDrawData& cData, const QSize& canvasSize, const U2Region& visibleRange) {
    int xCenter = posToXCoord(cData.pos, canvasSize, visibleRange);
    CHECK(xCenter != -1, );

    int xLeft = xCenter - CUT_SITE_HALF_WIDTH;
    int xRight = xCenter + CUT_SITE_HALF_WIDTH;
    int yFlat = (cData.direct ? cData.r.top() - CUT_SITE_HALF_HEIGHT : cData.r.bottom() + CUT_SITE_HALF_HEIGHT);
    int yPeak = (cData.direct ? cData.r.top() + CUT_SITE_HALF_HEIGHT : cData.r.bottom() - CUT_SITE_HALF_HEIGHT);

    QPolygon triangle;
    triangle << QPoint(xLeft, yFlat) << QPoint(xCenter, yPeak) << QPoint(xRight, yFlat) << QPoint(xLeft, yFlat);

    QPainterPath path;
    path.addPolygon(triangle);

    p.fillPath(path, cData.color);
    p.drawPath(path);
}

QString SequenceViewAnnotatedRenderer::prepareAnnotationText(const SharedAnnotationData& a, const AnnotationSettings* as) const {
    if (!as->showNameQuals || as->nameQuals.isEmpty()) {
        return a->name;
    }
    QVector<U2Qualifier> qs;
    foreach (const QString& qn, as->nameQuals) {
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
    arr << QPoint(x - dx, rect.top() - FEATURE_ARROW_VLEN);
    arr << QPoint(x + dx, rect.top() + rect.height() / 2);
    arr << QPoint(x - dx, rect.bottom() + FEATURE_ARROW_VLEN);
    arr << QPoint(x - dx, rect.top() - FEATURE_ARROW_VLEN);
    QPainterPath arrowPath;
    arrowPath.addPolygon(arr);

    QPainterPath dRectPath;
    dRectPath.addRect(leftArrow ? x : x - (FEATURE_ARROW_HLEN - 1), rect.top(), FEATURE_ARROW_HLEN, rect.height());

    path = path.subtracted(dRectPath);
    path = path.united(arrowPath);
}

qint64 SequenceViewAnnotatedRenderer::correctCutPos(qint64 pos) const {
    const bool isCircular = ctx->getSequenceObject()->isCircular();
    const qint64 sequenceLength = ctx->getSequenceLength();
    qint64 result = pos;
    if (isCircular) {
        if (result < 0) {
            result = sequenceLength + result;
        } else if (result > sequenceLength) {
            result = result - sequenceLength;
        }
    } else if (result < 0 || result > sequenceLength) {
        // out of sequence, do not draw
        result = INCORRECT_CUT_POSITION;
    }

    return result;
}

}  // namespace U2
