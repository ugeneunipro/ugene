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

#include "U2Msa.h"

namespace U2 {

U2MsaGap::U2MsaGap()
    : offset(0),
      gap(0)
{

}

U2MsaGap::U2MsaGap(qint64 off, qint64 gap)
    : offset(off),
      gap(gap)
{

}

qint64 U2MsaGap::endPos() const {
    return offset + gap;
}

void U2MsaGap::setEndPos(qint64 newEndPos) {
    gap = newEndPos - offset;
}

bool U2MsaGap::isValid() const {
    return ((offset >= 0) && (gap > 0));
}

bool U2MsaGap::operator==(const U2MsaGap &g) const {
    return ((offset == g.offset) && (gap == g.gap));
}

bool U2MsaGap::lessThan(const U2MsaGap &first, const U2MsaGap &second) {
    return first.offset < second.offset;
}

U2MsaGap U2MsaGap::intersect(const U2MsaGap &anotherGap) const {
    const qint64 newOffset = qMax(offset, anotherGap.offset);
    const qint64 newEnd = qMin(endPos(), anotherGap.endPos());
    if (newOffset > newEnd) {
        return U2MsaGap();
    }
    return U2MsaGap(newOffset, newEnd - newOffset);
}

U2MsaGap::operator U2Region() const {
    return U2Region(offset, gap);
}

const qint64 U2MsaRow::INVALID_ROW_ID = -1;

U2MsaRow::U2MsaRow()
    : rowId(INVALID_ROW_ID),
      gstart(0),
      gend(0),
      length(0)
{

}

U2MsaRow::~U2MsaRow() {

}

const char U2Msa::GAP_CHAR = '-';
const char U2Msa::INVALID_CHAR = '\0';

U2Msa::U2Msa()
    : length(0)
{

}

U2Msa::U2Msa(const U2DataId &id, const QString &dbId, qint64 version)
    : U2Object(id, dbId, version)
{

}

U2DataType U2Msa::getType() const {
    return U2Type::Msa;
}

}   // namespace U2
