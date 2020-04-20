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
#include <U2Core/TextUtils.h>

#include "SMatrix.h"

namespace U2 {

#define DEFAULT_FILL_VALUE -1000000.0f

SMatrix::SMatrix(const QString& _name, const DNAAlphabet* _alphabet, const QList<SScore>& rawMatrix, const QString& _description)
    : name(_name), description(_description), alphabet(_alphabet) {
    validCharacters = alphabet->getAlphabetChars();
    TextUtils::charBounds(validCharacters.constData(), validCharacters.size(), minChar, maxChar);
    charsInRow = maxChar - minChar + 1;

    scores.resize(charsInRow*charsInRow);
    qFill(scores.data(), scores.data() + scores.size(), -1000000);

    minScore = 1000000;
    maxScore = -1000000;
    foreach(const SScore& s, rawMatrix) {
        int idx = getScoreIdx(s.c1, s.c2);
        scores[idx] = s.score;
        minScore = qMin(minScore, s.score);
        maxScore = qMax(maxScore, s.score);
    }

    //try to fix amino alphabet for extended symbols if needed: U(Selenocysteine) & O(Pyrrolysine)
    if (alphabet->isAmino()) {
        if (getScore('U', 'U') == DEFAULT_FILL_VALUE) { // no score for 'U' symbol, use score value for 'C' (Cysteine)
            copyCharValues('C', 'U');
        }
        if (getScore('O', 'O') == DEFAULT_FILL_VALUE) {// no score for 'U' symbol, use score value for 'K' (Lysine)
            copyCharValues('K', 'O');
        }
    }

    //now for all symbols in alphabet without score set the minimal score
    foreach(char c1, validCharacters) {
        foreach(char c2, validCharacters) {
            float score = getScore(c1, c2);
            if (score == DEFAULT_FILL_VALUE) {
                setScore(c1, c2, minScore);
            }
        }
    }
}

void SMatrix::copyCharValues(char srcChar, char dstChar) {
    for (int i = 0; i < validCharacters.size(); i++) {
        char c = validCharacters.at(i);
        float scoreSrc1 = getScore(srcChar, c);
        setScore(dstChar, c, scoreSrc1);
        float scoreSrc2 = getScore(c, srcChar);
        setScore(c, dstChar, scoreSrc2);
    }
}

QVariant SMatrix::toQVariant() const {
    QList<QVariant> res;
    res.append(name);
    res.append(description);
    res.append(alphabet->getId());
    res.append(QChar(minChar));
    res.append(QChar(maxChar));
    res.append(charsInRow);
    res.append(validCharacters);
    res.append(double(minScore));
    res.append(double(maxScore));

    res.append(scores.size());
    for (int i = 0; i < scores.size(); i++) {
        res.append(double(scores[i]));
    }

    return res;

}

SMatrix SMatrix::fromQVariant(const QVariant& v) {
    QList<QVariant> list = v.toList();
    SMatrix m;

    int n = 0;
    m.name = list.at(n++).toString();
    m.description = list.at(n++).toString();
    QString alphabetId = list.at(n++).toString();
    m.alphabet = AppContext::getDNAAlphabetRegistry()->findById(alphabetId);
    m.minChar = list.at(n++).toChar().toLatin1();
    m.maxChar = list.at(n++).toChar().toLatin1();
    m.charsInRow = list.at(n++).toInt();
    m.validCharacters = list.at(n++).toByteArray();
    m.minScore = (float)list.at(n++).toDouble();
    m.maxScore = (float)list.at(n++).toDouble();

    int nScores = list.at(n++).toInt();
    m.scores.resize(nScores);
    for (int i = 0; i < nScores; i++) {
        m.scores[i] = float(list.at(n++).toDouble());
    }

    if (m.name.isEmpty() || m.alphabet == NULL || m.validCharacters.isEmpty()
        || !m.validCharacters.contains(m.minChar) || !m.validCharacters.contains(m.maxChar)
        || m.maxChar - m.minChar + 1 != m.charsInRow) {
        coreLog.error("Error during substitution matrix deserialization!");
        return SMatrix();
    }
    return m;
}

} // namespace
