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

#include "GCFramePlot.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include "DNAGraphPackPlugin.h"
#include <U2Algorithm/RollingArray.h>


namespace U2 {

#define OFFSET_NULL "Frame 1"
#define OFFSET_ONE    "Frame 2"
#define OFFSET_TWO  "Frame 3"

GCFramePlotFactory::GCFramePlotFactory(QObject* p)
: GSequenceGraphFactory("GC Frame Plot", p)
{

}

bool GCFramePlotFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic();
}

QList<QSharedPointer<GSequenceGraphData> > GCFramePlotFactory::createGraphs(GSequenceGraphView* v) {
    Q_UNUSED(v);

    //TODO: All points should be calculated during one loop over the window.

    QList<QSharedPointer<GSequenceGraphData> > res;
    assert(isEnabled(v->getSequenceObject()));
    QSharedPointer<GSequenceGraphData> d = QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(OFFSET_NULL));
    d->ga = new GCFramePlotAlgorithm(0);
    res.append(d);

    QSharedPointer<GSequenceGraphData> d2 = QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(OFFSET_ONE));
    d2->ga = new GCFramePlotAlgorithm(1);
    res.append(d2);

    QSharedPointer<GSequenceGraphData> d3 = QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(OFFSET_TWO));
    d3->ga = new GCFramePlotAlgorithm(2);
    res.append(d3);

    return res;
}

GSequenceGraphDrawer* GCFramePlotFactory::getDrawer(GSequenceGraphView* v) {
    GSequenceGraphDrawer* d =  GSequenceGraphFactory::getDrawer(v);

    QMap<QString, QColor> colors;
    colors.insert(OFFSET_NULL, Qt::red);
    colors.insert(OFFSET_ONE, Qt::green);
    colors.insert(OFFSET_TWO, Qt::blue);

    d->setColors(colors);
    return d;
}


//////////////////////////////////////////////////////////////////////////
// GCFramePlotAlgorithm

GCFramePlotAlgorithm::GCFramePlotAlgorithm( int _offset )
:map(256, false), offset(_offset)
{
    map['G'] = map['C'] = true;
}

void GCFramePlotAlgorithm::windowStrategyWithoutMemorize(QVector<float>& res, const QByteArray& seq, int startPos,
    const GSequenceGraphWindowData* d, int nSteps, U2OpStatus &os)
{
    for (int i = 0; i < nSteps; i++) {
        int start = startPos + i * d->step;
        int end = start + d->window;
        int base_count = 0;

        while (start % 3 != offset) {
            start++;
        }

        for (int x = start; x < end; x += 3) {
            CHECK_OP(os, );
            char c = seq[x];
            if (map[(uchar)c]) {
                base_count++;
            }
        }
        res.append((base_count / (float)(d->window))*100*3);
    }
}

void GCFramePlotAlgorithm::calculate(QVector<float>& res, U2SequenceObject* o, const U2Region& vr,
    const GSequenceGraphWindowData* d, U2OpStatus &os)
{
    assert(d!=NULL);
    int nSteps = GSequenceGraphUtils::getNumSteps(vr, d->window, d->step);
    res.reserve(nSteps);
    const QByteArray &seq = getSequenceData(o, os);
    CHECK_OP(os, );
    int startPos = vr.startPos;
    windowStrategyWithoutMemorize(res, seq, startPos, d, nSteps, os);
}

} // namespace

