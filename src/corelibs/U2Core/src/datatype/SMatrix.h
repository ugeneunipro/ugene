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

#ifndef _U2_SUBST_MATRIX_H
#define _U2_SUBST_MATRIX_H

#include <U2Core/global.h>
#include <U2Core/DNAAlphabet.h>

#include <QVarLengthArray>

namespace U2 {

class U2CORE_EXPORT SScore {
public:
    char c1;
    char c2;
    float score;
    SScore(char _c1, char _c2, float _score) : c1(_c1), c2(_c2), score(_score){}
};

// Substitution, Scoring or Weight matrix model.
// Example: Blosum70, PAM200, VTML200
class U2CORE_EXPORT SMatrix {
public:
    SMatrix(const QString& name, const DNAAlphabet* alphabet,
                const QList<SScore>& rawMatrix,
                const QString& description = QString());

    //constructs empty anonymous matrix
    SMatrix() { alphabet = NULL, minChar = 0; maxChar = 0; charsInRow = 0;}

    bool isEmpty() const {return scores.size() == 0;}

    float getScore(char c1, char c2) const;

    void setScore(char c1, char c2, float score);

    const QString& getName()  const {return name;}

    const QString& getDescription()  const {return description;}

    const DNAAlphabet* getAlphabet() const {return alphabet;}

    float getMinScore() const {return minScore;}

    float getMaxScore() const {return maxScore;}

    //TODO: make this class serializable
    QVariant toQVariant() const;

    static SMatrix fromQVariant(const QVariant& v);

private:
    int getScoreIdx(char c1, char c2) const;
    void copyCharValues(char src, char dst);

    QString                 name;
    QString                 description;

    const DNAAlphabet*            alphabet;
    QVarLengthArray<float>  scores;          //TODO: make scores integer ?
    char                    minChar;   // used for optimization of scores size. Minimal character in the alphabet.
    char                    maxChar;   // used for optimization of scores size. Maximum character in the alphabet.
    int                     charsInRow;
    float                   minScore;
    float                   maxScore;
    QByteArray              validCharacters; // used only for debugging now. Use array but not Set since number of characters is low
};


inline float SMatrix::getScore(char c1, char c2) const {
    int idx = getScoreIdx(c1, c2);
    return scores[idx];
}

inline void SMatrix::setScore(char c1, char c2, float score) {
    int idx = getScoreIdx(c1, c2);
    scores[idx] = score;
}

inline int SMatrix::getScoreIdx(char c1, char c2) const {
    assert(validCharacters.contains(c1));
    assert(validCharacters.contains(c2));
    int d1 = c1 - minChar;
    int d2 = c2 - minChar;
    assert(d1 < charsInRow && d2 < charsInRow && d1 >= 0 && d2 >= 0);
    return d1 * charsInRow + d2;
}

} // namespace

#endif
