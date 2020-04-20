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

#ifndef _U2_SQLITE_UPGRADER_FROM_1_13_TO_1_25_H_
#define _U2_SQLITE_UPGRADER_FROM_1_13_TO_1_25_H_

#include "SqliteUpgrader.h"

namespace U2 {

class SqliteUpgraderFrom_1_13_To_1_25 : public SqliteUpgrader {
public:
    SqliteUpgraderFrom_1_13_To_1_25(SQLiteDbi *dbi);

    void upgrade(U2OpStatus &os) const;

private:
    void upgradeCoverageAttribute(U2OpStatus &os) const;
};

}   // namespace U2

#endif // _U2_SQLITE_UPGRADER_FROM_0_TO_1_13_H_
