/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

namespace U2 {

DECLARE_TEST(BAMUtilsUnitTests, convertBamToSam);
DECLARE_TEST(BAMUtilsUnitTests, convertSamToBamHeaderNoReference);
DECLARE_TEST(BAMUtilsUnitTests, convertSamToBamNoHeaderNoReference);
DECLARE_TEST(BAMUtilsUnitTests, convertSamToBamNoHeaderReference);
DECLARE_TEST(BAMUtilsUnitTests, isSortedTrue);
DECLARE_TEST(BAMUtilsUnitTests, isSortedFalse);
DECLARE_TEST(BAMUtilsUnitTests, sortBam);
DECLARE_TEST(BAMUtilsUnitTests, mergeBam);
DECLARE_TEST(BAMUtilsUnitTests, hasValidBamIndex);
DECLARE_TEST(BAMUtilsUnitTests, hasValidFastaIndex);
DECLARE_TEST(BAMUtilsUnitTests, createBamIndex);
DECLARE_TEST(BAMUtilsUnitTests, getBamIndexUrl);
DECLARE_TEST(BAMUtilsUnitTests, isEqualByLengthTrue);
DECLARE_TEST(BAMUtilsUnitTests, isEqualByLengthFalse);
DECLARE_TEST(BAMUtilsUnitTests, loadIndex);

}  // namespace U2

DECLARE_METATYPE(BAMUtilsUnitTests, convertBamToSam)
DECLARE_METATYPE(BAMUtilsUnitTests, convertSamToBamHeaderNoReference)
DECLARE_METATYPE(BAMUtilsUnitTests, convertSamToBamNoHeaderNoReference)
DECLARE_METATYPE(BAMUtilsUnitTests, convertSamToBamNoHeaderReference)
DECLARE_METATYPE(BAMUtilsUnitTests, isSortedTrue)
DECLARE_METATYPE(BAMUtilsUnitTests, isSortedFalse)
DECLARE_METATYPE(BAMUtilsUnitTests, sortBam)
DECLARE_METATYPE(BAMUtilsUnitTests, mergeBam)
DECLARE_METATYPE(BAMUtilsUnitTests, hasValidBamIndex)
DECLARE_METATYPE(BAMUtilsUnitTests, hasValidFastaIndex)
DECLARE_METATYPE(BAMUtilsUnitTests, createBamIndex)
DECLARE_METATYPE(BAMUtilsUnitTests, getBamIndexUrl)
DECLARE_METATYPE(BAMUtilsUnitTests, isEqualByLengthTrue)
DECLARE_METATYPE(BAMUtilsUnitTests, isEqualByLengthFalse)
DECLARE_METATYPE(BAMUtilsUnitTests, loadIndex)
