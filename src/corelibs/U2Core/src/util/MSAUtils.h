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

#ifndef _U2_MSA_UTILS_H_
#define _U2_MSA_UTILS_H_

#include <QSet>

#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

namespace U2 {

class U2CORE_EXPORT MSAUtils : public QObject {
    Q_OBJECT
public:

    static bool equalsIgnoreGaps(const MultipleAlignmentRow& row, int startPos, const QByteArray& pattern, int &alternateLen);

    static int getPatternSimilarityIgnoreGaps(const MultipleSequenceAlignmentRow& row, int startPos, const QByteArray& pattern, int &alternateLen);

    static MultipleSequenceAlignment seq2ma(const QList<GObject*>& dnas, U2OpStatus& os, bool useGenbankHeader = false, bool recheckAlphabetFromDataIfRaw = false);

    static MultipleSequenceAlignment seq2ma(const QList<DNASequence>& dnas, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw = false);

    static QList<DNASequence> ma2seq(const MultipleSequenceAlignment& ma, bool trimGaps);

    static QList<DNASequence> ma2seq(const MultipleSequenceAlignment& ma, bool trimGaps, const QSet<qint64>& rowIds);

    // Returns common alphabet for the sequences in the list. Returns RAW is the list is empty.
    static const DNAAlphabet* deriveCommonAlphabet(const QList<DNASequence>& sequenceList, bool recheckAlphabetFromDataIfRaw);
    static const DNAAlphabet* deriveCommonAlphabet(const QList<U2SequenceObject*>& sequenceList, bool recheckAlphabetFromDataIfRaw, U2OpStatus& os);
    // Returns common alphabet for all in the list. Returns RAW is the list is empty.
    static const DNAAlphabet* deriveCommonAlphabet(const QList<const DNAAlphabet*>& alphabetList);

    // Returns row index or -1 if name is not present
    static int getRowIndexByName(const MultipleSequenceAlignment &ma, const QString& name);

    //checks that alignment is not empty and all packed sequence parts has equal length
    static bool checkPackedModelSymmetry(const MultipleSequenceAlignment& ali, U2OpStatus& ti);

    static MultipleSequenceAlignmentObject * seqDocs2msaObj(QList<Document*> doc, const QVariantMap& hints, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw = false);
    static MultipleSequenceAlignmentObject * seqObjs2msaObj(const QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os, bool shallowCopy = false, bool recheckAlphabetFromDataIfRaw = false);

    /**
     * Compares rows in the 'origMsa' and 'newMsa' by names of the sequences.
     * It is assumed that:
     *   1) All rows in 'newMsa' are present in 'origMsa';
     *   2) Corresponding sequences are the same (however, only their length are verified);
     * The method modifies 'newMsa' to contain original rows and sequences IDs (from 'origMsa') and
     * returns the list of rows IDs in the order of 'newMsa'.
     * Note, that 'newMsa' may contain less rows than 'origMsa'
     * (e.g. ClustalO may decrease the number of sequences after the alignment).
     */
    static QList<qint64> compareRowsAfterAlignment(const MultipleSequenceAlignment& origMsa, MultipleSequenceAlignment& newMsa, U2OpStatus& os);
    static void copyRowFromSequence(MultipleSequenceAlignmentObject *msaObj, U2SequenceObject *seqObj, U2OpStatus &os);
    static U2MsaRow copyRowFromSequence(U2SequenceObject *seqObj, const U2DbiRef &dstDbi, U2OpStatus &os);
    static U2MsaRow copyRowFromSequence(DNASequence seq, const U2DbiRef &dstDbi, U2OpStatus &os);

    static MultipleSequenceAlignment setUniqueRowNames(const MultipleSequenceAlignment& ma);
    /**
      * Renames rows in the 'ma' to 'names' according to the following assumptions:
      *   1) 'ma' row names are integers from [0..n - 1] interval,
      *      where n is equal to row number of 'ma';
      *   2) Row name 'i' will be renamed to 'names[i]' value;
      **/
    static bool restoreRowNames(MultipleSequenceAlignment& ma, const QStringList& names);

    static QList<U2Region> getColumnsWithGaps(const U2MsaListGapModel &maGapModel, int length, int requiredGapsCount = -1);
    static void removeColumnsWithGaps(MultipleSequenceAlignment &msa, int requiredGapsCount = -1);
};

}   // namespace U2

#endif
