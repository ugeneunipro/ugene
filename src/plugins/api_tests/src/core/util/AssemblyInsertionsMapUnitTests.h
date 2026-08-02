/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

DECLARE_TEST(AssemblyInsertionsMapUnitTests, noReadsGiveNoColumns);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, insertionInTheMiddleOfTheRead);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, widestInsertionDefinesTheWidth);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, insertionAtTheReadStart);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, trailingInsertionIsIgnored);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, softClipIsNotAnInsertion);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, hugeInsertionIsTruncated);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, insertionOutsideTheRegionIsIgnored);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, columnsAndPositionsMatchEachOther);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, readIteratorReportsInsertedLetters);
DECLARE_TEST(AssemblyInsertionsMapUnitTests, readIteratorSkipsInsertionsBeforeStartPos);

}  // namespace U2

DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, noReadsGiveNoColumns)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, insertionInTheMiddleOfTheRead)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, widestInsertionDefinesTheWidth)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, insertionAtTheReadStart)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, trailingInsertionIsIgnored)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, softClipIsNotAnInsertion)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, hugeInsertionIsTruncated)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, insertionOutsideTheRegionIsIgnored)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, columnsAndPositionsMatchEachOther)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, readIteratorReportsInsertedLetters)
DECLARE_METATYPE(AssemblyInsertionsMapUnitTests, readIteratorSkipsInsertionsBeforeStartPos)
