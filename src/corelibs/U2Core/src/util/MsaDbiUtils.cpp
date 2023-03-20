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

#include "MsaDbiUtils.h"

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

namespace U2 {

/** Validates 'pos' in an alignment: it must be non-negative and less than or equal to the alignment length */
void MaDbiUtils::splitBytesToCharsAndGaps(const QByteArray& input, QByteArray& seqBytes, QVector<U2MsaGap>& gapModel) {
    bool previousCharIsGap = false;
    int gapsCount = 0;
    int gapsOffset = 0;

    for (int i = 0; i < input.count(); ++i) {
        // A char
        if (input.at(i) != U2Msa::GAP_CHAR) {
            if (previousCharIsGap) {
                U2MsaGap gap(gapsOffset, gapsCount);
                gapModel.append(gap);
                gapsCount = 0;
            }
            seqBytes.append(input.at(i));
            previousCharIsGap = false;
        }
        // A gap
        else {
            gapsCount++;
            // A gap before the end of the row
            if (i < input.count() - 1) {
                if (!previousCharIsGap) {
                    gapsOffset = i;
                }
                previousCharIsGap = true;
            }
            // A gap at the end of the row
            else {
                // Correct the offset if there is one gap at the end of the row
                if (1 == gapsCount) {
                    gapsOffset = i;
                }
                SAFE_POINT(gapsOffset >= 0, "Negative gap offset!", );
                SAFE_POINT(gapsCount > 0, "Non-positive gap length!", );
                U2MsaGap gap(gapsOffset, gapsCount);
                gapModel.append(gap);
            }
        }
    }

    SAFE_POINT(-1 == seqBytes.indexOf(U2Msa::GAP_CHAR), "Row sequence contains gaps!", );
}

qint64 MaDbiUtils::getMaLength(const U2EntityRef& maRef, U2OpStatus& os) {
    DbiConnection con(maRef.dbiRef, os);
    CHECK_OP(os, -1);

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT_EXT(nullptr != msaDbi, os.setError("NULL Msa Dbi!"), -1);

    qint64 length = msaDbi->getMsaLength(maRef.entityId, os);
    CHECK_OP(os, -1);

    return length;
}

void MaDbiUtils::updateMaLength(const U2EntityRef& maRef, qint64 newLen, U2OpStatus& os) {
    // Prepare the connection
    DbiConnection con(maRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT_EXT(nullptr != msaDbi, os.setError("NULL Msa Dbi!"), );

    // Update the data
    msaDbi->updateMsaLength(maRef.entityId, newLen, os);
}

U2AlphabetId MaDbiUtils::getMaAlphabet(const U2EntityRef& maRef, U2OpStatus& os) {
    DbiConnection con(maRef.dbiRef, os);
    CHECK_OP(os, U2AlphabetId());

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT_EXT(nullptr != msaDbi, os.setError("NULL Msa Dbi!"), U2AlphabetId());

    U2AlphabetId alphabet = msaDbi->getMsaAlphabet(maRef.entityId, os);
    CHECK_OP(os, U2AlphabetId());

    return alphabet;
}

void MaDbiUtils::updateMaAlphabet(const U2EntityRef& maRef, const U2AlphabetId& alphabet, U2OpStatus& os) {
    // Prepare the connection
    SAFE_POINT_EXT(alphabet.isValid(), os.setError("Invalid alphabet was passed !"), );
    DbiConnection con(maRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT_EXT(nullptr != msaDbi, os.setError("NULL Msa Dbi!"), );

    // Update the data
    msaDbi->updateMsaAlphabet(maRef.entityId, alphabet, os);
}

void MaDbiUtils::renameMa(const U2EntityRef& maRef, const QString& newName, U2OpStatus& os) {
    if (newName.isEmpty()) {
        os.setError(tr("Can't rename an alignment to an empty name!"));
    }

    // Prepare the connection
    DbiConnection con(maRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Update the name
    msaDbi->updateMsaName(maRef.entityId, newName, os);
}

void MaDbiUtils::updateRowGapModel(const U2EntityRef& msaRef, qint64 rowId, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Update the data
    msaDbi->updateGapModel(msaRef.entityId, rowId, gaps, os);
}

void MaDbiUtils::updateRowsOrder(const U2EntityRef& meRef, const QList<qint64>& rowsOrder, U2OpStatus& os) {
    // Prepare the connection
    DbiConnection con(meRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, "Msa Dbi is null!", );

    // Update the data
    msaDbi->setNewRowsOrder(meRef.entityId, rowsOrder, os);
}

void MaDbiUtils::renameRow(const U2EntityRef& msaRef, qint64 rowId, const QString& newName, U2OpStatus& os) {
    if (newName.isEmpty()) {
        os.setError(tr("Can't rename a row to an empty name!"));
    }

    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Update the row name
    msaDbi->updateRowName(msaRef.entityId, rowId, newName, os);
}

void MaDbiUtils::moveRows(const U2EntityRef& msaRef, const QList<qint64>& rowsToMove, const int delta, U2OpStatus& os) {
    DbiConnection con(msaRef.dbiRef, false, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    if (delta == 0 || rowsToMove.isEmpty()) {
        return;
    }

    QList<U2MsaRow> rows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_OP(os, );

    QList<qint64> rowIds;
    for (int i = 0; i < rows.length(); ++i) {
        rowIds << rows[i].rowId;
    }

    QList<QPair<int, int>> from_To;

    if (delta < 0) {
        int rowIndex = rowIds.indexOf(rowsToMove.first());
        if (rowIndex == -1) {
            os.setError("Invalid row list");
            return;
        }
        int moveToIndex = rowIndex + delta >= 0 ? rowIndex + delta : 0;
        from_To.append(QPair<int, int>(rowIndex, moveToIndex));
        for (int i = 1; i < rowsToMove.length(); ++i) {
            rowIndex = rowIds.indexOf(rowsToMove[i]);
            if (rowIndex == -1) {
                os.setError("Invalid row list");
                return;
            }
            if (rowIndex <= from_To[i - 1].first) {
                os.setError("List of rows to move is not ordered");
                return;
            }
            moveToIndex = rowIndex + delta > from_To[i - 1].second ? rowIndex + delta : from_To[i - 1].second + 1;
            from_To.append(QPair<int, int>(rowIndex, moveToIndex));
        }
    } else {
        int rowIndex = rowIds.indexOf(rowsToMove.last());
        if (rowIndex == -1) {
            os.setError("Invalid row list");
            return;
        }
        int moveToIndex = rowIndex + delta < rowIds.length() ? rowIndex + delta : rowIds.length() - 1;
        from_To.append(QPair<int, int>(rowIndex, moveToIndex));
        for (int i = 1; i < rowsToMove.length(); ++i) {
            rowIndex = rowIds.indexOf(rowsToMove[rowsToMove.length() - i - 1]);
            if (rowIndex == -1) {
                os.setError("Invalid row list");
                return;
            }
            if (rowIndex >= from_To[i - 1].first) {
                os.setError("List of rows to move is not ordered");
                return;
            }
            moveToIndex = rowIndex + delta < from_To[i - 1].second ? rowIndex + delta : from_To[i - 1].second - 1;
            from_To.append(QPair<int, int>(rowIndex, moveToIndex));
        }
    }
    QPair<int, int> coords;
    foreach (coords, from_To) {
        rowIds.move(coords.first, coords.second);
    }
    msaDbi->setNewRowsOrder(msaRef.entityId, rowIds, os);
    CHECK_OP(os, );
}

void MaDbiUtils::getStartAndEndSequencePositions(const QByteArray& seq, const QVector<U2MsaGap>& gaps, qint64 pos, qint64 count, qint64& startPosInSeq, qint64& endPosInSeq) {
    int rowLengthWithoutTrailingGap = MsaRowUtils::getRowLengthWithoutTrailing(seq, gaps);
    SAFE_POINT(pos < rowLengthWithoutTrailingGap, "Incorrect position!", );

    // Remove chars from the sequence
    // Calculate start position in the sequence
    if (U2Msa::GAP_CHAR == MsaRowUtils::charAt(seq, gaps, pos)) {
        int i = 1;
        while (U2Msa::GAP_CHAR == MsaRowUtils::charAt(seq, gaps, pos + i)) {
            if (MsaRowUtils::getRowLength(seq, gaps) == pos + i) {
                break;
            }
            i++;
        }
        startPosInSeq = MsaRowUtils::getUngappedPosition(gaps, seq.length(), pos + i);
    } else {
        startPosInSeq = MsaRowUtils::getUngappedPosition(gaps, seq.length(), pos);
    }

    // Calculate end position in the sequence
    int endRegionPos = pos + count;  // non-inclusive

    if (endRegionPos > rowLengthWithoutTrailingGap) {
        endRegionPos = rowLengthWithoutTrailingGap;
    }

    if (endRegionPos == rowLengthWithoutTrailingGap) {
        endPosInSeq = seq.length();
    } else {
        if (U2Msa::GAP_CHAR == MsaRowUtils::charAt(seq, gaps, endRegionPos)) {
            int i = 1;
            while (U2Msa::GAP_CHAR == MsaRowUtils::charAt(seq, gaps, endRegionPos + i)) {
                if (MsaRowUtils::getRowLength(seq, gaps) == endRegionPos + i) {
                    break;
                }
                i++;
            }
            endPosInSeq = MsaRowUtils::getUngappedPosition(gaps, seq.length(), endRegionPos + i);
        } else {
            endPosInSeq = MsaRowUtils::getUngappedPosition(gaps, seq.length(), endRegionPos);
        }
    }
}

DbiConnection* MaDbiUtils::getCheckedConnection(const U2DbiRef& dbiRef, U2OpStatus& os) {
    QScopedPointer<DbiConnection> con(new DbiConnection(dbiRef, os));
    CHECK_OP(os, nullptr);

    if (con->dbi == nullptr) {
        os.setError("NULL root dbi");
        return nullptr;
    }

    if (con->dbi->getMsaDbi() == nullptr) {
        os.setError("NULL MSA dbi");
        return nullptr;
    }

    if (con->dbi->getSequenceDbi() == nullptr) {
        os.setError("NULL sequence dbi");
        return nullptr;
    }
    return con.take();
}

/** Validates that all 'rowIds' contains in the alignment rows */
bool MaDbiUtils::validateRowIds(const MultipleSequenceAlignment& al, const QList<qint64>& rowIds) {
    QSet<qint64> alRowIds = al->getRowsIds().toSet();
    for (qint64 rowId : qAsConst(rowIds)) {
        if (!alRowIds.contains(rowId)) {
            coreLog.trace(QString("No row ID '%1' in '%2' alignment!").arg(rowId).arg(al->getName()));
            return false;
        }
    }
    return true;
}

void MaDbiUtils::validateRowIds(U2MsaDbi* msaDbi, const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) {
    QList<U2MsaRow> allRows = msaDbi->getRows(msaId, os);
    CHECK_OP(os, );
    QList<qint64> allRowIds;
    foreach (const U2MsaRow& row, allRows) {
        allRowIds << row.rowId;
    }
    foreach (qint64 rowId, rowIds) {
        if (!allRowIds.contains(rowId)) {
            os.setError(QString("No row ID '%1' in an alignment!").arg(rowId));
            return;
        }
    }
}

/////////////////////////////////////////////////////////////////
// Helper-methods for additional calculations

void MsaDbiUtils::calculateGapModelAfterInsert(QVector<U2MsaGap>& gapModel, qint64 pos, qint64 count) {
    SAFE_POINT(pos >= 0, QString("Invalid position '%1'!").arg(pos), );
    SAFE_POINT(count > 0, QString("Invalid characters count '%1'!").arg(count), );

    // There are no gaps yet
    if (gapModel.isEmpty()) {
        U2MsaGap gap(pos, count);
        gapModel.append(gap);
        return;
    }
    // There are gaps in the row
    else {
        // Insert gaps to the row beginning
        if (0 == pos) {
            U2MsaGap& firstGap = gapModel[0];
            if (0 == firstGap.startPos) {
                firstGap.length += count;
            } else {
                U2MsaGap beginningGap(0, count);
                gapModel.insert(0, beginningGap);
            }

            // Shift other gaps
            if (gapModel.count() > 1) {
                for (int i = 1; i < gapModel.count(); ++i) {
                    qint64 newOffset = gapModel[i].startPos + count;
                    gapModel[i].startPos = newOffset;
                }
            }
        }
        // Gaps are inserted to the middle of the row
        else {
            // A gap is near
            if (gapInPosition(gapModel, pos) ||
                gapInPosition(gapModel, pos - 1)) {
                // Find the gaps and append 'count' gaps to it
                // Shift all gaps that further in the row
                for (int i = 0; i < gapModel.count(); ++i) {
                    if (pos >= gapModel[i].startPos) {
                        if (pos <= gapModel[i].startPos + gapModel[i].length) {
                            gapModel[i].length += count;
                        }
                    } else {
                        gapModel[i].startPos += count;
                    }
                }
            }
            // Insert between chars
            else {
                bool found = false;

                int indexGreaterGaps = 0;
                for (int i = 0; i < gapModel.count(); ++i) {
                    if (pos > gapModel[i].startPos + gapModel[i].length) {
                        continue;
                    } else {
                        found = true;
                        U2MsaGap newGap(pos, count);
                        gapModel.insert(i, newGap);
                        indexGreaterGaps = i;
                        break;
                    }
                }

                // If found somewhere between existent gaps
                if (found) {
                    // Shift further gaps
                    for (int i = indexGreaterGaps + 1; i < gapModel.count(); ++i) {
                        gapModel[i].startPos += count;
                    }
                }
                // This is the last gap
                else {
                    U2MsaGap newGap(pos, count);
                    gapModel.append(newGap);
                }
            }
        }
    }
}

QList<U2MsaRow> MsaDbiUtils::cutOffLeadingGaps(QList<U2MsaRow>& rows) {
    int leadingGapsToRemove = INT_MAX;
    for (int i = 0; i < rows.length(); ++i) {
        // If some rows haven't any leading gaps,
        // If some rows haven't any gaps
        // If some rows first gap's offset isn't zero
        // return
        if (leadingGapsToRemove == 0 || rows[i].gaps.isEmpty() || rows[i].gaps.first().startPos != 0) {
            leadingGapsToRemove = 0;
            return QList<U2MsaRow>();
        }
        leadingGapsToRemove = qMin(leadingGapsToRemove, rows[i].gaps.first().length);
    }

    // If there is any leading gaps after all, they should be removed.
    if (leadingGapsToRemove != 0) {
        for (qint64 i = 0; i < rows.length(); ++i) {
            calculateGapModelAfterRemove(rows[i].gaps, 0, leadingGapsToRemove);
        }
    }
    return rows;
}

QList<U2MsaRow> MsaDbiUtils::cutOffTrailingGaps(QList<U2MsaRow>& rows, const qint64 msaLength) {
    QList<U2MsaRow> affectedRows;
    for (QList<U2MsaRow>::iterator rowIt = rows.begin(); rowIt < rows.end(); ++rowIt) {
        // If there are no gaps in the row, skip this row.
        if (rowIt->gaps.isEmpty()) {
            continue;
        }

        // Delete all gaps with offset after msa length.
        for (int gapReverseIndex = rowIt->gaps.size() - 1;
             gapReverseIndex >= 0 && gapReverseIndex < rowIt->gaps.size() && rowIt->gaps.at(gapReverseIndex).startPos > msaLength - 1;
             --gapReverseIndex) {
            rowIt->gaps.removeAt(gapReverseIndex++);
            affectedRows << *rowIt;
        }

        // Cut off all gaps with offset before msa length and end after msa length
        if (!rowIt->gaps.isEmpty() && rowIt->gaps.last().length + rowIt->gaps.last().startPos > msaLength) {
            rowIt->gaps.last().length = msaLength - rowIt->gaps.last().startPos;
            affectedRows << *rowIt;
        }
    }
    return affectedRows;
}

void MsaDbiUtils::calculateGapModelAfterRemove(QVector<U2MsaGap>& gapModel, qint64 pos, qint64 count) {
    QVector<U2MsaGap> newGapModel;
    qint64 endRegionPos = pos + count;  // non-inclusive
    foreach (U2MsaGap gap, gapModel) {
        qint64 gapEnd = gap.startPos + gap.length;
        if (gapEnd < pos) {
            newGapModel << gap;
        } else if (gapEnd <= endRegionPos) {
            if (gap.startPos < pos) {
                gap.length = pos - gap.startPos;
                newGapModel << gap;
            }
            // Otherwise just remove the gap (do not write to the new gap model)
        } else {
            if (gap.startPos < pos) {
                gap.length -= count;
                SAFE_POINT(gap.length >= 0, "Non-positive gap length!", );
                newGapModel << gap;
            } else if (gap.startPos < endRegionPos) {
                gap.length = gapEnd - endRegionPos;
                gap.startPos = pos;
                SAFE_POINT(gap.length > 0, "Non-positive gap length!", );
                SAFE_POINT(gap.startPos >= 0, "Negative gap offset!", );
                newGapModel << gap;
            } else {
                // Shift the gap
                gap.startPos -= count;
                SAFE_POINT(gap.startPos >= 0, "Negative gap offset!", );
                newGapModel << gap;
            }
        }
    }

    gapModel = newGapModel;
}

qint64 MsaDbiUtils::calculateGapsLength(const QVector<U2MsaGap>& gapModel) {
    qint64 length = 0;
    foreach (U2MsaGap elt, gapModel) {
        length += elt.length;
    }
    return length;
}

qint64 MsaDbiUtils::calculateRowLength(const U2MsaRow& row) {
    qint64 seqLength = row.gend - row.gstart;
    qint64 gapsLength = calculateGapsLength(row.gaps);
    return seqLength + gapsLength;
}

void MsaDbiUtils::mergeConsecutiveGaps(QVector<U2MsaGap>& gapModel) {
    QVector<U2MsaGap> newGapModel;
    if (gapModel.isEmpty()) {
        return;
    }

    newGapModel << gapModel[0];
    int indexInNewGapModel = 0;
    for (int i = 1; i < gapModel.count(); ++i) {
        int previousGapEnd = newGapModel[indexInNewGapModel].startPos +
                             newGapModel[indexInNewGapModel].length - 1;
        int currectGapStart = gapModel[i].startPos;
        SAFE_POINT(currectGapStart > previousGapEnd,
                   "Incorrect gap model during merging consecutive gaps!", );
        if (currectGapStart == previousGapEnd + 1) {
            // Merge gaps
            qint64 newGapLength = newGapModel[indexInNewGapModel].length + gapModel[i].length;
            SAFE_POINT(newGapLength > 0, "Non-positive gap length!", )
            newGapModel[indexInNewGapModel].length = newGapLength;
        } else {
            // Add the gap to the list
            newGapModel << gapModel[i];
            indexInNewGapModel++;
        }
    }
    gapModel = newGapModel;
}

void MsaDbiUtils::removeCharsFromRow(QByteArray& seq, QVector<U2MsaGap>& gaps, qint64 pos, qint64 count) {
    SAFE_POINT(pos >= 0, "Incorrect position!", );
    SAFE_POINT(count > 0, "Incorrect characters count!", );

    if (pos >= MsaRowUtils::getRowLength(seq, gaps)) {
        return;
    }

    if (pos < MsaRowUtils::getRowLengthWithoutTrailing(seq, gaps)) {
        qint64 startPosInSeq = -1;
        qint64 endPosInSeq = -1;
        MaDbiUtils::getStartAndEndSequencePositions(seq, gaps, pos, count, startPosInSeq, endPosInSeq);

        if ((startPosInSeq < endPosInSeq) && (-1 != startPosInSeq) && (-1 != endPosInSeq)) {
            U2OpStatus2Log os;
            DNASequenceUtils::removeChars(seq, startPosInSeq, endPosInSeq, os);
            SAFE_POINT_OP(os, );
        }
    }

    calculateGapModelAfterRemove(gaps, pos, count);
    mergeConsecutiveGaps(gaps);
}

void MaDbiUtils::calculateGapModelAfterReplaceChar(QVector<U2MsaGap>& gapModel, qint64 pos) {
    SAFE_POINT(pos >= 0, QString("Invalid position '%1'!").arg(pos), );

    for (int i = 0; i < gapModel.count(); ++i) {
        U2MsaGap& curGap = gapModel[i];
        qint64 gapStart = curGap.startPos;
        qint64 gapEnd = gapStart + curGap.length;
        if (pos >= gapStart && pos <= gapEnd) {
            if (pos == gapStart) {
                if (curGap.length == 1) {
                    gapModel.removeAt(i);
                } else {
                    curGap.startPos++;
                    curGap.length--;
                }
            } else if (pos == gapEnd - 1) {
                if (curGap.length == 1) {
                    gapModel.removeAt(i);
                } else {
                    curGap.length--;
                }
            } else {
                gapModel.removeAt(i);
                U2MsaGap firstGap(gapStart, pos - gapStart);
                U2MsaGap secondGap(pos + 1, gapEnd - pos - 1);
                gapModel.insert(i, secondGap);
                gapModel.insert(i, firstGap);
            }
            break;
        }
    }
}

void MsaDbiUtils::replaceCharsInRow(QByteArray& sequence, QVector<U2MsaGap>& gaps, const U2Region& range, char newChar, U2OpStatus& os) {
    for (qint64 pos = range.startPos; pos < range.endPos(); pos++) {
        qint64 lengthWithNoTrail = MsaRowUtils::getRowLengthWithoutTrailing(sequence, gaps);
        if (pos < lengthWithNoTrail) {
            qint64 posInSeq = -1;
            qint64 endPosInSeq = -1;
            MaDbiUtils::getStartAndEndSequencePositions(sequence, gaps, pos, 1, posInSeq, endPosInSeq);
            if (posInSeq >= 0 && endPosInSeq > posInSeq) {
                DNASequenceUtils::replaceChars(sequence, posInSeq, QByteArray(1, newChar), os);
                CHECK_OP(os, );
            } else {
                DNASequenceUtils::insertChars(sequence, posInSeq, QByteArray(1, newChar), os);
                CHECK_OP(os, );
                MaDbiUtils::calculateGapModelAfterReplaceChar(gaps, pos);
            }
        } else {
            sequence.append(newChar);
            if (pos != lengthWithNoTrail) {
                gaps.append(U2MsaGap(lengthWithNoTrail, pos - lengthWithNoTrail));
            }
        }
    }
}

void MsaDbiUtils::cropCharsFromRow(MultipleSequenceAlignmentRow& alRow, qint64 pos, qint64 count) {
    SAFE_POINT(pos >= 0, "Incorrect position!", );
    SAFE_POINT(count > 0, "Incorrect characters count!", );

    // Change the sequence
    qint64 initialRowLength = alRow->getRowLength();
    qint64 initialSeqLength = alRow->getUngappedLength();
    DNASequence modifiedSeq = alRow->getSequence();

    if (pos >= alRow->getRowLengthWithoutTrailing()) {
        DNASequenceUtils::makeEmpty(modifiedSeq);
    } else {
        qint64 startPosInSeq = -1;
        qint64 endPosInSeq = -1;
        MaDbiUtils::getStartAndEndSequencePositions(alRow->getSequence().seq, alRow->getGaps(), pos, count, startPosInSeq, endPosInSeq);

        // Remove inside a gap
        if ((startPosInSeq <= endPosInSeq) && (-1 != startPosInSeq) && (-1 != endPosInSeq)) {
            U2OpStatus2Log os;
            if (endPosInSeq < initialSeqLength) {
                DNASequenceUtils::removeChars(modifiedSeq, endPosInSeq, initialSeqLength, os);
                SAFE_POINT_OP(os, );
            }

            if (startPosInSeq > 0) {
                DNASequenceUtils::removeChars(modifiedSeq, 0, startPosInSeq, os);
                SAFE_POINT_OP(os, );
            }
        }
    }

    // Change the gap model
    QVector<U2MsaGap> gapModel = alRow->getGaps();
    if (pos + count < initialRowLength) {
        calculateGapModelAfterRemove(gapModel, pos + count, initialRowLength - pos - count);
    }

    if (pos > 0) {
        calculateGapModelAfterRemove(gapModel, 0, pos);
    }
    U2OpStatusImpl os;
    alRow->setRowContent(modifiedSeq, gapModel, os);
    CHECK_OP(os, );
}

/** Returns "true" if there is a gap on position "pos" */
bool MsaDbiUtils::gapInPosition(const QVector<U2MsaGap>& gapModel, qint64 pos) {
    foreach (const U2MsaGap& gap, gapModel) {
        if (gap.startPos + gap.length - 1 < pos) {
            continue;
        }
        if (gap.startPos > pos) {
            return false;
        }
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////
// MSA DBI Utilities

void MsaDbiUtils::updateMsa(const U2EntityRef& msaRef, const MultipleSequenceAlignment& ma, U2OpStatus& os) {
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, "NULL Msa Dbi!", );

    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    SAFE_POINT(seqDbi != nullptr, "NULL Sequence Dbi!", );

    U2AttributeDbi* attrDbi = con.dbi->getAttributeDbi();
    SAFE_POINT(attrDbi != nullptr, "NULL Attribute Dbi!", );

    //// UPDATE MSA OBJECT
    const DNAAlphabet* originalMaAlphabet = ma->getAlphabet();
    SAFE_POINT(originalMaAlphabet != nullptr, "The alignment alphabet is NULL!", );

    U2Msa msaObj;
    msaObj.id = msaRef.entityId;
    msaObj.visualName = ma->getName();
    msaObj.alphabet.id = originalMaAlphabet->getId();
    msaObj.length = ma->getLength();

    msaDbi->updateMsaName(msaRef.entityId, ma->getName(), os);
    CHECK_OP(os, );

    msaDbi->updateMsaAlphabet(msaRef.entityId, originalMaAlphabet->getId(), os);
    CHECK_OP(os, );

    msaDbi->updateMsaLength(msaRef.entityId, ma->getLength(), os);
    CHECK_OP(os, );

    //// UPDATE ROWS AND SEQUENCES
    // Get rows that are currently stored in the database
    QList<U2MsaRow> currentRows = msaDbi->getRows(msaRef.entityId, os);
    QList<qint64> currentRowIds;
    CHECK_OP(os, );

    QList<qint64> newRowsIds = ma->getRowsIds();
    QList<qint64> eliminatedRows;

    for (const U2MsaRow& currentRow : qAsConst(currentRows)) {
        currentRowIds.append(currentRow.rowId);

        // Update data for rows with the same row and sequence IDs
        if (newRowsIds.contains(currentRow.rowId)) {
            // Update sequence and row info
            U2MsaRow newRow = ma->getMsaRowByRowId(currentRow.rowId, os)->getRowDbInfo();
            CHECK_OP(os, );

            if (newRow.sequenceId != currentRow.sequenceId) {
                // Kill the row from the current alignment, it is incorrect. New row with this ID will be created later.
                MsaDbiUtils::removeRow(msaRef, currentRow.rowId, os);
                CHECK_OP(os, );

                currentRowIds.removeOne(currentRow.rowId);
                continue;
            }

            DNASequence sequence = ma->getMsaRowByRowId(newRow.rowId, os)->getSequence();
            CHECK_OP(os, );

            msaDbi->updateRowName(msaRef.entityId, newRow.rowId, sequence.getName(), os);
            CHECK_OP(os, );

            msaDbi->updateRowContent(msaRef.entityId, newRow.rowId, sequence.seq, newRow.gaps, os);
            CHECK_OP(os, );
        } else {
            // Remove rows that are no more present in the alignment
            eliminatedRows.append(currentRow.rowId);
        }
    }

    msaDbi->removeRows(msaRef.entityId, eliminatedRows, os);
    CHECK_OP(os, );

    // Add rows that are stored in memory, but are not present in the database,
    // remember the rows order
    QList<qint64> rowsOrder;
    for (int i = 0, n = ma->getRowCount(); i < n; ++i) {
        const MultipleSequenceAlignmentRow alRow = ma->getMsaRow(i);
        U2MsaRow row = alRow->getRowDbInfo();

        if (row.sequenceId.isEmpty() || !currentRowIds.contains(row.rowId)) {
            // Import the sequence
            DNASequence rowSeq = alRow->getSequence();
            U2Sequence sequence = U2Sequence();
            sequence.visualName = rowSeq.getName();
            sequence.circular = rowSeq.circular;
            sequence.length = rowSeq.length();

            const DNAAlphabet* rowSequenceAlphabet = rowSeq.alphabet;
            if (rowSequenceAlphabet == nullptr) {
                rowSequenceAlphabet = U2AlphabetUtils::findBestAlphabet(rowSeq.constData(), rowSeq.length());
            }
            SAFE_POINT(rowSequenceAlphabet != nullptr, "Failed to get alphabet for a sequence!", );
            sequence.alphabet.id = rowSequenceAlphabet->getId();

            seqDbi->createSequenceObject(sequence, "", os);
            CHECK_OP(os, );

            QVariantMap hints;
            const QByteArray& seqData = rowSeq.constSequence();
            seqDbi->updateSequenceData(sequence.id, U2_REGION_MAX, seqData, hints, os);
            CHECK_OP(os, );

            // Create the row
            row.rowId = -1;  // set the row ID automatically
            row.sequenceId = sequence.id;
            row.gstart = 0;
            row.gend = sequence.length;
            row.gaps = alRow->getGaps();
            MsaDbiUtils::addRow(msaRef, -1, row, os);
            CHECK_OP(os, );
        }
        rowsOrder.append(row.rowId);
    }

    //// UPDATE ROWS POSITIONS
    msaDbi->setNewRowsOrder(msaRef.entityId, rowsOrder, os);

    //// UPDATE MALIGNMENT ATTRIBUTES
    QVariantMap alInfo = ma->getInfo();

    foreach (QString key, alInfo.keys()) {
        QString val = alInfo.value(key).value<QString>();
        U2StringAttribute attr(msaRef.entityId, key, val);

        attrDbi->createStringAttribute(attr, os);
        CHECK_OP(os, );
    }
}

void MsaDbiUtils::updateRowContent(const U2EntityRef& msaRef, qint64 rowId, const QByteArray& seqBytes, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Update the data
    msaDbi->updateRowContent(msaRef.entityId, rowId, seqBytes, gaps, os);
}

void MsaDbiUtils::insertGaps(const U2EntityRef& msaRef, const QList<qint64>& rowIds, qint64 pos, qint64 count, U2OpStatus& os, bool collapseTrailingGaps) {
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Get the MSA properties
    const U2Msa msaObj = msaDbi->getMsaObject(msaRef.entityId, os);
    const qint64 alLength = msaObj.length;

    // Validate the position
    if (pos < 0 || pos > alLength) {
        coreLog.trace(QString("Invalid position '%1' in '%2' alignment!").arg(pos).arg(msaObj.visualName));
        os.setError(tr("Failed to insert gaps into an alignment!"));
        return;
    }

    // Validate the count of gaps
    if (count <= 0) {
        coreLog.trace(QString("Invalid value of characters count '%1'!").arg(count));
        os.setError(tr("Failed to insert gaps into an alignment!"));
        return;
    }

    // Insert gaps into rows
    QList<U2MsaRow> rows;
    foreach (qint64 rowId, rowIds) {
        const U2MsaRow row = msaDbi->getRow(msaRef.entityId, rowId, os);
        CHECK_OP(os, );

        rows.append(row);
    }

    int trailingGapsColumns = count;
    foreach (U2MsaRow row, rows) {
        // Calculate the new gap model
        calculateGapModelAfterInsert(row.gaps, pos, count);

        // Trim trailing gap (if any)
        qint64 seqLength = row.gend - row.gstart;
        qint64 gapsLength = 0;
        int diff = alLength - row.length;
        trailingGapsColumns = diff < trailingGapsColumns ? diff : trailingGapsColumns;

        for (int i = 0, n = row.gaps.count(); i < n; ++i) {
            const U2MsaGap& gap = row.gaps[i];
            if ((i == n - 1) && (gap.startPos >= seqLength + gapsLength)) {
                row.gaps.removeAt(i);
                break;
            }
            gapsLength += gap.length;
        }

        // Put the new gap model into the database
        msaDbi->updateGapModel(msaRef.entityId, row.rowId, row.gaps, os);
        CHECK_OP(os, );
    }

    if (collapseTrailingGaps) {
        qint64 enlargeAlignmentLength = 0;
        foreach (qint64 rowId, rowIds) {
            enlargeAlignmentLength = qMax(enlargeAlignmentLength, msaDbi->getRow(msaRef.entityId, rowId, os).length);
            CHECK_OP(os, );
        }
        if (msaObj.length < enlargeAlignmentLength) {
            msaDbi->updateMsaLength(msaRef.entityId, enlargeAlignmentLength, os);
            CHECK_OP(os, );
        }
    } else {
        int newLength = msaObj.length + count - trailingGapsColumns;
        if (msaObj.length < newLength) {
            msaDbi->updateMsaLength(msaRef.entityId, msaObj.length + count, os);
        }
        CHECK_OP(os, );
    }
}

void MsaDbiUtils::removeRegion(const U2EntityRef& msaRef, const QList<qint64>& rowIds, qint64 pos, qint64 count, U2OpStatus& os) {
    // Check parameters
    CHECK_EXT(pos >= 0, os.setError(QString("Negative MSA pos: %1").arg(pos)), );
    CHECK_EXT(count > 0, os.setError(QString("Wrong MSA base count: %1").arg(count)), );

    // Prepare the connection
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(msaRef.dbiRef, os));
    SAFE_POINT_OP(os, );
    U2MsaDbi* msaDbi = con->dbi->getMsaDbi();
    U2SequenceDbi* sequenceDbi = con->dbi->getSequenceDbi();

    U2Msa msa = msaDbi->getMsaObject(msaRef.entityId, os);
    SAFE_POINT_OP(os, );

    MaDbiUtils::validateRowIds(msaDbi, msaRef.entityId, rowIds, os);
    CHECK_OP(os, );

    qint64 rowNum = msaDbi->getNumOfRows(msaRef.entityId, os);
    bool keepAlignmentLength = true;
    if (rowNum == rowIds.size()) {
        keepAlignmentLength = false;
    }

    // Remove region for each row from the list
    foreach (qint64 rowId, rowIds) {
        U2MsaRow row = msaDbi->getRow(msaRef.entityId, rowId, os);
        SAFE_POINT_OP(os, );

        U2Region seqReg(row.gstart, row.gend - row.gstart);
        QByteArray seq = sequenceDbi->getSequenceData(row.sequenceId, seqReg, os);
        SAFE_POINT_OP(os, );

        // Calculate the modified row
        removeCharsFromRow(seq, row.gaps, pos, count);

        msaDbi->updateRowContent(msaRef.entityId, rowId, seq, row.gaps, os);
        SAFE_POINT_OP(os, );
    }
    if (!keepAlignmentLength) {
        msaDbi->updateMsaLength(msaRef.entityId, msa.length - count, os);
    }
}

void MsaDbiUtils::replaceCharactersInRow(const U2EntityRef& msaRef, qint64 rowId, const U2Region& range, char newChar, U2OpStatus& os) {
    SAFE_POINT_EXT(newChar != U2Msa::GAP_CHAR, os.setError("Can't use GAP for replacement!"), );

    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(msaRef.dbiRef, os));
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con->dbi->getMsaDbi();
    U2SequenceDbi* sequenceDbi = con->dbi->getSequenceDbi();

    U2Msa msa = msaDbi->getMsaObject(msaRef.entityId, os);
    CHECK_OP(os, );

    MaDbiUtils::validateRowIds(msaDbi, msaRef.entityId, {rowId}, os);
    CHECK_OP(os, );

    U2MsaRow row = msaDbi->getRow(msaRef.entityId, rowId, os);
    CHECK_OP(os, );

    qint64 msaLength = msaDbi->getMsaLength(msaRef.entityId, os);
    CHECK_OP(os, );
    CHECK_EXT(U2Region(0, msaLength).contains(range), os.setError(tr("Invalid range: %1 %2").arg(range.startPos).arg(range.endPos())), );

    U2Region sequenceRange(row.gstart, row.gend - row.gstart);
    QByteArray sequence = sequenceDbi->getSequenceData(row.sequenceId, sequenceRange, os);
    CHECK_OP(os, );

    replaceCharsInRow(sequence, row.gaps, range, newChar, os);
    CHECK_OP(os, );

    msaDbi->updateRowContent(msaRef.entityId, rowId, sequence, row.gaps, os);
}

QList<qint64> MsaDbiUtils::replaceNonGapCharacter(const U2EntityRef& msaRef, char oldChar, char newChar, U2OpStatus& os) {
    QList<qint64> modifiedRowIds;
    if (oldChar == newChar) {
        return modifiedRowIds;
    }
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(msaRef.dbiRef, os));
    CHECK_OP(os, modifiedRowIds);

    U2MsaDbi* msaDbi = con->dbi->getMsaDbi();
    U2SequenceDbi* sequenceDbi = con->dbi->getSequenceDbi();

    QList<qint64> rowIds = msaDbi->getOrderedRowIds(msaRef.entityId, os);
    CHECK_OP(os, modifiedRowIds);
    for (qint64 rowId : qAsConst(rowIds)) {
        U2MsaRow msaRow = msaDbi->getRow(msaRef.entityId, rowId, os);
        CHECK_OP(os, modifiedRowIds);
        U2Region sequenceRegion(msaRow.gstart, msaRow.gend - msaRow.gstart);
        QByteArray sequenceData = sequenceDbi->getSequenceData(msaRow.sequenceId, sequenceRegion, os);
        if (sequenceData.contains(oldChar)) {
            sequenceData.replace(oldChar, newChar);
            msaDbi->updateRowContent(msaRef.entityId, rowId, sequenceData, msaRow.gaps, os);
            CHECK_OP(os, modifiedRowIds);
            modifiedRowIds << rowId;
        }
    }
    return modifiedRowIds;
}

QList<qint64> MsaDbiUtils::keepOnlyAlphabetChars(const U2EntityRef& msaRef, const DNAAlphabet* alphabet, const QByteArray& replacementMap, U2OpStatus& os) {
    QList<qint64> modifiedRowIds;
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(msaRef.dbiRef, os));
    CHECK_OP(os, modifiedRowIds);

    bool hasReplacementMap = replacementMap.size() == 256;
    SAFE_POINT(hasReplacementMap || replacementMap.isEmpty(), "replacementMap has invalid size: " + QString::number(replacementMap.size()), modifiedRowIds);

    U2MsaDbi* msaDbi = con->dbi->getMsaDbi();
    U2SequenceDbi* sequenceDbi = con->dbi->getSequenceDbi();

    QList<qint64> rowIds = msaDbi->getOrderedRowIds(msaRef.entityId, os);
    CHECK_OP(os, modifiedRowIds);
    QByteArray alphabetChars = alphabet->getAlphabetChars();
    QBitArray validCharsMap = TextUtils::createBitMap(alphabetChars);
    char defaultChar = alphabet->getDefaultSymbol();
    for (qint64 rowId : qAsConst(rowIds)) {
        U2MsaRow msaRow = msaDbi->getRow(msaRef.entityId, rowId, os);
        CHECK_OP(os, modifiedRowIds);
        U2Region sequenceRegion(msaRow.gstart, msaRow.gend - msaRow.gstart);
        QByteArray sequenceData = sequenceDbi->getSequenceData(msaRow.sequenceId, sequenceRegion, os);
        bool isModified = false;
        for (int i = 0, n = sequenceData.length(); i < n; i++) {
            char c = sequenceData[i];
            if (!validCharsMap.testBit(c)) {
                isModified = true;
                char newChar = hasReplacementMap ? replacementMap[(quint8)c] : '\0';
                sequenceData[i] = validCharsMap.testBit(newChar) ? newChar : defaultChar;
            }
        }
        if (isModified) {
            msaDbi->updateRowContent(msaRef.entityId, rowId, sequenceData, msaRow.gaps, os);
            CHECK_OP(os, modifiedRowIds);
            modifiedRowIds << rowId;
        }
    }
    return modifiedRowIds;
}

QList<qint64> MsaDbiUtils::removeEmptyRows(const U2EntityRef& msaRef, const QList<qint64>& rowIds, U2OpStatus& os) {
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(msaRef.dbiRef, os));
    SAFE_POINT_OP(os, QList<qint64>());
    U2MsaDbi* msaDbi = con->dbi->getMsaDbi();
    U2SequenceDbi* sequenceDbi = con->dbi->getSequenceDbi();

    MaDbiUtils::validateRowIds(msaDbi, msaRef.entityId, rowIds, os);
    CHECK_OP(os, QList<qint64>());

    // find empty rows
    QList<qint64> emptyRowIds;
    foreach (qint64 rowId, rowIds) {
        U2MsaRow row = msaDbi->getRow(msaRef.entityId, rowId, os);
        SAFE_POINT_OP(os, QList<qint64>());
        U2Sequence seq = sequenceDbi->getSequenceObject(row.sequenceId, os);
        SAFE_POINT_OP(os, QList<qint64>());
        if (0 == seq.length) {
            emptyRowIds << row.rowId;
        }
    }
    if (!emptyRowIds.isEmpty()) {
        // remove empty rows
        msaDbi->removeRows(msaRef.entityId, emptyRowIds, os);
        SAFE_POINT_OP(os, QList<qint64>());
    }
    return emptyRowIds;
}

void MsaDbiUtils::crop(const U2EntityRef& msaRef, const QList<qint64>& rowIds, const U2Region& columnRange, U2OpStatus& os) {
    // Get the alignment.
    MultipleSequenceAlignmentExporter alExporter;
    MultipleSequenceAlignment al = alExporter.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_OP_EXT(os, uiLog.error(os.getError()), );

    // Validate the parameters.
    if (columnRange.startPos < 0 || columnRange.endPos() > al->getLength()) {
        os.setError(tr("Invalid crop column range: %1..%2").arg(columnRange.startPos + 1).arg(columnRange.endPos()));
        uiLog.error(os.getError());
        return;
    }
    if (rowIds.isEmpty()) {
        os.setError(tr("List of ids to crop is empty"));
        uiLog.error(os.getError());
        return;
    }
    if (!MaDbiUtils::validateRowIds(al, rowIds)) {
        os.setError(tr("Invalid crop row ids"));
        uiLog.error(os.getError());
        return;
    }
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, "NULL Msa Dbi!", );

    bool isRowLengthChanged = columnRange.length < al->getLength();

    // Crop or remove each row.
    for (int i = 0, n = al->getRowCount(); i < n; i++) {
        MultipleSequenceAlignmentRow row = al->getMsaRow(i)->getExplicitCopy();
        qint64 rowId = row->getRowId();
        if (!rowIds.contains(rowId)) {
            MsaDbiUtils::removeRow(msaRef, row->getRowId(), os);
            CHECK_OP(os, );
            continue;
        }
        if (!isRowLengthChanged) {
            continue;  // do not touch this column at all.
        }
        U2DataId sequenceId = row->getRowDbInfo().sequenceId;
        SAFE_POINT(!sequenceId.isEmpty(), "Empty sequence ID!", );

        // Calculate the modified row.
        cropCharsFromRow(row, columnRange.startPos, columnRange.length);

        // Put the new sequence and gap model into the database.
        msaDbi->updateRowContent(msaRef.entityId, rowId, row->getSequence().constSequence(), row->getGaps(), os);
        CHECK_OP(os, );
    }
    if (isRowLengthChanged) {
        msaDbi->updateMsaLength(msaRef.entityId, columnRange.length, os);
    }
    CHECK_OP(os, );
}

QList<qint64> MsaDbiUtils::trim(const U2EntityRef& msaRef, U2OpStatus& os) {
    const QList<qint64> invalidResult;
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, invalidResult);

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", invalidResult);

    qint64 msaLength = msaDbi->getMsaObject(msaRef.entityId, os).length;
    CHECK_OP(os, invalidResult);
    SAFE_POINT(msaLength >= 0, "Msa length is negative.", invalidResult);

    QList<U2MsaRow> rows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_OP(os, invalidResult);
    SAFE_POINT(!rows.isEmpty(), "Msa rows list is empty.", invalidResult);

    // Trim trailing gaps from gap model
    QList<U2MsaRow> modifiedRows = cutOffTrailingGaps(rows, msaLength);

    // Trim leading gaps, it changes length of msa, msaLength doesn`t update.
    const QList<U2MsaRow> cutOffStartResult = cutOffLeadingGaps(rows);
    // if cutting off leading gaps was performed then all rows has changed

    if (!cutOffStartResult.isEmpty()) {
        modifiedRows << cutOffStartResult;
    }

    QList<qint64> modifiedRowIds;
    // Update gap model
    foreach (U2MsaRow row, modifiedRows) {
        msaDbi->updateGapModel(msaRef.entityId, row.rowId, row.gaps, os);
        CHECK_OP(os, invalidResult);
        modifiedRowIds.append(row.rowId);
    }

    qint64 newMsaLen = -1;
    // check if rows contains modified rows whitoud leading gaps
    rows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_OP(os, invalidResult);
    SAFE_POINT(!rows.isEmpty(), "Msa rows list is empty.", invalidResult);
    foreach (U2MsaRow row, rows) {
        if (row.length != 0) {
            // row is not empty
            if (newMsaLen == -1) {
                newMsaLen = row.length;
            } else {
                newMsaLen = qMax(newMsaLen, row.length);
            }
        }
    }
    if (newMsaLen == -1) {
        // alignment is empty or full of gaps
        newMsaLen = 0;
    }

    msaDbi->updateMsaLength(msaRef.entityId, newMsaLen, os);
    return modifiedRowIds;
}

void MsaDbiUtils::addRow(const U2EntityRef& msaRef, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    // Validate the parameters
    SAFE_POINT_EXT(!row.sequenceId.isEmpty(), os.setError("Invalid sequence reference"), );

    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Add the row
    msaDbi->addRow(msaRef.entityId, posInMsa, row, os);
    CHECK_OP(os, );
}

void MsaDbiUtils::removeRow(const U2EntityRef& msaRef, qint64 rowId, U2OpStatus& os) {
    // Prepare the connection
    DbiConnection con(msaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(nullptr != msaDbi, "NULL Msa Dbi!", );

    // Remove the row
    msaDbi->removeRow(msaRef.entityId, rowId, os);
}

}  // namespace U2
