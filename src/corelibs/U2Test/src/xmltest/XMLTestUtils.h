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

#ifndef _U2_XML_TEST_UTILS_
#define _U2_XML_TEST_UTILS_

#include <U2Test/GTest.h>
#include "XMLTestFormat.h"

namespace U2 {


#define SIMPLE_XML_TEST_CONSTRUCT(ClassName, TFlags) \
    ClassName(XMLTestFormat* _tf, const QString& _name, GTest* _cp, \
        const GTestEnvironment* _env, const QList<GTest*>& _contexts, const QDomElement& _el) \
    : XmlTest(_name, _cp, _env, TFlags, _contexts){init(_tf, _el);} \


#define SIMPLE_XML_TEST_BODY(ClassName, TFlags) \
public:\
    SIMPLE_XML_TEST_CONSTRUCT(ClassName, TFlags) \
    void init(XMLTestFormat *tf, const QDomElement& el); \


#define SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(TestClass, TagName, TFlags) \
    SIMPLE_XML_TEST_BODY(TestClass, TFlags) \
    class TestClass##Factory : public XMLTestFactory { \
    public: \
        TestClass##Factory () : XMLTestFactory(TagName) {} \
        \
        virtual GTest* createTest(XMLTestFormat* tf, const QString& testName, GTest* cp, \
                    const GTestEnvironment* env, const QList<GTest*>& subtasks, const QDomElement& el) \
        { \
            return new TestClass(tf, testName, cp, env, subtasks, el); \
        }\
    };\
    \
    static XMLTestFactory* createFactory() {return new TestClass##Factory();}\


#define SIMPLE_XML_TEST_BODY_WITH_FACTORY(TestClass, TagName) \
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(TestClass, TagName, TaskFlags_NR_FOSCOE) \


class U2TEST_EXPORT XmlTest : public GTest {
public:
    XmlTest(const QString &taskName,
            GTest *cp,
            const GTestEnvironment *env,
            TaskFlags flags, const QList<GTest *> &subtasks = QList<GTest*>());

    void checkNecessaryAttributeExistence(const QDomElement &element, const QString &attribute);
    void checkAttribute(const QDomElement &element, const QString &attribute, const QStringList &acceptableValues, bool isNecessary);
    void checkBooleanAttribute(const QDomElement &element, const QString &attribute, bool isNecessary);
    int getInt(const QDomElement &element, const QString &attribute);
    qint64 getInt64(const QDomElement &element, const QString &attribute);
    double getDouble(const QDomElement &element, const QString &attribute);

    static const QString TRUE_VALUE;
    static const QString FALSE_VALUE;
};

class U2TEST_EXPORT XMLTestUtils {
public:
    static QList<XMLTestFactory*> createTestFactories();
    static void replacePrefix(const GTestEnvironment* env, QString &path);
    static bool parentTasksHaveError(Task* t);

    static const QString TMP_DATA_DIR_PREFIX;
    static const QString COMMON_DATA_DIR_PREFIX;
    static const QString LOCAL_DATA_DIR_PREFIX;
    static const QString SAMPLE_DATA_DIR_PREFIX;
    static const QString WORKFLOW_SAMPLES_DIR_PREFIX;
    static const QString WORKFLOW_OUTPUT_DIR_PREFIX;
    static const QString EXPECTED_OUTPUT_DIR_PREFIX;

    static const QString CONFIG_FILE_PATH;
};


//////////////////////////////////////////////////////////////////////////
// utility tasks

class XMLMultiTest : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(XMLMultiTest, "multi-test")
    ReportResult report();

    static const QString FAIL_ON_SUBTEST_FAIL;      // it defines whether the test should stop execution after the first error; is "true" by default
    static const QString LOCK_FOR_LOG_LISTENING;      // This attribute is used to avoid mixing log messages between different tests. Each test that listens to log should set this attribute to "true"
};

class GTest_Fail : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Fail, "fail", TaskFlag_NoRun)
    ReportResult report();
private:
    QString msg;
};

class GTest_DeleteTmpFile : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_DeleteTmpFile, "delete", TaskFlag_NoRun)
    ReportResult report();
private:
    QString url;
};

class GTest_CreateTmpFolder: public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CreateTmpFolder, "create-folder", TaskFlag_NoRun)
    ReportResult report();
private:
    QString url;
};


} //namespace

#endif

