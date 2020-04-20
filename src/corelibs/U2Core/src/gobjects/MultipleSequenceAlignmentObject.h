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

#ifndef _U2_MULTIPLE_SEQUENCE_ALIGNMENT_OBJECT_H_
#define _U2_MULTIPLE_SEQUENCE_ALIGNMENT_OBJECT_H_

#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class U2CORE_EXPORT MultipleSequenceAlignmentObject : public MultipleAlignmentObject {
    Q_OBJECT
public:
    MultipleSequenceAlignmentObject(const QString &name,
                                    const U2EntityRef &msaRef,
                                    const QVariantMap &hintsMap = QVariantMap(),
                                    const MultipleSequenceAlignment &msaData = MultipleSequenceAlignment());

    const MultipleSequenceAlignment getMsa() const;
    const MultipleSequenceAlignment getMsaCopy() const;

    /** GObject methods */
    //Actually this method doesn't exactly clone MSA database rows, row ID will be generated for each copied row again
    virtual MultipleSequenceAlignmentObject* clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;

    /** Const getters */
    char charAt(int seqNum, qint64 position) const;
    const MultipleSequenceAlignmentRow getMsaRow(int row) const;

    /**
     * Updates a gap model of the alignment.
     * The map must contain valid row IDs and corresponding gap models.
     */
    void updateGapModel(U2OpStatus &os, const U2MsaMapGapModel &rowsGapModel);
    void updateGapModel(const QList<MultipleSequenceAlignmentRow> &sourceRows);

    void crop(const U2Region &window, const QSet<QString> &rowNames);
    void crop(const U2Region &window, const QList<qint64> &rowIds);
    void crop(const U2Region &window);

    /** Methods to work with rows */
    void updateRow(U2OpStatus &os, int rowIdx, const QString &name, const QByteArray &seqBytes, const U2MsaRowGapModel &gapModel);

    /** Replace character in row and change alphabet, if it does not contain the character
    */
    void replaceCharacter(int startPos, int rowIndex, char newChar);

    void deleteColumnsWithGaps(U2OpStatus &os, int requiredGapsCount = -1);

    void insertGap(const U2Region& rows, int pos, int nGaps);

    void insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps);

private:
    void loadAlignment(U2OpStatus &os);
    void updateCachedRows(U2OpStatus &os, const QList<qint64> &rowIds);
    void updateDatabase(U2OpStatus &os, const MultipleAlignment &ma);

    void removeRowPrivate(U2OpStatus &os, const U2EntityRef &msaRef, qint64 rowId);
    void removeRegionPrivate(U2OpStatus &os, const U2EntityRef &maRef, const QList<qint64> &rows,
                             int startPos, int nBases);

};

}   // namespace U2

#endif // _U2_MULTIPLE_SEQUENCE_ALIGNMENT_OBJECT_H_
