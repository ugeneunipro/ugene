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

#include <QDomElement>
#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>

#include "UnitTestSuite.h"
#include "core/datatype/udr/RawDataUdrSchemaUnitTests.h"
#include "core/dbi/assembly/AssemblyDbiUnitTests.h"
#include "core/dbi/attribute/AttributeDbiUnitTests.h"
#include "core/dbi/features/FeatureDbiUnitTests.h"
#include "core/dbi/sequence/SequenceDbiUnitTests.h"
#include "core/dbi/udr/UdrDbiUnitTests.h"
#include "core/format/sqlite_msa_dbi/MsaDbiSQLiteSpecificUnitTests.h"
#include "core/gobjects/DNAChromatogramObjectUnitTests.h"
#include "core/gobjects/FeaturesTableObjectUnitTest.h"
#include "core/gobjects/MsaObjectUnitTests.h"
#include "core/gobjects/TextObjectUnitTests.h"

namespace U2 {

class GTestBuilder {
public:
    bool addTestCase(const QString& testCase) {
        if (testCase.isEmpty() || tests.keys().contains(testCase)) {
            return false;
        }
        tests[testCase] = QStringList();
        return true;
    }

    bool addTest(const QString& testCase, const QString& test) {
        if (!tests.keys().contains(testCase)) {
            return false;
        }
        tests[testCase].append(test); 
        return true;
    }

    QMap<QString, QStringList> getTests() {
        return tests;
    }
private:
    QMap<QString, QStringList> tests;
};


void UnitTestSuite::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    GTestBuilder builder;

    passed = 0;
    ignored = 0;
    failed = 0;

    QString testList = el.text();
    QTextStream stream(&testList);
    QString line = stream.readLine();
    QString testCase;
    while (!line.isNull()) {
        line = line.trimmed();
        if (line.startsWith('+')) {
            line.remove(0,1);
            line = line.trimmed();
            builder.addTest(testCase, line);
        } else if (!line.startsWith('-') && !line.startsWith('#')) {
            testCase = line;
            builder.addTestCase(line);
        } else if (line.startsWith('-')) {
            ignored++;
        }
        line = stream.readLine();
    }
    tests = builder.getTests();
}

void UnitTestSuite::prepare() {
    QString dataDir = env->getVar("COMMON_DATA_DIR");
    AppContext::getAppSettings()->getTestRunnerSettings()->setVar("COMMON_DATA_DIR", dataDir);
    tests_run();
}
void UnitTestSuite::test_run(const QString& testName) {
    UnitTest* t = (UnitTest*)QMetaType::create(QMetaType::type(testName.toStdString().c_str()));
    if (t != NULL) {
        t->SetUp();
        t->Test();
        if (!t->GetError().isEmpty()){
            taskLog.error(testName + ": " + t->GetError());
            failed++;
        } else {
            taskLog.info(testName + ": OK");
            passed++;
        }
    } else {
        taskLog.info(testName + ": Ignored");
        ignored++;
    }
}

void UnitTestSuite::tests_run(){

    foreach(const QString& suite, tests.keys()) {
        QStringList testList = tests.value(suite);
        foreach(const QString& testName, testList) {
            test_run(suite + "_" + testName);
        }
    }
}

void UnitTestSuite::cleanup() {
    AppContext::getAppSettings()->getTestRunnerSettings()->removeVar("COMMON_DATA_DIR");

    AssemblyTestData::shutdown();
    AttributeTestData::shutdown();
    DNAChromatogramObjectTestData::shutdown();
    FeatureTestData::shutdown();
    FeaturesTableObjectTestData::shutdown();
    MsaObjectTestData::shutdown();
    MsaSQLiteSpecificTestData::shutdown();
    RawDataUdrSchemaTestData::shutdown();
    SequenceTestData::shutdown();
    TextObjectTestData::shutdown();
    UdrTestData::shutdown();

    if (passed){
        taskLog.info("Test passed: " + QString::number(passed));
    }
    if (ignored){
        taskLog.info("Test ignored: " + QString::number(ignored));
    }
    if (failed){
        taskLog.info("Test failed: " + QString::number(failed));
        setError("Test failed");
    }

    XmlTest::cleanup();
}

} //namespace
