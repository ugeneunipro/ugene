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

#include <QDir>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>

#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include "XMLTestUtils.h"

namespace U2 {

const QString XmlTest::TRUE_VALUE = "true";
const QString XmlTest::FALSE_VALUE = "false";

XmlTest::XmlTest(const QString &taskName, GTest *cp, const GTestEnvironment *env, TaskFlags flags, const QList<GTest *> &subtasks)
    : GTest(taskName, cp, env, flags, subtasks)
{

}

void XmlTest::checkNecessaryAttributeExistence(const QDomElement &element, const QString &attribute) {
    CHECK_EXT(element.hasAttribute(attribute), failMissingValue(attribute), );
}

void XmlTest::checkAttribute(const QDomElement &element, const QString &attribute, const QStringList &acceptableValues, bool isNecessary) {
    if (isNecessary) {
        checkNecessaryAttributeExistence(element, attribute);
        CHECK_OP(stateInfo, );
    }

    CHECK_EXT(!element.hasAttribute(attribute) || acceptableValues.contains(element.attribute(attribute)),
              setError(QString("Attribute '%1' has inacceptable value. Acceptable values are: %2")
                       .arg(attribute).arg(acceptableValues.join(", "))), );
}

void XmlTest::checkBooleanAttribute(const QDomElement &element, const QString &attribute, bool isNecessary) {
    checkAttribute(element, attribute, QStringList( { TRUE_VALUE, FALSE_VALUE } ), isNecessary);
}

int XmlTest::getInt(const QDomElement &element, const QString &attribute) {
    checkNecessaryAttributeExistence(element, attribute);
    CHECK_OP(stateInfo, 0);

    bool success = false;
    const int result = element.attribute(attribute).toInt(&success);
    CHECK_EXT(success, wrongValue(attribute), 0);
    return result;
}

qint64 XmlTest::getInt64(const QDomElement &element, const QString &attribute) {
    checkNecessaryAttributeExistence(element, attribute);
    CHECK_OP(stateInfo, 0);

    bool success = false;
    const qint64 result = element.attribute(attribute).toLongLong(&success);
    CHECK_EXT(success, wrongValue(attribute), 0);
    return result;
}

double XmlTest::getDouble(const QDomElement &element, const QString &attribute) {
    checkNecessaryAttributeExistence(element, attribute);
    CHECK_OP(stateInfo, 0);

    bool success = false;
    const double result = element.attribute(attribute).toDouble(&success);
    CHECK_EXT(success, wrongValue(attribute), 0);
    return result;
}

const QString XMLTestUtils::TMP_DATA_DIR_PREFIX  = "!tmp_data_dir!";
const QString XMLTestUtils::COMMON_DATA_DIR_PREFIX = "!common_data_dir!";
const QString XMLTestUtils::LOCAL_DATA_DIR_PREFIX = "!input!";
const QString XMLTestUtils::SAMPLE_DATA_DIR_PREFIX = "!sample_data_dir!";
const QString XMLTestUtils::WORKFLOW_SAMPLES_DIR_PREFIX = "!workflow_samples!";
const QString XMLTestUtils::WORKFLOW_OUTPUT_DIR_PREFIX = "!workflow_output!";
const QString XMLTestUtils::EXPECTED_OUTPUT_DIR_PREFIX = "!expected!";

const QString XMLTestUtils::CONFIG_FILE_PATH = "!config_file_path!";

QList<XMLTestFactory*>  XMLTestUtils::createTestFactories() {
    QList<XMLTestFactory*> res;

    res.append(XMLMultiTest::createFactory());
    res.append(GTest_DeleteTmpFile::createFactory());
    res.append(GTest_Fail::createFactory());
    res.append(GTest_CreateTmpFolder::createFactory());

    return res;
}

void XMLTestUtils::replacePrefix(const GTestEnvironment *env, QString &path){
    QString result;

    // Determine which environment variable is required
    QString envVarName;
    QString prefix;
    if (path.startsWith(EXPECTED_OUTPUT_DIR_PREFIX)) {
        envVarName = "EXPECTED_OUTPUT_DIR";
        prefix = EXPECTED_OUTPUT_DIR_PREFIX;
    }
    else if (path.startsWith(TMP_DATA_DIR_PREFIX)) {
        envVarName = "TEMP_DATA_DIR";
        prefix = TMP_DATA_DIR_PREFIX;
    }
    else if (path.startsWith(COMMON_DATA_DIR_PREFIX)) {
        envVarName = "COMMON_DATA_DIR";
        prefix = COMMON_DATA_DIR_PREFIX;
    }
    else if (path.startsWith(WORKFLOW_OUTPUT_DIR_PREFIX)) {
        envVarName = "WORKFLOW_OUTPUT_DIR";
        prefix = WORKFLOW_OUTPUT_DIR_PREFIX;
    }
    else {
        algoLog.details(QString("There are no known prefixes in the path: '%1', the path was not modified").arg(path));
        return;
    }

    // Replace with the correct value
    QString prefixPath = env->getVar(envVarName);
    SAFE_POINT(!prefixPath.isEmpty(), QString("No value for environment variable '%1'!").arg(envVarName), );
    prefixPath += "/";

    int prefixSize = prefix.size();
    QStringList relativePaths = path.mid(prefixSize).split(";");

    foreach (const QString &path, relativePaths) {
        QString fullPath = prefixPath + path;
        result += fullPath + ";";
    }

    path = result.mid(0, result.size() - 1); // without the last ';'
}

bool XMLTestUtils::parentTasksHaveError(Task* t) {
    Task* parentTask = t->getParentTask();
    CHECK(nullptr != parentTask, false);

    bool result = false;
    if (parentTask->hasError()) {
        result = true;
    } else {
        result = parentTasksHaveError(parentTask);
    }
    return result;
}

const QString XMLMultiTest::FAIL_ON_SUBTEST_FAIL = "fail-on-subtest-fail";
const QString XMLMultiTest::LOCK_FOR_LOG_LISTENING = "lockForLogListening";

void XMLMultiTest::init(XMLTestFormat *tf, const QDomElement& el) {

    // This attribute is used to avoid mixing log messages between different tests
    // Each test that listens to log should set this attribute to "true"
    // See also: GTestLogHelper
    checkAttribute(el, LOCK_FOR_LOG_LISTENING, { "true", "false" }, false);
    CHECK_OP(stateInfo, );

    bool lockForLogListening = false;
    if ("true" == el.attribute(LOCK_FOR_LOG_LISTENING)) {
        lockForLogListening = true;
    }

    checkAttribute(el, FAIL_ON_SUBTEST_FAIL, { "true", "false" }, false);
    CHECK_OP(stateInfo, );

    if ("false" == el.attribute(FAIL_ON_SUBTEST_FAIL, "true")) {
        setFlag(TaskFlag_FailOnSubtaskError, false);
    }

    QDomNodeList subtaskNodes = el.childNodes();
    QList<Task*> subs;
    for(int i=0;i<subtaskNodes.size(); i++) {
        QDomNode n = subtaskNodes.item(i);
        if (!n.isElement()) {
            continue;
        }
        QDomElement subEl= n.toElement();
        QString name = subEl.tagName();
        QString err;
        GTest* subTest = tf->createTest(name, this, env, subEl, err);
        if (!err.isEmpty()) {
            stateInfo.setError(err);
            break;
        }
        assert(subTest);

        subs.append(subTest);
    }
    if (!hasError()) {
        if (lockForLogListening) {
            addTaskResource(TaskResourceUsage(RESOURCE_LISTEN_LOG_IN_TESTS, TaskResourceUsage::Write, true));
        }
        else {
            addTaskResource(TaskResourceUsage(RESOURCE_LISTEN_LOG_IN_TESTS, TaskResourceUsage::Read, true));
        }

        foreach(Task* t, subs) {
            addSubTask(t);
        }
    }
}


Task::ReportResult XMLMultiTest::report() {
    if (!hasError()) {
        Task* t = getSubtaskWithErrors();
        if (t!=NULL) {
            stateInfo.setError(t->getError());
        }
    }
    return ReportResult_Finished;
}

void GTest_Fail::init(XMLTestFormat*, const QDomElement& el) {
    msg = el.attribute("msg");
}

Task::ReportResult GTest_Fail::report() {
    stateInfo.setError(QString("Test failed: %1").arg(msg));
    return ReportResult_Finished;
}

void GTest_DeleteTmpFile::init(XMLTestFormat*, const QDomElement& el) {
    url = el.attribute("file");
    if (url.isEmpty()) {
        failMissingValue("url");
        return;
    }
    url = env->getVar("TEMP_DATA_DIR") + "/" + url;
}

Task::ReportResult GTest_DeleteTmpFile::report() {
    if (!QFileInfo(url).isDir()) {
        QFile::remove(url);
    } else{
        GUrlUtils::removeDir(url, stateInfo);
    }
    return ReportResult_Finished;
}

void GTest_CreateTmpFolder::init(XMLTestFormat*, const QDomElement& el) {
    url = el.attribute("folder");
    if (url.isEmpty()) {
        failMissingValue("folder");
        return;
    }
    url = env->getVar("TEMP_DATA_DIR") + "/" + url;

}

Task::ReportResult GTest_CreateTmpFolder::report() {
    if (!QDir(url).exists()) {
        bool ok = QDir::root().mkpath(url);
        if (!ok) {
            stateInfo.setError(QString("Cannot create folder: %1").arg(url));
        }
    }
    return ReportResult_Finished;
}

}//namespace
