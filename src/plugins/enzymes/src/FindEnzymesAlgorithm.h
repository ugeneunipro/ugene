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

#pragma once

#include <QList>
#include <QObject>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Task.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Type.h>

namespace U2 {

class FindEnzymesAlgListener {
public:
    ~FindEnzymesAlgListener() {
    }
    virtual void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand) = 0;
};

template<typename CompareFN>
class FindEnzymesAlgorithm {
public:
    void run(const DNASequence& sequence, const U2Region& region, const SEnzymeData& enzyme, FindEnzymesAlgListener* resultListener, TaskStateInfo& stateInfo, int resultPosShift = 0) {
        SAFE_POINT(enzyme->alphabet != nullptr, "No enzyme alphabet", );

        // look for results in direct strand
        run(sequence, region, enzyme, enzyme->seq.constData(), U2Strand::Direct, resultListener, stateInfo, resultPosShift);
        CHECK_OP(stateInfo, );

        // if enzyme is not symmetric - look in complementary strand too
        DNATranslation* tt = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(enzyme->alphabet);
        if (tt == nullptr) {
            return;
        }
        QByteArray revCompl = enzyme->seq;
        tt->translate(revCompl.data(), revCompl.size());
        TextUtils::reverse(revCompl.data(), revCompl.size());
        if (revCompl == enzyme->seq) {
            return;
        }
        run(sequence, region, enzyme, revCompl.constData(), U2Strand::Complementary, resultListener, stateInfo, resultPosShift);
    }

    void run(const DNASequence& sequence, const U2Region& region, const SEnzymeData& enzyme, const char* pattern, U2Strand stand, FindEnzymesAlgListener* resultListener, TaskStateInfo& ti, int resultPosShift = 0) {
        CompareFN fn(sequence.alphabet, enzyme->alphabet);
        const char* seq = sequence.constData();
        char unknownChar = sequence.alphabet->getDefaultSymbol();
        int plen = enzyme->seq.length();
        for (int pos = region.startPos, endPos = region.endPos() - plen + 1; pos < endPos; pos++) {
            bool match = matchSite(seq + pos, pattern, plen, unknownChar, fn);
            if (match) {
                resultListener->onResult(resultPosShift + pos, enzyme, stand);
            }
            CHECK_OP(ti, );
        }
        if (sequence.circular) {
            if (region.startPos + region.length == sequence.length()) {
                QByteArray buf;
                const QByteArray& dnaseq = sequence.seq;
                int size = enzyme->seq.size() - 1;
                int startPos = dnaseq.length() - size;
                buf.append(dnaseq.mid(startPos));
                buf.append(dnaseq.mid(0, size));
                for (int s = 0; s < size; s++) {
                    bool match = matchSite(buf.constData() + s, pattern, plen, unknownChar, fn);
                    if (match) {
                        resultListener->onResult(resultPosShift + s + startPos, enzyme, stand);
                    }
                    CHECK_OP(ti, );
                }
            }
        }
    }

    bool matchSite(const char* seq, const char* pattern, int plen, char unknownChar, const CompareFN& fn) const {
        bool match = true;
        for (int p = 0; p < plen && match; p++) {
            char c1 = seq[p];
            char c2 = pattern[p];
            bool seqUnknown = c1 == unknownChar;
            bool patternUnknown = c2 == unknownChar;
            if (seqUnknown && patternUnknown) {
                match = true;
            } else if (seqUnknown && !patternUnknown) {
                match = false;
            } else if (!seqUnknown && patternUnknown) {
                match = true;
            } else if (!seqUnknown && !patternUnknown) {
                match = fn.equals(c2, c1);
            }
        }
        return match;
    }
};

}  // namespace U2
