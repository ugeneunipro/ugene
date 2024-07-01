/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2Core/ChromatogramObject.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

class U2MsaGap;
class U2CORE_EXPORT ChromatogramUtils {
public:
    static void append(Chromatogram chromatogram, const Chromatogram& appendedChromatogram);
    static void removeBaseCalls(U2OpStatus& os, Chromatogram& chromatogram, int startPos, int endPos);
    static void removeRegion(U2OpStatus& os, Chromatogram& chromatogram, int startPos, int endPos);
    static bool checkAllFieldsEqual(const Chromatogram& first, const Chromatogram& second);
    static void crop(Chromatogram& chromatogram, int startPos, int length);
    static U2EntityRef import(U2OpStatus& os, const U2DbiRef& dbiRef, const QString& folder, const Chromatogram& chromatogram);
    static Chromatogram exportChromatogram(U2OpStatus& os, const U2EntityRef& chromatogramRef);
    static U2Chromatogram getChromatogramDbInfo(U2OpStatus& os, const U2EntityRef& chromatogramRef);

    static void updateChromatogramData(U2OpStatus& os, const U2EntityRef& chromatogramRef, const Chromatogram& chromatogram);
    static void updateChromatogramData(U2OpStatus& os, const U2DataId& masterId, const U2EntityRef& chromatogramRef, const Chromatogram& chromatogram);

    static U2EntityRef getChromatogramIdByRelatedSequenceId(U2OpStatus& os, const U2EntityRef& sequenceRef);
    static QString getChromatogramName(U2OpStatus& os, const U2EntityRef& chromatogramRef);
    static Chromatogram reverse(const Chromatogram& chromatogram);
    static Chromatogram complement(const Chromatogram& chromatogram);
    static Chromatogram reverseComplement(const Chromatogram& chromatogram);
    static U2Region sequenceRegion2TraceRegion(const Chromatogram& chromatogram, const U2Region& sequenceRegion);
    static void insertBase(Chromatogram& chromatogram, int posUngapped, const QVector<U2MsaGap>& gapModel, int posWithGaps);
    static Chromatogram getGappedChromatogram(const Chromatogram& chrom, const QVector<U2MsaGap>& gapModel);
};

}  // namespace U2
