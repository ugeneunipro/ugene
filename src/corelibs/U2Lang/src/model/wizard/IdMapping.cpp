/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "IdMapping.h"

namespace U2 {

IdMapping::IdMapping(const QString& _srcId, const QString& _dstId)
    : srcId(_srcId), dstId(_dstId) {
}

IdMapping::~IdMapping() {
}

QString IdMapping::getSrcId() const {
    return srcId;
}

QString IdMapping::getDstId() const {
    return dstId;
}

void IdMapping::invertMapping() {
    qSwap<QString>(srcId, dstId);
}

}  // namespace U2
