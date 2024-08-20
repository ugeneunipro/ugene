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

#include "BAMUtilsUnitTests.h"

#include <QDir>
#include <QUuid>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/TestRunnerSettings.h>

#include <U2Formats/BAMUtils.h>

namespace U2 {

static QString getTmpDir() {
    auto uuid = QUuid::createUuid().toString();
    auto tempDir = QDir::temp().absoluteFilePath(uuid.mid(1, uuid.size() - 2));
    bool ok = QDir(tempDir).mkpath(".");
    CHECK(ok, QString());

    return tempDir;
}

bool equals(const QString& path1, const QString& path2, U2OpStatus& os) {
    QFile f1(path1);
    QFile f2(path2);

    bool ok = f1.open(QIODevice::ReadOnly) && f2.open(QIODevice::ReadOnly);
    CHECK_EXT(ok, os.setError(f1.errorString() + " " + f2.errorString()), false);

    QByteArray byteArray1 = f1.readAll();
    QByteArray byteArray2 = f2.readAll();

    CHECK_EXT((f1.error() == QFile::NoError && f2.error() == QFile::NoError), os.setError(f1.errorString() + " " + f2.errorString()), false);
    return byteArray1 == byteArray2;
}


IMPLEMENT_TEST(BAMUtilsUnitTests, convertBamToSam) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString unitTestDataDir = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/bam_to_sam/";
    QString bamFile = unitTestDataDir + "input.bam";
    auto tempDir = getTmpDir();
    CHECK_TRUE(!tempDir.isEmpty(), "Can't create tmp dir");

    QString expectedSamFile = tempDir + "/" + "output.sam";
    QString samFile = unitTestDataDir + "output.sam";

    U2OpStatusImpl os;
    BAMUtils::convertBamToSam(os, bamFile, expectedSamFile);
    CHECK_NO_ERROR(os);
    bool ok = equals(expectedSamFile, samFile, os);
    CHECK_TRUE(ok, "Result fules are not equal");
}

static void convertSamToBam(const QString& directory, bool hasReference, U2OpStatus& os) {
    auto tempDir = getTmpDir();
    CHECK_EXT(!tempDir.isEmpty(), os.setError("Can't create tmp dir"), );

    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString unitTestDataDir = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/" + directory + "/";
    QString samFile = unitTestDataDir + "input.sam";
    QString samReferenceFile;
    if (hasReference) {
        samReferenceFile = unitTestDataDir + "reference.fa";
    }
    QString bamFile = tempDir + "/" + "output.bam";
    BAMUtils::convertSamToBam(os, samFile, bamFile, samReferenceFile);
    CHECK_OP(os, );

    QString expectedBamFile = unitTestDataDir + "output.bam";
    bool ok = equals(expectedBamFile, bamFile, os);
    CHECK_EXT(ok, os.setError("Result fules are not equal"), );
}

IMPLEMENT_TEST(BAMUtilsUnitTests, convertSamToBamHeaderNoReference) {
    U2OpStatusImpl os;
    convertSamToBam("sam_to_bam_header_no_ref", false, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

IMPLEMENT_TEST(BAMUtilsUnitTests, convertSamToBamNoHeaderNoReference) {
    U2OpStatusImpl os;
    convertSamToBam("sam_to_bam_no_header_no_ref", false, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

IMPLEMENT_TEST(BAMUtilsUnitTests, convertSamToBamNoHeaderReference) {
    U2OpStatusImpl os;
    convertSamToBam("sam_to_bam_no_header_reference", true, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

static void isSorted(bool sorted, U2OpStatus& os) {
    auto tempDir = getTmpDir();
    CHECK_EXT(!tempDir.isEmpty(), os .setError("Can't create tmp dir"), );

    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString bamFile = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/is_sorted_" + (sorted ? "true" : "false") + "/input.bam";
    bool isSorted = BAMUtils::isSortedBam(bamFile, os);
    CHECK_OP(os, );
    CHECK_EXT(isSorted == sorted, os.setError(QString("Expected: %1, current: %2").arg(sorted).arg(isSorted)), );
}

IMPLEMENT_TEST(BAMUtilsUnitTests, isSortedTrue) {
    U2OpStatusImpl os;
    isSorted(true, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

IMPLEMENT_TEST(BAMUtilsUnitTests, isSortedFalse) {
    U2OpStatusImpl os;
    isSorted(false, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

IMPLEMENT_TEST(BAMUtilsUnitTests, sortBam) {
    auto tempDir = getTmpDir();
    CHECK_TRUE(!tempDir.isEmpty(), "Can't create tmp dir");

    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString unitTestDataDir = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/sort_bam/";
    QString bamFile = unitTestDataDir + "input.bam";
    QString sortedbamFile = unitTestDataDir + "output";
    U2OpStatusImpl os;
    auto sortedUrl = BAMUtils::sortBam(bamFile, sortedbamFile, os);
    CHECK_TRUE(!os.hasError(), os.getError());

    QString expectedBamFile = unitTestDataDir + "output.bam";
    bool ok = equals(expectedBamFile, sortedUrl.getURLString(), os);
    CHECK_TRUE(ok, "Result fules are not equal");
}

IMPLEMENT_TEST(BAMUtilsUnitTests, mergeBam) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString inputFiles = trs->getVar("COMMON_DATA_DIR") + "/regression/7862/orig.bam";
    auto uuid = QUuid::createUuid().toString();
    auto tempDir = QDir::temp().absoluteFilePath(uuid.mid(1, uuid.size() - 2));
    bool ok = QDir(tempDir).mkpath(".");
    CHECK_TRUE(ok, "Can't create tmp dir");

    QStringList bamUrls;
    for (int i = 0; i < 1050; i++) {
        auto fileName = tempDir + "/" + QString::number(i + 1) + ".bam";
        ok = QFile::copy(inputFiles, fileName);
        CHECK_TRUE(ok, QString("Can't copy %1 to %2").arg(inputFiles).arg(fileName));

        bamUrls << fileName;
    }
    QString resExpectedFile = tempDir + "/" + "res.bam";

    U2OpStatusImpl os;
    auto resFile = BAMUtils::mergeBam(bamUrls, resExpectedFile, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(resExpectedFile, resFile.getURLString(), "Files not equal");
    // 1050 input files + 1 result
    CHECK_TRUE(QDir(tempDir).entryList({ "*.bam" }).size() == 1051, "Incorrect files number");
}

IMPLEMENT_TEST(BAMUtilsUnitTests, hasValidBamIndex) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString bamFile = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/has_valid_bam_index/input.bam";
    U2OpStatusImpl os;
    auto hasValidBamIndex = BAMUtils::hasValidBamIndex(bamFile);
    CHECK_TRUE(!os.hasError(), os.getError());
    CHECK_TRUE(hasValidBamIndex, "Bam index should be valid");
}

IMPLEMENT_TEST(BAMUtilsUnitTests, hasValidFastaIndex) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString faFile = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/has_valid_fasta_index/input.fa";
    auto hasValidFastaIndex = BAMUtils::hasValidFastaIndex(faFile);
    CHECK_TRUE(hasValidFastaIndex, "Fasta index should valid");
}

IMPLEMENT_TEST(BAMUtilsUnitTests, createBamIndex) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString bamFile = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/create_bam_index/input.bam";
    U2OpStatusImpl os;
    BAMUtils::createBamIndex(bamFile, os);
    CHECK_TRUE(!os.hasError(), os.getError());
    CHECK_TRUE(QFileInfo::exists(bamFile + ".bai"), "Index wasn't created");
}

IMPLEMENT_TEST(BAMUtilsUnitTests, getBamIndexUrl) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString bamFile = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/get_bam_index_url/input.bam";
    auto indexUrl = BAMUtils::getBamIndexUrl(bamFile);
    auto indexUrlString = indexUrl.getURLString();
    coreLog.details(BAMUtils::tr("Current index urls string: \"%1\"").arg(indexUrlString));
    auto baiExpectedString = bamFile + ".bai";
    coreLog.details(BAMUtils::tr("Expected index urls string: \"%1\"").arg(baiExpectedString));
    CHECK_TRUE(QDir::toNativeSeparators(indexUrlString) ==
               QDir::toNativeSeparators(baiExpectedString), "Not an index fix");
}

static void isEqualByLength(bool equal, U2OpStatus& os) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString dataDir = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/is_equal_by_length_" + (equal ? "true" : "false") + "/";
    QString input1 = dataDir + "input1.bam";
    QString input2 = dataDir + "input2.bam";
    bool isEqual = BAMUtils::isEqualByLength(input1, input2, os);
    if (equal) {
        CHECK_OP(os, );
    } else {
        os.setError("");
    }
    CHECK_EXT(isEqual == equal, os.setError(QString("Expected: %1, current: %2").arg(equal).arg(isEqual)), );
}

IMPLEMENT_TEST(BAMUtilsUnitTests, isEqualByLengthTrue) {
    U2OpStatusImpl os;
    isEqualByLength(true, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

IMPLEMENT_TEST(BAMUtilsUnitTests, isEqualByLengthFalse) {
    U2OpStatusImpl os;
    isEqualByLength(false, os);
    CHECK_TRUE(!os.hasError(), os.getError());
}

IMPLEMENT_TEST(BAMUtilsUnitTests, loadIndex) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString bamFile = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/bam_utils/load_index/input.bam";
    auto index = BAMUtils::loadIndex(bamFile);
    CHECK_TRUE(index != nullptr, "Index is null");
}

}
