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

#include <U2Core/Chromatogram.h>
#include <U2Core/DNASequence.h>
#include <U2Core/Msa.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2Msa.h>

namespace U2 {

struct U2CORE_EXPORT MsaRowSnapshot {
    /** Row ID in the database. */
    qint64 rowId;
    DNASequence sequence;
    U2DataId chromatogramId;
    Chromatogram chromatogram;
    QVector<U2MsaGap> gaps;
    qint64 rowLength;
    QVariantMap additionalInfo;
};

/** Getting a multiple sequence alignment from DBI */
class U2CORE_EXPORT MsaExportUtils {
public:
    MsaExportUtils() = default;

    /** Returns in-memory alignment model. */
    static Msa loadAlignment(const U2DbiRef& dbiRef, const U2DataId& msaId, U2OpStatus& os);

    /** Returns in-memory rows model. */
    static QList<MsaRowSnapshot> loadRows(const U2DbiRef& dbiRef,
                                          const U2DataId& msaId,
                                          const QList<qint64>& rowIds,
                                          U2OpStatus& os);

private:
    static QList<U2MsaRow> readRows(const U2DataId&, U2OpStatus&, const DbiConnection& connection);
    static QList<U2MsaRow> readRows(const U2DataId&, const QList<qint64>& rowIds, U2OpStatus&, const DbiConnection& connection);
    static QList<MsaRowSnapshot> loadRows(const QList<U2MsaRow>& rows, U2OpStatus& os, const DbiConnection& connection);
    static QVariantMap loadAlignmentInfo(const U2DataId& dataId, U2OpStatus& os, const DbiConnection& connection);
};

}  // namespace U2
