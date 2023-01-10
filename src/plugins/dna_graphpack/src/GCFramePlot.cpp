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

#include "GCFramePlot.h"

#include <U2Algorithm/RollingArray.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>

#include "DNAGraphPackPlugin.h"

namespace U2 {

#define OFFSET_NULL "Frame 1"
#define OFFSET_ONE "Frame 2"
#define OFFSET_TWO "Frame 3"

GCFramePlotFactory::GCFramePlotFactory(QObject* p)
    : GSequenceGraphFactory("GC Frame Plot", p) {
}

bool GCFramePlotFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic();
}

QList<QSharedPointer<GSequenceGraphData>> GCFramePlotFactory::createGraphs(GSequenceGraphView* view) {
    // TODO: All points should be calculated during one loop over the window.

    assert(isEnabled(view->getSequenceObject()));
    return {
        QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, OFFSET_NULL, new GCFramePlotAlgorithm(0))),
        QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, OFFSET_ONE, new GCFramePlotAlgorithm(1))),
        QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, OFFSET_TWO, new GCFramePlotAlgorithm(2))),
    };
}

GSequenceGraphDrawer* GCFramePlotFactory::getDrawer(GSequenceGraphView* v) {
    GSequenceGraphDrawer* d = GSequenceGraphFactory::getDrawer(v);

    QMap<QString, QColor> colors;
    colors.insert(OFFSET_NULL, Qt::red);
    colors.insert(OFFSET_ONE, Qt::green);
    colors.insert(OFFSET_TWO, Qt::blue);

    d->setColors(colors);
    return d;
}

//////////////////////////////////////////////////////////////////////////
// GCFramePlotAlgorithm

GCFramePlotAlgorithm::GCFramePlotAlgorithm(int _offset)
    : offset(_offset) {
}

void GCFramePlotAlgorithm::windowStrategyWithoutMemorize(QVector<float>& res,
                                                         const QByteArray& seq,
                                                         int startPos,
                                                         qint64 window,
                                                         qint64 step,
                                                         qint64 nSteps,
                                                         U2OpStatus& os) {
    for (int i = 0; i < nSteps; i++) {
        int start = startPos + i * step;
        int end = start + window;
        int base_count = 0;

        while (start % 3 != offset) {
            start++;
        }

        for (int x = start; x < end; x += 3) {
            CHECK_OP(os, );
            char c = seq[x];
            if (c == 'C' || c == 'G') {
                base_count++;
            }
        }
        res.append((base_count / (float)window) * 100 * 3);
    }
}

void GCFramePlotAlgorithm::calculate(QVector<float>& result, U2SequenceObject* sequenceObject, qint64 window, qint64 step, U2OpStatus& os) {
    U2Region vr(0, sequenceObject->getSequenceLength());
    int nSteps = GSequenceGraphUtils::getNumSteps(vr, window, step);
    result.reserve(nSteps);
    QByteArray seq = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    windowStrategyWithoutMemorize(result, seq, vr.startPos, window, step, nSteps, os);
}

}  // namespace U2
