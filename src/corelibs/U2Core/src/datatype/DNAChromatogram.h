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

#pragma once

#include <QSharedPointer>
#include <QVector>

#include <U2Core/global.h>

namespace U2 {

class ChromatogramData;

/** In-memory representation of the chromatogram. */
class U2CORE_EXPORT DNAChromatogram : public QSharedDataPointer<ChromatogramData> {
public:
    DNAChromatogram();
    DNAChromatogram(ChromatogramData* data);
};

class U2CORE_EXPORT ChromatogramData : public QSharedData {
public:
    enum class Trace {
        Trace_A = 0,
        Trace_C = 1,
        Trace_G = 2,
        Trace_T = 3,
    };

    /* Chromatogram trace and the corresponding peak height */
    struct TraceAndValue {
        TraceAndValue(Trace t, int v)
            : trace(t), value(v) {
        }

        // Chromatogram trace
        Trace trace = Trace::Trace_A;
        // Height of the @trace peak
        int value = 0;
    };

    ChromatogramData() = default;

    QString name = "chromatogram";
    int traceLength = 0;
    int seqLength = 0;
    QVector<ushort> baseCalls;
    QVector<ushort> A;
    QVector<ushort> C;
    QVector<ushort> G;
    QVector<ushort> T;
    QVector<char> prob_A;
    QVector<char> prob_C;
    QVector<char> prob_G;
    QVector<char> prob_T;
    bool hasQV = false;

    ushort getValue(const Trace& trace, qint64 position) const;

    bool operator==(const ChromatogramData& otherChromatogram) const;

    /* The "U2::DNAChromatogram::Trace" enum and the corresponding "char" symbol */
    static const QVarLengthArray<char, 4> BASE_BY_TRACE;

    bool isEmpty() const;
};

}  // namespace U2
