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

#ifndef _U2_DNA_TRANSLATION_IMPL_H_
#define _U2_DNA_TRANSLATION_IMPL_H_

#include <QMap>
#include <QVarLengthArray>

#include <U2Core/DNATranslation.h>

namespace U2 {

class DNATranslation1to1Impl : public DNATranslation {
public:
    DNATranslation1to1Impl(const QString& id, const QString& _name, const DNAAlphabet* src, const DNAAlphabet* dst, QByteArray mapper121);

    virtual qint64 translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const;
    virtual int translate(char* src_and_dst, int len) const;

    virtual bool isOne2One() const {return true;}

    virtual QByteArray getOne2OneMapper() const { return map;}

private:
    QByteArray map;

};

struct U2CORE_EXPORT Triplet {
public:
    Triplet() { c[0] = c[1] = c[2] = '\0'; }
    Triplet(const char* s) { c[0] = s[0]; c[1] = s[1]; c[2] = s[2]; }
    Triplet(char c0, char c1, char c2) {c[0] = c0; c[1] = c1; c[2] = c2;}

    char c[3];
};

//Triplet extended with probability value, used for back translation
struct U2CORE_EXPORT TripletP {
public:
    TripletP() { c[0] = c[1] = c[2] = '\0'; p = 0; }
    TripletP(const char* s) { c[0] = s[0]; c[1] = s[1]; c[2] = s[2]; p = 0;}
    TripletP(char c0, char c1, char c2) {c[0] = c0; c[1] = c1; c[2] = c2; p = 0;}

    TripletP(const char* s, int p0) { c[0] = s[0]; c[1] = s[1]; c[2] = s[2]; p = p0;}
    TripletP(char c0, char c1, char c2, int p0) {c[0] = c0; c[1] = c1; c[2] = c2; p = p0;}

    bool operator <(const TripletP &other) const { return (p > other.p); }

    char c[3];
    int p; //probability value, values in [0..100]
};

class U2CORE_EXPORT Index3To1 {
public:
    Index3To1();

    void init(const QList<Triplet>& t);

    void init(const QByteArray& alphabetChars);

    int indexOf(const Triplet& t) const {
        return indexOf(t.c[0], t.c[1], t.c[2]);
    }

    int indexOf(const char* str) const {
        return indexOf(str[0], str[1], str[2]);
    }

    int    indexOf(char c1, char c2, char c3) const {
        int m1 = maskByChar[(quint8)c1];
        int m2 = maskByChar[(quint8)c2];
        int m3 = maskByChar[(quint8)c3];
        int i = (m1<<(bitsPerCharX2))+(m2<<bitsPerChar) + m3;
        assert(i >= 0 && i < indexSize);
        return i;
    }

    int getIndexSize() const {return indexSize;}

private:
    int     indexSize;
    int     bitsPerChar;
    int     bitsPerCharX2;
    char    maskByChar[256];
};



template <typename T>
class Mapping3To1 {
public:
    Mapping3To1(const Triplet& _t, const T& _res) : t(_t), res(_res){}

    Triplet   t;
    T       res;
};


template <typename T>
class IndexedMapping3To1 {
    Q_DISABLE_COPY(IndexedMapping3To1)
public:
    IndexedMapping3To1(const QList<Mapping3To1<T> >& rawMapping, const T& defaultVal) {
        //init index;
        QList<Triplet> ts;
        foreach(const Mapping3To1<T>& m, rawMapping) {
            ts.append(m.t);
        }
        index.init(ts);

        //allocate value area and fill it with default values
        int size = index.getIndexSize();
        assert(size > 0);
        resultByIndex = new T[size];
        qFill(resultByIndex, resultByIndex+size, defaultVal);

        //assign indexed values
        foreach(const Mapping3To1<T>& m, rawMapping) {
            int i = index.indexOf(m.t);
            resultByIndex[i] = m.res;
        }
    }

    IndexedMapping3To1(const QByteArray& alphabetChars, const T& defaultVal) {
        //init index
        index.init(alphabetChars);

        //allocate value area and fill it with default values
        int size = index.getIndexSize();
        assert(size > 0);
        resultByIndex = new T[size];
        qFill(resultByIndex, resultByIndex+size, defaultVal);
    }

    ~IndexedMapping3To1() {
        delete[] resultByIndex;
    }

    const T& map(char c1, char c2, char c3) const {
        int i = index.indexOf(c1, c2, c3);
        return resultByIndex[i];
    }

    const T& map(const char* str) const {
        int i = index.indexOf(str);
        return resultByIndex[i];
    }

    const T& map(const Triplet& t) const {
        int i = index.indexOf(&t.c[0]);
        return resultByIndex[i];
    }

    T& mapNC(char c1, char c2, char c3)  {
        int i = index.indexOf(c1, c2, c3);
        return resultByIndex[i];
    }

    T& mapNC(const char* str) {
        int i = index.indexOf(str);
        return resultByIndex[i];
    }

    T& mapNC(const Triplet& t)  {
        int i = index.indexOf(&t.c[0]);
        return resultByIndex[i];
    }

    T*& mapData() { return resultByIndex;}

    int getMapSize() const {return index.getIndexSize();}
private:
    Index3To1 index;
    T*        resultByIndex;
};

enum DNATranslationRole {
    DNATranslationRole_Unknown,
    DNATranslationRole_Start,
    DNATranslationRole_Start_Alternative,
    DNATranslationRole_Stop,
    DNATranslationRole_Num
};

class DNATranslation3to1Impl : public DNATranslation {
public:
    DNATranslation3to1Impl(const QString& _id, const QString& _name, const DNAAlphabet* src, const DNAAlphabet* dst,
        const QList<Mapping3To1<char> >& mapping, char defaultChar, const QMap<DNATranslationRole,QList<Triplet> >& roles);
    virtual ~DNATranslation3to1Impl();

    virtual qint64 translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const;
    virtual int translate(char* src_and_dst, int len) const;

    virtual bool isThree2One() const {return true;}

    char translate3to1(char c1, char c2, char c3) const {
        return index.map(c1, c2, c3);
    }

    QMap<DNATranslationRole,QList<Triplet> > getCodons() const {return roles;}

    bool isStartCodon(const char* s) const;
    bool isStartCodon(char c1, char c2, char c3) const;
    bool isStopCodon(const char* s) const;
    bool isStopCodon(char c1, char c2, char c3) const;
    bool isCodon(DNATranslationRole role, const char* s) const;
    bool isCodon(DNATranslationRole role, char c1, char c2, char c3) const;

private:
    IndexedMapping3To1<char> index;
    QMap<DNATranslationRole,QList<Triplet> > roles;
    char** codons;
    int* cod_lens;
};

inline bool DNATranslation3to1Impl::isCodon(DNATranslationRole role, char c1, char c2, char c3) const {
    assert(role >=0 && role < DNATranslationRole_Num);
    const int len = cod_lens[role];
    const char* arr = codons[role];
    for (int i = 0; i < len ; i+=3) {
        if (arr[i] == c1 && arr[i+1] == c2 && arr[i+2] == c3) return true;
    }
    return false;
}

inline bool DNATranslation3to1Impl::isStartCodon(const char* s) const {
    return isCodon(DNATranslationRole_Start, s[0],s[1],s[2]);
}

inline bool DNATranslation3to1Impl::isStartCodon(char c1, char c2, char c3) const {
    return isCodon(DNATranslationRole_Start, c1,c2,c3);
}

inline bool DNATranslation3to1Impl::isStopCodon(const char* s) const {
    return isCodon(DNATranslationRole_Stop, s[0],s[1],s[2]);
}

inline bool DNATranslation3to1Impl::isStopCodon(char c1, char c2, char c3) const {
    return isCodon(DNATranslationRole_Stop, c1,c2,c3);
}

inline bool DNATranslation3to1Impl::isCodon(DNATranslationRole role, const char* s) const {
    return isCodon(role, s[0], s[1], s[2]);
}

////////////////////////////////////// 1->3
struct BackTranslationRules {
public:
    int index[256];
    QVarLengthArray<TripletP, 66> map;
};

enum BackTranslationMode {
    USE_MOST_PROBABLE_CODONS,
    USE_FREQUENCE_DISTRIBUTION
};

class DNATranslation1to3Impl : public DNATranslation {
public:

    DNATranslation1to3Impl(const QString& _id, const QString& _name, const DNAAlphabet* src, const DNAAlphabet* dst,
        const BackTranslationRules& rules);
    virtual ~DNATranslation1to3Impl();

    virtual qint64 translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity, BackTranslationMode mode) const;
    virtual qint64 translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const;
    virtual int translate(char* src_and_dst, int len) const;

    virtual bool isOne2Three() const {return true;}

private:
    BackTranslationRules index;
};

} //namespace

#endif

