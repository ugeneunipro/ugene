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

#ifndef _U2_ALPHABET_UTILS_H_
#define _U2_ALPHABET_UTILS_H_

#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Timer.h>
#include <U2Core/U2Alphabet.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

class DNAAlphabetComparator {
public:
    DNAAlphabetComparator(const DNAAlphabet* _al1, const DNAAlphabet* _al2) : al1(_al1), al2(_al2) {
        assert(al1->getType() == al2->getType());
    }
    virtual ~DNAAlphabetComparator(){}

    virtual bool equals(char c1, char c2) const = 0;

    const DNAAlphabet *al1;
    const DNAAlphabet *al2;
};

//NOTE: use comparators as template params to optimize comparison with virtual call

// compares symbols exactly, 'N' does not match any symbol ('N'!='N')
class ExactDNAAlphabetComparatorN0 : public DNAAlphabetComparator {
public:
    ExactDNAAlphabetComparatorN0(const DNAAlphabet* _al1, const DNAAlphabet* _al2) : DNAAlphabetComparator(_al1, _al2){assert(al1==al2);}

    virtual bool equals(char c1, char c2) const {return c1 == c2 && c1!='N' && c2!='N';}
};

// compares symbols, 'N' does not match any symbol except 'N'
class ExactDNAAlphabetComparatorStrict : public DNAAlphabetComparator {
public:
    ExactDNAAlphabetComparatorStrict(const DNAAlphabet* _al1, const DNAAlphabet* _al2) : DNAAlphabetComparator(_al1, _al2){assert(al1==al2);}

    virtual bool equals(char c1, char c2) const {return c1 == c2;}
};

// compares symbols, 'N' matches any symbol if found in first sequence. N in  the second sequence matches 'N' only
class ExactDNAAlphabetComparatorN1M: public DNAAlphabetComparator {
public:
    ExactDNAAlphabetComparatorN1M(const DNAAlphabet* _al1, const DNAAlphabet* _al2) : DNAAlphabetComparator(_al1, _al2){assert(al1==al2);}

    virtual bool equals(char c1, char c2) const {return c1 == c2 || c1 == 'N';}
};

// compares symbols, 'N' matches any symbol
class ExactDNAAlphabetComparatorN1M_N2M : public DNAAlphabetComparator {
public:
    ExactDNAAlphabetComparatorN1M_N2M(const DNAAlphabet* _al1, const DNAAlphabet* _al2) : DNAAlphabetComparator(_al1, _al2){assert(al1==al2);}

    virtual bool equals(char c1, char c2) const {return c1 == c2 || c1 == 'N' || c2 == 'N';}
};



#define DNA_AL_EX_INDEX_SIZE ('Z'- ' ' + 1)

// compares extended alphabet symbols, 'N' matches any symbol
class U2CORE_EXPORT ExtendedDNAlphabetComparator : public DNAAlphabetComparator {
public:
    ExtendedDNAlphabetComparator(const DNAAlphabet* _al1, const DNAAlphabet* _al2);
    virtual bool equals(char c1, char c2) const;

private:
    inline void buildIndex();
    inline int  getMatchMask(char c) const;
    inline int  bit(char c) const {assert(c>='A' && c<='Z'); return c - 'A' + 1;}
    int index[DNA_AL_EX_INDEX_SIZE];
};


int  ExtendedDNAlphabetComparator::getMatchMask(char c) const {
    int i = c - ' ';
    SAFE_POINT(i >= 0 && i<DNA_AL_EX_INDEX_SIZE, QObject::tr("Symbol is not belong to alphabet"), 0);
    return index[i];
}

class U2CORE_EXPORT U2AlphabetUtils {
public:

    static bool matches(const DNAAlphabet* al, const char* seq, qint64 len);

    static bool matches(const DNAAlphabet* al, const char* seq, qint64 len, const U2Region& r);

    static char getDefaultSymbol(const U2AlphabetId& alphaId);

    static void assignAlphabet(MultipleSequenceAlignment& ma);

    static void assignAlphabet(MultipleSequenceAlignment& ma, char ignore);


    static const DNAAlphabet* getById(const U2AlphabetId& id) {return getById(id.id);}

    static const DNAAlphabet* getById(const QString& id);



    static const DNAAlphabet* findBestAlphabet(const char* seq, qint64 len);

    static const DNAAlphabet* findBestAlphabet(const QByteArray& arr) {return findBestAlphabet(arr.constData(), arr.length());}

    static const DNAAlphabet* findBestAlphabet(const char* seq, qint64 len, const QVector<U2Region>& regionsToProcess);

    static const DNAAlphabet* findBestAlphabet(const QByteArray& arr, const QVector<U2Region>& regionsToProcess) {return findBestAlphabet(arr.constData(), arr.length(), regionsToProcess);}

    static QList<const DNAAlphabet*> findAllAlphabets(const char* seq, qint64 len);

    static QList<const DNAAlphabet*> findAllAlphabets(const QByteArray& arr) {return findAllAlphabets(arr.constData(), arr.length());}

    static QList<const DNAAlphabet*> findAllAlphabets(const char* seq, qint64 len, const QVector<U2Region>& regionsToProcess);

    static QList<const DNAAlphabet*> findAllAlphabets(const QByteArray& arr, const QVector<U2Region>& regionsToProcess) {return findAllAlphabets(arr.constData(), arr.length(), regionsToProcess);}

    static const DNAAlphabet* deriveCommonAlphabet(const DNAAlphabet* al1, const DNAAlphabet* al2);

    static const DNAAlphabet* getExtendedAlphabet(const DNAAlphabet* al);
};

}//namespace

#endif
