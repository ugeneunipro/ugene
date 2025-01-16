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

#pragma once

#include <U2Core/U2Alphabet.h>
#include <U2Core/U2Type.h>

namespace U2 {

/** Sequence representation in DBI. */
class U2CORE_EXPORT U2Sequence : public U2Object {
public:
    U2Sequence() = default;

    U2Sequence(const U2DataId& id, const QString& dbId, qint64 version)
        : U2Object(id, dbId, version) {
    }

    /** Sequence alphabet id. */
    U2AlphabetId alphabet;

    /** Length of the sequence. */
    qint64 length = 0;

    /** A flag to mark that sequence is circular. */
    bool circular = false;

    U2DataType getType() const override {
        return U2Type::Sequence;
    }
};

}  // namespace U2
