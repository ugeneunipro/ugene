/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "U2DbiPackUtils.h"

#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QByteArray U2DbiPackUtils::VERSION("0");
const char U2DbiPackUtils::SEP = '\t';
const char U2DbiPackUtils::SECOND_SEP = 11;

QByteArray U2DbiPackUtils::packGaps(const QVector<U2MsaGap>& gaps) {
    QByteArray result;
    foreach (const U2MsaGap& gap, gaps) {
        if (!result.isEmpty()) {
            result += ";";
        }
        result += QByteArray::number(gap.startPos);
        result += ",";
        result += QByteArray::number(gap.length);
    }
    return "\"" + result + "\"";
}

bool U2DbiPackUtils::unpackGaps(const QByteArray& str, QVector<U2MsaGap>& gaps) {
    CHECK(str.startsWith('\"') && str.endsWith('\"'), false);
    QByteArray gapsStr = str.mid(1, str.length() - 2);
    if (gapsStr.isEmpty()) {
        return true;
    }

    QList<QByteArray> tokens = gapsStr.split(';');
    foreach (const QByteArray& t, tokens) {
        QList<QByteArray> gapTokens = t.split(',');
        CHECK(2 == gapTokens.size(), false);
        bool ok = false;
        U2MsaGap gap;
        gap.startPos = gapTokens[0].toLongLong(&ok);
        CHECK(ok, false);
        gap.length = gapTokens[1].toLongLong(&ok);
        CHECK(ok, false);
        gaps << gap;
    }
    return true;
}

QByteArray U2DbiPackUtils::packGapDetails(qint64 rowId, const QVector<U2MsaGap>& oldGaps, const QVector<U2MsaGap>& newGaps) {
    QByteArray result = VERSION;
    result += SEP;
    result += QByteArray::number(rowId);
    result += SEP;
    result += packGaps(oldGaps);
    result += SEP;
    result += packGaps(newGaps);
    return result;
}

bool U2DbiPackUtils::unpackGapDetails(const QByteArray& modDetails, qint64& rowId, QVector<U2MsaGap>& oldGaps, QVector<U2MsaGap>& newGaps) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(4 == tokens.size(), QString("Invalid gap modDetails string '%1'").arg(QString(modDetails)), false);
    {  // version
        SAFE_POINT(VERSION == tokens[0], QString("Invalid modDetails version '%1'").arg(tokens[0].data()), false);
    }
    {  // rowId
        bool ok = false;
        rowId = tokens[1].toLongLong(&ok);
        SAFE_POINT(ok, QString("Invalid gap modDetails rowId '%1'").arg(tokens[1].data()), false);
    }
    {  // oldGaps
        bool ok = unpackGaps(tokens[2], oldGaps);
        SAFE_POINT(ok, QString("Invalid gap string '%1'").arg(tokens[2].data()), false);
    }
    {  // newGaps
        bool ok = unpackGaps(tokens[3], newGaps);
        SAFE_POINT(ok, QString("Invalid gap string '%1'").arg(tokens[3].data()), false);
    }
    return true;
}

QByteArray U2DbiPackUtils::packGapDetails(qint64 rowId, const U2DataId& relatedObjectId, const QVector<U2MsaGap>& oldGaps, const QVector<U2MsaGap>& newGaps) {
    QByteArray result = VERSION;
    result += SEP;
    result += QByteArray::number(rowId);
    result += SEP;
    result += relatedObjectId.toHex();
    result += SEP;
    result += packGaps(oldGaps);
    result += SEP;
    result += packGaps(newGaps);
    return result;
}

bool U2DbiPackUtils::unpackGapDetails(const QByteArray& modDetails, qint64& rowId, U2DataId& relatedObjectId, QVector<U2MsaGap>& oldGaps, QVector<U2MsaGap>& newGaps) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(5 == tokens.size(), QString("Invalid gap modDetails string '%1'").arg(QString(modDetails)), false);
    {  // version
        SAFE_POINT(VERSION == tokens[0], QString("Invalid modDetails version '%1'").arg(tokens[0].data()), false);
    }
    {  // rowId
        bool ok = false;
        rowId = tokens[1].toLongLong(&ok);
        SAFE_POINT(ok, QString("Invalid gap modDetails rowId '%1'").arg(tokens[1].data()), false);
    }
    {  // relatedObjectId
        relatedObjectId = QByteArray::fromHex(tokens[2]);
    }
    {  // oldGaps
        bool ok = unpackGaps(tokens[3], oldGaps);
        SAFE_POINT(ok, QString("Invalid gap string '%1'").arg(tokens[3].data()), false);
    }
    {  // newGaps
        bool ok = unpackGaps(tokens[4], newGaps);
        SAFE_POINT(ok, QString("Invalid gap string '%1'").arg(tokens[4].data()), false);
    }
    return true;
}

QByteArray U2DbiPackUtils::packRowOrder(const QList<qint64>& rowIds) {
    QByteArray result;
    for (qint64 rowId : qAsConst(rowIds)) {
        if (!result.isEmpty()) {
            result += ",";
        }
        result += QByteArray::number(rowId);
    }
    return "\"" + result + "\"";
}

bool U2DbiPackUtils::unpackRowOrder(const QByteArray& str, QList<qint64>& rowsIds) {
    CHECK(str.startsWith('\"') && str.endsWith('\"'), false);
    QByteArray orderStr = str.mid(1, str.length() - 2);
    if (orderStr.isEmpty()) {
        return true;
    }

    QList<QByteArray> tokens = orderStr.split(',');
    foreach (const QByteArray& t, tokens) {
        bool ok = false;
        rowsIds << t.toLongLong(&ok);
        CHECK(ok, false);
    }
    return true;
}

QByteArray U2DbiPackUtils::packRowOrderDetails(const QList<qint64>& oldOrder, const QList<qint64>& newOrder) {
    QByteArray result = VERSION;
    result += SEP;
    result += packRowOrder(oldOrder);
    result += SEP;
    result += packRowOrder(newOrder);
    return result;
}

bool U2DbiPackUtils::unpackRowOrderDetails(const QByteArray& modDetails, QList<qint64>& oldOrder, QList<qint64>& newOrder) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(3 == tokens.size(), QString("Invalid rows order modDetails string '%1'").arg(QString(modDetails)), false);
    {  // version
        SAFE_POINT(VERSION == tokens[0], QString("Invalid modDetails version '%1'").arg(tokens[0].data()), false);
    }
    {  // oldOrder
        bool ok = unpackRowOrder(tokens[1], oldOrder);
        SAFE_POINT(ok, QString("Invalid rows order string '%1'").arg(tokens[1].data()), false);
    }
    {  // newGaps
        bool ok = unpackRowOrder(tokens[2], newOrder);
        SAFE_POINT(ok, QString("Invalid rows order string '%1'").arg(tokens[2].data()), false);
    }

    return true;
}

QByteArray U2DbiPackUtils::packRow(int rowIndex, const U2MsaRow& row) {
    QByteArray result = VERSION;
    result += SEP;
    result += QByteArray::number(rowIndex);
    result += SEP;
    result += QByteArray::number(row.rowId);
    result += SEP;
    result += row.sequenceId.toHex();
    result += SEP;
    result += QByteArray::number(row.gstart);
    result += SEP;
    result += QByteArray::number(row.gend);
    result += SEP;
    result += packGaps(row.gaps);
    return result;
}

bool U2DbiPackUtils::unpackRow(const QByteArray& modDetails, int& rowIndex, U2MsaRow& row) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(tokens.size() == 7, QString("Invalid added row modDetails string '%1'").arg(QString(modDetails)), false);
    {  // version
        SAFE_POINT(tokens[0] == VERSION, QString("Invalid modDetails version '%1'").arg(tokens[0].data()), false);
    }
    {  // row index
        bool ok = false;
        rowIndex = tokens[1].toInt(&ok);
        SAFE_POINT(ok, QString("Invalid added row modDetails posInMsa '%1'").arg(tokens[1].data()), false);
    }
    {  // rowId
        bool ok = false;
        row.rowId = tokens[2].toLongLong(&ok);
        SAFE_POINT(ok, QString("Invalid added row modDetails rowId '%1'").arg(tokens[2].data()), false);
    }
    // sequenceId
    row.sequenceId = QByteArray::fromHex(tokens[3]);

    {  // gstart
        bool ok = false;
        row.gstart = tokens[4].toLongLong(&ok);
        SAFE_POINT(ok, QString("Invalid added row modDetails gstart '%1'").arg(tokens[5].data()), false);
    }
    {  // gend
        bool ok = false;
        row.gend = tokens[5].toLongLong(&ok);
        SAFE_POINT(ok, QString("Invalid added row modDetails gend '%1'").arg(tokens[6].data()), false);
    }
    {  // gaps
        bool ok = unpackGaps(tokens[6], row.gaps);
        SAFE_POINT(ok, QString("Invalid added row modDetails gaps '%1'").arg(tokens[7].data()), false);
    }
    return true;
}

QByteArray U2DbiPackUtils::packRowInfo(const U2MsaRow& row) {
    QByteArray result;
    result += QByteArray::number(row.rowId);
    result += SECOND_SEP;
    result += row.sequenceId.toHex();
    result += SECOND_SEP;
    result += QByteArray::number(row.gstart);
    result += SECOND_SEP;
    result += QByteArray::number(row.gend);
    result += SECOND_SEP;
    result += QByteArray::number(row.length);
    return result;
}

bool U2DbiPackUtils::unpackRowInfo(const QByteArray& str, U2MsaRow& row) {
    QList<QByteArray> tokens = str.split(SECOND_SEP);
    CHECK(tokens.count() == 5, false);
    bool ok = false;
    row.rowId = tokens[0].toLongLong(&ok);
    CHECK(ok, false);
    row.sequenceId = QByteArray::fromHex(tokens[1]);
    row.gstart = tokens[2].toLongLong(&ok);
    CHECK(ok, false);
    row.gend = tokens[3].toLongLong(&ok);
    CHECK(ok, false);
    row.length = tokens[4].toLongLong(&ok);
    CHECK(ok, false);

    return true;
}

QByteArray U2DbiPackUtils::packRowInfoDetails(const U2MsaRow& oldRow, const U2MsaRow& newRow) {
    QByteArray result = VERSION;
    result += SEP;
    result += packRowInfo(oldRow);
    result += SEP;
    result += packRowInfo(newRow);
    return result;
}

bool U2DbiPackUtils::unpackRowInfoDetails(const QByteArray& modDetails, U2MsaRow& oldRow, U2MsaRow& newRow) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(tokens.count() == 3, QString("Invalid modDetails '%1'!").arg(QString(modDetails)), false);
    SAFE_POINT(VERSION == tokens[0], QString("Invalid modDetails version '%1'").arg(QString(tokens[0])), false);
    bool ok;
    ok = unpackRowInfo(tokens[1], oldRow);
    CHECK(ok, false);
    ok = unpackRowInfo(tokens[2], newRow);
    CHECK(ok, false);

    return true;
}

QByteArray U2DbiPackUtils::packAlphabetDetails(const U2AlphabetId& oldAlphabet, const U2AlphabetId& newAlphabet) {
    QByteArray result = VERSION;
    result += SEP;
    result += oldAlphabet.id.toLatin1();
    result += SEP;
    result += newAlphabet.id.toLatin1();
    return result;
}

bool U2DbiPackUtils::unpackAlphabetDetails(const QByteArray& modDetails, U2AlphabetId& oldAlphabet, U2AlphabetId& newAlphabet) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(3 == tokens.count(), QString("Invalid modDetails '%1'!").arg(QString(modDetails)), false);
    SAFE_POINT(VERSION == tokens[0], QString("Invalid modDetails version '%1'").arg(QString(tokens[0])), false);

    oldAlphabet = QString(tokens[1]);
    newAlphabet = QString(tokens[2]);

    if (!oldAlphabet.isValid() || !newAlphabet.isValid()) {
        return false;
    }

    return true;
}

QByteArray U2DbiPackUtils::packObjectNameDetails(const QString& oldName, const QString& newName) {
    QByteArray result = VERSION;
    result += SEP;
    result += oldName.toUtf8();
    result += SEP;
    result += newName.toUtf8();
    return result;
}

bool U2DbiPackUtils::unpackObjectNameDetails(const QByteArray& modDetails, QString& oldName, QString& newName) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(3 == tokens.count(), "Invalid modDetails!", false);
    SAFE_POINT(VERSION == tokens[0], "Invalid modDetails version!", false);
    SAFE_POINT(!QString(tokens[1]).isEmpty(), "Invalid modDetails!", false);
    SAFE_POINT(!QString(tokens[2]).isEmpty(), "Invalid modDetails!", false);

    oldName = QString::fromUtf8(tokens[1]);
    newName = QString::fromUtf8(tokens[2]);
    return true;
}

QByteArray U2DbiPackUtils::packRows(const QList<int>& rowIndexes, const QList<U2MsaRow>& rows) {
    SAFE_POINT(rowIndexes.size() == rows.size(), "Different lists sizes", "");
    QByteArray result = VERSION;
    QList<int>::ConstIterator pi = rowIndexes.begin();
    QList<U2MsaRow>::ConstIterator ri = rows.begin();
    for (; ri != rows.end(); ri++, pi++) {
        result += SECOND_SEP + packRow(*pi, *ri);
    }
    return result;
}

bool U2DbiPackUtils::unpackRows(const QByteArray& modDetails, QList<int>& rowIndexes, QList<U2MsaRow>& rows) {
    QList<QByteArray> tokens = modDetails.split(SECOND_SEP);
    SAFE_POINT(tokens.count() > 0, QString("Invalid modDetails '%1'!").arg(QString(modDetails)), false);
    QByteArray modDetailsVersion = tokens.takeFirst();
    SAFE_POINT(modDetailsVersion == VERSION, QString("Invalid modDetails version '%1'").arg(QString(modDetailsVersion)), false);
    for (const QByteArray& token : qAsConst(tokens)) {
        int pos = 0;
        U2MsaRow row;
        bool ok = unpackRow(token, pos, row);
        CHECK(ok, false);
        rowIndexes << pos;
        rows << row;
    }
    return true;
}

QByteArray U2DbiPackUtils::packSequenceDataDetails(const U2Region& replacedRegion, const QByteArray& oldData, const QByteArray& newData, const QVariantMap& hints) {
    // replacedRegion length is used only for check, it is not stored
    SAFE_POINT(replacedRegion.length >= oldData.length(), "oldData length does not match to the region length.", QByteArray());
    QByteArray result = VERSION;
    result += SEP;
    result += QByteArray::number(replacedRegion.startPos);
    result += SEP;
    result += oldData;
    result += SEP;
    result += newData;
    result += SEP;
    result += packSequenceDataHints(hints);
    return result;
}

bool U2DbiPackUtils::unpackSequenceDataDetails(const QByteArray& modDetails, U2Region& replacedRegion, QByteArray& oldData, QByteArray& newData, QVariantMap& hints) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(tokens.count() == 5, QString("Invalid modDetails '%1'!").arg(QString(modDetails)), false);
    SAFE_POINT(VERSION == tokens[0], QString("Invalid modDetails version '%1'").arg(QString(tokens[0])), false);

    SAFE_POINT(!QString(tokens[1]).isEmpty(), "Invalid modDetails!", false);

    bool ok = false;
    replacedRegion = U2Region(tokens[1].toLongLong(&ok), tokens[2].length());
    CHECK(ok, false);

    oldData = tokens[2];
    newData = tokens[3];
    ok = unpackSequenceDataHints(tokens[4], hints);
    CHECK(ok, false);
    return true;
}

QByteArray U2DbiPackUtils::packSequenceDataHints(const QVariantMap& hints) {
    QByteArray result;
    foreach (QString key, hints.keys()) {
        if (!result.isEmpty()) {
            result += ";";
        }
        result += key.toLatin1() + "," + hints[key].toByteArray();
    }
    return "\"" + result + "\"";
}

bool U2DbiPackUtils::unpackSequenceDataHints(const QByteArray& str, QVariantMap& hints) {
    CHECK(str.startsWith('\"') && str.endsWith('\"'), false);
    QByteArray hintsStr = str.mid(1, str.length() - 2);
    if (hintsStr.isEmpty()) {
        return true;
    }

    QList<QByteArray> tokens = hintsStr.split(';');
    foreach (const QByteArray& t, tokens) {
        QList<QByteArray> hintTokens = t.split(',');
        CHECK(2 == hintTokens.size(), false);
        hints.insert(QString(hintTokens[0]), QVariant(hintTokens[1]));
    }
    return true;
}

QByteArray U2DbiPackUtils::packAlignmentLength(qint64 oldLen, qint64 newLen) {
    QByteArray result;
    result += QByteArray::number(oldLen);
    result += SEP;
    result += QByteArray::number(newLen);
    return result;
}

bool U2DbiPackUtils::unpackAlignmentLength(const QByteArray& modDetails, qint64& oldLen, qint64& newLen) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(tokens.count() == 2, QString("Invalid modDetails '%1'!").arg(QString(modDetails)), false);

    bool ok = false;
    oldLen = tokens.first().toInt(&ok);
    CHECK(ok, false);
    newLen = tokens.last().toInt(&ok);
    CHECK(ok, false);

    return true;
}

QByteArray U2DbiPackUtils::packUdr(const QByteArray& oldData, const QByteArray& newData) {
    QByteArray result;
    result += oldData.toHex();
    result += SEP;
    result += newData.toHex();
    return result;
}

bool U2DbiPackUtils::unpackUdr(const QByteArray& modDetails, QByteArray& oldData, QByteArray& newData) {
    QList<QByteArray> tokens = modDetails.split(SEP);
    SAFE_POINT(tokens.count() == 2, QString("Invalid modDetails, wrong tokens count: %1. Expected - 2.").arg(tokens.size()), false);

    oldData = QByteArray::fromHex(tokens.first());
    newData = QByteArray::fromHex(tokens.last());

    return true;
}

}  // namespace U2
