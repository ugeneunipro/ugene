/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "DetViewSingleLineRenderer.h"

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GraphUtils.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>

#include "ov_sequence/DetViewSequenceEditor.h"

namespace U2 {

/************************************************************************/
/* DetViewSingleLineRenderer */
/************************************************************************/
DetViewSingleLineRenderer::TranslationMetrics::TranslationMetrics(const SequenceObjectContext* ctx,
                                                                  const U2Region& visibleRange,
                                                                  const QFont& commonSequenceFont) {
    visibleFrames = ctx->getTranslationRowsVisibleStatus();

    minUsedPos = qMax(visibleRange.startPos - 1, qint64(0));
    maxUsedPos = qMin(visibleRange.endPos() + 1, ctx->getSequenceLength());

    seqBlockRegion = U2Region(minUsedPos, maxUsedPos - minUsedPos);

    startC = QColor(0, 0x99, 0);
    stopC = QColor(0x99, 0, 0);

    fontB = commonSequenceFont;
    fontB.setBold(true);
    fontI = commonSequenceFont;
    fontI.setItalic(true);

    sequenceFontSmall = commonSequenceFont;
    sequenceFontSmall.setPointSize(commonSequenceFont.pointSize() - 1);
    fontBS = sequenceFontSmall;
    fontBS.setBold(true);
    fontIS = sequenceFontSmall;
    fontIS.setItalic(true);
}

DetViewSingleLineRenderer::DetViewSingleLineRenderer(DetView* detView, SequenceObjectContext* ctx)
    : DetViewRenderer(detView, ctx),
      numLines(0),
      rulerLine(0),
      directLine(0),
      complementLine(0),
      firstDirectTransLine(0),
      firstComplTransLine(0) {
}

qint64 DetViewSingleLineRenderer::coordToPos(const QPoint& p, const QSize& /*canvasSize*/, const U2Region& visibleRange) const {
    return qMin(visibleRange.startPos + p.x() / commonMetrics.charWidth, visibleRange.endPos());
}

QList<U2Region> DetViewSingleLineRenderer::getAnnotationXRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, const QSize& canvasSize, const U2Region& visibleRange) const {
    CHECK(annotationSettings->visible, {});

    const auto& regs = annotation->getRegions();
    SAFE_POINT(0 <= locationRegionIndex && locationRegionIndex < regs.size(), "Annotation should contain locationRegionIndex", {});

    const auto& annotationRegion = regs.at(locationRegionIndex);
    bool selected = detView->getSequenceContext()->getAnnotationsSelection()->getAnnotations().contains(annotation);

    return { getAnnotationXRange(annotationRegion, visibleRange, canvasSize, selected) };
}

QList<U2Region> DetViewSingleLineRenderer::getAnnotationYRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, const QSize& canvasSize, const U2Region& /*visibleRange*/) const {
    return QList<U2Region>() << getAnnotationYRange(annotation, locationRegionIndex, annotationSettings, canvasSize.height());
}

U2Region DetViewSingleLineRenderer::getAnnotationYRange(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, int availableHeight) const {
    const SharedAnnotationData& annotationData = annotation->getData();
    const U2Strand strand = annotationData->getStrand();
    const bool isOnComplementStrand = strand.isComplementary() && detView->hasComplementaryStrand();
    const int frame = U1AnnotationUtils::getRegionFrame(detView->getSequenceLength(), strand, annotationData->isOrder(), locationRegionIndex, annotationData->getRegions());

    // Annotations are drawn on translation frames only in TS_ShowAllFrames or in TS_SetUpFramesManually modes.
    bool canDrawOnTranslation = ctx->getTranslationState() == SequenceObjectContext::TS_ShowAllFrames || ctx->getTranslationState() == SequenceObjectContext::TS_SetUpFramesManually;
    bool isOnTranslation = canDrawOnTranslation && annotationSettings->amino;
    int line = isOnTranslation ? (isOnComplementStrand ? getVisibleComplTransLine(frame) : getVisibleDirectTransLine(frame))
                               : (isOnComplementStrand ? complementLine : directLine);

    // Fallback to the non-translation direct/complement line if getVisibleDirectTransLine/getVisibleComplTransLine returns -1.
    if (line == -1) {
        // Use direct line if complement line is not visible (-1).
        line = isOnComplementStrand ? qMax(complementLine, directLine) : directLine;
    }
    SAFE_POINT(line != -1, "Unable to calculate vertical position of the annotation!", U2Region());

    int y = getLineY(line, availableHeight);
    return U2Region(y, commonMetrics.lineHeight);
}

U2Region DetViewSingleLineRenderer::getCutSiteYRange(const U2Strand& mirroredStrand, int availableHeight) const {
    int line = mirroredStrand.isDirect() ? directLine : qMax(complementLine, directLine);
    int y = getLineY(line, availableHeight);
    return U2Region(y, commonMetrics.lineHeight);
}

int DetViewSingleLineRenderer::getMinimumHeight() const {
    return getOneLineHeight();
}

qint64 DetViewSingleLineRenderer::getOneLineHeight() const {
    return numLines * commonMetrics.lineHeight + 5;
}

qint64 DetViewSingleLineRenderer::getLinesCount(const QSize& /*canvasSize*/) const {
    return 1;
}

int DetViewSingleLineRenderer::getContentIndentY(int canvasHeight) const {
    qint64 contentIndentY = (canvasHeight - getOneLineHeight()) / 2;
    contentIndentY = qMax((qint64)0, contentIndentY);
    contentIndentY -= detView->getVerticalScrollBarPosition() * commonMetrics.lineHeight;
    return contentIndentY;
}

int DetViewSingleLineRenderer::getRowsInLineCount() const {
    return numLines;
}

QSize DetViewSingleLineRenderer::getBaseCanvasSize(const U2Region& visibleRange) const {
    return QSize(visibleRange.length * commonMetrics.charWidth, getMinimumHeight());
}

bool DetViewSingleLineRenderer::isOnTranslationsLine(const QPoint& p, const QSize& canvasSize, const U2Region& /*visibleRange*/) const {
    int y = p.y();
    int availableHeight = canvasSize.height();
    if (firstDirectTransLine != -1) {
        U2Region dtr(getLineY(firstDirectTransLine, availableHeight), 3 * commonMetrics.lineHeight);
        if (dtr.contains(y)) {
            return true;
        }
    }
    if (firstComplTransLine != -1) {
        U2Region ctr(getLineY(firstComplTransLine, availableHeight), 3 * commonMetrics.lineHeight);
        if (ctr.contains(y)) {
            return true;
        }
    }
    return false;
}

bool DetViewSingleLineRenderer::isOnAnnotationLine(const QPoint& p, Annotation* a, int region, const AnnotationSettings* as, const QSize& canvasSize, const U2Region&) const {
    U2Region yRange = getAnnotationYRange(a, region, as, canvasSize.height());
    return yRange.contains(p.y());
}

void DetViewSingleLineRenderer::drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    p.fillRect(QRect(QPoint(0, 0), canvasSize), Qt::white);
    p.setPen(Qt::black);

    updateLines();

    AnnotationDisplaySettings displaySettings;
    displaySettings.displayAnnotationArrows = false;
    displaySettings.displayAnnotationNames = false;
    drawAnnotations(p, canvasSize, visibleRange, displaySettings);
    int availableHeight = canvasSize.height();
    drawDirect(p, availableHeight, visibleRange);
    drawComplement(p, availableHeight, visibleRange);
    drawTranslations(p, availableHeight, visibleRange);
    drawRuler(p, canvasSize, visibleRange);
}

void DetViewSingleLineRenderer::drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    p.setPen(Qt::black);

    updateLines();

    AnnotationDisplaySettings displaySettins;
    displaySettins.displayAnnotationArrows = false;
    displaySettins.displayAnnotationNames = false;
    drawAnnotationSelection(p, canvasSize, visibleRange, displaySettins);

    bool hasAnnotationesSelectedInVisibleRange = detView->isAnnotationSelectionInVisibleRange();
    if (hasAnnotationesSelectedInVisibleRange) {
        int availableHeight = canvasSize.height();
        drawDirect(p, availableHeight, visibleRange);
        drawComplement(p, availableHeight, visibleRange);
        drawTranslations(p, availableHeight, visibleRange);
    }

    drawSequenceSelection(p, canvasSize, visibleRange);
}

void DetViewSingleLineRenderer::drawCursor(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    CHECK(detView->isEditMode(), );
    DetViewSequenceEditor* editor = detView->getEditor();
    CHECK(editor != nullptr, );
    int pos = editor->getCursorPosition();
    CHECK(visibleRange.contains(pos) || pos == visibleRange.endPos(), );

    int ymargin = commonMetrics.yCharOffset / 2;
    int y = getLineY(directLine, canvasSize.height()) - 2 * ymargin;
    int height = commonMetrics.lineHeight + 4 * ymargin;
    int x = posToXCoord(pos, canvasSize, visibleRange);

    QPen pen(editor->getCursorColor());
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    p.setPen(pen);
    p.drawLine(x, y, x, y + height);
    p.drawLine(x - ymargin, y, x + ymargin, y);
    p.drawLine(x - ymargin, y + height, x + ymargin, y + height);
}

void DetViewSingleLineRenderer::update() {
    updateLines();
}

void DetViewSingleLineRenderer::drawDirect(QPainter& p, int availableHeight, const U2Region& visibleRange) {
    p.setFont(commonMetrics.sequenceFont);
    p.setPen(Qt::black);

    U2OpStatusImpl os;
    QByteArray sequence = ctx->getSequenceData(visibleRange, os);
    CHECK_OP(os, );
    const char* seq = sequence.constData();

    // draw base line;
    int y = getTextY(directLine, availableHeight);
    for (int i = 0; i < visibleRange.length; i++) {
        char nucl = seq[i];
        p.drawText(i * commonMetrics.charWidth + commonMetrics.xCharOffset, y, QString(nucl));
    }
}

void DetViewSingleLineRenderer::drawComplement(QPainter& p, int availableHeight, const U2Region& visibleRange) {
    p.setFont(commonMetrics.sequenceFont);
    p.setPen(Qt::black);

    if (complementLine > 0) {
        U2OpStatusImpl os;
        QByteArray visibleSequence = ctx->getSequenceData(visibleRange, os);
        CHECK_OP(os, );
        const char* seq = visibleSequence.constData();

        DNATranslation* complTrans = ctx->getComplementTT();
        CHECK(complTrans != nullptr, );
        QByteArray map = complTrans->getOne2OneMapper();
        int y = getTextY(complementLine, availableHeight);
        for (int i = 0; i < visibleRange.length; i++) {
            char nucl = seq[i];
            char complNucl = map.at(nucl);
            p.drawText(i * commonMetrics.charWidth + commonMetrics.xCharOffset, y, QString(complNucl));
        }
    }
}

static QByteArray translate(DNATranslation* t, const char* seq, qint64 seqLen) {
    QByteArray res(seqLen / 3, 0);
    t->translate(seq, seqLen, res.data(), seqLen / 3);
    return res;
}

static QByteArray translateSelection(const QVector<U2Region>& regions, DNATranslation* t, const char* seq, qint64 seqLen, qint64 offset, const U2Strand direction) {
    QByteArray resultTranslation;

    foreach (U2Region reg, regions) {
        reg = U2Region(direction == U2Strand::Direct ? offset : 0, seqLen).intersect(reg);
        const qint64 endPos = reg.endPos();
        const qint64 endOfVisibleArea = offset + seqLen;

        if (reg.length < 3 || (direction == U2Strand::Direct && (reg.startPos > endOfVisibleArea || endPos < offset)) || (direction == U2Strand::Complementary && (reg.startPos > seqLen))) {
            continue;
        }

        int mod = reg.length % 3;
        qint64 length = 0;
        switch (mod) {
            case 0:
                if (reg.startPos % 3 == 2 && offset == 0) {
                    length = endPos - offset - 2;
                } else {
                    length = endPos + 1;
                    if (direction == U2Strand::Direct) {
                        length -= offset;
                    }
                }
                break;
            case 1:
                if (reg.startPos % 3 == 2 && offset == 0) {
                    length = endPos - offset - 3;
                } else {
                    length = endPos;
                    if (direction == U2Strand::Direct) {
                        length -= offset;
                    }
                }
                break;
            case 2:
                if (reg.startPos % 3 == 2 && offset == 0) {
                    length = endPos - offset - 4;
                } else {
                    length = endPos - 1;
                    if (direction == U2Strand::Direct) {
                        length -= offset;
                    }
                }
                break;
        }

        QByteArray currentTranslation = translate(t, seq, length);

        int translationSize = currentTranslation.size();

        int selTranslationSize = 0;
        switch (mod) {
            case 0:
                selTranslationSize = (reg.length + 1) / 3;
                break;
            case 1:
                selTranslationSize = reg.length / 3;
                break;
            case 2:
                selTranslationSize = (reg.length - 1) / 3;
                break;
        }

        const int indent = translationSize - selTranslationSize - resultTranslation.size();
        QByteArray gaps = QByteArray(indent, ' ');
        currentTranslation = currentTranslation.right(selTranslationSize);
        if (!currentTranslation.isEmpty()) {
            resultTranslation += gaps;
            resultTranslation += currentTranslation;
        }
    }
    return resultTranslation;
}

static QByteArray translateComplSelection(const QVector<U2Region>& regions, DNATranslation* t, const char* seq, const qint64 seqLen, const U2Region& visibleRange) {
    QVector<U2Region> revesedRegions;
    foreach (const U2Region& reg, regions) {
        const qint64 endPos = reg.endPos();
        const qint64 startPos = (visibleRange.startPos + seqLen) - endPos;
        const U2Region newReg(startPos, reg.length);
        revesedRegions << newReg;
    }
    U2Region::reverse(revesedRegions);

    QByteArray amino = translateSelection(revesedRegions, t, seq, seqLen, visibleRange.startPos, U2Strand::Complementary);

    return amino;
}

static int correctLine(QVector<bool> visibleRows, int line) {
    int retLine = line;
    SAFE_POINT(visibleRows.size() == 6, "Count of translation rows is not equel to 6", -1);
    for (int i = 0; i < line; i++) {
        if (!visibleRows[i + 3]) {
            retLine--;
        }
    }
    return retLine;
}

void DetViewSingleLineRenderer::drawTranslations(QPainter& p, int availableHeight, const U2Region& visibleRange) {
    CHECK(firstDirectTransLine >= 0 || firstComplTransLine >= 0, );
    TranslationMetrics translationMetrics(detView->getSequenceContext(), visibleRange, commonMetrics.sequenceFont);

    U2OpStatusImpl os;
    QByteArray seqBlockData = ctx->getSequenceData(translationMetrics.seqBlockRegion, os);
    CHECK_OP(os, );
    const char* seqBlock = seqBlockData.constData();

    QList<SharedAnnotationData> annotationsInRange;
    foreach (Annotation* a, detView->findAnnotationsInRange(visibleRange)) {
        annotationsInRange << a->getData();
    }

    p.save();
    p.setFont(commonMetrics.sequenceFont);

    const char* visibleSequence = seqBlock + (visibleRange.startPos - translationMetrics.seqBlockRegion.startPos);
    drawDirectTranslations(p, visibleRange, visibleSequence, annotationsInRange, translationMetrics, availableHeight);

    if (detView->hasComplementaryStrand()) {  // reverse translations
        drawComplementTranslations(p, visibleRange, seqBlock, annotationsInRange, translationMetrics, availableHeight);
    }
    p.restore();
}

void DetViewSingleLineRenderer::drawDirectTranslations(QPainter& painter,
                                                       const U2Region& visibleRange,
                                                       const char* visibleSequence,
                                                       const QList<SharedAnnotationData>& annotationsInRange,
                                                       const TranslationMetrics& translationMetrics,
                                                       int availableHeight) {
    DNATranslation* aminoTT = detView->getAminoTT();
    CHECK(aminoTT != nullptr, );

    int lineIndex = firstDirectTransLine;
    for (int frameIndex = 0; frameIndex < 3; frameIndex++) {
        if (!translationMetrics.visibleFrames[frameIndex]) {
            continue;
        }
        int visibleRangeFrameIndex = visibleRange.startPos % 3;
        qint64 translationStartPos = visibleRange.startPos + frameIndex - visibleRangeFrameIndex - 3;
        while (translationStartPos < qMax((qint64)0, visibleRange.startPos - 1)) {
            translationStartPos += 3;
        }
        qint64 translationIndent = translationStartPos - visibleRange.startPos;
        const char* sequenceToTranslate = visibleSequence + translationIndent;
        qint64 sequenceToTranslateLength = qMin(visibleRange.endPos() + 1, ctx->getSequenceLength()) - translationStartPos;
        QByteArray aminoSequence = translate(aminoTT, sequenceToTranslate, sequenceToTranslateLength);

        QVector<bool> aminoVisibilityFlags(aminoSequence.size(), true);  // Visible amino acid positions. All positions are visible by default.
        if (ctx->getTranslationState() == SequenceObjectContext::TS_AnnotationsOrSelection) {
            aminoVisibilityFlags.fill(false);  // Mark as visible only acids within selected regions.
            U2Region translatableRange(visibleRange.startPos + translationIndent, aminoSequence.length() * 3);
            for (const U2Region& selectedRegion : qAsConst(ctx->getSequenceSelection()->getSelectedRegions())) {
                if (selectedRegion.startPos % 3 == frameIndex) {
                    U2Region regionToTranslate = translatableRange.intersect(selectedRegion);
                    qint64 regionAminoOffset = (regionToTranslate.startPos - translationStartPos) / 3;
                    for (int regionAminoIndex = 0; regionAminoIndex < regionToTranslate.length / 3; regionAminoIndex++) {
                        aminoVisibilityFlags[regionAminoOffset + regionAminoIndex] = true;
                    }
                }
            }
        }

        int y = getTextY(lineIndex, availableHeight);
        lineIndex++;
        for (int aminoIndex = 0; aminoIndex < aminoSequence.length(); aminoIndex++) {
            if (!aminoVisibilityFlags[aminoIndex]) {
                continue;
            }
            int xPos = 3 * aminoIndex + translationIndent + 1;
            SAFE_POINT(xPos >= 0 && xPos < visibleRange.length, "X-Position is out of visible range", );
            int x = xPos * commonMetrics.charWidth + commonMetrics.xCharOffset;

            QColor charColor;
            bool inAnnotation = deriveTranslationCharColor(translationStartPos + aminoIndex * 3,
                                                           U2Strand::Direct,
                                                           annotationsInRange,
                                                           charColor);

            setFontAndPenForTranslation(sequenceToTranslate + aminoIndex * 3, charColor, inAnnotation, painter, translationMetrics);
            painter.drawText(x, y, QString(aminoSequence.at(aminoIndex)));
        }
    }
}

void DetViewSingleLineRenderer::drawComplementTranslations(QPainter& p,
                                                           const U2Region& visibleRange,
                                                           const char* seqBlock,
                                                           const QList<SharedAnnotationData>& annotationsInRange,
                                                           TranslationMetrics& trMetrics,
                                                           int availableHeight) {
    bool isTranslateAnnotationOrSelection = (ctx->getTranslationState() == SequenceObjectContext::TS_AnnotationsOrSelection);
    QVector<U2Region> regions;
    QList<QVector<U2Region>> sortedRegions = QList<QVector<U2Region>>() << QVector<U2Region>() << QVector<U2Region>() << QVector<U2Region>();
    if (isTranslateAnnotationOrSelection) {
        regions = ctx->getSequenceSelection()->getSelectedRegions();
        QList<int> mods;
        for (int i = 3; i < 6; i++) {
            trMetrics.visibleFrames[i] = false;
        }
        foreach (const U2Region& reg, regions) {
            const qint64 startReversePos = ctx->getSequenceLength() - reg.endPos();
            int mod = startReversePos % 3;
            trMetrics.visibleFrames[3 + mod] = true;

            const int intersectedLength = visibleRange.intersect(reg).length;
            if (intersectedLength > reg.length % 3 + 1) {
                sortedRegions[mod] << reg;
                if (!mods.contains(mod)) {
                    mods << mod;
                }
            }
        }
    }

    DNATranslation* complTable = ctx->getComplementTT();
    SAFE_POINT(complTable != nullptr, "Complement translation table is NULL", );
    QByteArray revComplDna(trMetrics.seqBlockRegion.length, 0);
    complTable->translate(seqBlock, trMetrics.seqBlockRegion.length, revComplDna.data(), trMetrics.seqBlockRegion.length);
    TextUtils::reverse(revComplDna.data(), revComplDna.size());
    for (int i = 0; i < 3; i++) {
        int indent = (detView->getSequenceLength() - visibleRange.endPos() + i) % 3;
        qint64 revComplStartPos = visibleRange.endPos() - indent + 3;  // start of the reverse complement sequence in direct coords
        if (revComplStartPos > trMetrics.maxUsedPos) {
            revComplStartPos -= 3;
        }
        qint64 revComplDnaOffset = trMetrics.maxUsedPos - revComplStartPos;
        SAFE_POINT(revComplDnaOffset >= 0, "Complement translations offset is negative", );
        int complLine = (detView->getSequenceLength() - revComplStartPos) % 3;

        if (trMetrics.visibleFrames[complLine + 3] == true) {
            DNATranslation* translation = detView->getAminoTT();
            CHECK(translation != nullptr, );

            const char* revComplData = revComplDna.constData();
            const char* seq = revComplData + revComplDnaOffset;
            qint64 seqLen = revComplStartPos - trMetrics.minUsedPos;

            QByteArray amino;
            if (isTranslateAnnotationOrSelection) {
                std::sort(sortedRegions[complLine].begin(), sortedRegions[complLine].end());
                amino = translateComplSelection(sortedRegions[complLine], translation, seq, seqLen, visibleRange);
            } else {
                amino = translate(translation, seq, seqLen);
            }

            if (amino.isEmpty()) {
                continue;
            }

            complLine = correctLine(trMetrics.visibleFrames, complLine);

            int y = getTextY(firstComplTransLine + complLine, availableHeight);
            int dx = visibleRange.endPos() - revComplStartPos;
            for (int j = 0, n = amino.length(); j < n; j++, seq += 3) {
                char amin = amino[j];
                int xpos = visibleRange.length - (3 * j + 2 + dx);
                SAFE_POINT(xpos >= 0 && xpos < visibleRange.length, "Position is out of visible range", );
                int x = xpos * commonMetrics.charWidth + commonMetrics.xCharOffset;

                QColor charColor;
                bool inAnnotation = deriveTranslationCharColor(trMetrics.maxUsedPos - (seq - revComplDna.constData()),
                                                               U2Strand::Complementary,
                                                               annotationsInRange,
                                                               charColor);

                setFontAndPenForTranslation(seq, charColor, inAnnotation, p, trMetrics);

                p.drawText(x, y, QString(amin));
            }
        }
    }
}

void DetViewSingleLineRenderer::drawRuler(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    int y = getLineY(rulerLine, canvasSize.height()) + 2;
    int firstCharStart = posToXCoord(visibleRange.startPos, canvasSize, visibleRange);
    int lastCharStart = posToXCoord(visibleRange.endPos() - 1, canvasSize, visibleRange);
    int firstCharCenter = firstCharStart + commonMetrics.charWidth / 2;
    int firstLastLen = lastCharStart - firstCharStart;
    GraphUtils::RulerConfig c;
    GraphUtils::drawRuler(p, QPoint(firstCharCenter, y), firstLastLen, visibleRange.startPos + 1, visibleRange.endPos(), commonMetrics.rulerFont, c);
}

void DetViewSingleLineRenderer::drawSequenceSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    DNASequenceSelection* sel = ctx->getSequenceSelection();
    CHECK(!sel->isEmpty(), );

    QPen framePen(Qt::black, 1, Qt::DashLine);
    p.setPen(framePen);

    const QVector<U2Region>& selectedRegions = sel->getSelectedRegions();
    for (const U2Region& selectedRegion : qAsConst(selectedRegions)) {
        U2Region visibleSelectedRegion = selectedRegion.intersect(visibleRange);
        if (visibleSelectedRegion.isEmpty()) {
            continue;
        }
        highlight(p, visibleSelectedRegion, directLine, canvasSize, visibleRange);
        if (detView->hasComplementaryStrand()) {
            highlight(p, visibleSelectedRegion, complementLine, canvasSize, visibleRange);
        }
        if (detView->hasTranslations()) {
            int translLine = posToDirectTransLine(selectedRegion.startPos);
            if (translLine >= 0 && visibleSelectedRegion.length >= 3) {
                qint64 frameStart = visibleSelectedRegion.startPos;
                qint64 frameLength = visibleSelectedRegion.length;
                if (selectedRegion.endPos() == visibleSelectedRegion.endPos()) {
                    frameLength -= (visibleSelectedRegion.endPos() - selectedRegion.startPos) % 3;
                }
                U2Region frameRegion(frameStart, frameLength);
                highlight(p, frameRegion, translLine, canvasSize, visibleRange);
            }
            if (detView->hasComplementaryStrand()) {
                int complTransLine = posToComplTransLine(selectedRegion.endPos());
                if (complTransLine >= 0 && visibleSelectedRegion.length >= 3) {
                    qint64 frameEnd = visibleSelectedRegion.endPos();
                    qint64 frameLength = visibleSelectedRegion.length;
                    if (selectedRegion.startPos == visibleSelectedRegion.startPos) {
                        frameLength -= (selectedRegion.endPos() - visibleSelectedRegion.startPos) % 3;
                    }
                    U2Region frameRegion(frameEnd - frameLength, frameLength);
                    highlight(p, frameRegion, complTransLine, canvasSize, visibleRange);
                }
            }
        }
    }
}

int DetViewSingleLineRenderer::getLineY(int line, int availableHeight) const {
    int contentIndent = getContentIndentY(availableHeight);
    return contentIndent + 2 + line * commonMetrics.lineHeight;
}

int DetViewSingleLineRenderer::getTextY(int line, int availableHeight) const {
    return getLineY(line, availableHeight) + commonMetrics.lineHeight - commonMetrics.yCharOffset;
}

int DetViewSingleLineRenderer::getVisibleDirectTransLine(int absoluteFrameNumber) const {
    int lineNumber = firstDirectTransLine + absoluteFrameNumber;

    const QVector<bool> rowsVisibility = ctx->getTranslationRowsVisibleStatus();
    const int halfRowsCount = rowsVisibility.size() / 2;
    SAFE_POINT(absoluteFrameNumber < halfRowsCount, "Unexpected translation line number", -1);
    if (!rowsVisibility[absoluteFrameNumber]) {
        return -1;
    } else {
        for (int i = 0; i < absoluteFrameNumber; ++i) {
            if (!rowsVisibility[i]) {
                --lineNumber;
            }
        }
        return lineNumber;
    }
}

int DetViewSingleLineRenderer::getVisibleComplTransLine(int absoluteFrameNumber) const {
    int lineNumber = firstComplTransLine + absoluteFrameNumber;

    const QVector<bool> rowsVisibility = ctx->getTranslationRowsVisibleStatus();
    const int halfRowsCount = rowsVisibility.size() / 2;
    SAFE_POINT(absoluteFrameNumber < halfRowsCount, "Unexpected translation line number", -1);
    if (!rowsVisibility[halfRowsCount + absoluteFrameNumber]) {
        return -1;
    } else {
        for (int i = halfRowsCount; i < halfRowsCount + absoluteFrameNumber; ++i) {
            if (!rowsVisibility[i]) {
                --lineNumber;
            }
        }
        return lineNumber;
    }
}

void DetViewSingleLineRenderer::updateLines() {
    numLines = -1;
    rulerLine = -1;
    directLine = -1;
    complementLine = -1;
    firstDirectTransLine = -1;
    firstComplTransLine = -1;

    if (!detView->hasComplementaryStrand() && !detView->hasTranslations()) {
        directLine = 0;
        rulerLine = 1;
        numLines = 2;
    } else if (detView->hasComplementaryStrand() && detView->hasTranslations()) {
        // change
        firstDirectTransLine = 0;
        directLine = 3;
        rulerLine = 4;
        complementLine = 5;
        firstComplTransLine = 6;
        numLines = 9;

        QVector<bool> v = ctx->getTranslationRowsVisibleStatus();
        for (int i = 0; i < 6; i++) {
            if (!v[i]) {
                if (i < 3) {
                    directLine--;
                    rulerLine--;
                    complementLine--;
                    firstComplTransLine--;
                }
                numLines--;
            }
        }

        if (ctx->getTranslationState() == SequenceObjectContext::TS_AnnotationsOrSelection && numLines == 3) {
            directLine = 1;
            rulerLine = 2;
            complementLine = 3;
            firstComplTransLine = 4;
            numLines = 5;
        }
    } else if (detView->hasComplementaryStrand()) {
        directLine = 0;
        rulerLine = 1;
        complementLine = 2;
        numLines = 3;
    } else {
        firstDirectTransLine = 0;
        directLine = 3;
        rulerLine = 4;
        numLines = 5;

        QVector<bool> v = ctx->getTranslationRowsVisibleStatus();
        for (int i = 0; i < 3; i++) {
            if (!v[i]) {
                directLine--;
                rulerLine--;
                numLines--;
            }
        }

        if (ctx->getTranslationState() == SequenceObjectContext::TS_AnnotationsOrSelection && numLines == 3) {
            directLine = 1;
            rulerLine = 2;
            numLines = 3;
        }
    }
    SAFE_POINT(numLines > 0, "Nothing to render. Lines count is less then 1", );
}

bool DetViewSingleLineRenderer::deriveTranslationCharColor(qint64 pos,
                                                           const U2Strand& strand,
                                                           const QList<SharedAnnotationData>& annotationsInRange,
                                                           QColor& result) {
    // logic:
    // no annotations found -> grey
    // found annotation that is on translation -> black
    // 1 annotation found on nucleic -> darker(annotation color)
    // 2+ annotations found on nucleic -> black

    int nAnnotations = 0;
    const U2Region tripletRange = strand.isComplementary() ? U2Region(pos - 2, 2) : U2Region(pos, 2);
    AnnotationSettings* as = nullptr;
    AnnotationSettingsRegistry* registry = AppContext::getAnnotationsSettingsRegistry();
    const int sequenceLen = detView->getSequenceLength();
    foreach (const SharedAnnotationData& aData, annotationsInRange) {
        if (aData->getStrand() != strand) {
            continue;
        }
        bool annotationOk = false;
        AnnotationSettings* tas = nullptr;
        const bool order = aData->isOrder();
        const QVector<U2Region>& location = aData->getRegions();
        for (int i = 0, n = location.size(); i < n; i++) {
            const U2Region& r = location.at(i);
            if (!r.contains(tripletRange)) {
                continue;
            }
            const int regionFrame = U1AnnotationUtils::getRegionFrame(sequenceLen, strand, order, i, location);
            const int posFrame = strand.isComplementary() ? (sequenceLen - pos) % 3 : pos % 3;
            if (regionFrame == posFrame) {
                tas = registry->getAnnotationSettings(aData);
                if (tas->visible) {
                    annotationOk = true;
                    break;
                }
            }
        }
        if (annotationOk) {
            nAnnotations++;
            as = tas;
            if (nAnnotations > 1) {
                break;
            }
        }
    }
    if (0 == nAnnotations) {
        result = Qt::gray;
        return false;
    }

    if (nAnnotations > 1) {
        result = Qt::black;
        return true;
    }

    const TriState aminoState = as->amino ? TriState_Yes : TriState_No;
    const bool aminoOverlap = (aminoState == TriState_Yes);  // annotation is drawn on amino strand -> use black color for letters
    result = aminoOverlap ? Qt::black : as->color.darker(300);

    return true;
}

void DetViewSingleLineRenderer::setFontAndPenForTranslation(const char* seq,
                                                            const QColor& charColor,
                                                            bool inAnnotation,
                                                            QPainter& p,
                                                            const TranslationMetrics& trMetrics) {
    DNATranslation3to1Impl* aminoTable = (DNATranslation3to1Impl*)ctx->getAminoTT();
    SAFE_POINT(aminoTable != nullptr, "Amino translation table is NULL", );
    if (aminoTable->isStartCodon(seq)) {
        p.setPen(inAnnotation ? charColor : trMetrics.startC);
        p.setFont(inAnnotation ? trMetrics.fontB : trMetrics.fontBS);
    } else if (aminoTable->isCodon(DNATranslationRole_Start_Alternative, seq)) {
        p.setPen(inAnnotation ? charColor : trMetrics.startC);
        p.setFont(inAnnotation ? trMetrics.fontI : trMetrics.fontIS);
    } else if (aminoTable->isStopCodon(seq)) {
        p.setPen(inAnnotation ? charColor : trMetrics.stopC);
        p.setFont(inAnnotation ? trMetrics.fontB : trMetrics.fontBS);
    } else {
        p.setPen(charColor);
        p.setFont(inAnnotation ? commonMetrics.sequenceFont : trMetrics.sequenceFontSmall);
    }
}

void DetViewSingleLineRenderer::highlight(QPainter& p, const U2Region& regionToHighlight, int line, const QSize& canvasSize, const U2Region& visibleRange) {
    SAFE_POINT(line >= 0, "Unexpected sequence view line number", );

    int x = posToXCoord(regionToHighlight.startPos, canvasSize, visibleRange);
    int width = posToXCoord(regionToHighlight.endPos(), canvasSize, visibleRange) - x;
    int y = getLineY(line, canvasSize.height());
    int height = commonMetrics.lineHeight;
    p.save();

    QPen pen = p.pen();
    pen.setColor(Qt::gray);
    pen.setWidth(2);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(x, y, width, height);

    p.setBrush(Qt::darkGray);
    p.setCompositionMode(QPainter::CompositionMode_ColorBurn);
    p.drawRect(x, y, width, height);

    p.restore();
}

int DetViewSingleLineRenderer::posToDirectTransLine(int p) const {
    SAFE_POINT(firstDirectTransLine >= 0, "Invalid direct translation line number", -1);
    const int absoluteLineNumber = p % 3;
    return getVisibleDirectTransLine(absoluteLineNumber);
}

int DetViewSingleLineRenderer::posToComplTransLine(int p) const {
    SAFE_POINT(firstComplTransLine >= 0, "Invalid complementary translation line number", -1);
    const int absoluteLineNumber = (detView->getSequenceLength() - p) % 3;
    return getVisibleComplTransLine(absoluteLineNumber);
}

}  // namespace U2
