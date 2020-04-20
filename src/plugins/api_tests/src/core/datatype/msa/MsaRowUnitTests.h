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

#ifndef _U2_MSA_ROW_UNIT_TESTS_H_
#define _U2_MSA_ROW_UNIT_TESTS_H_

#include <U2Core/MultipleSequenceAlignment.h>

#include <unittest.h>


namespace U2 {

class MsaRowTestUtils {
public:
    static MultipleSequenceAlignmentRow initTestRowWithGaps(MultipleSequenceAlignment& ma);
    static MultipleSequenceAlignmentRow initTestRowWithGapsInMiddle(MultipleSequenceAlignment& ma);
    static MultipleSequenceAlignmentRow initTestRowWithTrailingGaps(MultipleSequenceAlignment& ma);
    static MultipleSequenceAlignmentRow initTestRowWithoutGaps(MultipleSequenceAlignment& ma);
    static MultipleSequenceAlignmentRow initTestRowForModification(MultipleSequenceAlignment& ma);
    static MultipleSequenceAlignmentRow initEmptyRow(MultipleSequenceAlignment& ma);
    static QString getRowData(const MultipleSequenceAlignmentRow &row);

    static const int rowWithGapsLength;
    static const int rowWithGapsInMiddleLength;
    static const int rowWithoutGapsLength;

    static const QString rowWithGapsName;
};

/**
 * The row is created by adding it to an alignment!
 * It is created from a byte array / from a sequence.
 * Row core (start, end, length and bytes) and row length is also verified:
 *   ^ fromBytes          - create a row from a byte array, no trailing gaps
 *   ^ fromBytesTrailing  - create a row from a byte array, there are trailing gaps
 *   ^ fromBytesGaps      - create a row from a byte array, all items are gaps
 *   ^ oneTrailing        - create a row from a byte array, there is only one trailing gap (gap length = 1)
 *   ^ twoTrailing        - create a row from a byte array, there is only one trailing gap (gap length = 2)
 *   ^ oneMiddleGap       - create a row from a byte array, there is only one middle gap
 *   ^ noGaps             - create a row from a byte array, there is no gaps
 *   ^ fromSeq            - create a row from a sequence (without gaps) and a gap model
 *   ^ fromSeqTrailing    - create a row from a sequence (without gaps) and a gap model, there are trailing gaps
 *   ^ fromSeqWithGaps    - create a row from a sequence with gaps (opStatus is set to error)
 *   ^ gapPositionTooBig  - create a row from a sequence, a gap offset is bigger than the core length (opStatus is set to error)
 *   ^ negativeGapPos     - create a row from a sequence, the gap model is incorrect (negative gap position)
 *   ^ negativeGapOffset  - create a row from a sequence, the gap model is incorrect (negative gap offset)
 */
DECLARE_TEST(MsaRowUnitTests, createRow_fromBytes);
DECLARE_TEST(MsaRowUnitTests, createRow_fromBytesTrailing);
DECLARE_TEST(MsaRowUnitTests, createRow_fromBytesGaps);
DECLARE_TEST(MsaRowUnitTests, createRow_oneTrailing);
DECLARE_TEST(MsaRowUnitTests, createRow_twoTrailing);
DECLARE_TEST(MsaRowUnitTests, createRow_oneMiddleGap);
DECLARE_TEST(MsaRowUnitTests, createRow_noGaps);
DECLARE_TEST(MsaRowUnitTests, createRow_fromSeq);
DECLARE_TEST(MsaRowUnitTests, createRow_fromSeqTrailing);
DECLARE_TEST(MsaRowUnitTests, createRow_fromSeqWithGaps);
DECLARE_TEST(MsaRowUnitTests, createRow_gapPositionTooBig);
DECLARE_TEST(MsaRowUnitTests, createRow_negativeGapPos);
DECLARE_TEST(MsaRowUnitTests, createRow_negativeGapOffset);

/**
 * Verify getting/setting of a row name:
 *   ^ rowFromBytes  - when a row has been created from a byte array
 *   ^ rowFromSeq    - when a row has been created from a sequence
 *   ^ setName       - a new name can be set
 */
DECLARE_TEST(MsaRowUnitTests, rowName_rowFromBytes);
DECLARE_TEST(MsaRowUnitTests, rowName_rowFromSeq);
DECLARE_TEST(MsaRowUnitTests, rowName_setName);

/**
 * Verify "toByteArray" method:
 *   ^ noGaps                   - gap model is empty
 *   ^ gapsInBeginningAndMiddle - gaps are in the beginning of the sequence and in the middle
 *   ^ lengthTooShort           - the length is less than the core length (opStatus is set to error)
 *   ^ greaterLength            - the length is greater than the core length (additional gaps are appended to the end)
 *   ^ trailing                 - there are trailing gaps in the row
 */
DECLARE_TEST(MsaRowUnitTests, toByteArray_noGaps);
DECLARE_TEST(MsaRowUnitTests, toByteArray_gapsInBeginningAndMiddle);
DECLARE_TEST(MsaRowUnitTests, toByteArray_incorrectLength);
DECLARE_TEST(MsaRowUnitTests, toByteArray_greaterLength);
DECLARE_TEST(MsaRowUnitTests, toByteArray_trailing);

/**
 * Verify simplifying of a row:
 *   ^ beginningAndMiddleGaps - removes all gaps, returns "true"
 *   ^ nothingToRemove        - there are no gaps, returns "false"
 */
DECLARE_TEST(MsaRowUnitTests, simplify_gaps);
DECLARE_TEST(MsaRowUnitTests, simplify_nothingToRemove);

/**
 * Verify appending of one row to another:
 *   ^ noGapBetweenRows   - lengthBefore exactly equals to the row length
 *   ^ gapBetweenRows     - lengthBefore is greater than the row length
 *   ^ offsetInAnotherRow - gap at the beginning of the appended row
 *   ^ trailingInFirst    - there are trailing gaps in the first row, lengthBefore is greater
 *   ^ trailingAndOffset  - there are trailing gaps in the first row, offset in the appended one
 *   ^ invalidLength      - length before appended row is too short => error
 */
DECLARE_TEST(MsaRowUnitTests, append_noGapBetweenRows);
DECLARE_TEST(MsaRowUnitTests, append_gapBetweenRows);
DECLARE_TEST(MsaRowUnitTests, append_offsetInAnotherRow);
DECLARE_TEST(MsaRowUnitTests, append_trailingInFirst);
DECLARE_TEST(MsaRowUnitTests, append_trailingAndOffset);
DECLARE_TEST(MsaRowUnitTests, append_invalidLength);

/**
 * Setting row content:
 *   ^ empty          - make the row empty
 *   ^ trailingGaps   - bytes contain trailing gaps
 *   ^ offsetNoGap    - offset is specified, the sequence has no gaps at the beginning
 *   ^ offsetGap      - offset is specified, the sequence has gaps at the beginning
 *   ^ emptyAndOffset - empty sequence + (offset > 0)
 */
DECLARE_TEST(MsaRowUnitTests, setRowContent_empty);
DECLARE_TEST(MsaRowUnitTests, setRowContent_trailingGaps);
DECLARE_TEST(MsaRowUnitTests, setRowContent_offsetNoGap);
DECLARE_TEST(MsaRowUnitTests, setRowContent_offsetGap);
DECLARE_TEST(MsaRowUnitTests, setRowContent_emptyAndOffset);

/**
 * Inserting number of gaps into a row:
 *   ^ empty               - row is initially empty
 *   ^ toGapPosLeft        - there is a gap at the left side of the position (and a non-gap char at the right)
 *   ^ toGapPosRight       - there is a gap at the right side of the position
 *   ^ toGapPosInside      - gaps are inserted between gaps
 *   ^ insideChars         - gaps are inserted between chars
 *   ^ toZeroPosNoGap      - insert gaps to the beginning, there is no gap there
 *   ^ toZeroPosGap        - insert gaps to the beginning, there is already a gap offset
 *   ^ toLastPosNoGap      - insert gaps before the last char in the row
 *   ^ toLastPosGap        - insert a gap before the last gap in the row (between gaps)
 *   ^ toLastPosOneGap     - insert a gap before the last and the only gap in the row
 *   ^ noGapsYet           - insert to a row without gaps
 *   ^ onlyGaps            - the row consists of gaps only
 *   ^ oneChar             - the row consists of one char, insert offset to the beginning
 *   ^ tooBigPosition      - position is greater than the row length => skip
 *   ^ negativePosition    - negative position => skip
 *   ^ negativeNumOfChars  - negative chars count => error
 */
DECLARE_TEST(MsaRowUnitTests, insertGaps_empty);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toGapPosLeft);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toGapPosRight);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toGapPosInside);
DECLARE_TEST(MsaRowUnitTests, insertGaps_insideChars);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toZeroPosNoGap);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toZeroPosGap);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toLastPosNoGap);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toLastPosGap);
DECLARE_TEST(MsaRowUnitTests, insertGaps_toLastPosOneGap);
DECLARE_TEST(MsaRowUnitTests, insertGaps_noGapsYet);
DECLARE_TEST(MsaRowUnitTests, insertGaps_onlyGaps);
DECLARE_TEST(MsaRowUnitTests, insertGaps_oneChar);
DECLARE_TEST(MsaRowUnitTests, insertGaps_tooBigPosition);
DECLARE_TEST(MsaRowUnitTests, insertGaps_negativePosition);
DECLARE_TEST(MsaRowUnitTests, insertGaps_negativeNumOfChars);

/**
 * Removing chars from a row:
 *   ^ empty               - row is initially empty => skip
 *   ^ insideGap1          - start and end positions are inside gaps areas
 *   ^ insideGap2          - the same as above, but shifted
 *   ^ leftGapSide         - 'pos' is a gap, there is a char at ('pos' - 1)
 *   ^ rightGapSide        - 'pos' is a char, there is a gap at ('pos' - 1)
 *   ^ insideSeq1          - start and end positions are inside chars areas
 *   ^ insideSeq2          - the same as above, but shifted
 *   ^ fromZeroPosGap      - 'pos' = 0, there is a gap
 *   ^ fromZeroPosChar     - 'pos' = 0, there is a char
 *   ^ lastPosExactly      - 'pos' is the last char in the row, 'count' = 1, no gaps at the cut end
 *   ^ fromLastPos         - 'pos' is the last char in the row, 'count' > 1, no gaps at the cut end
 *   ^ insideOneGap1       - a region inside a long gap is removed (middle and end gaps in "---")
 *   ^ insideOneGap2       - a gap inside a long gap is removed (middle in "---")
 *   ^ insideOneGapLong    - several gaps inside longer gaps region
 *   ^ insideTrailingGap   - remove gap chars inside a long trailing gap
 *   ^ insideCharsOne      - one char inside non-gap chars region
 *   ^ negativePosition    - negative 'pos' has been specified => error
 *   ^ negativeNumOfChars  - negative 'count' has been specified => error
 *   ^ gapsAtRowEnd1       - trailing gaps are not removed ('pos' + 'count' bigger than the row length is also verified)
 *   ^ gapsAtRowEnd2       - the same as above, but with the only gap at the end
 *   ^ onlyGapsAfterRemove - all non-gap chars are removed
 *   ^ emptyAfterRemove    - all chars and gaps are removed
 *   ^ oneCharInGaps       - remove a char with gaps at the left and right side
 */
DECLARE_TEST(MsaRowUnitTests, remove_empty);
DECLARE_TEST(MsaRowUnitTests, remove_insideGap1);
DECLARE_TEST(MsaRowUnitTests, remove_insideGap2);
DECLARE_TEST(MsaRowUnitTests, remove_leftGapSide);
DECLARE_TEST(MsaRowUnitTests, remove_rightGapSide);
DECLARE_TEST(MsaRowUnitTests, remove_insideSeq1);
DECLARE_TEST(MsaRowUnitTests, remove_insideSeq2);
DECLARE_TEST(MsaRowUnitTests, remove_fromZeroPosGap);
DECLARE_TEST(MsaRowUnitTests, remove_fromZeroPosChar);
DECLARE_TEST(MsaRowUnitTests, remove_lastPosExactly);
DECLARE_TEST(MsaRowUnitTests, remove_fromLastPos);
DECLARE_TEST(MsaRowUnitTests, remove_insideOneGap1);
DECLARE_TEST(MsaRowUnitTests, remove_insideOneGap2);
DECLARE_TEST(MsaRowUnitTests, remove_insideOneGapLong);
DECLARE_TEST(MsaRowUnitTests, remove_insideTrailingGap);
DECLARE_TEST(MsaRowUnitTests, remove_insideCharsOne);
DECLARE_TEST(MsaRowUnitTests, remove_toBiggerPosition);
DECLARE_TEST(MsaRowUnitTests, remove_negativePosition);
DECLARE_TEST(MsaRowUnitTests, remove_negativeNumOfChars);
DECLARE_TEST(MsaRowUnitTests, remove_gapsAtRowEnd1);
DECLARE_TEST(MsaRowUnitTests, remove_gapsAtRowEnd2);
DECLARE_TEST(MsaRowUnitTests, remove_onlyGapsAfterRemove);
DECLARE_TEST(MsaRowUnitTests, remove_emptyAfterRemove);
DECLARE_TEST(MsaRowUnitTests, remove_oneCharInGaps);

/**
 * Getting a char at the specified position:
 *   ^ allCharsNoOffset  - verify all indexes of a row without gap offset in the beginning
 *   ^ offsetAndTrailing - verify gaps at the beginning and end of a row
 *   ^ onlyCharsInRow    - there are no gaps in the row
 */
DECLARE_TEST(MsaRowUnitTests, charAt_allCharsNoOffset);
DECLARE_TEST(MsaRowUnitTests, charAt_offsetAndTrailing);
DECLARE_TEST(MsaRowUnitTests, charAt_onlyCharsInRow);

/**
 * Checking if rows are equal (method "isRowContentEqual", "operator==", "operator!="):
 *   ^ sameContent         - rows contents are equal
 *   ^ noGaps              - rows contents are equal, there are no gaps in the rows
 *   ^ trailingInFirst     - rows contents are equal except there is a trailing gap in the first row
 *   ^ trailingInSecond    - rows contents are equal except there is a trailing gap in the second row
 *   ^ trailingInBoth      - rows contents are equal except trailing gaps, i.e. both rows have trailing gaps and sizes of the gaps are different
 *   ^ diffGapModelsGap    - gaps models are different (lengths of gaps are different)
 *   ^ diffGapModelsOffset - gaps models are different (offsets of gaps are different)
 *   ^ diffNumOfGaps       - gaps models are different (number of gaps differs)
 *   ^ diffSequences       - sequences differ
 */
DECLARE_TEST(MsaRowUnitTests, rowsEqual_sameContent);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_noGaps);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_trailingInFirst);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_trailingInSecond);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_trailingInBoth);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_diffGapModelsGap);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_diffGapModelsOffset);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_diffNumOfGaps);
DECLARE_TEST(MsaRowUnitTests, rowsEqual_diffSequences);

/**
 * Verify ungapped sequence length and getting of an ungapped position:
 *   ^ rowWithoutOffset  - verify the length and position for a row without gaps at the beginning
 *   ^ offsetTrailing    - verify the length and position for a row with gaps at the beginning
 */
DECLARE_TEST(MsaRowUnitTests, ungapped_rowWithoutOffset);
DECLARE_TEST(MsaRowUnitTests, ungapped_offsetTrailing);

/**
 * Cropping a row:
 *   ^ empty               - row is initially empty => skip
 *   ^ insideGap1          - start and end positions are inside gaps areas
 *   ^ insideGap2          - the same as above, but shifted
 *   ^ leftGapSide         - 'pos' is a gap, there is a char at ('pos' - 1)
 *   ^ rightGapSide        - 'pos' is a char, there is a gap at ('pos' - 1)
 *   ^ insideSeq1          - start and end positions are inside chars areas
 *   ^ insideSeq2          - the same as above, but shifted
 *   ^ fromZeroPosGap      - 'pos' = 0, there is a gap
 *   ^ fromZeroPosChar     - 'pos' = 0, there is a char
 *   ^ lastPosExactly      - 'pos' is the last char in the row, 'count' = 1, no gaps at the cut end
 *   ^ fromLastPos         - 'pos' is the last char in the row, 'count' > 1, no gaps at the cut end
 *   ^ insideOneGap1       - a region inside a long gap is removed (middle and end gaps in "---")
 *   ^ insideOneGap2       - a gap inside a long gap (middle in "---")
 *   ^ insideOneGapLong    - several gaps inside longer gaps region
 *   ^ insideCharsOne      - one char inside non-gap chars region
 *   ^ negativePosition    - negative 'pos' has been specified => error
 *   ^ negativeNumOfChars  - negative 'count' has been specified => error
 *   ^ trailing            - there are trailing gaps in the row
 *   ^ trailingToGaps      - there are trailing gaps in the row, the row is cropped to gaps only
 *   ^ cropTrailing        - trailing gaps are cropped
 *   ^ oneCharInGaps       - remove a char with gaps at the left and right side
 *   ^ posMoreThanLength   - the specified position is greater than the row length => make row empty
 */
DECLARE_TEST(MsaRowUnitTests, crop_empty);
DECLARE_TEST(MsaRowUnitTests, crop_insideGap1);
DECLARE_TEST(MsaRowUnitTests, crop_insideGap2);
DECLARE_TEST(MsaRowUnitTests, crop_leftGapSide);
DECLARE_TEST(MsaRowUnitTests, crop_rightGapSide);
DECLARE_TEST(MsaRowUnitTests, crop_insideSeq1);
DECLARE_TEST(MsaRowUnitTests, crop_insideSeq2);
DECLARE_TEST(MsaRowUnitTests, crop_fromZeroPosGap);
DECLARE_TEST(MsaRowUnitTests, crop_fromZeroPosChar);
DECLARE_TEST(MsaRowUnitTests, crop_lastPosExactly);
DECLARE_TEST(MsaRowUnitTests, crop_fromLastPos);
DECLARE_TEST(MsaRowUnitTests, crop_insideOneGap1);
DECLARE_TEST(MsaRowUnitTests, crop_insideOneGap2);
DECLARE_TEST(MsaRowUnitTests, crop_insideOneGapLong);
DECLARE_TEST(MsaRowUnitTests, crop_insideCharsOne);
DECLARE_TEST(MsaRowUnitTests, crop_negativePosition);
DECLARE_TEST(MsaRowUnitTests, crop_negativeNumOfChars);
DECLARE_TEST(MsaRowUnitTests, crop_trailing);
DECLARE_TEST(MsaRowUnitTests, crop_trailingToGaps);
DECLARE_TEST(MsaRowUnitTests, crop_cropTrailing);
DECLARE_TEST(MsaRowUnitTests, crop_oneCharInGaps);
DECLARE_TEST(MsaRowUnitTests, crop_posMoreThanLength);

/**
 * Getting mid of a row - only one case is verified as
 * mid uses "crop" method.
 */
DECLARE_TEST(MsaRowUnitTests, mid_general);

/** Converting to upper case. It is also verified that the name of the row is not changed. */
DECLARE_TEST(MsaRowUnitTests, upperCase_general);

/**
 * Replacing chars in a row:
 *   ^ charToChar           - all 'A' in a row are replaced by 'G'.
 *   ^ nothingToReplace     - no 'origChar' in a row to replace by a gap.
 *   ^ tildasToGapsNoGaps   - all 'origChar' ('~') are replaced by gaps.
 *   ^ tildasToGapsWithGaps - the row contains both gaps and 'origChar' ('~'), replaced by gaps.
                              Shifted gaps offset and merging of gaps is also verified.
 *   ^ trailingGaps         - trailing gaps are not removed.
 */
DECLARE_TEST(MsaRowUnitTests, replaceChars_charToChar);
DECLARE_TEST(MsaRowUnitTests, replaceChars_nothingToReplace);
DECLARE_TEST(MsaRowUnitTests, replaceChars_tildasToGapsNoGaps);
DECLARE_TEST(MsaRowUnitTests, replaceChars_tildasToGapsWithGaps);
DECLARE_TEST(MsaRowUnitTests, replaceChars_trailingGaps);


} // namespace

DECLARE_METATYPE(MsaRowUnitTests, createRow_fromBytes)
DECLARE_METATYPE(MsaRowUnitTests, createRow_fromBytesTrailing)
DECLARE_METATYPE(MsaRowUnitTests, createRow_fromBytesGaps)
DECLARE_METATYPE(MsaRowUnitTests, createRow_oneTrailing)
DECLARE_METATYPE(MsaRowUnitTests, createRow_twoTrailing)
DECLARE_METATYPE(MsaRowUnitTests, createRow_oneMiddleGap)
DECLARE_METATYPE(MsaRowUnitTests, createRow_noGaps)
DECLARE_METATYPE(MsaRowUnitTests, createRow_fromSeq)
DECLARE_METATYPE(MsaRowUnitTests, createRow_fromSeqTrailing)
DECLARE_METATYPE(MsaRowUnitTests, createRow_fromSeqWithGaps)
DECLARE_METATYPE(MsaRowUnitTests, createRow_gapPositionTooBig)
DECLARE_METATYPE(MsaRowUnitTests, createRow_negativeGapPos)
DECLARE_METATYPE(MsaRowUnitTests, createRow_negativeGapOffset)
DECLARE_METATYPE(MsaRowUnitTests, rowName_rowFromBytes)
DECLARE_METATYPE(MsaRowUnitTests, rowName_rowFromSeq)
DECLARE_METATYPE(MsaRowUnitTests, rowName_setName)
DECLARE_METATYPE(MsaRowUnitTests, toByteArray_noGaps)
DECLARE_METATYPE(MsaRowUnitTests, toByteArray_gapsInBeginningAndMiddle)
DECLARE_METATYPE(MsaRowUnitTests, toByteArray_incorrectLength)
DECLARE_METATYPE(MsaRowUnitTests, toByteArray_greaterLength)
DECLARE_METATYPE(MsaRowUnitTests, toByteArray_trailing)
DECLARE_METATYPE(MsaRowUnitTests, simplify_gaps)
DECLARE_METATYPE(MsaRowUnitTests, simplify_nothingToRemove)
DECLARE_METATYPE(MsaRowUnitTests, append_noGapBetweenRows)
DECLARE_METATYPE(MsaRowUnitTests, append_gapBetweenRows)
DECLARE_METATYPE(MsaRowUnitTests, append_offsetInAnotherRow)
DECLARE_METATYPE(MsaRowUnitTests, append_trailingInFirst)
DECLARE_METATYPE(MsaRowUnitTests, append_trailingAndOffset)
DECLARE_METATYPE(MsaRowUnitTests, append_invalidLength)
DECLARE_METATYPE(MsaRowUnitTests, setRowContent_empty)
DECLARE_METATYPE(MsaRowUnitTests, setRowContent_trailingGaps)
DECLARE_METATYPE(MsaRowUnitTests, setRowContent_offsetNoGap)
DECLARE_METATYPE(MsaRowUnitTests, setRowContent_offsetGap)
DECLARE_METATYPE(MsaRowUnitTests, setRowContent_emptyAndOffset)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_empty)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toGapPosLeft)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toGapPosRight)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toGapPosInside)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_insideChars)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toZeroPosNoGap)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toZeroPosGap)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toLastPosNoGap)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toLastPosGap)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_toLastPosOneGap)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_noGapsYet)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_onlyGaps)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_oneChar)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_tooBigPosition)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_negativePosition)
DECLARE_METATYPE(MsaRowUnitTests, insertGaps_negativeNumOfChars)
DECLARE_METATYPE(MsaRowUnitTests, remove_empty)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideGap1)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideGap2)
DECLARE_METATYPE(MsaRowUnitTests, remove_leftGapSide)
DECLARE_METATYPE(MsaRowUnitTests, remove_rightGapSide)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideSeq1)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideSeq2)
DECLARE_METATYPE(MsaRowUnitTests, remove_fromZeroPosGap)
DECLARE_METATYPE(MsaRowUnitTests, remove_fromZeroPosChar)
DECLARE_METATYPE(MsaRowUnitTests, remove_lastPosExactly)
DECLARE_METATYPE(MsaRowUnitTests, remove_fromLastPos)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideOneGap1)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideOneGap2)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideOneGapLong)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideTrailingGap)
DECLARE_METATYPE(MsaRowUnitTests, remove_insideCharsOne)
DECLARE_METATYPE(MsaRowUnitTests, remove_negativePosition)
DECLARE_METATYPE(MsaRowUnitTests, remove_negativeNumOfChars)
DECLARE_METATYPE(MsaRowUnitTests, remove_gapsAtRowEnd1)
DECLARE_METATYPE(MsaRowUnitTests, remove_gapsAtRowEnd2)
DECLARE_METATYPE(MsaRowUnitTests, remove_onlyGapsAfterRemove)
DECLARE_METATYPE(MsaRowUnitTests, remove_emptyAfterRemove)
DECLARE_METATYPE(MsaRowUnitTests, remove_oneCharInGaps)
DECLARE_METATYPE(MsaRowUnitTests, charAt_allCharsNoOffset)
DECLARE_METATYPE(MsaRowUnitTests, charAt_offsetAndTrailing)
DECLARE_METATYPE(MsaRowUnitTests, charAt_onlyCharsInRow)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_sameContent)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_noGaps)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_trailingInFirst)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_trailingInSecond)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_trailingInBoth)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_diffGapModelsGap)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_diffGapModelsOffset)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_diffNumOfGaps)
DECLARE_METATYPE(MsaRowUnitTests, rowsEqual_diffSequences)
DECLARE_METATYPE(MsaRowUnitTests, ungapped_rowWithoutOffset)
DECLARE_METATYPE(MsaRowUnitTests, ungapped_offsetTrailing)
DECLARE_METATYPE(MsaRowUnitTests, crop_empty)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideGap1)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideGap2)
DECLARE_METATYPE(MsaRowUnitTests, crop_leftGapSide)
DECLARE_METATYPE(MsaRowUnitTests, crop_rightGapSide)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideSeq1)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideSeq2)
DECLARE_METATYPE(MsaRowUnitTests, crop_fromZeroPosGap)
DECLARE_METATYPE(MsaRowUnitTests, crop_fromZeroPosChar)
DECLARE_METATYPE(MsaRowUnitTests, crop_lastPosExactly)
DECLARE_METATYPE(MsaRowUnitTests, crop_fromLastPos)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideOneGap1)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideOneGap2)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideOneGapLong)
DECLARE_METATYPE(MsaRowUnitTests, crop_insideCharsOne)
DECLARE_METATYPE(MsaRowUnitTests, crop_negativePosition)
DECLARE_METATYPE(MsaRowUnitTests, crop_negativeNumOfChars)
DECLARE_METATYPE(MsaRowUnitTests, crop_trailing)
DECLARE_METATYPE(MsaRowUnitTests, crop_trailingToGaps)
DECLARE_METATYPE(MsaRowUnitTests, crop_cropTrailing)
DECLARE_METATYPE(MsaRowUnitTests, crop_oneCharInGaps)
DECLARE_METATYPE(MsaRowUnitTests, crop_posMoreThanLength)
DECLARE_METATYPE(MsaRowUnitTests, mid_general)
DECLARE_METATYPE(MsaRowUnitTests, upperCase_general)
DECLARE_METATYPE(MsaRowUnitTests, replaceChars_charToChar)
DECLARE_METATYPE(MsaRowUnitTests, replaceChars_nothingToReplace)
DECLARE_METATYPE(MsaRowUnitTests, replaceChars_tildasToGapsNoGaps)
DECLARE_METATYPE(MsaRowUnitTests, replaceChars_tildasToGapsWithGaps)
DECLARE_METATYPE(MsaRowUnitTests, replaceChars_trailingGaps)

#endif
