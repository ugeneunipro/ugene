/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "DynTableTests.h"

#include <U2Algorithm/DynTable.h>

#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

static DynTable buildTable(const QString& patternA, const QString& patternB, bool allowInsDels, U2OpStatus *os) {
    DynTable dynTable(patternA.length(), patternB.length(), allowInsDels, os);
    dynTable.dump();
    for (int a = 0; a < patternA.length(); a++) {
        if (a > 0) {
            dynTable.shiftColumn();
        }
        for (int b = 0; b < patternB.length(); b++) {
            dynTable.match(b, patternB.at(b) == patternA.at(a));
            dynTable.dump();
        }
    }
    dynTable.dump();
    fflush(stdout);
    return dynTable;
}

static int getLastLen(const QString& patternA, const QString& patternB, bool allowInsDels, U2OpStatus* os) {
    return buildTable(patternA, patternB, allowInsDels, os).getLastLen();
}

IMPLEMENT_TEST(DynTableTests, bigTable) {
    // With no ins-del the size must always be the minimum of width or height.
    // The getLen function should also be safe to work with large values.
    // The result matrix size in this test is 5k*10k*4=200mb.
    U2OpStatusImpl os;
    {
        DynTable dynTable(10 * 1000, 5 * 1000, false, &os);
        int length = dynTable.getLastLen();
        CHECK_EQUAL(5 * 1000, length, "1. getLastLen()");
    }
    {
        DynTable dynTable(5 * 1000, 10 * 1000, false, &os);
        int length = dynTable.getLastLen();
        CHECK_EQUAL(5 * 1000, length, "2. getLastLen()");
    }
}

IMPLEMENT_TEST(DynTableTests, fullMatch) {
    U2OpStatusImpl os;
    CHECK_EQUAL(3, getLastLen("ABC", "ABC", false, &os), "Match length");
}

IMPLEMENT_TEST(DynTableTests, fullMismatch) {
    U2OpStatusImpl os;
    CHECK_EQUAL(3, getLastLen("ABC", "DEF", false, &os), "Match length");
}

IMPLEMENT_TEST(DynTableTests, mismatch) {
    U2OpStatusImpl os;
    CHECK_EQUAL(3, getLastLen("ABC", "AXC", false, &os), "1. Match length");
    CHECK_EQUAL(3, getLastLen("ABC", "ABD", false, &os), "2. Match length");
    CHECK_EQUAL(3, getLastLen("ABC", "EBC", false, &os), "3. Match length");
}

IMPLEMENT_TEST(DynTableTests, insDelMode_fullMatch) {
    U2OpStatusImpl os;
    CHECK_EQUAL(3, getLastLen("ABC", "ABC", true, &os), "Match length");
}

IMPLEMENT_TEST(DynTableTests, insDelMode_fullMismatch) {
    U2OpStatusImpl os;
    CHECK_EQUAL(0, getLastLen("ABC", "DEF", true, &os), "Match length");
}

IMPLEMENT_TEST(DynTableTests, insDelMode_mismatch) {
    U2OpStatusImpl os;
    CHECK_EQUAL(3, getLastLen("ABC", "AXC", true, &os), "1. Match length");
    CHECK_EQUAL(3, getLastLen("ABC", "ABD", true, &os), "2. Match length");
    CHECK_EQUAL(2, getLastLen("ABC", "EBC", true, &os), "3. Match length");  // Match starts with 'B'.
}

IMPLEMENT_TEST(DynTableTests, insDelMode_deletion) {
    U2OpStatusImpl os;
    CHECK_EQUAL(3, getLastLen("ABC", "AB", true, &os), "1. Match length");  // Match starts with 'A' in the first pattern => length (offset) is 3.
    CHECK_EQUAL(1, getLastLen("ABC", "AC", true, &os), "2. Match length");  // Only 'C' is matched.
    CHECK_EQUAL(2, getLastLen("ABC", "BC", true, &os), "3. Match length");  // Only 'BC' is matched.
}

IMPLEMENT_TEST(DynTableTests, insDelMode_insertion) {
    U2OpStatusImpl os;
    CHECK_EQUAL(2, getLastLen("AB", "ABC", true, &os), "1. Match length");
    CHECK_EQUAL(2, getLastLen("AB", "ACB", true, &os), "2. Match length");
    CHECK_EQUAL(2, getLastLen("AB", "CAB", true, &os), "3. Match length");
}

}  // namespace U2
