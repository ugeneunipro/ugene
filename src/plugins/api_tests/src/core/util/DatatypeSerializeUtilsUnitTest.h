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

#ifndef _U2_DATATYPE_SERIALIZE_UTILS_UNIT_TESTS_H_
#define _U2_DATATYPE_SERIALIZE_UTILS_UNIT_TESTS_H_

#include <U2Core/DNAChromatogram.h>

#include <unittest.h>

#include "core/dbi/DbiTest.h"

namespace U2 {

class CompareUtils {
public:
    static void checkEqual(const DNAChromatogram &chr1, const DNAChromatogram &chr2, U2OpStatus &os);
};

/* DNAChromatogramSerializer 1 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, DNAChromatogramSerializer_true);
/* DNAChromatogramSerializer 2 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, DNAChromatogramSerializer_false);
/* NewickPhyTreeSerializer 1 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, NewickPhyTreeSerializer);
/* NewickPhyTreeSerializer 2 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, NewickPhyTreeSerializer_failed);
/* BioStruct3DSerializer 1 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, BioStruct3DSerializer);
/* BioStruct3DSerializer 2 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, BioStruct3DSerializer_failed);
/* WMatrixSerializer 1 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, WMatrixSerializer);
/* WMatrixSerializer 2 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, WMatrixSerializer_failed);
/* FMatrixSerializer 1 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, FMatrixSerializer);
/* FMatrixSerializer 2 */
DECLARE_TEST(DatatypeSerializeUtilsUnitTest, FMatrixSerializer_failed);

} // U2

DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, DNAChromatogramSerializer_true);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, DNAChromatogramSerializer_false);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, NewickPhyTreeSerializer);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, NewickPhyTreeSerializer_failed);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, BioStruct3DSerializer);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, BioStruct3DSerializer_failed);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, WMatrixSerializer);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, WMatrixSerializer_failed);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, FMatrixSerializer);
DECLARE_METATYPE(DatatypeSerializeUtilsUnitTest, FMatrixSerializer_failed);

#endif // _U2_DATATYPE_SERIALIZE_UTILS_UNIT_TESTS_H_
