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


#ifndef _U2_MSA_UNIT_TESTS_H_
#define _U2_MSA_UNIT_TESTS_H_

#include <U2Core/MultipleSequenceAlignment.h>

#include <unittest.h>


namespace U2 {

class MsaTestUtils {
public:
    static MultipleSequenceAlignment initTestAlignment();
    static QString getRowData(const MultipleSequenceAlignment&, int rowNum);
    static bool testAlignmentNotChanged(const MultipleSequenceAlignment&);

    static const int rowsNum;
    static const int firstRowLength;
    static const int secondRowLength;

    static const QString alignmentName;
};

/** Clearing of a non-empty alignment */
DECLARE_TEST(MsaUnitTests, clear_notEmpty);

/**
 * Alignment name:
 *   ^ ctor     - name, specified in the constructor
 *   ^ setName  - set a new name
 */
DECLARE_TEST(MsaUnitTests, name_ctor);
DECLARE_TEST(MsaUnitTests, name_setName);

/**
 * Alignment alphabet:
 *   ^ ctor        - alphabet, specified in the constructor
 *   ^ setAlphabet - set a new alphabet
 */
DECLARE_TEST(MsaUnitTests, alphabet_ctor);
DECLARE_TEST(MsaUnitTests, alphabet_setAlphabet);

/** Alignment info */
DECLARE_TEST(MsaUnitTests, info_setGet);

/**
 * Alignment length:
 *   ^ isEmptyFalse   - method "isEmpty" returns "false" for a non-empty alignment
 *   ^ isEmptyTrue    - method "isEmpty" returns "true" for an empty alignment
 *   ^ get            - getting length of a non-empty alignment
 *   ^ getForEmpty    - getting length of an empty alignment
 *   ^ setLessLength  - set length less than the current one to a non-empty
 *                      alignment, the alignment is cropped
 */
DECLARE_TEST(MsaUnitTests, length_isEmptyFalse);
DECLARE_TEST(MsaUnitTests, length_isEmptyTrue);
DECLARE_TEST(MsaUnitTests, length_get);
DECLARE_TEST(MsaUnitTests, length_getForEmpty);
DECLARE_TEST(MsaUnitTests, length_setLessLength);

/**
 * Number of rows:
 *   ^ notEmpty - number of rows in a non-empty alignment
 *   ^ empty    - zero rows in an empty alignment
 */
DECLARE_TEST(MsaUnitTests, numOfRows_notEmpty);
DECLARE_TEST(MsaUnitTests, numOfRows_empty);

/**
 * Trimming an alignment:
 *   ^ biggerLength      - length bigger than a maximum row length was set, now it is trimmed
 *   ^ leadingGapColumns - leading gap columns are removed
 *   ^ nothingToTrim     - trim() returns "false"
 *   ^ rowWithoutGaps    - no errors when an alignment contains a row without gaps
 *   ^ empty             - trim() returns "false"
 *   ^ trailingGapInOne  - there are two rows and nothing to trim, one row has a trailing gap
 */
DECLARE_TEST(MsaUnitTests, trim_biggerLength);
DECLARE_TEST(MsaUnitTests, trim_leadingGapColumns);
DECLARE_TEST(MsaUnitTests, trim_nothingToTrim);
DECLARE_TEST(MsaUnitTests, trim_rowWithoutGaps);
DECLARE_TEST(MsaUnitTests, trim_empty);
DECLARE_TEST(MsaUnitTests, trim_trailingGapInOne);

/**
 * Removing all gaps from an alignment:
 *   ^ withGaps    - gaps in a non-empty alignment are removed, simplify returns "true"
 *   ^ withoutGaps - no gaps to remove, simplify returns "false"
 *   ^ empty       - an empty alignment, simplify returns "false"
 */
DECLARE_TEST(MsaUnitTests, simplify_withGaps);
DECLARE_TEST(MsaUnitTests, simplify_withoutGaps);
DECLARE_TEST(MsaUnitTests, simplify_empty);

/**
 * Verify methods "sortRowsByName" and "sortRowsBySimilarity":
 *   ^ byNameAsc         - sort rows by name in ascending order
 *   ^ byNameDesc        - sort rows by name in descending order
 *   ^ twoSimilar        - sort rows by similarity, two rows are similar
 *   ^ threeSimilar      - sort rows by similarity, three rows are similar
 *   ^ similarTwoRegions - sort rows by similarity, two groups of similar sequences
 */
DECLARE_TEST(MsaUnitTests, sortRows_byNameAsc);
DECLARE_TEST(MsaUnitTests, sortRows_byNameDesc);
DECLARE_TEST(MsaUnitTests, sortRows_twoSimilar);
DECLARE_TEST(MsaUnitTests, sortRows_threeSimilar);
DECLARE_TEST(MsaUnitTests, sortRows_similarTwoRegions);

/**
 * Getting rows and rows' names:
 *   ^ oneRow              - verify "getRow" method on a non-empty alignment with a valid row index
 *   ^ severalRows         - verify "getRows" method on a non-empty alignment
 *   ^ rowNames            - verify "getRowNames" method on a non-empty alignment
 */
DECLARE_TEST(MsaUnitTests, getRows_oneRow);
DECLARE_TEST(MsaUnitTests, getRows_severalRows);
DECLARE_TEST(MsaUnitTests, getRows_rowNames);

/**
 * Getting character at the specified position:
 *   ^ nonGapChar - there is a non-gap char in the specified row/position
 *   ^ gap        - there is a gap in the specified row/position
 */
DECLARE_TEST(MsaUnitTests, charAt_nonGapChar);
DECLARE_TEST(MsaUnitTests, charAt_gap);

/**
 * Inserting gaps into an alignment:
 *   ^ validParams       - gaps are inserted into a row
 *   ^ toBeginningLength - a gap is inserted to a row beginning, the length of the alignment is properly recalculated
 *   ^ negativeRowIndex  - row index is negative => error
 *   ^ tooBigRowIndex    - row index is greater than the number of rows => error
 *   ^ negativePos       - position is negative => error
 *   ^ tooBigPos         - position is greater than the alignment length => error
 *   ^ negativeCount     - gaps count is negative => error
 */
DECLARE_TEST(MsaUnitTests, insertGaps_validParams);
DECLARE_TEST(MsaUnitTests, insertGaps_toBeginningLength);
DECLARE_TEST(MsaUnitTests, insertGaps_negativeRowIndex);
DECLARE_TEST(MsaUnitTests, insertGaps_tooBigRowIndex);
DECLARE_TEST(MsaUnitTests, insertGaps_negativePos);
DECLARE_TEST(MsaUnitTests, insertGaps_tooBigPos);
DECLARE_TEST(MsaUnitTests, insertGaps_negativeCount);

/**
 * Removing chars from an alignment:
 *   ^ validParamsAndTrimmed - chars are removed, the alignment is trimmed (gaps columns are removed, length is recalculated)
 *   ^ negativeRowIndex      - row index is negative => error
 *   ^ tooBigRowIndex        - row index is greater than the number of rows => error
 *   ^ negativePos           - position is negative => error
 *   ^ tooBigPos             - position is greater than the alignment length => error
 *   ^ negativeCount         - gaps count is negative => error
 */
DECLARE_TEST(MsaUnitTests, removeChars_validParams);
DECLARE_TEST(MsaUnitTests, removeChars_negativeRowIndex);
DECLARE_TEST(MsaUnitTests, removeChars_tooBigRowIndex);
DECLARE_TEST(MsaUnitTests, removeChars_negativePos);
DECLARE_TEST(MsaUnitTests, removeChars_tooBigPos);
DECLARE_TEST(MsaUnitTests, removeChars_negativeCount);

/**
 * Removing a region:
 *   ^ validParams - remove a sub-alignment
 *   ^ removeEmpty - parameter removeEmptyRows is set to "True"
 *   ^ trimmed     - the alignment is trimmed after removing the region, the length has been modified
 */
DECLARE_TEST(MsaUnitTests, removeRegion_validParams);
DECLARE_TEST(MsaUnitTests, removeRegion_removeEmpty);
DECLARE_TEST(MsaUnitTests, removeRegion_trimmed);

/** Renaming a row */
DECLARE_TEST(MsaUnitTests, renameRow_validParams);

/**
 * Setting a new row content:
 *   ^ validParamsAndTrimmed - a row content is changed, the alignment is NOT trimmed
 *   ^ lengthIsIncreased     - a row content becomes longer than the initial alignment length
 */
DECLARE_TEST(MsaUnitTests, setRowContent_validParamsAndNotTrimmed);
DECLARE_TEST(MsaUnitTests, setRowContent_lengthIsIncreased);

/** Converting all rows to upper case */
DECLARE_TEST(MsaUnitTests, upperCase_charsAndGaps);

/** Cropping an alignment */
DECLARE_TEST(MsaUnitTests, crop_validParams);

/** Getting mid of an alignmentVerify method "mid" */
DECLARE_TEST(MsaUnitTests, mid_validParams);

/**
 * Adding a new row to the alignment:
 *   ^ appendRowFromBytes  - a new row is created from bytes and appended to the end of the alignment
 *   ^ rowFromBytesToIndex - a new row is created from bytes and inserted to the specified index
 *   ^ zeroBound           - incorrect row index "-2" => the new row is inserted to the beginning
 *   ^ rowsNumBound        - incorrect row index more than the number of rows => the row is appended
 */
DECLARE_TEST(MsaUnitTests, addRow_appendRowFromBytes);
DECLARE_TEST(MsaUnitTests, addRow_rowFromBytesToIndex);
DECLARE_TEST(MsaUnitTests, addRow_zeroBound);
DECLARE_TEST(MsaUnitTests, addRow_rowsNumBound);

/**
 * Removing a row from the alignment:
 *   ^ validIndex     - row index is valid => the row is removed
 *   ^ negativeIndex  - row index is negative => error
 *   ^ tooBigIndex    - row index is bigger than the number of rows => error
 *   ^ emptyAlignment - all rows are removed from the alignment, the length is set to zero
 */
DECLARE_TEST(MsaUnitTests, removeRow_validIndex);
DECLARE_TEST(MsaUnitTests, removeRow_negativeIndex);
DECLARE_TEST(MsaUnitTests, removeRow_tooBigIndex);
DECLARE_TEST(MsaUnitTests, removeRow_emptyAlignment);

/**
 * Moving rows block:
 *   ^ positiveDelta - rows are moved downwards
 *   ^ negativeDelta - rows are moved upwards
 */
DECLARE_TEST(MsaUnitTests, moveRowsBlock_positiveDelta);
DECLARE_TEST(MsaUnitTests, moveRowsBlock_negativeDelta);

/** Replacing chars in an alignment row */
DECLARE_TEST(MsaUnitTests, replaceChars_validParams);

/** Appending chars to an alignment row */
DECLARE_TEST(MsaUnitTests, appendChars_validParams);

/** Verify operator+= */
DECLARE_TEST(MsaUnitTests, operPlusEqual_validParams);

/**
 * Verify operator!= :
 *   ^ equal    - alignments are equal
 *   ^ notEqual - alignments are not equal (one of the alignments is empty)
 */
DECLARE_TEST(MsaUnitTests, operNotEqual_equal);
DECLARE_TEST(MsaUnitTests, operNotEqual_notEqual);

/**
 * Verify if the alignment has gaps:
 *   ^ gaps   - there are gaps in the alignment
 *   ^ noGaps - there are NO gaps in the alignment
 */
DECLARE_TEST(MsaUnitTests, hasEmptyGapModel_gaps);
DECLARE_TEST(MsaUnitTests, hasEmptyGapModel_noGaps);


} // namespace

DECLARE_METATYPE(MsaUnitTests, clear_notEmpty);
DECLARE_METATYPE(MsaUnitTests, name_ctor);
DECLARE_METATYPE(MsaUnitTests, name_setName);
DECLARE_METATYPE(MsaUnitTests, alphabet_ctor);
DECLARE_METATYPE(MsaUnitTests, alphabet_setAlphabet);
DECLARE_METATYPE(MsaUnitTests, info_setGet);
DECLARE_METATYPE(MsaUnitTests, length_isEmptyFalse);
DECLARE_METATYPE(MsaUnitTests, length_isEmptyTrue);
DECLARE_METATYPE(MsaUnitTests, length_get);
DECLARE_METATYPE(MsaUnitTests, length_getForEmpty);
DECLARE_METATYPE(MsaUnitTests, length_setLessLength);
DECLARE_METATYPE(MsaUnitTests, numOfRows_notEmpty);
DECLARE_METATYPE(MsaUnitTests, numOfRows_empty);
DECLARE_METATYPE(MsaUnitTests, trim_biggerLength);
DECLARE_METATYPE(MsaUnitTests, trim_leadingGapColumns);
DECLARE_METATYPE(MsaUnitTests, trim_nothingToTrim);
DECLARE_METATYPE(MsaUnitTests, trim_rowWithoutGaps);
DECLARE_METATYPE(MsaUnitTests, trim_empty);
DECLARE_METATYPE(MsaUnitTests, trim_trailingGapInOne);
DECLARE_METATYPE(MsaUnitTests, simplify_withGaps);
DECLARE_METATYPE(MsaUnitTests, simplify_withoutGaps);
DECLARE_METATYPE(MsaUnitTests, simplify_empty);
DECLARE_METATYPE(MsaUnitTests, sortRows_byNameAsc);
DECLARE_METATYPE(MsaUnitTests, sortRows_byNameDesc);
DECLARE_METATYPE(MsaUnitTests, sortRows_twoSimilar);
DECLARE_METATYPE(MsaUnitTests, sortRows_threeSimilar);
DECLARE_METATYPE(MsaUnitTests, sortRows_similarTwoRegions);
DECLARE_METATYPE(MsaUnitTests, getRows_oneRow);
DECLARE_METATYPE(MsaUnitTests, getRows_severalRows);
DECLARE_METATYPE(MsaUnitTests, getRows_rowNames);
DECLARE_METATYPE(MsaUnitTests, charAt_nonGapChar);
DECLARE_METATYPE(MsaUnitTests, charAt_gap);
DECLARE_METATYPE(MsaUnitTests, insertGaps_validParams);
DECLARE_METATYPE(MsaUnitTests, insertGaps_toBeginningLength);
DECLARE_METATYPE(MsaUnitTests, insertGaps_negativeRowIndex);
DECLARE_METATYPE(MsaUnitTests, insertGaps_tooBigRowIndex);
DECLARE_METATYPE(MsaUnitTests, insertGaps_negativePos);
DECLARE_METATYPE(MsaUnitTests, insertGaps_tooBigPos);
DECLARE_METATYPE(MsaUnitTests, insertGaps_negativeCount);
DECLARE_METATYPE(MsaUnitTests, removeChars_validParams);
DECLARE_METATYPE(MsaUnitTests, removeChars_negativeRowIndex);
DECLARE_METATYPE(MsaUnitTests, removeChars_tooBigRowIndex);
DECLARE_METATYPE(MsaUnitTests, removeChars_negativePos);
DECLARE_METATYPE(MsaUnitTests, removeChars_tooBigPos);
DECLARE_METATYPE(MsaUnitTests, removeChars_negativeCount);
DECLARE_METATYPE(MsaUnitTests, removeRegion_validParams);
DECLARE_METATYPE(MsaUnitTests, removeRegion_removeEmpty);
DECLARE_METATYPE(MsaUnitTests, removeRegion_trimmed);
DECLARE_METATYPE(MsaUnitTests, renameRow_validParams);
DECLARE_METATYPE(MsaUnitTests, setRowContent_validParamsAndNotTrimmed);
DECLARE_METATYPE(MsaUnitTests, setRowContent_lengthIsIncreased);
DECLARE_METATYPE(MsaUnitTests, upperCase_charsAndGaps)
DECLARE_METATYPE(MsaUnitTests, crop_validParams);
DECLARE_METATYPE(MsaUnitTests, mid_validParams);
DECLARE_METATYPE(MsaUnitTests, addRow_appendRowFromBytes);
DECLARE_METATYPE(MsaUnitTests, addRow_rowFromBytesToIndex);
DECLARE_METATYPE(MsaUnitTests, addRow_zeroBound);
DECLARE_METATYPE(MsaUnitTests, addRow_rowsNumBound);
DECLARE_METATYPE(MsaUnitTests, removeRow_validIndex);
DECLARE_METATYPE(MsaUnitTests, removeRow_negativeIndex);
DECLARE_METATYPE(MsaUnitTests, removeRow_tooBigIndex);
DECLARE_METATYPE(MsaUnitTests, removeRow_emptyAlignment);
DECLARE_METATYPE(MsaUnitTests, moveRowsBlock_positiveDelta);
DECLARE_METATYPE(MsaUnitTests, moveRowsBlock_negativeDelta);
DECLARE_METATYPE(MsaUnitTests, replaceChars_validParams);
DECLARE_METATYPE(MsaUnitTests, appendChars_validParams);
DECLARE_METATYPE(MsaUnitTests, operPlusEqual_validParams);
DECLARE_METATYPE(MsaUnitTests, operNotEqual_equal);
DECLARE_METATYPE(MsaUnitTests, operNotEqual_notEqual);
DECLARE_METATYPE(MsaUnitTests, hasEmptyGapModel_gaps);
DECLARE_METATYPE(MsaUnitTests, hasEmptyGapModel_noGaps);


#endif

