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

#include <U2Algorithm/RollingArray.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>

#include "DNAGraphPackPlugin.h"

#include "BaseContentGraph.h"

/* TRANSLATOR U2::BaseContentGraphFactory */

namespace U2 {

static QString nameByType(BaseContentGraphFactory::GType t) {
    if (t == BaseContentGraphFactory::AG) {
        return BaseContentGraphFactory::tr("AG Content (%)");
    }
    return BaseContentGraphFactory::tr("GC Content (%)");

}

BaseContentGraphFactory::BaseContentGraphFactory(GType t, QObject* p)
: GSequenceGraphFactory(nameByType(t), p) , map(256, false)
{
    if ( t == BaseContentGraphFactory::AG) {
        map['A']=map['G']=true;
    } else {
        map['G']=map['C']=true;
    }
}

bool BaseContentGraphFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic();
}

QList<QSharedPointer<GSequenceGraphData> > BaseContentGraphFactory::createGraphs(GSequenceGraphView* v) {
    Q_UNUSED(v);
    QList<QSharedPointer<GSequenceGraphData> > res;
    assert(isEnabled(v->getSequenceObject()));
    QSharedPointer<GSequenceGraphData> d = QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(getGraphName()));
    d->ga = new BaseContentGraphAlgorithm(map);
    res.append(d);
    return res;
}


//////////////////////////////////////////////////////////////////////////
// BaseContentGraphAlgorithm

BaseContentGraphAlgorithm::BaseContentGraphAlgorithm(const QBitArray& _map)  :  map(_map)
{
}

void BaseContentGraphAlgorithm::windowStrategyWithoutMemorize(QVector<float> &res, const QByteArray &seq,
    int startPos, const GSequenceGraphWindowData *d, int nSteps, U2OpStatus &os)
{
    for (int i = 0; i < nSteps; i++) {
        int start = startPos + i * d->step;
        int end = start + d->window;
        int base_count = 0;
        for (int x = start; x < end; x++) {
            CHECK_OP(os, );
            char c = seq[x];
            if (map[(uchar)c]) {
                base_count++;
            }
        }
        res.append((base_count / (float)(d->window))*100);
    }
}

void BaseContentGraphAlgorithm::calculate(QVector<float> &res, U2SequenceObject *o, const U2Region &vr,
    const GSequenceGraphWindowData *d, U2OpStatus &os)
{
    SAFE_POINT(d != NULL, L10N::nullPointerError("window graph"), );
    SAFE_POINT(o != NULL, L10N::nullPointerError("sequence object"), );

    int nSteps = GSequenceGraphUtils::getNumSteps(vr, d->window, d->step);
    res.reserve(nSteps);
    const QByteArray &seq = getSequenceData(o, os);
    CHECK_OP(os, );
    int startPos = vr.startPos;
    windowStrategyWithoutMemorize(res, seq, startPos, d, nSteps, os);
}

} // namespace

