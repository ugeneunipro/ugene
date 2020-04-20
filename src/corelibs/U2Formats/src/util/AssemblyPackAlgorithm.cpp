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

#include "AssemblyPackAlgorithm.h"

#include <U2Core/Timer.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/Log.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2Region.h>

namespace U2 {

static qint64 selectProw(qint64* tails, qint64 start, qint64 end) {
    for (int i = 0; i < PACK_TAIL_SIZE; i++) {
        if (tails[i] <= start) {
            tails[i] = end;
            return i;
        }
    }
    return -1;
}

PackAlgorithmContext::PackAlgorithmContext() {
    maxProw = 0;
    nReads = 0;
    peakEnd = -1;
    peakRow = PACK_TAIL_SIZE;
    tails.resize(PACK_TAIL_SIZE);
}

#define PACK_TRACE_CHECKPOINT 100000

void AssemblyPackAlgorithm::pack(PackAlgorithmAdapter& adapter, U2AssemblyPackStat& stat, U2OpStatus& os) {
    //Algorithm idea:
    //  select * reads ordered by start position
    //  keep tack (tail) of used rows to assign packed row for reads (N elements)
    //  if all elements are used -> assign -1 to read and postprocess it later

    GTIMER(c1, t1, "AssemblyPackAlgorithm::pack");
    quint64 t0 = GTimer::currentTimeMicros();
    int nPacked = 0;

    stat.maxProw = 0;
    gauto_array<qint64> tails(new qint64[PACK_TAIL_SIZE]);
    qFill(tails.get(), tails.get() + PACK_TAIL_SIZE, -1);

    QScopedPointer< U2DbiIterator<PackAlgorithmData> > allReadsIterator(adapter.selectAllReads(os));
    PackAlgorithmContext ctx;
    while (allReadsIterator->hasNext() && !os.isCoR()) {
        PackAlgorithmData read = allReadsIterator->next();
        int prow = packRead(U2Region(read.leftmostPos, read.effectiveLen), ctx, os);
        adapter.assignProw(read.readId, prow, os);
        stat.maxProw = ctx.maxProw;

        if ((++nPacked % PACK_TRACE_CHECKPOINT) == 0) {
            perfLog.trace(QString("Assembly: number packed reads so far: %1 of %2 (%3%)").arg(nPacked).arg(stat.readsCount).arg(100 * nPacked / stat.readsCount));
        }
    }
    assert(stat.readsCount == nPacked);

    t1.stop();
    perfLog.trace(QString("Assembly: algorithm pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000 * 1000)));
}

int AssemblyPackAlgorithm::packRead(const U2Region& reg, PackAlgorithmContext& ctx, U2OpStatus&) {
    int prow = selectProw(ctx.tails.data(), reg.startPos, reg.endPos());
    if (prow == -1) {
        if (reg.startPos > ctx.peakEnd) {
            ctx.peakRow = PACK_TAIL_SIZE;
        }
        prow = ctx.peakRow;
        ctx.peakRow++;
        ctx.peakEnd = reg.endPos();
    }
    ctx.maxProw = qMax(prow, ctx.maxProw);
    return prow;
}

} //namespace
