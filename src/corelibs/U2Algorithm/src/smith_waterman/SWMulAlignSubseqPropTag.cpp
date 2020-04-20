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

#include "SWMulAlignSubseqPropTag.h"
#include <U2Core/U2Region.h>

namespace U2 {

QString SWMulAlignSubseqPropTag::expandTag(const QVariant & argument) const {
    assert(argument.canConvert<U2Region>());
    U2Region subsequence = argument.value<U2Region>();

    quint64 tagExpansion = 0;
    switch(posType) {
        case START:
            tagExpansion = subsequence.startPos + 1;
            break;
        case END:
            tagExpansion = subsequence.endPos();
            break;
        case LENGTH:
            tagExpansion = subsequence.length;
            break;
        default:
            assert(0);
    }

    return QString::number(tagExpansion);
}

} //namespace
