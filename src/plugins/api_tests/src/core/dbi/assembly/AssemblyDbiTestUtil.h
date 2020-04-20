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

#ifndef _ASSEMBLY_DBI_TEST_UTIL_H_
#define _ASSEMBLY_DBI_TEST_UTIL_H_

#include <U2Core/U2Assembly.h>


namespace U2 {

class AssemblyDbiTestUtil {
public:
    static bool compareCigar(const QList<U2CigarToken>& c1, const QList<U2CigarToken>& c2);

    static bool compareReads(const U2AssemblyRead& r1, const U2AssemblyRead& r2);

    static bool findRead(const U2AssemblyRead& subj, QList<U2AssemblyRead>& reads);

    static bool compareReadLists(U2DbiIterator<U2AssemblyRead>* iter, QList<U2AssemblyRead>& expectedReads);

    static void var2readList(const QVariantList& varList, QList<U2AssemblyRead>& reads);

public:
    static const char* ERR_INVALID_ASSEMBLY_ID;
};

} //namespace

Q_DECLARE_METATYPE(U2::U2AssemblyRead);

#endif
