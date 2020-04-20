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

#ifndef _U2_DNA_CHROMATOGRAM_H_
#define _U2_DNA_CHROMATOGRAM_H_

#include <QVector>
#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT DNAChromatogram {
public:
    enum Trace {
        Trace_A,
        Trace_C,
        Trace_G,
        Trace_T,
    };

    DNAChromatogram();

    QString name;
    int traceLength;
    int seqLength;
    QVector<ushort> baseCalls;
    QVector<ushort> A;
    QVector<ushort> C;
    QVector<ushort> G;
    QVector<ushort> T;
    QVector<char> prob_A;
    QVector<char> prob_C;
    QVector<char> prob_G;
    QVector<char> prob_T;
    bool hasQV;

    ushort getValue(Trace trace, qint64 position) const;

    bool operator ==(const DNAChromatogram &otherChromatogram) const;

    static const ushort INVALID_VALUE;
    static const char   DEFAULT_PROBABILITY;
};

} //namespace

#endif
