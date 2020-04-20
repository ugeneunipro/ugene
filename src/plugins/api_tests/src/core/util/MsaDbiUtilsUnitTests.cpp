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

#include "MsaDbiUtilsUnitTests.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/U2AlphabetUtils.h>

namespace U2 {

const QString MsaDbiUtilsTestUtils::alignmentName = QString("MsaDbiUtils test alignment");
TestDbiProvider MsaDbiUtilsTestUtils::dbiProvider = TestDbiProvider();
const QString& MsaDbiUtilsTestUtils::MSA_DB_URL("msa-dbi-utils.ugenedb");
U2MsaDbi* MsaDbiUtilsTestUtils::msaDbi = NULL;
U2SequenceDbi* MsaDbiUtilsTestUtils::sequenceDbi = NULL;


void MsaDbiUtilsTestUtils::init() {
    SAFE_POINT(NULL == msaDbi, "msaDbi has been already initialized!", );
    SAFE_POINT(NULL == sequenceDbi, "sequenceDbi has been already initialized!", );

    bool ok = dbiProvider.init(MSA_DB_URL, true);
    SAFE_POINT(ok, "Dbi provider failed to initialize in MsaTestData::init()!",);

    U2Dbi* dbi = dbiProvider.getDbi();
    msaDbi = dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "Failed to get msaDbi!",);

    sequenceDbi = dbi->getSequenceDbi();
    SAFE_POINT(NULL != sequenceDbi, "Failed to get sequenceDbi!",);
}

void MsaDbiUtilsTestUtils::shutdown() {
    if (NULL != msaDbi) {
        SAFE_POINT(NULL != sequenceDbi, "sequenceDbi must also be not NULL on this step!", );

        U2OpStatusImpl os;
        dbiProvider.close();
        msaDbi = NULL;
        sequenceDbi = NULL;
        SAFE_POINT_OP(os, );
    }
}

U2MsaDbi* MsaDbiUtilsTestUtils::getMsaDbi() {
    if (NULL == msaDbi) {
        init();
    }
    return msaDbi;
}

U2SequenceDbi* MsaDbiUtilsTestUtils::getSequenceDbi() {
    if (NULL == sequenceDbi) {
        init();
    }
    return sequenceDbi;
}


U2EntityRef MsaDbiUtilsTestUtils::initTestAlignment(const qint64 rowCount) {
    getMsaDbi();
    getSequenceDbi();

    SAFE_POINT(NULL != msaDbi, "MsaDbi is NULL", U2EntityRef());
    SAFE_POINT(NULL != sequenceDbi, "SequenceDbi is NULL", U2EntityRef());

    U2OpStatusImpl os;
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_OP(os, U2EntityRef());

    QList<U2MsaRow> rows;
    for (int i = 0; i < rowCount; ++i) {
        U2Sequence sequence;
        sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
        sequence.visualName = QString::number(i);
        sequenceDbi->createSequenceObject(sequence, "", os);
        CHECK_OP(os, U2EntityRef());

        U2MsaRow row;
        row.rowId = i;
        row.sequenceId = sequence.id;
        row.gstart = 0;
        row.gend = 5;

        rows << row;
    }

    msaDbi->addRows(msaId, rows, os);
    CHECK_OP(os, U2EntityRef());

    U2EntityRef msaRef(msaDbi->getRootDbi()->getDbiRef(), msaId);
    return msaRef;
}

U2EntityRef MsaDbiUtilsTestUtils::initTestAlignment(const QStringList& rowsData = QStringList()) {
    getMsaDbi();
    getSequenceDbi();

    SAFE_POINT(NULL != msaDbi, "MsaDbi is NULL", U2EntityRef());
    SAFE_POINT(NULL != sequenceDbi, "SequenceDbi is NULL", U2EntityRef());

    U2OpStatusImpl os;
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_OP(os, U2EntityRef());

    qint64 rowCount = rowsData.length();

    QList<U2MsaRow> rows;
    for (int i = 0; i < rowCount; ++i) {
        QByteArray seqData;
        QList<U2MsaGap> gapModel;
        MaDbiUtils::splitBytesToCharsAndGaps(rowsData[i].toLatin1(), seqData, gapModel);

        U2Sequence sequence;
        sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
        sequence.visualName = QString::number(i);
        sequence.length = rowsData[i].length();
        sequenceDbi->createSequenceObject(sequence, "", os);
        CHECK_OP(os, U2EntityRef());
        sequenceDbi->updateSequenceData(sequence.id, U2Region(0, 0), seqData, QVariantMap(), os);
        CHECK_OP(os, U2EntityRef());

        U2MsaRow row;
        row.rowId = i;
        row.sequenceId = sequence.id;
        row.gstart = 0;
        row.gend = seqData.length();
        row.gaps = gapModel;

        rows << row;
    }

    msaDbi->addRows(msaId, rows, os);
    CHECK_OP(os, U2EntityRef());

    U2EntityRef msaRef(msaDbi->getRootDbi()->getDbiRef(), msaId);
    return msaRef;
}

U2EntityRef MsaDbiUtilsTestUtils::initTestAlignment(QList<U2MsaRow>& rows) {
    getMsaDbi();
    getSequenceDbi();

    SAFE_POINT(NULL != msaDbi, "MsaDbi is NULL", U2EntityRef());
    SAFE_POINT(NULL != sequenceDbi, "SequenceDbi is NULL", U2EntityRef());

    U2OpStatusImpl os;
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_OP(os, U2EntityRef());

    msaDbi->addRows(msaId, rows, os);
    CHECK_OP(os, U2EntityRef());

    U2EntityRef msaRef(msaDbi->getRootDbi()->getDbiRef(), msaId);
    return msaRef;
}

void Utils::addRow(U2Dbi *dbi, const U2DataId &msaId,
    const QByteArray &name, const QByteArray &seq, const QList<U2MsaGap> &gaps,
    U2OpStatus &os) {
    U2Sequence sequence;
    sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    sequence.visualName = name;
    dbi->getSequenceDbi()->createSequenceObject(sequence, "", os);
    CHECK_OP(os, );

    U2Region reg(0, 0);
    dbi->getSequenceDbi()->updateSequenceData(sequence.id, reg, seq, QVariantMap(), os);
    CHECK_OP(os, );

    U2MsaRow row;
    row.rowId = -1;
    row.sequenceId = sequence.id;
    row.gstart = 0;
    row.gend = seq.length();
    row.gaps = gaps;

    dbi->getMsaDbi()->addRow(msaId, -1, row, os);
}

U2MsaRow MsaDbiUtilsTestUtils::addRow(const QByteArray &name, const QByteArray &seq, const QList<U2MsaGap> &gaps, U2OpStatus &os) {
    U2Sequence sequence;
    sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    sequence.visualName = name;
    sequenceDbi->createSequenceObject(sequence, "", os);
    CHECK_OP(os, U2MsaRow());

    U2Region reg(0, 0);
    sequenceDbi->updateSequenceData(sequence.id, reg, seq, QVariantMap(), os);
    CHECK_OP(os, U2MsaRow());

    U2MsaRow row;
    row.rowId = -1;
    row.sequenceId = sequence.id;
    row.gstart = 0;
    row.gend = seq.length();
    row.gaps = gaps;
    return row;
}

U2EntityRef MsaDbiUtilsTestUtils::removeRegionTestAlignment(U2OpStatus &os) {
    U2MsaDbi *msaDbi = getMsaDbi();
    U2SequenceDbi *sequenceDbi = getSequenceDbi();
    SAFE_POINT(NULL != msaDbi, "MsaDbi is NULL", U2EntityRef());
    SAFE_POINT(NULL != sequenceDbi, "SequenceDbi is NULL", U2EntityRef());

    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_OP(os, U2EntityRef());

    U2Dbi *dbi = msaDbi->getRootDbi();
    SAFE_POINT(NULL != dbi, "Root dbi is NULL", U2EntityRef());

    Utils::addRow(dbi, msaId, "1", "TAAGACTTCTAA", QList<U2MsaGap>() << U2MsaGap(12, 2), os);
    Utils::addRow(dbi, msaId, "2", "TAAGCTTACTA", QList<U2MsaGap>() << U2MsaGap(11, 3), os);
    Utils::addRow(dbi, msaId, "3", "TTAGTTTATTA", QList<U2MsaGap>() << U2MsaGap(11, 3), os);
    Utils::addRow(dbi, msaId, "4", "TCAGTCTATTA", QList<U2MsaGap>() << U2MsaGap(1, 2) << U2MsaGap(5, 1), os);
    Utils::addRow(dbi, msaId, "5", "TCAGTTTATTA", QList<U2MsaGap>() << U2MsaGap(1, 2) << U2MsaGap(5, 1), os);
    Utils::addRow(dbi, msaId, "6", "TTAGTCTACTA", QList<U2MsaGap>() << U2MsaGap(1, 2) << U2MsaGap(5, 1), os);
    Utils::addRow(dbi, msaId, "7", "TCAGATTATTA", QList<U2MsaGap>() << U2MsaGap(1, 2) << U2MsaGap(5, 1), os);
    Utils::addRow(dbi, msaId, "8", "TTAGATTGCTA", QList<U2MsaGap>() << U2MsaGap(1, 1) << U2MsaGap(12, 2), os);
    Utils::addRow(dbi, msaId, "9", "TTAGATTATTA", QList<U2MsaGap>() << U2MsaGap(11, 3), os);
    Utils::addRow(dbi, msaId, "10", "", QList<U2MsaGap>() << U2MsaGap(0, 14), os);
    Utils::addRow(dbi, msaId, "11", "", QList<U2MsaGap>() << U2MsaGap(0, 14), os);
    Utils::addRow(dbi, msaId, "12", "", QList<U2MsaGap>() << U2MsaGap(0, 14), os);
    Utils::addRow(dbi, msaId, "13", "", QList<U2MsaGap>() << U2MsaGap(0, 14), os);
    CHECK_OP(os, U2EntityRef());

    return  U2EntityRef(msaDbi->getRootDbi()->getDbiRef(), msaId);
}

QStringList MsaDbiUtilsTestUtils::getRowNames(U2EntityRef msaRef) {
    SAFE_POINT(msaRef.isValid(), "msaRef no valid", QStringList())
    U2OpStatusImpl os;
    QStringList res;

    QList<U2MsaRow> rows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_OP(os, QStringList());
    foreach (U2MsaRow row, rows) {
        res << sequenceDbi->getSequenceObject(row.sequenceId, os).visualName;
        CHECK_OP(os, QStringList());
    }

    return res;
}


IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddle) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(3, 2);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    int delta = -1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(3, 4);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddleToTheTop) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(3, 0);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    int delta = -3;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up to i");

    expected.move(3, 0);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    delta = -4;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step up to i+1");

    expected.move(3, 1);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    delta = -2;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step up to i-1");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddleToTheBottom) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(3, 6);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    int delta = 3;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step down to i");

    expected.move(3, 6);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    delta = 4;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down to i+1");

    expected.move(3, 5);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId;

    delta = 2;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down to i-1");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_oneBlockInMiddle) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(3, 2);
    expected.move(4, 3);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[4].rowId;

    int delta = -1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(4, 5);
    expected.move(3, 4);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[4].rowId;

    delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_oneBlockInMiddleToTheOutside) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(3, 0);
    expected.move(4, 1);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[4].rowId;

    int delta = -9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(4, 6);
    expected.move(3, 5);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[4].rowId;

    delta = 9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksInMiddleWithoutGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(2, 1);
    expected.move(4, 3);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[2].rowId << allRows[4].rowId;

    int delta = -1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(4, 5);
    expected.move(2, 3);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[2].rowId << allRows[4].rowId;

    delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksInMiddleWithGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(2, 0);
    expected.move(4, 1);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[2].rowId << allRows[4].rowId;

    int delta = -9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(4, 6);
    expected.move(2, 5);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[2].rowId << allRows[4].rowId;

    delta = 9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnTopWithoutGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(0, 0);
    expected.move(3, 2);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[3].rowId;

    int delta = -1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(0, 1);
    expected.move(3, 4);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[3].rowId;

    delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnTopWithGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(0, 0);
    expected.move(3, 1);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[3].rowId;

    int delta = -9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(3, 6);
    expected.move(0, 5);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[3].rowId;

    delta = 9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnBottomWithoutGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(6, 6);
    expected.move(3, 4);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[6].rowId;

    int delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step down");

    expected.move(6, 5);
    expected.move(3, 2);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[6].rowId;

    delta = -1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step up");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnBottomWithGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(6, 6);
    expected.move(3, 5);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[6].rowId;

    int delta = 9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step down");

    expected.move(3, 0);
    expected.move(6, 1);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[3].rowId << allRows[6].rowId;

    delta = -9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step up");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithoutGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(0, 0);
    expected.move(4, 3);
    expected.move(6, 5);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[4].rowId << allRows[6].rowId;

    int delta = -1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(0, 1);
    expected.move(2, 3);
    expected.move(6, 6);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[2].rowId << allRows[6].rowId;

    delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithOnceGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(0, 0);
    expected.move(2, 1);
    expected.move(6, 3);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[2].rowId << allRows[6].rowId;

    int delta = -3;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(6, 6);
    expected.move(2, 5);
    expected.move(0, 3);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[2].rowId << allRows[6].rowId;

    delta = 3;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithTwiceGluing) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);
    QStringList expected = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    expected.move(0, 0);
    expected.move(2, 1);
    expected.move(6, 2);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[2].rowId << allRows[6].rowId;

    int delta = -9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    QStringList actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    if (expected.length() != rowCount) {
        SetError("Unexpected count of rows");
        return;
    }
    CHECK_TRUE(expected == actual, "step up");

    expected.move(6, 6);
    expected.move(2, 5);
    expected.move(0, 4);
    rowsToMove.clear();
    allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[0].rowId << allRows[2].rowId << allRows[6].rowId;

    delta = 9;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    CHECK_NO_ERROR(os);

    actual = MsaDbiUtilsTestUtils::getRowNames(msaRef);
    CHECK_TRUE(expected == actual, "step down");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_UnorderedList) {
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);

    QList<qint64> rowsToMove;
    QList<U2MsaRow> allRows = msaDbi->getRows(msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    rowsToMove << allRows[5].rowId << allRows[3].rowId;

    int delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    if (!(!os.isCanceled() && os.hasError() && os.getError() == "List of rows to move is not ordered")) {
        CHECK_TRUE(false, "List not ordered");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, moveRows_InvalidRowList) {
    // for init
    MsaDbiUtilsTestUtils::getMsaDbi();
    U2OpStatusImpl os;
    int rowCount = 7;

    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(rowCount);

    QList<qint64> rowsToMove;
    rowsToMove << -1;

    int delta = 1;
    MaDbiUtils::moveRows(msaRef, rowsToMove, delta, os);
    if (!(!os.isCanceled() && os.hasError() && os.getError() == "Invalid row list")) {
        CHECK_TRUE(false, "Invalid row list");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_noGaps) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "AACCGGTT" << "CCGGTTAA" << "GGTTAACC");

    //Prepare expected state
    QStringList expected = QStringList() << "AACCGGTT" << "CCGGTTAA" << "GGTTAACC";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(8, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_leadingGaps) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "--AACCGGTT" << "--CCGGTTAA" << "--GGTTA--C");

    //Prepare expected state
    QStringList expected = QStringList() << "AACCGGTT" << "CCGGTTAA" << "GGTTA--C";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(8, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_trailingGaps) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "AACCGGTT--" << "CCG--TAA--" << "GGTTAACC--");

    //Prepare expected state
    QStringList expected = QStringList() << "AACCGGTT" << "CCG--TAA" << "GGTTAACC";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(8, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_leadingGapsCutOff) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "---TAACCGG" << "--CCGGTTAA" << "--GGTTAACC");

    //Prepare expected state
    QStringList expected = QStringList() << "-TAACCGG" << "CCGGTTAA" << "GGTTAACC";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(8, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_trailingGapsCutOff) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "AACCGGT---" << "CCGGTTAA--" << "GGTTAACC--");

    //Prepare expected state
    QStringList expected = QStringList() << "AACCGGT-" << "CCGGTTAA" << "GGTTAACC";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(8, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_leadingAndTrailingGaps) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "--AACCGGTT--" << "---ACCGGT--" << "----CCGGTT---");

    //Prepare expected state
    QStringList expected = QStringList() << "AACCGGTT" << "-ACCGGT-" << "--CCGGTT";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(8, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, trim_gapsOnly) {
    //Init test data
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::initTestAlignment(QStringList() << "------" << "----" << "-----");

    //Prepare expected state
    QStringList expected = QStringList() << "" << "" << "";

    //Call test function
    MsaDbiUtils::trim(msaRef, os);

    //Check actual state
    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, al->getLength(), "Wrong msa length.");
    CHECK_EQUAL(expected.length(), al->getNumRows(), "Wrong rows count.");

    QStringList actual;
    actual << al->getMsaRow(0)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(1)->toByteArray(os, al->getLength());
    actual << al->getMsaRow(2)->toByteArray(os, al->getLength());
    for (int i = 0; i < expected.length(); ++i) {
        CHECK_EQUAL(expected[i], actual[i], "Wrong msa data.");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_oneRow) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds << baseRowIds[1];
    MsaDbiUtils::removeRegion(msaRef, rowIds, 8, 3, os);
    CHECK_NO_ERROR(os);

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(14, al->getLength(), "Wrong msa length");

    QByteArray row0 = al->getMsaRow(0)->toByteArray(os, al->getLength());
    QByteArray row1 = al->getMsaRow(1)->toByteArray(os, al->getLength());
    QByteArray row2 = al->getMsaRow(2)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString("TAAGACTTCTAA--"), QString(row0), "Wrong msa row");
    CHECK_EQUAL(QString("TAAGCTTA------"), QString(row1), "Wrong msa row");
    CHECK_EQUAL(QString("TTAGTTTATTA---"), QString(row2), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_threeRows) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds << baseRowIds[1] << baseRowIds[8] << baseRowIds[5];
    MsaDbiUtils::removeRegion(msaRef, rowIds, 2, 8, os);
    CHECK_NO_ERROR(os);

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(14, al->getLength(), "Wrong msa length");

    QByteArray row1 = al->getMsaRow(1)->toByteArray(os, al->getLength());
    QByteArray row5 = al->getMsaRow(5)->toByteArray(os, al->getLength());
    QByteArray row8 = al->getMsaRow(8)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString("TAA-----------"), QString(row1), "Wrong msa row");
    CHECK_EQUAL(QString("T-ACTA--------"), QString(row5), "Wrong msa row");
    CHECK_EQUAL(QString("TTA-----------"), QString(row8), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_lengthChange) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds << baseRowIds[3] << baseRowIds[4] << baseRowIds[5] << baseRowIds[6];
    MsaDbiUtils::removeRegion(msaRef, rowIds, 5, 1, os);
    CHECK_NO_ERROR(os);

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(14, al->getLength(), "Wrong msa length");

    QByteArray row3 = al->getMsaRow(3)->toByteArray(os, al->getLength());
    QByteArray row4 = al->getMsaRow(4)->toByteArray(os, al->getLength());
    QByteArray row5 = al->getMsaRow(5)->toByteArray(os, al->getLength());
    QByteArray row6 = al->getMsaRow(6)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString("T--CAGTCTATTA-"), QString(row3), "Wrong msa row");
    CHECK_EQUAL(QString("T--CAGTTTATTA-"), QString(row4), "Wrong msa row");
    CHECK_EQUAL(QString("T--TAGTCTACTA-"), QString(row5), "Wrong msa row");
    CHECK_EQUAL(QString("T--CAGATTATTA-"), QString(row6), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_allRows) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds = baseRowIds;
    MsaDbiUtils::removeRegion(msaRef, rowIds, 0, 3, os);
    CHECK_NO_ERROR(os);

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(11, al->getLength(), "Wrong msa length");

    QByteArray row0 = al->getMsaRow(0)->toByteArray(os, al->getLength());
    QByteArray row1 = al->getMsaRow(1)->toByteArray(os, al->getLength());
    QByteArray row2 = al->getMsaRow(2)->toByteArray(os, al->getLength());
    QByteArray row3 = al->getMsaRow(3)->toByteArray(os, al->getLength());
    QByteArray row4 = al->getMsaRow(4)->toByteArray(os, al->getLength());
    QByteArray row5 = al->getMsaRow(5)->toByteArray(os, al->getLength());
    QByteArray row6 = al->getMsaRow(6)->toByteArray(os, al->getLength());
    QByteArray row7 = al->getMsaRow(7)->toByteArray(os, al->getLength());
    QByteArray row8 = al->getMsaRow(8)->toByteArray(os, al->getLength());
    QByteArray row9 = al->getMsaRow(9)->toByteArray(os, al->getLength());
    QByteArray row10 = al->getMsaRow(10)->toByteArray(os, al->getLength());
    QByteArray row11 = al->getMsaRow(11)->toByteArray(os, al->getLength());
    QByteArray row12 = al->getMsaRow(12)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString("GACTTCTAA--"), QString(row0), "Wrong msa row");
    CHECK_EQUAL(QString("GCTTACTA---"), QString(row1), "Wrong msa row");
    CHECK_EQUAL(QString("GTTTATTA---"), QString(row2), "Wrong msa row");
    CHECK_EQUAL(QString("CA-GTCTATTA"), QString(row3), "Wrong msa row");
    CHECK_EQUAL(QString("CA-GTTTATTA"), QString(row4), "Wrong msa row");
    CHECK_EQUAL(QString("TA-GTCTACTA"), QString(row5), "Wrong msa row");
    CHECK_EQUAL(QString("CA-GATTATTA"), QString(row6), "Wrong msa row");
    CHECK_EQUAL(QString("AGATTGCTA--"), QString(row7), "Wrong msa row");
    CHECK_EQUAL(QString("GATTATTA---"), QString(row8), "Wrong msa row");
    CHECK_EQUAL(QString("-----------"), QString(row9), "Wrong msa row");
    CHECK_EQUAL(QString("-----------"), QString(row10), "Wrong msa row");
    CHECK_EQUAL(QString("-----------"), QString(row11), "Wrong msa row");
    CHECK_EQUAL(QString("-----------"), QString(row12), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_all) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds = baseRowIds;
    MsaDbiUtils::removeRegion(msaRef, rowIds, 0, 14, os);
    CHECK_NO_ERROR(os);

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, al->getLength(), "Wrong msa length");

    QByteArray row0 = al->getMsaRow(0)->toByteArray(os, al->getLength());
    QByteArray row12 = al->getMsaRow(12)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString(""), QString(row0), "Wrong msa row");
    CHECK_EQUAL(QString(""), QString(row12), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_negativePos) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds << baseRowIds[0];
    U2OpStatusImpl tmpOs;
    MsaDbiUtils::removeRegion(msaRef, rowIds, -1, 14, tmpOs);
    CHECK_TRUE(tmpOs.hasError(), "No error occurred for negative pos");

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(14, al->getLength(), "Wrong msa length");

    QByteArray row0 = al->getMsaRow(0)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString("TAAGACTTCTAA--"), QString(row0), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_wrongId) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    qint64 customId;
    while (true == baseRowIds.contains(customId)) {
        customId = rand();
    }

    QStringList ids;
    foreach (qint64 id, rowIds) {
        ids << QString::number(id);
    }
    coreLog.details(QString("MsaDbiUtilsUnitTests_removeRegion_wrongId: Existing IDs: %1; a custom wrong ID: %2").arg(ids.join(", ")).arg(customId));

    rowIds << customId;
    U2OpStatusImpl tmpOs;
    MsaDbiUtils::removeRegion(msaRef, rowIds, 0, 5, tmpOs);
    CHECK_TRUE(tmpOs.hasError(), "No error occurred for wrong ID");
    coreLog.details(QString("MsaDbiUtilsUnitTests_removeRegion_wrongId: an error message in tmpOs: %1").arg(tmpOs.getError()));
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, removeRegion_wrongCount) {
    U2OpStatusImpl os;
    U2EntityRef msaRef = MsaDbiUtilsTestUtils::removeRegionTestAlignment(os);
    CHECK_NO_ERROR(os);

    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    QList<qint64> baseRowIds = msaDbi->getRowsOrder(msaRef.entityId, os);
    CHECK_NO_ERROR(os);

    QList<qint64> rowIds;
    rowIds << baseRowIds[0];
    U2OpStatusImpl tmpOs;
    MsaDbiUtils::removeRegion(msaRef, rowIds, 0, 0, tmpOs);
    CHECK_TRUE(tmpOs.hasError(), "No error occurred for wrong count");

    MultipleSequenceAlignmentExporter ex;
    MultipleSequenceAlignment al = ex.getAlignment(msaRef.dbiRef, msaRef.entityId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(14, al->getLength(), "Wrong msa length");

    QByteArray row0 = al->getMsaRow(0)->toByteArray(os, al->getLength());
    CHECK_EQUAL(QString("TAAGACTTCTAA--"), QString(row0), "Wrong msa row");
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_empty) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // Call test function
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(9, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_nothingNew) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Test call without new information
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(9, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_newOrder) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    // The first call is correct
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Change order of rows
    ma->moveRowsBlock(1, 1, 1);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // Test call with new row order
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(9, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_newName) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Change alignemnt name
    ma->setName("New name");

    // Test call with new alignment name
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(9, msa.length, "msa length");
    CHECK_EQUAL("New name", msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_newAlphabet) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Change alignment alphabet
    const DNAAlphabet* newAlphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    ma->setAlphabet(newAlphabet);

    // Test call with new alphabet
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(9, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_newContent) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Change sequence data in the second row
    ma->setRowContent(1, "AA--AA--AA--AA");

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // Test call with changed data in second row
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(14, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_newSequence) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Export an alignment from the dbi to the memory (to set actual IDs)
    MultipleSequenceAlignmentExporter exporter;
    MultipleSequenceAlignment expMa;
    expMa = exporter.getAlignment(dbiRef, msaId, os);
    CHECK_NO_ERROR(os);

    // Set a new sequence ID for the second row
    U2Sequence newSeq;
    newSeq.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    newSeq.circular = false;
    seqDbi->createSequenceObject(newSeq, "", os);
    CHECK_NO_ERROR(os);

    U2MsaRow changedRow = expMa->getMsaRow(1)->getRowDbInfo();
    changedRow.sequenceId = newSeq.id;
    expMa->removeRow(1, os);
    CHECK_NO_ERROR(os);
    DNASequence newDnaSeq("New sequence", "CGCGCGAAT");
    expMa->addRow(changedRow, newDnaSeq, os);
    CHECK_NO_ERROR(os);

    QList<MultipleSequenceAlignmentRow> expMaRows = expMa->getMsaRows();

    // Test call with changed sequence ID
    MsaDbiUtils::updateMsa(msaRef, expMa, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(11, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expMaRows.count(), rows.count(), "rows count");
    for (int i = 0; i < expMaRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(expMaRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(expMaRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(expMaRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(expMaRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(expMaRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(expMaRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(expMaRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_additionalRows) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Add new rows to the alignment
    ma->addRow("5", "ATATATATAT", -1);
    ma->addRow("6", "CGCG--CGCG", -1);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // Test call without new information
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(10, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_removeRows) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Remove some rows from the alignment
    ma->removeRow(1, os);
    CHECK_NO_ERROR(os);
    ma->removeRow(2, os);
    CHECK_NO_ERROR(os);

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // Test call without some rows
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(9, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

IMPLEMENT_TEST(MsaDbiUtilsUnitTests, updateMsa_clear) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaDbiUtilsTestUtils::getMsaDbi();
    U2SequenceDbi* seqDbi = MsaDbiUtilsTestUtils::getSequenceDbi();
    U2DataId msaId = msaDbi->createMsaObject("", MsaDbiUtilsTestUtils::alignmentName, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    U2DbiRef dbiRef(msaDbi->getRootDbi()->getFactoryId(), msaDbi->getRootDbi()->getDbiId());
    U2EntityRef msaRef(dbiRef, msaId);

    // Prepare input data
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(MsaDbiUtilsTestUtils::alignmentName, alphabet);
    ma->addRow("1", "AAAA--AAA", -1);
    ma->addRow("2", "C--CCCCCC", -1);
    ma->addRow("3", "GG-GGGG-G", -1);
    ma->addRow("4", "TTT-TTTT", -1);

    // The first call is correct (updateMsa_empty test)
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Clear alignment
    ma->clear();

    QList<MultipleSequenceAlignmentRow> maRows = ma->getMsaRows();

    // Test call with empty alignment
    MsaDbiUtils::updateMsa(msaRef, ma, os);
    CHECK_NO_ERROR(os);

    // Verify msa object
    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), msa.alphabet.id, "msa alphabet");
    CHECK_EQUAL(0, msa.length, "msa length");
    CHECK_EQUAL(MsaDbiUtilsTestUtils::alignmentName, msa.visualName, "msa name");

    // Verify rows and sequences
    QList<U2MsaRow> rows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(maRows.count(), rows.count(), "rows count");
    for (int i = 0; i < maRows.count(); ++i) {
        // Verify row object
        CHECK_EQUAL(maRows[i]->getCoreStart(), rows[i].gstart, "row start");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), rows[i].gend, "row end");
        CHECK_EQUAL(maRows[i]->getRowLengthWithoutTrailing(), rows[i].length, "row length");
        CHECK_TRUE(maRows[i]->getGapModel() == rows[i].gaps, "row gaps");

        // Verify sequence object
        U2Sequence seq = seqDbi->getSequenceObject(rows[i].sequenceId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), seq.alphabet.id, "sequence alphabet");
        CHECK_EQUAL(maRows[i]->getUngappedLength(), seq.length, "sequence length");
        CHECK_EQUAL(maRows[i]->getSequence().getName(), seq.visualName, "sequence name");

        // Verify sequence data
        QByteArray seqData = seqDbi->getSequenceData(rows[i].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(maRows[i]->getSequence().constSequence()), QString(seqData), "sequence data");
    }
}

}   // namespace
