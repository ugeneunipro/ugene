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

class TextObjectTestData {
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
DECLARE_TEST(TextObjectUnitTests, createInstance);
/* createInstance 2 */
DECLARE_TEST(TextObjectUnitTests, createInstance_WrongDbi);
/* getText 1 */
DECLARE_TEST(TextObjectUnitTests, getText);
/* getText 2 */
DECLARE_TEST(TextObjectUnitTests, getText_Null);
/* setText */
DECLARE_TEST(TextObjectUnitTests, setText);
/* clone 1 */
DECLARE_TEST(TextObjectUnitTests, clone);
/* clone 2 */
DECLARE_TEST(TextObjectUnitTests, clone_NullDbi);
/* clone 3 */
DECLARE_TEST(TextObjectUnitTests, clone_NullObj);
/* remove */
DECLARE_TEST(TextObjectUnitTests, remove);

}  // namespace U2

DECLARE_METATYPE(TextObjectUnitTests, createInstance);
DECLARE_METATYPE(TextObjectUnitTests, createInstance_WrongDbi);
DECLARE_METATYPE(TextObjectUnitTests, getText);
DECLARE_METATYPE(TextObjectUnitTests, getText_Null);
DECLARE_METATYPE(TextObjectUnitTests, setText);
DECLARE_METATYPE(TextObjectUnitTests, clone);
DECLARE_METATYPE(TextObjectUnitTests, clone_NullDbi);
DECLARE_METATYPE(TextObjectUnitTests, clone_NullObj);
DECLARE_METATYPE(TextObjectUnitTests, remove);
