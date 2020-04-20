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

#include <QPolygonF>

#include <U2Algorithm/MSAConsensusAlgorithmClustal.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusAlgorithmStrict.h>
#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/MSAEditor.h>

#include "MaGraphCalculationTask.h"

namespace U2 {

MaGraphCalculationTask::MaGraphCalculationTask(MultipleAlignmentObject* maObject, int width, int height)
    : BackgroundTask<QPolygonF>(tr("Render overview"), TaskFlag_None),
      ma(maObject->getMultipleAlignmentCopy()), // SANGER_TODO: getiing before any check
      memLocker(stateInfo),
      msaLength(0),
      seqNumber(0),
      width(width),
      height(height)
{
    SAFE_POINT_EXT(maObject != NULL, setError(tr("MSA is NULL")), );
    msaLength = maObject->getLength();
    seqNumber = maObject->getNumRows();
    if(!memLocker.tryAcquire(maObject->getMultipleAlignment()->getLength() * maObject->getMultipleAlignment()->getNumRows())) {
        setError(memLocker.getError());
        return;
    }
//    ma = msa->getMultipleAlignmentCopy();
    connect(maObject, SIGNAL(si_invalidateAlignmentObject()), this, SLOT(cancel()));
    connect(maObject, SIGNAL(si_startMaUpdating()), this, SLOT(cancel()));
    connect(maObject, SIGNAL(si_alignmentChanged(MultipleAlignment,MaModificationInfo)), this, SLOT(cancel()));
}

void MaGraphCalculationTask::run() {
    CHECK(!hasError(), );
    emit si_calculationStarted();
    constructPolygon(result);
    emit si_calculationStoped();
}

void MaGraphCalculationTask::constructPolygon(QPolygonF &polygon) {
    SAFE_POINT_EXT(width != 0, setError(tr("Overview width is zero")), );
    stateInfo.setProgress(0);
    emit si_progressChanged();

    if (msaLength == 0 || seqNumber == 0) {
        polygon = QPolygonF();
        return;
    }

    double stepY = height / static_cast<double>(100);
    QVector<QPointF> points;
    points.append(QPointF(0, height));

    if ( msaLength < width ) {
        double stepX = width / static_cast<double>(msaLength);
        points.append(QPointF(0, qRound( height - stepY * static_cast<double>(getGraphValue(0)))));
        for (int pos = 0; pos < msaLength; pos++) {
            if (isCanceled()) {
                polygon = QPolygonF();
                return;
            }
            int percent = getGraphValue(pos);
            points.append(QPointF(qRound( stepX * static_cast<double>(pos) + stepX / 2),
                                  height - stepY * percent));
            stateInfo.setProgress(100 * pos / msaLength);
            emit si_progressChanged();
        }
        points.append(QPointF( width, qRound( height - stepY * static_cast<double>(getGraphValue(msaLength - 1)))));

    } else {
        double stepX = msaLength / static_cast<double>(width);
        for (int pos = 0; pos < width; pos++) {
            double average = 0;
            int count = 0;
            for (int i = stepX * pos; i < qRound( stepX * (pos + 1) ); i++) {
                if (isCanceled()) {
                    polygon = QPolygonF();
                    return;
                }
                if (i > msaLength) {
                    break;
                }
                average += getGraphValue(i);
                count++;
            }
            CHECK(count != 0, );
            average /= count;
            points.append( QPointF(pos, height - stepY * average ));
            stateInfo.setProgress(100 * pos / width);
            emit si_progressChanged();
        }
    }

    points.append(QPointF(width, height));
    polygon = QPolygonF(points);
    stateInfo.setProgress(100);
    emit si_progressChanged();
}

MaConsensusOverviewCalculationTask::MaConsensusOverviewCalculationTask(MultipleAlignmentObject* msa,
                                    int width, int height)
    : MaGraphCalculationTask(msa, width, height)
{
    SAFE_POINT_EXT(AppContext::getMSAConsensusAlgorithmRegistry() != NULL, setError(tr("MSAConsensusAlgorithmRegistry is NULL!")), );

    MSAConsensusAlgorithmFactory* factory = AppContext::getMSAConsensusAlgorithmRegistry()->getAlgorithmFactory(BuiltInConsensusAlgorithms::STRICT_ALGO);
    SAFE_POINT_EXT(factory != NULL, setError(tr("Strict consensus algorithm factory is NULL")), );

    SAFE_POINT_EXT(msa != NULL, setError(tr("MSA is NULL")), );
    algorithm = factory->createAlgorithm(msa->getMultipleAlignment());
    algorithm->setParent(this);
}

int MaConsensusOverviewCalculationTask::getGraphValue(int pos) const {
    int score = 0;
    algorithm->getConsensusCharAndScore(ma, pos, score);
    return qRound(score * 100. / seqNumber);
}

MaGapOverviewCalculationTask::MaGapOverviewCalculationTask(MultipleAlignmentObject* msa, int width, int height)
    : MaGraphCalculationTask(msa, width, height) {}

int MaGapOverviewCalculationTask::getGraphValue(int pos) const {
    int gapCounter = 0;
    for (int seq = 0; seq < seqNumber; seq++) {
        if (pos > ma->getLength()) {
            continue;
        }
        uchar c = static_cast<uchar>(ma->charAt(seq, pos));
        if (c == U2Msa::GAP_CHAR) {
            gapCounter++;
        }
    }

    return qRound(gapCounter * 100. / seqNumber);
}

MaClustalOverviewCalculationTask::MaClustalOverviewCalculationTask(MultipleAlignmentObject *msa, int width, int height)
    : MaGraphCalculationTask(msa, width, height) {
    SAFE_POINT_EXT(AppContext::getMSAConsensusAlgorithmRegistry() != NULL, setError(tr("MSAConsensusAlgorithmRegistry is NULL!")), );

    MSAConsensusAlgorithmFactory* factory = AppContext::getMSAConsensusAlgorithmRegistry()->getAlgorithmFactory(BuiltInConsensusAlgorithms::CLUSTAL_ALGO);
    SAFE_POINT_EXT(factory != NULL, setError(tr("Clustal algorithm factory is NULL")), );

    SAFE_POINT_EXT(msa != NULL, setError(tr("MSA is NULL")), );
    algorithm = factory->createAlgorithm(ma);
    algorithm->setParent(this);
}

int MaClustalOverviewCalculationTask::getGraphValue(int pos) const {
    char c = algorithm->getConsensusChar(ma, pos);

    switch (c) {
    case '*':
        return 100;
    case ':':
        return 60;
    case '.':
        return 30;
    default:
        return 0;
    }
}

MaHighlightingOverviewCalculationTask::MaHighlightingOverviewCalculationTask(MaEditor *editor,
                                                                               const QString &colorSchemeId,
                                                                               const QString &highlightingSchemeId,
                                                                               int width, int height)
    : MaGraphCalculationTask(editor->getMaObject(), width, height) {

    SAFE_POINT_EXT(AppContext::getMsaHighlightingSchemeRegistry() != NULL,
                   setError(tr("MSA highlighting scheme registry is NULL")), );
    MsaHighlightingSchemeFactory* f_hs = AppContext::getMsaHighlightingSchemeRegistry()->getSchemeFactoryById( highlightingSchemeId );
    SAFE_POINT_EXT(f_hs != NULL, setError(tr("MSA highlighting scheme factory with '%1' id is NULL").arg(highlightingSchemeId)), );

    highlightingScheme = f_hs->create(this, editor->getMaObject());
    schemeId = f_hs->getId();

    MsaColorSchemeFactory* f_cs = AppContext::getMsaColorSchemeRegistry()->getSchemeFactoryById( colorSchemeId );
    colorScheme = f_cs->create(this, editor->getMaObject());

    U2OpStatusImpl os;
    refSequenceId = ma->getRowIndexByRowId(editor->getReferenceRowId(), os);
}

bool MaHighlightingOverviewCalculationTask::isCellHighlighted(const MultipleAlignment &ma, MsaHighlightingScheme *highlightingScheme,
                                                               MsaColorScheme *colorScheme,
                                                               int seq, int pos,
                                                               int refSeq)
{
    SAFE_POINT(colorScheme != NULL, tr("Color scheme is NULL"), false);
    SAFE_POINT(highlightingScheme != NULL, tr("Highlighting scheme is NULL"), false);
    SAFE_POINT(highlightingScheme->getFactory() != NULL, tr("Highlighting scheme factory is NULL"), false);
    QString schemeId = highlightingScheme->getFactory()->getId();

    if (seq == refSeq || isEmptyScheme(schemeId) ||
            ((refSeq == U2MsaRow::INVALID_ROW_ID) && !isGapScheme(schemeId) &&
            !highlightingScheme->getFactory()->isRefFree())) {
        if (colorScheme->getBackgroundColor(seq, pos, ma->charAt(seq, pos)) != QColor()) {
            return true;
        }
    }
    else {
        char refChar;
        if (isGapScheme(schemeId) || highlightingScheme->getFactory()->isRefFree()) {
            refChar = '\n';
        } else {
            refChar = ma->charAt(refSeq, pos);
        }

        char c = ma->charAt(seq, pos);
        bool highlight = false;
        QColor unused;
        highlightingScheme->process(refChar, c, unused, highlight, pos, seq);
        if (highlight) {
            return true;
        }
    }

    return false;
}

int MaHighlightingOverviewCalculationTask::getGraphValue(int pos) const {
    CHECK(seqNumber != 0, 0);

    int counter = 0;
    for (int i = 0; i < seqNumber; i++) {
        if ( isCellHighlighted(i, pos) ) {
            counter++;
        }
    }

    return 100 * counter / seqNumber;
}

bool MaHighlightingOverviewCalculationTask::isGapScheme(const QString &schemeId) {
    return (schemeId == MsaHighlightingScheme::GAPS);
}

bool MaHighlightingOverviewCalculationTask::isEmptyScheme(const QString &schemeId) {
    return (schemeId == MsaHighlightingScheme::EMPTY);
}

bool MaHighlightingOverviewCalculationTask::isCellHighlighted(int seq, int pos) const {
    return isCellHighlighted(ma, highlightingScheme, colorScheme, seq, pos, refSequenceId);
}

} // namespace
