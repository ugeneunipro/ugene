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

#ifndef _U2_DI_PROPERTIES_H_
#define _U2_DI_PROPERTIES_H_

#include <assert.h>

#include <QObject>
#include <QString>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT DiProperty : public QObject {
    Q_OBJECT
public:
    static int inline index(char c1, char c2);
    static int inline index(char c);
    static char inline fromIndex(int index);
    static char inline fromIndexLo(int index);
    static char inline fromIndexHi(int index);
};

int DiProperty::index(char n1, char n2) {
    int idx = (index(n1) << 2) + index(n2);
    assert(idx >= 0 && idx < 16);
    return idx;
}

int DiProperty::index(char c) {
    if (c == 'A') { return 0; }
    if (c == 'C') { return 1; }
    if (c == 'G') { return 2; }
    if (c == 'T' || c == 'U') { return 3; }
    return 0; //default is 'A'
}

char DiProperty::fromIndex(int index) {
    assert(index >= 0);
    assert(index < 4);
    char a[] = "ACGT";
    return a[index];
}

char DiProperty::fromIndexLo(int index) {
    assert(index >= 0);
    assert(index < 16);
    return fromIndex(index & 3);
}

char DiProperty::fromIndexHi(int index) {
    assert(index >= 0);
    assert(index < 16);
    return fromIndex(index >> 2);
}

}//namespace

#endif

