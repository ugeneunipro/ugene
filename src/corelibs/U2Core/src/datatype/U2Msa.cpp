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

#include "U2Msa.h"

namespace U2 {

U2MsaGap::U2MsaGap(qint64 _startPos, qint64 _length)
    : startPos((int)_startPos),
      length((int)_length) {
}

int U2MsaGap::endPos() const {
    return startPos + length;
}

bool U2MsaGap::isValid() const {
    return startPos >= 0 && length > 0;
}

bool U2MsaGap::operator==(const U2MsaGap& g) const {
    return ((startPos == g.startPos) && (length == g.length));
}

bool U2MsaGap::lessThan(const U2MsaGap& first, const U2MsaGap& second) {
    return first.startPos < second.startPos;
}

U2MsaGap U2MsaGap::intersect(const U2MsaGap& anotherGap) const {
    qint64 newOffset = qMax(startPos, anotherGap.startPos);
    qint64 newEnd = qMin(endPos(), anotherGap.endPos());
    if (newOffset > newEnd) {
        return {};
    }
    return {newOffset, newEnd - newOffset};
}

U2MsaGap::operator U2Region() const {
    return {startPos, length};
}

const qint64 U2MsaRow::INVALID_ROW_ID = -1;

const char U2Msa::GAP_CHAR = '-';
const char U2Msa::INVALID_CHAR = '\0';

U2Msa::U2Msa(const U2DataType& _type)
    : type(_type) {
}

U2Msa::U2Msa(const U2DataType& _type, const U2DataId& id, const QString& dbId, qint64 version)
    : U2Object(id, dbId, version), type(_type) {
}

U2DataType U2Msa::getType() const {
    return type;
}

}  // namespace U2
