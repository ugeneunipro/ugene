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

#ifndef _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_EXPORTER_H_
#define _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_EXPORTER_H_

#include <U2Core/DbiConnection.h>
#include <U2Core/MultipleChromatogramAlignment.h>

#include "McaRowInnerData.h"

namespace U2 {

class U2CORE_EXPORT MultipleChromatogramAlignmentExporter {
public:
    MultipleChromatogramAlignment getAlignment(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId) const;
    QMap<qint64, McaRowMemoryData> getMcaRowMemoryData(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId, const QList<qint64> rowIds) const;

private:
    QList<U2McaRow> exportRows(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId) const;
    QList<U2McaRow> exportRows(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId, const QList<qint64> rowIds) const;
    QList<McaRowMemoryData> exportDataOfRows(U2OpStatus &os, const QList<U2McaRow> &rows) const;
    DNASequence exportSequence(U2OpStatus &os, const U2DataId &sequenceId) const;
    QVariantMap exportRowAdditionalInfo(U2OpStatus &os, const U2DataId &chromatogramId) const;
    QVariantMap exportAlignmentInfo(U2OpStatus &os, const U2DataId &mcaId) const;
    U2Mca exportAlignmentObject(U2OpStatus &os, const U2DataId &mcaId) const;

    mutable DbiConnection connection;
};

}   // namespace U2

#endif // _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_EXPORTER_H_
