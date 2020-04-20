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

#ifndef _U2_MULTIPLE_SEQUENCE_ALIGNMENT_EXPORTER_H_
#define _U2_MULTIPLE_SEQUENCE_ALIGNMENT_EXPORTER_H_

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

struct MsaRowReplacementData {
    MsaRowReplacementData(const DNASequence &_sequence, const U2MsaRow &_row)
        : sequence(_sequence), row(_row) {}

    DNASequence sequence;
    U2MsaRow row;
};

/** Getting a multiple sequence alignment from DBI */
class U2CORE_EXPORT MultipleSequenceAlignmentExporter {
public:
    MultipleSequenceAlignmentExporter();

    MultipleSequenceAlignment                          getAlignment(const U2DbiRef& dbiRef, const U2DataId& msaId,
        U2OpStatus& os) const;
    U2Msa                               getAlignmentObject(const U2DbiRef& dbiRef, const U2DataId& msaId, U2OpStatus& os) const;
    QList<MsaRowReplacementData> getAlignmentRows(const U2DbiRef& dbiRef, const U2DataId& msaId,
        const QList<qint64> rowIds, U2OpStatus& os) const;

private:
    QList<U2MsaRow>                     exportRows(const U2DataId&, U2OpStatus&) const;
    QList<U2MsaRow>                     exportRows(const U2DataId&, const QList<qint64> rowIds, U2OpStatus&) const;
    QList<DNASequence>                  exportSequencesOfRows(QList<U2MsaRow>, U2OpStatus&) const;
    QVariantMap                         exportAlignmentInfo(const U2DataId&, U2OpStatus&) const;
    U2Msa                               exportAlignmentObject(const U2DataId&, U2OpStatus&) const;

    mutable DbiConnection                       con;
};

} // namespace

#endif // _U2_MULTIPLE_SEQUENCE_ALIGNMENT_EXPORTER_H_
