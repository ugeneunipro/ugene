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

#ifndef _U2_DNA_SEQUENCE_UTILS_H_
#define _U2_DNA_SEQUENCE_UTILS_H_

#include <U2Core/DNASequence.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Region.h>

namespace U2 {

enum DnaSequencesMatchStatus {
    MatchExactly,
    DoNotMatch
};

/** Utilities for DNASequences */
class U2CORE_EXPORT DNASequenceUtils {
public:
    /** Appends "appendedSequence" to "sequence" */
    static void append(DNASequence& sequence, const DNASequence& appendedSequence);

    /** Compares two sequences */
    static DnaSequencesMatchStatus compare(const DNASequence& firstSeq, const DNASequence& secondSec);

    /** Removes chars from 'startPos' (inclusive) to 'endPos' (non-inclusive) */
    static void removeChars(DNASequence& sequence, int startPos, int endPos, U2OpStatus& os);
    static void removeChars(QByteArray& sequence, int startPos, int endPos, U2OpStatus& os);

    /** Replace chars from 'startPos' (inclusive) to 'endPos' (non-inclusive) */
    static void replaceChars(QByteArray& sequence, int startPos, const QByteArray& newChars, U2OpStatus& os);

    /** Insert chars to 'startPos' */
    static void insertChars(QByteArray& sequence, int startPos, const QByteArray& newChars, U2OpStatus& os);

    /** Converts characters of the sequence to upper case */
    static void toUpperCase(DNASequence& sequence);

    /** Make the sequence empty (do not change name, alphabet, etc.) */
    static void makeEmpty(DNASequence& sequence);

    /** Returns the reverse sequence */
    static QByteArray reverse(const QByteArray &sequence);

    /** Returns the reverse DNASequence */
    static DNASequence reverse(const DNASequence& sequence);

    /** Returns the complement sequence */
    static QByteArray complement(const QByteArray &sequence, const DNAAlphabet *alphabet = nullptr);

    /** Returns the complement DNASequence */
    static DNASequence complement(const DNASequence& sequence);

    /** Returns the reverse-complement sequence */
    static QByteArray reverseComplement(const QByteArray &sequence, const DNAAlphabet *alphabet = nullptr);

    /** Returns the reverse-complement DNASequence */
    static DNASequence reverseComplement(const DNASequence& sequence);

    static void crop(DNASequence &sequence, int startPos, int length);

    static U2Region trimByQuality(DNASequence &sequence, int qualityThreshold, int minSequenceLength, bool trimBothEnds);
};

} // namespace

#endif
