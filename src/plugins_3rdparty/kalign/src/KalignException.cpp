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

#include "KalignException.h"
#include <assert.h>
#include <string.h>

extern "C" void throwKalignException(char *message) {
    throw U2::KalignException(message);
}

extern "C" void checkAllocatedMemory(void *ptr) {
    if (NULL == ptr) {
        throw U2::KalignException("Not enough memory to finish KAlign task");
    }
}

namespace U2 {

KalignException::KalignException(const char* _str) {
    int len = strlen(_str);
    assert(len < 4096);
    memcpy(str, _str, len);
    str[len] = '\0';
}

KalignException::KalignException() {
    memset(str, '\0', 4096);
}

} // namespace U2
