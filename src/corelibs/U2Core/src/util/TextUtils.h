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

#ifndef _U2_TEXT_UTILS_H_
#define _U2_TEXT_UTILS_H_

#include <U2Core/global.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <assert.h>

#include <QBitArray>
#include <QCoreApplication>
#include <QSet>
#include <QStringList>
#include <QTextStream>
#include <QVector>
#include <QWidget>

namespace U2 {

class U2CORE_EXPORT TextUtils {
    Q_DECLARE_TR_FUNCTIONS(TextUtils)
public:
    static const QBitArray ALPHAS;
    static const QBitArray ALPHA_NUMS;
    static const QBitArray NUMS;
    static const QBitArray WHITES;
    static const QBitArray LINE_BREAKS;
    static const QBitArray BINARY;
    static const QBitArray LESS_THAN;
    static const QBitArray GREATER_THAN;

    static const QByteArray UPPER_CASE_MAP;
    static const QByteArray LOWER_CASE_MAP;

    static const QByteArray SPACE_LINE;

    static const QBitArray QUALIFIER_NAME_CHARS;

    inline static const char* getLineOfSpaces(int nspaces);

    inline static bool fits(const QBitArray& map, const char* str, int len);

    inline static bool contains(const QBitArray& map, const char* str, int len);

    static QBitArray createBitMap(char c1);

    static QBitArray createBitMap(const QByteArray& chars, bool val = true);

    static QByteArray createMap(const QBitArray& bits, char defaultChar);

    inline static int skip(const QBitArray& map, const char* seq, int len);

    inline static int keep(const char* srcSeq, int len, char* dstSeq, const QBitArray& keepMap);

    inline static int keep(char* srcSeq, int len, const QBitArray& keepMap) {return keep(srcSeq, len, srcSeq, keepMap);}

    inline static int remove(const char* srcSeq, int len, char* dstSeq, const QBitArray& removeMap);

    inline static int remove(char* srcSeq, int len, const QBitArray& removeMap) {return remove(srcSeq, len, srcSeq, removeMap);}

    inline static void replace(char* seq, int len, const QBitArray& fromMap, char to);

    inline static void applyMap(const QBitArray& map, const char* srcSeq, int len, char* dstSeq);

    inline static void applyMap(const QBitArray& map, char* seq, int len) { applyMap(map, seq, len, seq);}

    inline static void translate(const QByteArray& map, char* seq, int len) { translate(map, seq, len, seq);}

    inline static void translate(const QByteArray& map, const char* src, qint64 len, char* dst);

    inline static void reverse(const char* srcSeq, char* dstSeq, int len);

    inline static void reverse(char* seq, int len);

    inline static bool equals(const char* str1, const char* str2, int n);

    inline static bool isQuoted(const char* str, int len, int qChar);

    static QString variate(const QString& prefix, const QString& sep, const QSet<QString>& filter, bool mustHaveSuffix = false, int startSeed = 0);

    inline static void charBounds(const char* data, int dataSize, char& minChar, char& maxChar);

    static QByteArray cutByteOrderMarks(const QByteArray& data, QString& errorMessage);

    static qint64 cutByteOrderMarks(char* data, QString& errorMessage, qint64 buffLen = -1);

    //todo: move this method to another class
    inline static QByteArray selectIdx256(const QBitArray& map, bool sign);

    // Returns first index of the character not equal to 'c'. Returns -1 if not found
    inline static int firstIndexOfNotEqualChar(const char* seq, int size, char c);

    // Returns last index of the character not equal to 'c'. Returns -1 if not found
    inline static int lastIndexOfNotEqualChar(const char* seq, int size, char c);

    // Wraps input string for valid output to CSV following RFC 4180
    inline static void wrapForCSV(QString& str);

    inline static QStringList transposeCSVRows(const QStringList& rows, const QString& delimiter="\t");
};

template <typename T>
inline void reverseVector(QVector<T>&);

inline const char* TextUtils::getLineOfSpaces(int nspaces) {
    assert(nspaces >=0 && nspaces <= SPACE_LINE.size());
    return SPACE_LINE.data() + SPACE_LINE.size() - 1 - nspaces;
}

inline bool TextUtils::fits(const QBitArray& map, const char* str, int len) {
    for(int i=0; i<len; i++) {
        unsigned char c = str[i];
        if (!map[c]) {
            return false;
        }
    }
    return true;
}


inline bool TextUtils::contains(const QBitArray& map, const char* str, int len) {
    for(int i=0; i<len; i++) {
        unsigned char c = str[i];
        if (map[c]) {
            return true;
        }
    }
    return false;
}

inline int TextUtils::skip(const QBitArray& map, const char* seq, int len) {
    int i = 0;
    for (; i < len; i++) {
        unsigned char c = seq[i];
        if (!map[c]) {
            break;
        }
    }
    return i;
}

inline int TextUtils::keep(const char* srcSeq, int srcLen, char* dstSeq, const QBitArray& keepMap) {
    int dstLen = 0;
    for (int i=0;i<srcLen;i++) {
        unsigned char c = srcSeq[i];
        if (keepMap[c]) {
            dstSeq[dstLen] = c;
            dstLen++;
        }
    }
    return dstLen;
}

inline int TextUtils::remove(const char* srcSeq, int srcLen, char* dstSeq, const QBitArray& removeMap) {
    int dstLen = 0;
    for (int i=0;i<srcLen;i++) {
        unsigned char c = srcSeq[i];
        if (!removeMap[c]) {
            dstSeq[dstLen] = c;
            dstLen++;
        }
    }
    return dstLen;
}

inline void TextUtils::replace(char* seq, int len, const QBitArray& fromMap, char to) {
    for (int i=0;i<len;i++) {
        unsigned char c = seq[i];
        if (fromMap[c]) {
            seq[i] = to;
        }
    }
}

inline void TextUtils::applyMap(const QBitArray& map, const char* srcSeq, int len, char* dstSeq) {
    for (int i=0;i<len;i++) {
        uchar c = srcSeq[i];
        dstSeq[i]=map[c];
    }
}

inline void TextUtils::translate(const QByteArray& map, const char* src, qint64 len, char* dst) {
    const char* src_pos = src;
    const char* mapData = map.constData();
    for (char *dst_pos = dst, *end_pos = dst + len; dst_pos < end_pos; ++dst_pos,++src_pos) {
        uchar idx = (uchar)*src_pos;
        *dst_pos = mapData[idx];
    }
}

inline void TextUtils::reverse(const char* srcSeq, char* dstSeq, int len) {
    if (srcSeq == dstSeq) {
        reverse(dstSeq, len);
        return;
    }
    assert(qAbs(srcSeq-dstSeq) > len);

    for (int i = 0, j = len-1; i < len; i++, j--) {
        char c = srcSeq[i];
        dstSeq[j] = c;
    }
}


inline void TextUtils::reverse(char* seq, int len) {
    for (int i = 0, j = len-1; i < j; i++, j--) {
        char c = seq[j];
        seq[j] = seq[i];
        seq[i] = c;
    }
}

template <typename T>
inline void reverseVector(QVector<T>& seq) {
    const int len = seq.size();
    for (int i = 0, j = len-1; i < j; i++, j--) {
        T t = seq[j];
        seq[j] = seq[i];
        seq[i] = t;
    }
}


inline bool TextUtils::equals(const char* str1, const char* str2, int n) {
    assert(n>0);
    if (str1[0] != str2[0]) {
        return false;
    }
    for(int i=1; i<n; i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}


inline bool TextUtils::isQuoted(const char* str, int len, int qChar) {
    assert(len>=0);
    if (len < 2 || str[0]!=qChar || str[len-1]!=qChar) {
        return false;
    }
    int nquotes = 2;
    for(int i=1, n=len-1; i<n; i++) {
        if (str[i]==qChar) {
            nquotes++;
        }
    }
    return (nquotes & 0x1) == 0; //all opened quotes are closed if odd number
}


inline QByteArray TextUtils::selectIdx256(const QBitArray& map, bool sign) {
    int n = map.size();
    assert(n <= 256);
    QByteArray res;
    for (int i=0; i < n; i++) {
        if (map[i] == sign) {
            res.append((char)i);
        }
    }
    return res;
}


inline void TextUtils::charBounds(const char* data, int dataSize, char& minChar, char& maxChar) {
    assert(dataSize > 0);
    maxChar = minChar = data[0];
    for (int i=1; i < dataSize; i++) {
        char c = data[i];
        minChar = qMin(minChar, c);
        maxChar = qMax(maxChar, c);
    }
}

// returns first index of the character not equal to 'c'
inline int TextUtils::firstIndexOfNotEqualChar(const char* seq, int size, char c) {
    for (int i = 0; i < size; i++) {
        char ci = seq[i];
        if (ci != c) {
            return i;
        }
    }
    return -1;
}

// returns last index of the character not equal to 'c'
inline int TextUtils::lastIndexOfNotEqualChar(const char* seq, int size, char c) {
    for (int i = size - 1; i >= 0; i--) {
        char ci = seq[i];
        if (ci != c) {
            return i;
        }
    }
    return -1;
}

// Wraps input string for valid output to CSV following RFC 4180
inline void TextUtils::wrapForCSV(QString& str) {
    if (!str.contains(",")) {
        return;
    }
    str.replace("\"", "\"\"");
    str.prepend("\"");
    str.append("\"");
}

inline QStringList TextUtils::transposeCSVRows(const QStringList& rows, const QString& delimiter){
    QStringList transposedRows;
    if(rows.length() == 0){
        return transposedRows;
    }
    int columnNumber = rows.at(0).split(delimiter).length();
    for (int i = 0; i < columnNumber; i++){
        transposedRows.append("");
    }

    foreach (const QString& originalRow, rows){
        QStringList originalColumns = originalRow.split(delimiter);
        for (int i = 0; i < originalColumns.length() && i < columnNumber; i++){
            transposedRows[i].append(originalColumns.at(i));
            transposedRows[i].append(delimiter);
        }
    }

    for (int i = 0; i < columnNumber; i++){
        transposedRows[i].remove(transposedRows[i].length()-1,1);
    }
    return transposedRows;
}

} //namespace

#endif
