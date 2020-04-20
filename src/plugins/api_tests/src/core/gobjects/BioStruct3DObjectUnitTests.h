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

#ifndef _U2_BIO_STRUCT_3D_OBJECT_UNIT_TESTS_H_
#define _U2_BIO_STRUCT_3D_OBJECT_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <unittest.h>

namespace U2 {

class U2ObjectDbi;
class UdrDbi;

class BioStruct3DObjectTestData {
public:
    static void shutdown();
    static U2DbiRef getDbiRef();
    static U2EntityRef getObjRef();
    static U2ObjectDbi * getObjDbi();
    static UdrDbi * getUdrDbi();
    static const BioStruct3D & getBioStruct();

    static BioStruct3D readBioStruct(const QString &fileName, U2OpStatus &os, bool useSessionDbi = true);

private:
    static TestDbiProvider dbiProvider;
    static const QString UDR_DB_URL;
    static bool inited;
    static U2EntityRef objRef;
    static BioStruct3D bioStruct;

private:
    static void init();
    static void initData();
};

/* createInstance 1 */
DECLARE_TEST(BioStruct3DObjectUnitTests, createInstance);
/* createInstance 2 */
DECLARE_TEST(BioStruct3DObjectUnitTests, createInstance_WrongDbi);
/* getBioStruct3D 1 */
DECLARE_TEST(BioStruct3DObjectUnitTests, getBioStruct3D);
/* getBioStruct3D 2 */
DECLARE_TEST(BioStruct3DObjectUnitTests, getBioStruct3D_Null);
/* clone 1 */
DECLARE_TEST(BioStruct3DObjectUnitTests, clone);
/* clone 2 */
DECLARE_TEST(BioStruct3DObjectUnitTests, clone_NullDbi);
/* clone 3 */
DECLARE_TEST(BioStruct3DObjectUnitTests, clone_NullObj);
/* remove */
DECLARE_TEST(BioStruct3DObjectUnitTests, remove);

} // U2

DECLARE_METATYPE(BioStruct3DObjectUnitTests, createInstance);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, createInstance_WrongDbi);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, getBioStruct3D);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, getBioStruct3D_Null);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, clone);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, clone_NullDbi);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, clone_NullObj);
DECLARE_METATYPE(BioStruct3DObjectUnitTests, remove);

#endif // _U2_BIO_STRUCT_3D_OBJECT_UNIT_TESTS_H_
