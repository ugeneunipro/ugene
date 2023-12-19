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

#include <U2Core/U2Alphabet.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

/**
    Gap model for Multiple Alignment: for every row it keeps gaps map
*/

class U2MsaGap;

/**
 * Gap represention in MSA row sequence.
 * Same as U2Region but uses 32 bit values (we do not support MSA sizes above 32 bits).
 *
 * 'startPos' in the gap is a position in the gapped sequence.
 *
 * TODO: create a templated U2Region<size> variant and make U2MsaGap to extend it.
 */
class U2CORE_EXPORT U2MsaGap {
public:
    U2MsaGap() = default;

    U2MsaGap(qint64 startPos, qint64 length);

    int endPos() const;  // not inclusive

    /** The gap is valid if it has a length >= 0 & startPos >=0. */
    bool isValid() const;

    bool operator==(const U2MsaGap& g) const;

    /** Compares 2 gaps by 'startPos'. */
    static bool lessThan(const U2MsaGap& first, const U2MsaGap& second);

    /** Returns another gap (region) that is intersection of this and 'anotherGap' regions. */
    U2MsaGap intersect(const U2MsaGap& anotherGap) const;

    operator U2Region() const;

    /** Offset of the gap in the gapped sequence*/
    int startPos = 0;

    /** Number of gaps */
    int length = 0;
};

/**
    Row of multiple alignment: gaps map and sequence id
*/
class U2CORE_EXPORT U2MsaRow {
public:
    U2MsaRow() = default;

    bool hasValidChildObjectIds() const;

    /** Id of the row in the database */
    qint64 rowId = INVALID_ROW_ID;

    /** Id of the sequence of the row in the database */
    U2DataId sequenceId;

    /** Start of the row in the sequence */
    qint64 gstart = 0;  // TODO: rename or remove, if it is not used

    /** End of the row in the sequence */
    qint64 gend = 0;

    /** A gap model for the row */
    QVector<U2MsaGap> gaps;

    /** Length of the sequence characters and gaps of the row (without trailing) */
    qint64 length = 0;

    U2DataId chromatogramId;

    static const qint64 INVALID_ROW_ID;
};


/** Shared database model for MSA and MCA. */
class U2CORE_EXPORT U2Msa : public U2Object {
public:
    U2Msa(const U2DataType& type);
    U2Msa(const U2DataType& type, const U2DataId& id, const QString& dbId, qint64 version);

    U2DataType getType() const override;

    /** Alignment alphabet. All sequence in alignment must have alphabet that fits into alignment alphabet */
    U2AlphabetId alphabet;

    /** Length of the alignment */
    qint64 length = 0;

    U2DataType type = U2Type::Msa;

    static const char GAP_CHAR;
    static const char INVALID_CHAR;
};

}  // namespace U2
