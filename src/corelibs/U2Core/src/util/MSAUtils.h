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

#ifndef _U2_MSA_UTILS_H_
#define _U2_MSA_UTILS_H_

#include <QSet>

#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

namespace U2 {

class U2CORE_EXPORT MSAUtils : public QObject {
    Q_OBJECT
public:
    static bool equalsIgnoreGaps(const MultipleAlignmentRow& row, int startPos, const QByteArray& pattern, int& alternateLen);

    static int getPatternSimilarityIgnoreGaps(const MultipleSequenceAlignmentRow& row, int startPos, const QByteArray& pattern, int& alternateLen);

    static MultipleSequenceAlignment seq2ma(const QList<GObject*>& dnas, U2OpStatus& os, bool useGenbankHeader = false, bool recheckAlphabetFromDataIfRaw = false);

    static MultipleSequenceAlignment seq2ma(const QList<DNASequence>& dnas, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw = false);

    /**
     * Extracts list of sequences from MA.
     * If 'rowIdFilter' is empty - all rows are used, otherwise only rows included into the set are exported.
     * The order of the exported rows matches the original order in the MSA.
     * If column region is not empty - the sequences are cropped first and gaps trimmed next.
     */
    static QList<DNASequence> convertMsaToSequenceList(const MultipleSequenceAlignment& msa,
                                                       U2OpStatus& os,
                                                       bool trimGaps = false,
                                                       const QSet<qint64>& rowIdFilter = {},
                                                       const U2Region& columnRegion = {});

    // Returns common alphabet for the sequences in the list. Returns RAW is the list is empty.
    static const DNAAlphabet* deriveCommonAlphabet(const QList<DNASequence>& sequenceList, bool recheckAlphabetFromDataIfRaw);
    static const DNAAlphabet* deriveCommonAlphabet(const QList<U2SequenceObject*>& sequenceList, bool recheckAlphabetFromDataIfRaw, U2OpStatus& os);
    // Returns common alphabet for all in the list. Returns RAW is the list is empty.
    static const DNAAlphabet* deriveCommonAlphabet(const QList<const DNAAlphabet*>& alphabetList);

    // Returns row index or -1 if name is not present
    static int getRowIndexByName(const MultipleSequenceAlignment& ma, const QString& name);

    // checks that alignment is not empty and all packed sequence parts has equal length
    static bool checkPackedModelSymmetry(const MultipleSequenceAlignment& ali, U2OpStatus& ti);

    static MultipleSequenceAlignmentObject* seqDocs2msaObj(QList<Document*> doc, const QVariantMap& hints, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw = false);
    static MultipleSequenceAlignmentObject* seqObjs2msaObj(const QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os, bool shallowCopy = false, bool recheckAlphabetFromDataIfRaw = false);

    /**
     * Assigns row & sequence ids to rows in 'newMsa' based in the info in 'origMsa'.
     * Checks rows equality using name & sequence.
     *
     * Indexes of non-matched rows are added to the removedRowIndexes/addedRowIndexes list.
     */
    static void assignOriginalDataIds(const MultipleSequenceAlignment& origMsa,
                                      MultipleSequenceAlignment& newMsa,
                                      QList<int>& removedRowIndexes,
                                      QList<int>& addedRowIndexes);

    /**
     * Same as 'assignOriginalDataIds' above but fails if any row is failed to map.
     * The method guarantees that origMsa and newMsa have equal count of rows.
     */
    static void assignOriginalDataIds(const MultipleSequenceAlignment& origMsa,
                                      MultipleSequenceAlignment& newMsa,
                                      U2OpStatus& os);

    static void copyRowFromSequence(MultipleSequenceAlignmentObject* msaObj, U2SequenceObject* seqObj, U2OpStatus& os);
    static U2MsaRow copyRowFromSequence(U2SequenceObject* seqObj, const U2DbiRef& dstDbi, U2OpStatus& os);
    static U2MsaRow copyRowFromSequence(DNASequence seq, const U2DbiRef& dstDbi, U2OpStatus& os);

    /**
     * Creates a copy of the 'msa' and aAssigns index based names to each result MSA row: 0, 1, 2, 3.
     * Appends 'prefix' to the result index name if provided.
     */
    static MultipleSequenceAlignment createCopyWithIndexedRowNames(const MultipleSequenceAlignment& msa, const QString& prefix = "");

    /** Returns unique MSA row name. Uses rowName as is if it can't be found in the currentlyUsedNamesSet or rolls the suffix of rowName. */
    static QString rollMsaRowName(const QString& rowName, const QSet<QString>& usedRowNamesSet, const QString& suffixSeparator = "_");

    /**
     * Renames rows in the 'msa' to 'names' according to the following convention:
     *   1) Current 'msa' row names are integers from [0..n - 1] interval, where 'n' is a row index in 'msa';
     *   2) Row name 'i' will be renamed to 'names[i]' value;
     *
     *  The optional 'prefix' field may be used to detect indexed rows. In this case all rows with no given prefix will not be renamed, but passed as is.
     *
     *  The method returns 'true' if all rows with a correct prefix were renamed.
     *
     *  TODO: this method is deprecated and all usages of it will be removed. Use 'restoreOriginalRowProperties' instead.
     **/
    static bool restoreOriginalRowNamesFromIndexedNames(MultipleSequenceAlignment& msa, const QStringList& names, const QString& prefix = "");

    /**
     * Restores original rowId, sequenceId and rowName from using the by-name mapping in resultMa.
     * With the by-name mapping the resultMa contains sequence names with 'prefix<Row-Index>' where '<Row-Index>' is the original row index.
     *
     * If the prefix is not empty and the result sequence does not start with the prefix - do not update result row properties.
     * The method returns 'true' if all rows with a non-empty prefix were found and updated.
     */
    static bool restoreOriginalRowProperties(MultipleSequenceAlignment& resultMa, const MultipleSequenceAlignment& originalMa, const QString& prefix = "");
    /**
     * Returns list of columns with desired quantity of gaps.
     */
    static QList<U2Region> getColumnsWithGaps(const QList<QVector<U2MsaGap>>& maGapModel, const QList<MultipleAlignmentRow>& rows, int alignmentLength, int requiredGapsCount = -1);
    static void removeColumnsWithGaps(MultipleSequenceAlignment& msa, int requiredGapsCount = -1);

    /**
     * Adds all MSA rows from the list into database.
     * Assigns result row and sequence ids to the rows in the list.
     */
    static void addRowsToMsa(U2EntityRef& msaObjectRef, QList<MultipleSequenceAlignmentRow>& rows, U2OpStatus& os);
};

}  // namespace U2

#endif
