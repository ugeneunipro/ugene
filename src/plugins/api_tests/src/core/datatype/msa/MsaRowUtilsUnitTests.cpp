/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MsaRowUtilsUnitTests.h"

#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

static const QByteArray SEQUENCE = "0123456789";

/** Helper method that returns QString instead of QByteArray. Used for compatibility with CHECK_EQUAL macro. */
static QString getGappedSubsequence(const U2Region& region, const QByteArray& sequence, const QVector<U2MsaGap>& gaps) {
    QByteArray result = MsaRowUtils::getGappedSubsequence(region, sequence, gaps);
    return QString::fromUtf8(result);
}

IMPLEMENT_TEST(MsaRowUtilsUnitTests, getGappedSubsequence_noGaps) {
    CHECK_EQUAL("----------", getGappedSubsequence({0, 10}, "", {}), "");
    CHECK_EQUAL("0123456789", getGappedSubsequence({0, 10}, SEQUENCE, {}), "");
    CHECK_EQUAL("0123456789-----", getGappedSubsequence({0, 15}, SEQUENCE, {}), "");
    CHECK_EQUAL("0", getGappedSubsequence({0, 1}, SEQUENCE, {}), "");
    CHECK_EQUAL("0123", getGappedSubsequence({0, 4}, SEQUENCE, {}), "");
    CHECK_EQUAL("--01", getGappedSubsequence({-2, 4}, SEQUENCE, {}), "");
    CHECK_EQUAL("23456", getGappedSubsequence({2, 5}, SEQUENCE, {}), "");
    CHECK_EQUAL("89", getGappedSubsequence({8, 2}, SEQUENCE, {}), "");
    CHECK_EQUAL("89-", getGappedSubsequence({8, 3}, SEQUENCE, {}), "");
    CHECK_EQUAL("-0123456789-", getGappedSubsequence({-1, 12}, SEQUENCE, {}), "");
}

IMPLEMENT_TEST(MsaRowUtilsUnitTests, getGappedSubsequence_leadingGap) {
    QVector<U2MsaGap> gap = {U2MsaGap(0, 2)};
    CHECK_EQUAL("----------", getGappedSubsequence({0, 10}, "", gap), "");
    CHECK_EQUAL("--01234567", getGappedSubsequence({0, 10}, SEQUENCE, gap), "");
    CHECK_EQUAL("--0123456789---", getGappedSubsequence({0, 15}, SEQUENCE, gap), "");
    CHECK_EQUAL("-", getGappedSubsequence({0, 1}, SEQUENCE, gap), "");
    CHECK_EQUAL("--01", getGappedSubsequence({0, 4}, SEQUENCE, gap), "");
    CHECK_EQUAL("----", getGappedSubsequence({-2, 4}, SEQUENCE, gap), "");
    CHECK_EQUAL("01234", getGappedSubsequence({2, 5}, SEQUENCE, gap), "");
    CHECK_EQUAL("67", getGappedSubsequence({8, 2}, SEQUENCE, gap), "");
    CHECK_EQUAL("678", getGappedSubsequence({8, 3}, SEQUENCE, gap), "");
    CHECK_EQUAL("---012345678", getGappedSubsequence({-1, 12}, SEQUENCE, gap), "");
}

IMPLEMENT_TEST(MsaRowUtilsUnitTests, getGappedSubsequence_midGap) {
    QVector<U2MsaGap> gap = {U2MsaGap(5, 2)};
    CHECK_EQUAL("----------", getGappedSubsequence({0, 10}, "", gap), "");
    CHECK_EQUAL("01234-----", getGappedSubsequence({0, 10}, "01234", gap), "");
    CHECK_EQUAL("01234--567", getGappedSubsequence({0, 10}, SEQUENCE, gap), "");
    CHECK_EQUAL("01234--56789---", getGappedSubsequence({0, 15}, SEQUENCE, gap), "");
    CHECK_EQUAL("0", getGappedSubsequence({0, 1}, SEQUENCE, gap), "");
    CHECK_EQUAL("01234", getGappedSubsequence({0, 5}, SEQUENCE, gap), "");
    CHECK_EQUAL("01234-", getGappedSubsequence({0, 6}, SEQUENCE, gap), "");
    CHECK_EQUAL("--012", getGappedSubsequence({-2, 5}, SEQUENCE, gap), "");
    CHECK_EQUAL("234--", getGappedSubsequence({2, 5}, SEQUENCE, gap), "");
    CHECK_EQUAL("234--56", getGappedSubsequence({2, 7}, SEQUENCE, gap), "");
    CHECK_EQUAL("67", getGappedSubsequence({8, 2}, SEQUENCE, gap), "");
    CHECK_EQUAL("4--5", getGappedSubsequence({4, 4}, SEQUENCE, gap), "");
    CHECK_EQUAL("-01234--5678", getGappedSubsequence({-1, 12}, SEQUENCE, gap), "");
}

IMPLEMENT_TEST(MsaRowUtilsUnitTests, getGappedSubsequence_multiGaps) {
    QVector<U2MsaGap> gaps = {U2MsaGap(2, 2), U2MsaGap(6, 2)};
    CHECK_EQUAL("----------", getGappedSubsequence({0, 10}, "", gaps), "");
    CHECK_EQUAL("01--23--45", getGappedSubsequence({0, 10}, SEQUENCE, gaps), "");
    CHECK_EQUAL("01--23--456789-", getGappedSubsequence({0, 15}, SEQUENCE, gaps), "");
    CHECK_EQUAL("0", getGappedSubsequence({0, 1}, SEQUENCE, gaps), "");
    CHECK_EQUAL("01--", getGappedSubsequence({0, 4}, SEQUENCE, gaps), "");
    CHECK_EQUAL("01--2", getGappedSubsequence({0, 5}, SEQUENCE, gaps), "");
    CHECK_EQUAL("01--23", getGappedSubsequence({0, 6}, SEQUENCE, gaps), "");
    CHECK_EQUAL("01--23-", getGappedSubsequence({0, 7}, SEQUENCE, gaps), "");
    CHECK_EQUAL("1--23--", getGappedSubsequence({1, 7}, SEQUENCE, gaps), "");
    CHECK_EQUAL("--23--4", getGappedSubsequence({2, 7}, SEQUENCE, gaps), "");
    CHECK_EQUAL("-23--45", getGappedSubsequence({3, 7}, SEQUENCE, gaps), "");
    CHECK_EQUAL("--01-", getGappedSubsequence({-2, 5}, SEQUENCE, gaps), "");
    CHECK_EQUAL("45", getGappedSubsequence({8, 2}, SEQUENCE, gaps), "");
    CHECK_EQUAL("23--", getGappedSubsequence({4, 4}, SEQUENCE, gaps), "");
    CHECK_EQUAL("-01--23--456", getGappedSubsequence({-1, 12}, SEQUENCE, gaps), "");
}

}  // namespace U2
