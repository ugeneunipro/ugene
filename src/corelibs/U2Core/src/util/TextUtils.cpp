/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "TextUtils.h"

#include <assert.h>

namespace U2 {

static QBitArray getAlphas();
static QBitArray getAlphaNums();
static QBitArray getNums();
static QBitArray getWhites();
static QBitArray getLines();
static QBitArray getBinary();
static QByteArray getUpperCaseMap();
static QByteArray getLowerCaseMap();
static QByteArray getSpaceLine();
static QBitArray getLessThan();
static QBitArray getGrearThan();
static QBitArray getQualNameAllowedSymbols();

const QBitArray TextUtils::ALPHAS = getAlphas();
const QBitArray TextUtils::ALPHA_NUMS = getAlphaNums();
const QBitArray TextUtils::NUMS = getNums();
const QBitArray TextUtils::WHITES = getWhites();
const QBitArray TextUtils::LINE_BREAKS = getLines();
const QBitArray TextUtils::BINARY = getBinary();
const QByteArray TextUtils::UPPER_CASE_MAP = getUpperCaseMap();
const QByteArray TextUtils::LOWER_CASE_MAP = getLowerCaseMap();
const QByteArray TextUtils::SPACE_LINE = getSpaceLine();
const QBitArray TextUtils::LESS_THAN = getLessThan();
const QBitArray TextUtils::GREATER_THAN = getGrearThan();
const QBitArray TextUtils::QUALIFIER_NAME_CHARS = getAlphas() | getNums() | getQualNameAllowedSymbols();


//TODO: optimize shared data structs access! -> replace it with arrays with bounds checking in debug

static QBitArray getEmptyBitMap() {
    return QBitArray(256);
}

static QBitArray getQualNameAllowedSymbols(){
    QBitArray res = getEmptyBitMap();
    res['-']=res['\'']=res['_']=res['*']=true;
    return res;
}

static QBitArray getBinary() {
    QBitArray res = getEmptyBitMap();
    res.fill(true, 0, 31);
    res &= ~getWhites();
    return res;
}

static QBitArray getAlphaNums() {
    QBitArray res  = getAlphas();
    res|= getNums();
    return res;
}


static QBitArray getAlphas() {
    QBitArray res = getEmptyBitMap();
    res.fill(true, 'A', 'Z'+1);
    res.fill(true, 'a', 'z'+1);
    return res;
}

static QBitArray getNums() {
    QBitArray res = getEmptyBitMap();
    res.fill(true, '0', '9'+1);
    return res;
}

static QBitArray getWhites() {
    //'\t', '\n', '\v', '\f', '\r', and ' '
    QBitArray res = getEmptyBitMap();
    res['\t']=res['\n']=res['\v']=res['\f']=res['\r']=res[' ']=true;
    return res;
}

QBitArray getLines() {
    QBitArray res = getEmptyBitMap();
    res['\r'] = res['\n'] = true;
    return res;
}

QBitArray getLessThan() {
    QBitArray res = getEmptyBitMap();
    res['<'] = true;
    return res;
}

QBitArray getGrearThan() {
    QBitArray res = getEmptyBitMap();
    res['>'] = true;
    return res;
}


QBitArray TextUtils::createBitMap(char c1) {
    QBitArray res = getEmptyBitMap();
    res[quint8(c1)] = true;
    return res;
}

QBitArray TextUtils::createBitMap(const QByteArray& chars, bool val) {
    QBitArray res = getEmptyBitMap();
    for (int i = 0, n = chars.size(); i < n ; i++) {
        char c = chars[i];
        res[quint8(c)] = val;
    }
    return res;
}

QByteArray TextUtils::createMap(const QBitArray& bits, char defaultChar) {
    assert(bits.size() == 256);
    QByteArray res(256, 0);
    for(int i=0;i<256; i++) {
        res[i] = bits[i] ? (char)i : defaultChar;
    }
    return res;
}

static QByteArray getUpperCaseMap() {
    QByteArray b(256, 0);
    for(int i=0;i<b.size();i++) {
        char c = (char)i;
        if (c >= 'a' && c<='z') {
            c-='a'-'A';
        }
        b[i] = c;
    }
    return b;
}

static QByteArray getLowerCaseMap() {
    QByteArray b(256, 0);
    for(int i=0;i<b.size();i++) {
        char c = (char)i;
        if (c >= 'A' && c<='Z') {
            c+='a'-'A';
        }
        b[i] = c;
    }
    return b;
}

static QByteArray getSpaceLine() {
    QByteArray res(4096, ' ');
    res[4096]='\0';
    return res;
}

QString TextUtils::variate(const QString& prefix, const QString& sep, const QSet<QString>& filter, bool mustHaveSuffix, int startSeed) {
    int seed = startSeed;
    QString res = prefix;
    if (!mustHaveSuffix && !filter.contains(res) && !res.isEmpty()) {
        return res;
    }
    do {
        res = prefix + sep + QString::number(seed++);
    } while (filter.contains(res));
    return res;
}

QByteArray TextUtils::cutByteOrderMarks(const QByteArray& data) {
    QTextStream textStream(data);
    textStream.setGenerateByteOrderMark(false);
    QByteArray resultData = textStream.readAll().toLocal8Bit();
    return resultData;
}

qint64 TextUtils::cutByteOrderMarks(char* data, qint64 buffLen) {
    CHECK(buffLen != 0, 0);

    QByteArray byteArrayData = buffLen != -1 ? QByteArray(data, buffLen) : QByteArray(data);
    QByteArray resByteArrayData = cutByteOrderMarks(byteArrayData);
    qint64 result = resByteArrayData.size();
    memcpy(data, resByteArrayData.data(), result);

    return result;
}

}//namespace
