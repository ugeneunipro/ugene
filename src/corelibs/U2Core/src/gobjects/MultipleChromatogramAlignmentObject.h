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

#include <U2Core/GObject.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/MultipleAlignment.h>

namespace U2 {

class MultipleAlignment;
class U2SequenceObject;

class U2CORE_EXPORT MultipleChromatogramAlignmentObject : public MultipleAlignmentObject {
    Q_OBJECT
public:
    enum TrimEdge {
        Left,
        Right

    };

    static const QString MCAOBJECT_REFERENCE;

    MultipleChromatogramAlignmentObject(const QString& name,
                                        const U2EntityRef& mcaRef,
                                        const QVariantMap& hintsMap = QVariantMap(),
                                        const MultipleAlignment& mca = MultipleAlignment(MultipleAlignmentDataType::MCA));

    virtual ~MultipleChromatogramAlignmentObject();

    GObject* clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints = QVariantMap()) const;

    char charAt(int seqNum, qint64 position) const;

    U2SequenceObject* getReferenceObj() const;

    // inserts column of gaps with newChar at rowIndex row
    void insertCharacter(int rowIndex, int pos, char newChar);

    void insertGap(const U2Region& rows, int pos, int nGaps);

    void insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps);

    void deleteColumnsWithGaps(U2OpStatus& os);

    void trimRow(const int rowIndex, int currentPos, U2OpStatus& os, TrimEdge edge);

    /**
     * Updates the corresponding alternative mutations.
     * Set the second strongest character, if it's peak height persantage is more then @threshold and @showAlternativeMutations is true.
     */
    void updateAlternativeMutations(bool showAlternativeMutations, int threshold, U2OpStatus& os);

    void saveState();
    void releaseState();
    int getReferenceLengthWithGaps() const;

private:
    void loadAlignment(U2OpStatus& os);
    void updateCachedRows(U2OpStatus& os, const QList<qint64>& rowIds);
    void updateDatabase(U2OpStatus& os, const MultipleAlignment& ma);
    void removeRowPrivate(U2OpStatus& os, const U2EntityRef& mcaRef, qint64 rowId);
    void removeRegionPrivate(U2OpStatus& os, const U2EntityRef& maRef, const QList<qint64>& rows, int startPos, int nBases);

    QList<U2Region> getColumnsWithGaps() const;
    QVector<U2MsaGap> getReferenceGapModel() const;

    mutable U2SequenceObject* referenceObj;
};

}  // namespace U2
