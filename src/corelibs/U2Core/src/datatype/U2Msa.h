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

#ifndef _U2_MSA_H_
#define _U2_MSA_H_

#include <U2Core/U2Alphabet.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

/**
    Gap model for Multiple Alignment: for every row it keeps gaps map
*/

class U2MsaGap;

typedef QList<U2MsaGap> U2MsaRowGapModel;
typedef QList<U2MsaRowGapModel> U2MsaListGapModel;
typedef QMap<qint64, U2MsaRowGapModel> U2MsaMapGapModel;

class U2CORE_EXPORT U2MsaGap  {
public:
    U2MsaGap();
    U2MsaGap(qint64 off, qint64 gap);

    qint64 endPos() const;    // not inclusive
    void setEndPos(qint64 endPos);    // not inclusive

    bool isValid() const;

    bool operator==(const U2MsaGap &g) const;

    static bool lessThan(const U2MsaGap &first, const U2MsaGap &second);

    U2MsaGap intersect(const U2MsaGap &anotherGap) const;

    operator U2Region() const;

    /** Offset of the gap in sequence*/
    qint64 offset;

    /** number of gaps */
    qint64 gap;
};

/**
    Row of multiple alignment: gaps map and sequence id
*/
class U2CORE_EXPORT U2MsaRow {
public:
    U2MsaRow();
    virtual ~U2MsaRow();

    bool isValid() const;

    /** Id of the row in the database */
    qint64          rowId;

    /** Id of the sequence of the row in the database */
    U2DataId        sequenceId;

    /** Start of the row in the sequence */
    qint64          gstart;         // TODO: rename or remove, if it is not used

    /** End of the row in the sequence */
    qint64          gend;

    /** A gap model for the row */
    QList<U2MsaGap> gaps;

    /** Length of the sequence characters and gaps of the row (without trailing) */
    qint64          length;

    static const qint64 INVALID_ROW_ID;
};

/**
    Multiple sequence alignment representation
*/
class U2CORE_EXPORT U2Msa : public U2Object {
public:
    U2Msa();
    U2Msa(const U2DataId &id, const QString &dbId, qint64 version);

    U2DataType getType() const;

    /** Alignment alphabet. All sequence in alignment must have alphabet that fits into alignment alphabet */
    U2AlphabetId    alphabet;

    /** Length of the alignment */
    qint64          length;

    static const char GAP_CHAR;
    static const char INVALID_CHAR;
};

}   // namespace U2

#endif // _U2_MSA_H_
