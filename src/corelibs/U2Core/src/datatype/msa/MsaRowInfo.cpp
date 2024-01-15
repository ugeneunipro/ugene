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

#include "MsaRowInfo.h"

namespace U2 {

const QString MsaRowInfo::REVERSED = "reversed";
const QString MsaRowInfo::COMPLEMENTED = "complemented";

void MsaRowInfo::setReversed(QVariantMap& info, bool reversed) {
    info[REVERSED] = reversed;
}

bool MsaRowInfo::getReversed(const QVariantMap& info) {
    return info.value(REVERSED, false).toBool();
}

void MsaRowInfo::setComplemented(QVariantMap& info, bool complemented) {
    info[COMPLEMENTED] = complemented;
}

bool MsaRowInfo::getComplemented(const QVariantMap& info) {
    return info.value(COMPLEMENTED, false).toBool();
}

}  // namespace U2
