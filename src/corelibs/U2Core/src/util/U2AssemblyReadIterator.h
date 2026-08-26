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

#include <U2Core/U2Assembly.h>

namespace U2 {

// TODO: remove hard-coded '-' gap symbol. Allow custom gap symbols ?

/**
 * Iterator for Next-Gen Sequencing short reads based on CIGAR model
 * (see enum U2CigarOp for details about CIGAR operations). Provides transparent
 * iterations through read sequence, skipping clipping/padding/insertions and
 * returning special symbols when going through deletions or skipped regions.
 *
 * Handling of CIGAR operations:
 * 1. Hard clips (H) and padding (P) operations are silently skipped.
 * 2. Soft clips (S) and insertions (I) do present in read sequence. So this operations
 *    are skipped together with the corresponding letters of the sequence.
 *    Letters skipped for an insertion are still reported by getInsertionBeforeLastLetter().
 * 3. Deletions (D) and skipped regions (N) are 'virtual letters'. Iterator returns gap
 *    symbol when iterating through them.
 * 4. Matches/mismatches (M/=/X) are treated normally.
 */
class U2CORE_EXPORT U2AssemblyReadIterator {
public:
    /**
     * Constructs iterator from raw data and CIGAR model. Iterations can be started
     * form any position using startPos argument.
     */
    U2AssemblyReadIterator(const QByteArray& read_, QList<U2CigarToken> cigar_, int startPos = 0);

    /**
     * Returns true if the next letter is a match (mismatch) or deletion/skip.
     * If the read ends with clipping/insertion/etc hasNext() returns false.
     */
    bool hasNext() const;

    /**
     * Returns next letter for match/mismatch or gap symbol for deletion/skip.
     */
    char nextLetter();

    /**
     * Read letters of the insertions (I) that were skipped right before the letter returned by
     * the last nextLetter() call, empty if there was no insertion there. Insertions consumed while
     * seeking to 'startPos' are not reported: they are located before the first returned letter.
     * Soft clips (S) are never reported: they are not aligned to the reference at all.
     */
    const QByteArray& getInsertionBeforeLastLetter() const {
        return insertionBeforeLastLetter;
    }

private:
    void advanceToNextToken();

    void skip();
    void skipInsertion();
    void skipPaddingAndHardClip();

    bool isMatch() const;
    bool isInsertion() const;
    bool isDeletion() const;
    bool isPaddingOrHardClip() const;

    int offsetInRead;
    const QByteArray& read;

    int offsetInToken;
    int offsetInCigar;
    QList<U2CigarToken> cigar;

    /** Collected by skipInsertion(), reset on every nextLetter() call. */
    QByteArray insertionBeforeLastLetter;
    /** While true skipInsertion() does not collect anything: used to ignore the initial seek. */
    bool seeking;
};

extern void shortReadIteratorSmokeTest();

}  // namespace U2
