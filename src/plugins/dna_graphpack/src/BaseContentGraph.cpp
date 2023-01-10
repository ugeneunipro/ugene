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

#include "BaseContentGraph.h"

#include <U2Algorithm/RollingArray.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>

#include "DNAGraphPackPlugin.h"

/* TRANSLATOR U2::BaseContentGraphFactory */

namespace U2 {

static QString nameByType(BaseContentGraphFactory::GType t) {
    if (t == BaseContentGraphFactory::AG) {
        return BaseContentGraphFactory::tr("AG Content (%)");
    }
    return BaseContentGraphFactory::tr("GC Content (%)");
}

BaseContentGraphFactory::BaseContentGraphFactory(GType t, QObject* p)
    : GSequenceGraphFactory(nameByType(t), p), map(256, false) {
    if (t == BaseContentGraphFactory::AG) {
        map['A'] = map['G'] = true;
    } else {
        map['G'] = map['C'] = true;
    }
}

bool BaseContentGraphFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic();
}

QList<QSharedPointer<GSequenceGraphData>> BaseContentGraphFactory::createGraphs(GSequenceGraphView* view) {
    assert(isEnabled(view->getSequenceObject()));
    return {QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, getGraphName(), new BaseContentGraphAlgorithm(map)))};
}

//////////////////////////////////////////////////////////////////////////
// BaseContentGraphAlgorithm

BaseContentGraphAlgorithm::BaseContentGraphAlgorithm(const QBitArray& _map)
    : map(_map) {
}

void BaseContentGraphAlgorithm::windowStrategyWithoutMemorize(QVector<float>& res,
                                                              const QByteArray& seq,
                                                              qint64 startPos,
                                                              qint64 window,
                                                              qint64 step,
                                                              qint64 nSteps,
                                                              U2OpStatus& os) {
    for (int i = 0; i < nSteps; i++) {
        int start = startPos + i * step;
        int end = start + window;
        int base_count = 0;
        for (int x = start; x < end; x++) {
            CHECK_OP(os, );
            char c = seq[x];
            if (map[(uchar)c]) {
                base_count++;
            }
        }
        res.append((base_count / (float)window) * 100);
    }
}

void BaseContentGraphAlgorithm::calculate(QVector<float>& result, U2SequenceObject* sequenceObject, qint64 window, qint64 step, U2OpStatus& os) {
    U2Region vr(0, sequenceObject->getSequenceLength());
    int nSteps = GSequenceGraphUtils::getNumSteps(vr, window, step);
    result.reserve(nSteps);
    QByteArray seq = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    windowStrategyWithoutMemorize(result, seq, vr.startPos, window, step, nSteps, os);
}

}  // namespace U2
