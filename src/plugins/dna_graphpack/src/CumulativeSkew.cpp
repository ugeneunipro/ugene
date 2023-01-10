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

#include "CumulativeSkew.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>

#include "DNAGraphPackPlugin.h"

/* TRANSLATOR U2::CumulativeSkewGraphFactory */

/**
 *A Grigoriev  It is used to predict origin and terminus locations in
 * bacterial and archaeal genomes
 **/

namespace U2 {
static QString nameByType(CumulativeSkewGraphFactory::GCumulativeSkewType t) {
    if (t == CumulativeSkewGraphFactory::AT) {
        return CumulativeSkewGraphFactory::tr("AT Cumulative Skew");
    }
    return CumulativeSkewGraphFactory::tr("GC Cumulative Skew");
}

CumulativeSkewGraphFactory::CumulativeSkewGraphFactory(GCumulativeSkewType t, QObject* p)
    : GSequenceGraphFactory(nameByType(t), p) {
    if (t == CumulativeSkewGraphFactory::AT) {
        cumPair.first = 'A';
        cumPair.second = 'T';
    } else {
        cumPair.first = 'G';
        cumPair.second = 'C';
    }
}

bool CumulativeSkewGraphFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic();
}

QList<QSharedPointer<GSequenceGraphData>> CumulativeSkewGraphFactory::createGraphs(GSequenceGraphView* view) {
    assert(isEnabled(view->getSequenceObject()));
    return {QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, getGraphName(), new CumulativeSkewGraphAlgorithm(cumPair)))};
}

//////////////////////////////////////////////////////////////////////////
// CumulativeSkewGraphAlgorithm

CumulativeSkewGraphAlgorithm::CumulativeSkewGraphAlgorithm(const QPair<char, char>& _p)
    : p(_p) {
}

float CumulativeSkewGraphAlgorithm::getValue(int begin, int end, const QByteArray& seq) const {
    int leap = end - begin;
    float resultValue = 0;
    int len;
    for (int window = 0; window < end; window += leap) {
        int first = 0;
        int second = 0;
        if (window + leap > end)
            len = window - end;
        else
            len = leap;
        for (int i = 0; i < len; ++i) {
            char c = seq[window + i];
            if (c == p.first) {
                first++;
                continue;
            }
            if (c == p.second) {
                second++;
            }
        }
        if (first + second > 0)
            resultValue += (float)(first - second) / (first + second);
    }
    return resultValue;
}

void CumulativeSkewGraphAlgorithm::calculate(QVector<float>& result, U2SequenceObject* sequenceObject, qint64 window, qint64 step, U2OpStatus& os) {
    U2Region vr(0, sequenceObject->getSequenceLength());
    int nSteps = GSequenceGraphUtils::getNumSteps(vr, window, step);
    result.reserve(nSteps);

    QByteArray seq = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );

    for (int i = 0; i < nSteps; i++) {
        CHECK_OP(os, );
        int start = vr.startPos + i * step;
        int end = start + window;
        float value = getValue(start, end, seq);
        result.append(value);
    }
}

}  // namespace U2
