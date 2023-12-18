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

#include <U2Core/U2Region.h>
#include <U2Core/global.h>

namespace U2 {

class MultipleChromatogramAlignment;
class U2EntityRef;
class U2MsaRow;
class U2OpStatus;

class U2CORE_EXPORT McaDbiUtils : public QObject {
public:
    static void updateMca(U2OpStatus& os, const U2EntityRef& mcaRef, const MultipleChromatogramAlignment& mca);

    /*
     * Replaces the set of characters.
     * It's better than use @replaceCharacterInRow several times, because this function opens database just one time.
     */
    static void replaceCharactersInRow(const U2EntityRef& mcaRef, qint64 rowId, QHash<qint64, char> newCharList, U2OpStatus& os);
};

}  // namespace U2
