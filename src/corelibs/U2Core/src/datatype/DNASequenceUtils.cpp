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

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DNASequenceUtils.h"

namespace U2 {

void DNASequenceUtils::append(DNASequence& sequence, const DNASequence& appendedSequence) {
    sequence.seq += appendedSequence.constSequence();
}

DnaSequencesMatchStatus DNASequenceUtils::compare(const DNASequence& firstSeq, const DNASequence& secondSec) {
    if (firstSeq.constSequence() == secondSec.constSequence()) {
        return MatchExactly;
    } else {
        return DoNotMatch;
    }
}

void DNASequenceUtils::removeChars(DNASequence& sequence, int startPos, int endPos, U2OpStatus& os) {
    removeChars(sequence.seq, startPos, endPos, os);
}

void DNASequenceUtils::removeChars(QByteArray& sequence, int startPos, int endPos, U2OpStatus& os) {
    if ((endPos <= startPos) || (startPos < 0) || (endPos > sequence.length())) {
        coreLog.trace(L10N::internalError("incorrect parameters was passed to DNASequenceUtils::removeChars, "
            "startPos '%1', endPos '%2', sequence length '%3'").arg(startPos).arg(endPos).arg(sequence.length()));
        os.setError("Can't remove chars from a sequence.");
        return;
    }

    sequence.remove(startPos, endPos - startPos);
}

void DNASequenceUtils::insertChars(QByteArray& sequence, int startPos, const QByteArray& newChars, U2OpStatus& os) {
    int endPos = startPos + newChars.length();
    CHECK_EXT(newChars.length() > 0, os.setError("Array of chars for replacing is empty!"), );
    if ((startPos < 0) || (startPos > sequence.length())) {
        coreLog.trace(L10N::internalError("incorrect parameters was passed to DNASequenceUtils::insertChars, "
            "startPos '%1', endPos '%2', sequence length '%3'!").arg(startPos).arg(endPos).arg(sequence.length()));
        os.setError("Can't remove chars from a sequence.");
        return;
    }

    sequence.insert(startPos, newChars);
}

void DNASequenceUtils::replaceChars(QByteArray& sequence, int startPos, const QByteArray& newChars, U2OpStatus& os) {
    int endPos = startPos + newChars.length();
    CHECK_EXT(newChars.length() > 0, os.setError("Array of chars for replacing is empty!"), );
    if ((startPos < 0) || (endPos > sequence.length())) {
        coreLog.trace(QString("Internal error: incorrect parameters was passed to DNASequenceUtils::replaceChars,"
            "startPos '%1', endPos '%2', sequence length '%3'!").arg(startPos).arg(endPos).arg(sequence.length()));
        os.setError("Can't remove chars from a sequence.");
        return;
    }

    sequence.replace(startPos, newChars.length(), newChars);
}

void DNASequenceUtils::toUpperCase(DNASequence& sequence) {
    sequence.seq = sequence.seq.toUpper();
}

void DNASequenceUtils::makeEmpty(DNASequence& sequence) {
    sequence.seq = QByteArray();
}

QByteArray DNASequenceUtils::reverse(const QByteArray &sequence) {
    QByteArray result = sequence;
    TextUtils::reverse(result.data(), result.length());
    return result;
}

DNASequence DNASequenceUtils::reverse(const DNASequence& dnaSequence) {
    DNASequence newDnaSequence(dnaSequence);
    newDnaSequence.seq = DNASequenceUtils::reverse(dnaSequence.seq);
    newDnaSequence.quality = DNAQuality(DNASequenceUtils::reverse(dnaSequence.quality.qualCodes), dnaSequence.quality.type);

    return newDnaSequence;
}

QByteArray DNASequenceUtils::complement(const QByteArray &sequence, const DNAAlphabet *alphabet) {
    if (nullptr == alphabet) {
        alphabet = U2AlphabetUtils::findBestAlphabet(sequence.data(), sequence.length());
    }
    SAFE_POINT(NULL != alphabet, L10N::nullPointerError("DNA Alphabet"), "");

    DNATranslation *translator = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(alphabet);
    SAFE_POINT(NULL != translator, L10N::nullPointerError("DNA Translator"), "");

    QByteArray result(sequence.length(), 0);
    translator->translate(sequence.constData(), sequence.length(), result.data(), result.length());
    return result;
}

DNASequence DNASequenceUtils::complement(const DNASequence& dnaSequence) {
    DNASequence newDnaSequence(dnaSequence);
    newDnaSequence.seq = DNASequenceUtils::complement(dnaSequence.seq, dnaSequence.alphabet);

    return newDnaSequence;
}

QByteArray DNASequenceUtils::reverseComplement(const QByteArray &sequence, const DNAAlphabet *alphabet) {
    return reverse(complement(sequence, alphabet));
}

DNASequence DNASequenceUtils::reverseComplement(const DNASequence& dnaSequence) {
    return reverse(complement(dnaSequence));
}

void DNASequenceUtils::crop(DNASequence &sequence, int startPos, int length) {
    sequence.quality.qualCodes = sequence.quality.qualCodes.mid(startPos, length);
    sequence.seq = sequence.seq.mid(startPos, length);
}

U2Region DNASequenceUtils::trimByQuality(DNASequence &sequence, int qualityThreshold, int minSequenceLength, bool trimBothEnds) {
    const int sequenceLength = sequence.length();
    CHECK_EXT(sequenceLength <= sequence.quality.qualCodes.length(), crop(sequence, 0, 0), U2Region());

    int endPosition = sequenceLength - 1;
    for (; endPosition >= 0; endPosition--) {
        if (sequence.quality.getValue(endPosition) >= qualityThreshold) {
            break;
        }
    }

    int beginPosition = 0;
    if (trimBothEnds) {
        for (; beginPosition <= endPosition; beginPosition++) {
            if (sequence.quality.getValue(beginPosition) >= qualityThreshold) {
                break;
            }
        }
    }

    const bool isRegionAcceptable = (endPosition >= beginPosition && endPosition - beginPosition + 1 >= minSequenceLength);
    CHECK_EXT(isRegionAcceptable, crop(sequence, 0, 0), U2Region());

    crop(sequence, beginPosition, endPosition - beginPosition + 1);
    return U2Region(beginPosition, endPosition - beginPosition + 1);
}

} // namespace
