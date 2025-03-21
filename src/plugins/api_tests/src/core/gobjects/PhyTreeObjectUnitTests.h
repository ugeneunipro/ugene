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

#include <unittest.h>

#include "core/dbi/DbiTest.h"

namespace U2 {

class U2ObjectDbi;
class UdrDbi;

class PhyTreeObjectTestData {
public:
    static void shutdown();
    static U2DbiRef getDbiRef();
    static U2EntityRef getObjRef();
    static U2ObjectDbi* getObjDbi();
    static UdrDbi* getUdrDbi();

private:
    static TestDbiProvider dbiProvider;
    static const QString UDR_DB_URL;
    static bool inited;
    static U2EntityRef objRef;

private:
    static void init();
    static void initData();
};

/* createInstance 1 */
DECLARE_TEST(PhyTreeObjectUnitTests, createInstance);
/* createInstance 2 */
DECLARE_TEST(PhyTreeObjectUnitTests, createInstance_WrongDbi);
/* getText 1 */
DECLARE_TEST(PhyTreeObjectUnitTests, getTree);
/* getText 2 */
DECLARE_TEST(PhyTreeObjectUnitTests, getTree_Null);
/* setText */
DECLARE_TEST(PhyTreeObjectUnitTests, setTree);
/* clone 1 */
DECLARE_TEST(PhyTreeObjectUnitTests, clone);
/* clone 2 */
DECLARE_TEST(PhyTreeObjectUnitTests, clone_NullDbi);
/* clone 3 */
DECLARE_TEST(PhyTreeObjectUnitTests, clone_NullObj);
/* remove */
DECLARE_TEST(PhyTreeObjectUnitTests, remove);

}  // namespace U2

DECLARE_METATYPE(PhyTreeObjectUnitTests, createInstance);
DECLARE_METATYPE(PhyTreeObjectUnitTests, createInstance_WrongDbi);
DECLARE_METATYPE(PhyTreeObjectUnitTests, getTree);
DECLARE_METATYPE(PhyTreeObjectUnitTests, getTree_Null);
DECLARE_METATYPE(PhyTreeObjectUnitTests, setTree);
DECLARE_METATYPE(PhyTreeObjectUnitTests, clone);
DECLARE_METATYPE(PhyTreeObjectUnitTests, clone_NullDbi);
DECLARE_METATYPE(PhyTreeObjectUnitTests, clone_NullObj);
DECLARE_METATYPE(PhyTreeObjectUnitTests, remove);
