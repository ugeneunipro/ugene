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

#ifndef _U2_DNA_CHROMATOGRAM_OBJECT_UNIT_TESTS_H_
#define _U2_DNA_CHROMATOGRAM_OBJECT_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <U2Core/DNAChromatogram.h>

#include <unittest.h>

namespace U2 {

class U2ObjectDbi;
class UdrDbi;

class DNAChromatogramObjectTestData {
public:
    static void shutdown();
    static U2DbiRef getDbiRef();
    static U2EntityRef getObjRef();
    static U2ObjectDbi * getObjDbi();
    static UdrDbi * getUdrDbi();
    static const DNAChromatogram & getChromatogram();

private:
    static TestDbiProvider dbiProvider;
    static const QString UDR_DB_URL;
    static bool inited;
    static U2EntityRef objRef;
    static DNAChromatogram chroma;

private:
    static void init();
    static void initData();
};

/* createInstance 1 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, createInstance);
/* createInstance 2 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, createInstance_WrongDbi);
/* getChromatogram 1 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, getChromatogram);
/* getChromatogram 2 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, getChromatogram_Null);
/* clone 1 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, clone);
/* clone 2 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, clone_NullDbi);
/* clone 3 */
DECLARE_TEST(DNAChromatogramObjectUnitTests, clone_NullObj);
/* remove */
DECLARE_TEST(DNAChromatogramObjectUnitTests, remove);

} // U2

DECLARE_METATYPE(DNAChromatogramObjectUnitTests, createInstance);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, createInstance_WrongDbi);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, getChromatogram);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, getChromatogram_Null);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, clone);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, clone_NullDbi);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, clone_NullObj);
DECLARE_METATYPE(DNAChromatogramObjectUnitTests, remove);

#endif // _U2_DNA_CHROMATOGRAM_OBJECT_UNIT_TESTS_H_
