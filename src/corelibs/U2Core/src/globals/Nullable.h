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
#include <U2Core/U2SafePoints.h>

namespace U2 {
/**
 * NP is 'Nullable Pointer' (a shortcut).
 * Helper class to handle nullable pointer values in a safer (than default) and explicit way.
 * Any access to the pointer value before null-check will trigger SAFE_POINT.
 */
template<typename T>
class NP {
public:
    NP(T* _value)
        : value(_value) {
    }

    bool operator==(const T* anotherValue) const {
        isChecked = true;
        return value == anotherValue;
    }

    bool operator!=(const T* anotherValue) const {
        isChecked = true;
        return value != anotherValue;
    }

    /**
     * Returns pointer value that is guaranteed not to be 'nullptr'.
     * Requires that one of the equality check operators (==/!=) is called before this method.
     * Use 'getNullable' to access the pointer value directly with no checks.
     */
    T* get() const {
        SAFE_POINT(isChecked, "Nullable::get is called with null checks for null!", value);
        SAFE_POINT(value != nullptr, "Nullable::get is called for a nullptr value!", value);
        return value;
    }

    /**
     * Returns original value with no null-checks.
     * May be used in code like `if (auto v = ptr.getNullable()) {...}`.
     */
    T* getNullable() const {
        return value;
    }

private:
    T* value;
    mutable bool isChecked = false;
};

}  // namespace U2
