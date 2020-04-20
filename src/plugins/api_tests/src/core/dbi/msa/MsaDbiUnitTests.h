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

#ifndef _U2_MSA_DBI_UNIT_TESTS_H_
#define _U2_MSA_DBI_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <unittest.h>


namespace U2 {

class U2MsaDbi;
class U2SequenceDbi;

class MsaTestData {
public:
    static void init();
    static void shutdown();

    static U2MsaDbi* getMsaDbi();
    static U2SequenceDbi* getSequenceDbi();

private:
    static TestDbiProvider dbiProvider;
    static const QString& MSA_DB_URL;
    static U2MsaDbi* msaDbi;
    static U2SequenceDbi* sequenceDbi;
};

/** Create and get a MSA */
DECLARE_TEST(MsaDbiUnitTests, createMsaObject);

/** Add rows to a MSA and get them */
DECLARE_TEST(MsaDbiUnitTests, addRows);

/** Remove rows from a MSA */
DECLARE_TEST(MsaDbiUnitTests, removeRows);

} // namespace

DECLARE_METATYPE(MsaDbiUnitTests, createMsaObject);
DECLARE_METATYPE(MsaDbiUnitTests, addRows);
DECLARE_METATYPE(MsaDbiUnitTests, removeRows);

#endif
