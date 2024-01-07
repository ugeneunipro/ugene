/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
const QBitArray TextUtils::QUALIFIER_NAME_CHARS = getAlphas() | getNums() | getQualNameAllowedSymbols();

// TODO: optimize shared data structs access! -> replace it with arrays with bounds checking in debug

static QBitArray getEmptyBitMap() {
    return QBitArray(256);
}

static QBitArray getQualNameAllowedSymbols() {
    QBitArray res = getEmptyBitMap();
    res['-'] = res['\''] = res['_'] = res['*'] = res['%'] = true;
    return res;
}

static QBitArray getBinary() {
    QBitArray res = getEmptyBitMap();
    res.fill(true, 0, 31);
    res &= ~getWhites();
    return res;
}

static QBitArray getAlphaNums() {
    QBitArray res = getAlphas();
    res |= getNums();
    return res;
}

static QBitArray getAlphas() {
    QBitArray res = getEmptyBitMap();
    res.fill(true, 'A', 'Z' + 1);
    res.fill(true, 'a', 'z' + 1);
    return res;
}

static QBitArray getNums() {
    QBitArray res = getEmptyBitMap();
    res.fill(true, '0', '9' + 1);
    return res;
}

static QBitArray getWhites() {
    //'\t', '\n', '\v', '\f', '\r', and ' '
    QBitArray res = getEmptyBitMap();
    res['\t'] = res['\n'] = res['\v'] = res['\f'] = res['\r'] = res[' '] = true;
    return res;
}

QBitArray getLines() {
    QBitArray res = getEmptyBitMap();
    res['\r'] = res['\n'] = true;
    return res;
}

QBitArray TextUtils::createBitMap(char c1) {
    QBitArray res = getEmptyBitMap();
    res[quint8(c1)] = true;
    return res;
}

QBitArray TextUtils::createBitMap(const QByteArray& chars, bool val) {
    QBitArray res = getEmptyBitMap();
    for (int i = 0, n = chars.size(); i < n; i++) {
        char c = chars[i];
        res[quint8(c)] = val;
    }
    return res;
}

QByteArray TextUtils::createMap(const QBitArray& bits, char defaultChar) {
    assert(bits.size() == 256);
    QByteArray res(256, 0);
    for (int i = 0; i < 256; i++) {
        res[i] = bits[i] ? (char)i : defaultChar;
    }
    return res;
}

static QByteArray getUpperCaseMap() {
    QByteArray b(256, 0);
    for (int i = 0; i < b.size(); i++) {
        char c = (char)i;
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        b[i] = c;
    }
    return b;
}

static QByteArray getLowerCaseMap() {
    QByteArray b(256, 0);
    for (int i = 0; i < b.size(); i++) {
        char c = (char)i;
        if (c >= 'A' && c <= 'Z') {
            c += 'a' - 'A';
        }
        b[i] = c;
    }
    return b;
}

/**
 * Returns array of spaces with a length of 4K+1 ending with 0 (the last byte).
 * The constData() method from the returned array can be safely used as a 0-terminated 1-byte string of length 4k.
 */
static QByteArray getSpaceLine() {
    int spaceCount = 4096;
    QByteArray res(spaceCount + 1, ' ');
    res[spaceCount] = '\0';
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

QByteArray TextUtils::cutByteOrderMarks(const QByteArray& data, QString& errorMessage) {
    QTextStream textStream(data);
    textStream.setGenerateByteOrderMark(false);
    QByteArray resultData = textStream.readAll().toLocal8Bit();
    if (resultData.size() > data.size()) {
        errorMessage = tr("The text file can't be read. Check the file encoding and make sure the file is not corrupted.");
        resultData = QByteArray();
    }
    return resultData;
}

qint64 TextUtils::cutByteOrderMarks(char* data, QString& errorMessage, qint64 buffLen) {
    CHECK(buffLen != 0, 0);

    QByteArray byteArrayData = buffLen != -1 ? QByteArray(data, buffLen) : QByteArray(data);
    QByteArray resByteArrayData = cutByteOrderMarks(byteArrayData, errorMessage);
    CHECK(errorMessage.isEmpty(), -1);

    qint64 result = resByteArrayData.size();
    memcpy(data, resByteArrayData.data(), result);

    return result;
}

QStringList TextUtils::split(const QString& text, int chunkSize) {
    if (text.length() < chunkSize) {
        return {text};
    }
    QStringList result;
    for (int i = 0; i < text.length(); i += chunkSize) {
        result << text.mid(i, qMin(i + chunkSize, text.length()) - i);
    }
    return result;
}

QList<QByteArray> TextUtils::split(const QByteArray& text, int chunkSize) {
    if (text.length() < chunkSize) {
        return {text};
    }
    QList<QByteArray> result;
    for (int i = 0; i < text.length(); i += chunkSize) {
        result << text.mid(i, qMin(i + chunkSize, text.length()) - i);
    }
    return result;
}

void TextUtils::replace(QString& text, const QBitArray& latin1CharCodeMap, QChar replacementChar) {
    for (int i = 0; i < text.length(); i++) {
        uchar latin1Char = text.at(i).toLatin1();
        if (latin1CharCodeMap.at(latin1Char)) {
            text[i] = replacementChar;
        }
    }
}

QString TextUtils::readFirstLine(const QString& text) {
    QString textCopy = text;
    QTextStream stream(&textCopy);
    return stream.readLine();
}

bool TextUtils::isLineBreak(const QString& text, int charIndex) {
    uchar bitIndex = uchar((text.at(charIndex).toLatin1()));
    return LINE_BREAKS.testBit(bitIndex);
}

bool TextUtils::isWhiteSpace(const QString& text, int charIndex) {
    uchar bitIndex = uchar((text.at(charIndex).toLatin1()));
    return WHITES.testBit(bitIndex);
}

bool TextUtils::isWhiteSpace(const QString& text) {
    for (int i = 0; i < text.length(); i++) {
        if (!isWhiteSpace(text, i)) {
            return false;
        }
    }
    return true;
}

int TextUtils::findIndexOfFirstWhiteSpace(const QString& text) {
    for (int i = 0; i < text.length(); i++) {
        if (isWhiteSpace(text, i)) {
            return i;
        }
    }
    return -1;
}

QString TextUtils::skip(const QBitArray& map, const QString& text) {
    for (int i = 0, n = text.length(); i < n; i++) {
        uchar c = uchar(text[i].toLatin1());
        if (!map.testBit(c)) {
            return i == 0 ? text : text.right(n - i);
        }
    }
    return "";
}

wchar_t* TextUtils::toWideCharsArray(const QString& text) {
    auto wideCharsText = new wchar_t[text.length() + 1];
    int unicodeFileNameLength = text.toWCharArray(wideCharsText);
    wideCharsText[unicodeFileNameLength] = 0;
    return wideCharsText;
}


}  // namespace U2
