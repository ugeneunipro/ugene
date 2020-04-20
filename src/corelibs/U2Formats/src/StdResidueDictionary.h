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

#ifndef _U2_STD_RESIDUE_DICTIONARY_H_
#define _U2_STD_RESIDUE_DICTIONARY_H_

#include <QByteArray>
#include <QHash>
#include <QMutex>

namespace U2 {

class AsnNode;

enum ResidueType {
    DEOXYRIBONUCLEOTIDE, RIBONUCLEOTIDE, AMINO_ACID, OTHER = 255
};

struct StdBond {
    int atom1Id;
    int atom2Id;
};

struct StdAtom {
    QByteArray name;
    int atomicNum;
};

struct StdResidue {
    QByteArray name;
    ResidueType type;
    char code;
    QHash<int, StdAtom> atoms;
    QList<StdBond> bonds;
};

class StdResidueDictionary {
    QHash<int, StdResidue> residues;
    bool load(const QString& fileName);
    void buildDictionaryFromAsnTree(AsnNode* rootElem);
    bool validate() const;
    static QMutex standardDictionaryLock;
    static QScopedPointer<StdResidueDictionary> standardDictionary;
public:
    ~StdResidueDictionary();
    static StdResidueDictionary* createStandardDictionary();
    static StdResidueDictionary* createFromAsnTree(AsnNode* rootElem);
    //TODO : move this method to AppContext
    static const StdResidueDictionary* getStandardDictionary();
    static ResidueType getResidueTypeByName(const QByteArray& name);
    const StdResidue getResidueById(int id) const;
};

} // namespace

#endif // _U2_STD_RESIDUE_DICTIONARY_H_
