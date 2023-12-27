/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "XMLTestUtils.h"

#include <QDir>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>

#include <U2Core/AppResources.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString XmlTest::TRUE_VALUE = "true";
const QString XmlTest::FALSE_VALUE = "false";

XmlTest::XmlTest(const QString& taskName, GTest* cp, const GTestEnvironment* env, TaskFlags flags, const QList<GTest*>& subtasks)
    : GTest(taskName, cp, env, flags, subtasks) {
}

void XmlTest::checkNecessaryAttributeExistence(const QDomElement& element, const QString& attribute) {
    CHECK_EXT(element.hasAttribute(attribute), failMissingValue(attribute), );
}

void XmlTest::checkAttribute(const QDomElement& element, const QString& attribute, const QStringList& acceptableValues, bool isNecessary) {
    if (isNecessary) {
        checkNecessaryAttributeExistence(element, attribute);
        CHECK_OP(stateInfo, );
    }

    CHECK_EXT(!element.hasAttribute(attribute) || acceptableValues.contains(element.attribute(attribute)),
              setError(QString("Attribute '%1' has unacceptable value. Acceptable values are: %2")
                           .arg(attribute)
                           .arg(acceptableValues.join(", "))), );
}

void XmlTest::checkBooleanAttribute(const QDomElement& element, const QString& attribute, bool isNecessary) {
    checkAttribute(element, attribute, QStringList({TRUE_VALUE, FALSE_VALUE}), isNecessary);
}

int XmlTest::getInt(const QDomElement& element, const QString& attribute) {
    checkNecessaryAttributeExistence(element, attribute);
    CHECK_OP(stateInfo, 0);

    bool success = false;
    const int result = element.attribute(attribute).toInt(&success);
    CHECK_EXT(success, wrongValue(attribute), 0);
    return result;
}

qint64 XmlTest::getInt64(const QDomElement& element, const QString& attribute) {
    checkNecessaryAttributeExistence(element, attribute);
    CHECK_OP(stateInfo, 0);

    bool success = false;
    const qint64 result = element.attribute(attribute).toLongLong(&success);
    CHECK_EXT(success, wrongValue(attribute), 0);
    return result;
}

double XmlTest::getDouble(const QDomElement& element, const QString& attribute) {
    checkNecessaryAttributeExistence(element, attribute);
    CHECK_OP(stateInfo, 0);

    bool success = false;
    const double result = element.attribute(attribute).toDouble(&success);
    CHECK_EXT(success, wrongValue(attribute), 0);
    return result;
}

const QString XMLTestUtils::TMP_DATA_DIR_PREFIX = "!tmp_data_dir!";
const QString XMLTestUtils::COMMON_DATA_DIR_PREFIX = "!common_data_dir!";
const QString XMLTestUtils::LOCAL_DATA_DIR_PREFIX = "!input!";
const QString XMLTestUtils::SAMPLE_DATA_DIR_PREFIX = "!sample_data_dir!";
const QString XMLTestUtils::WORKFLOW_SAMPLES_DIR_PREFIX = "!workflow_samples!";
const QString XMLTestUtils::WORKFLOW_OUTPUT_DIR_PREFIX = "!workflow_output!";
const QString XMLTestUtils::EXPECTED_OUTPUT_DIR_PREFIX = "!expected!";

const QString XMLTestUtils::CONFIG_FILE_PATH = "!config_file_path!";

QList<XMLTestFactory*> XMLTestUtils::createTestFactories() {
    QList<XMLTestFactory*> res;

    res.append(XMLMultiTest::createFactory());
    res.append(GTest_DeleteTmpFile::createFactory());
    res.append(GTest_Fail::createFactory());
    res.append(GTest_CreateTmpFolder::createFactory());

    return res;
}

void XMLTestUtils::replacePrefix(const GTestEnvironment* env, QString& path) {
    QString result;

    // Determine which environment variable is required
    QString envVarName;
    QString prefix;
    if (path.startsWith(EXPECTED_OUTPUT_DIR_PREFIX)) {
        envVarName = "EXPECTED_OUTPUT_DIR";
        prefix = EXPECTED_OUTPUT_DIR_PREFIX;
    } else if (path.startsWith(TMP_DATA_DIR_PREFIX)) {
        envVarName = "TEMP_DATA_DIR";
        prefix = TMP_DATA_DIR_PREFIX;
    } else if (path.startsWith(COMMON_DATA_DIR_PREFIX)) {
        envVarName = "COMMON_DATA_DIR";
        prefix = COMMON_DATA_DIR_PREFIX;
    } else if (path.startsWith(WORKFLOW_OUTPUT_DIR_PREFIX)) {
        envVarName = "WORKFLOW_OUTPUT_DIR";
        prefix = WORKFLOW_OUTPUT_DIR_PREFIX;
    } else {
        return;
    }

    // Replace with the correct value
    QString prefixPath = env->getVar(envVarName);
    SAFE_POINT(!prefixPath.isEmpty(), QString("No value for environment variable '%1'!").arg(envVarName), );
    prefixPath += "/";

    int prefixSize = prefix.size();
    QStringList relativePaths = path.mid(prefixSize).split(";");

    for (const QString& relativePath : qAsConst(relativePaths)) {
        QString fullPath = prefixPath + relativePath;
        result += fullPath + ";";
    }

    path = result.mid(0, result.size() - 1);  // without the last ';'
}

bool XMLTestUtils::parentTasksHaveError(Task* t) {
    Task* parentTask = t->getParentTask();
    CHECK(parentTask != nullptr, false);
    return parentTask->hasError() || parentTasksHaveError(parentTask);
}

const QString XMLMultiTest::FAIL_ON_SUBTEST_FAIL = "fail-on-subtest-fail";

/**
 * Returns 'true' if the whole test must be executed under 'lock-log'.
 * This lock is needed if any element of the test checks log.
 */
static bool checkIfLogLockIsNeeded(const QDomElement& multiTestElement) {
    QList<QString> logMessageAttributePrefixes = {"message", "no-message"};

    QDomNodeList childNodes = multiTestElement.childNodes();
    for (int i = 0; i < childNodes.length(); i++) {
        QDomNode node = childNodes.at(i);
        if (node.isElement()) {
            QDomNamedNodeMap attributes = node.attributes();
            for (int j = 0; j < attributes.length(); j++) {
                QDomNode attribute = attributes.item(j);
                QString attributeName = attribute.nodeName();
                for (const QString& logPrefix : qAsConst(logMessageAttributePrefixes)) {
                    if (attributeName.startsWith(logPrefix)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void XMLMultiTest::init(XMLTestFormat* tf, const QDomElement& el) {
    bool lockForLogListening = checkIfLogLockIsNeeded(el);

    checkAttribute(el, FAIL_ON_SUBTEST_FAIL, {"true", "false"}, false);
    CHECK_OP(stateInfo, );

    if ("false" == el.attribute(FAIL_ON_SUBTEST_FAIL, "true")) {
        setFlag(TaskFlag_FailOnSubtaskError, false);
    }

    QDomNodeList subtaskNodes = el.childNodes();
    QList<Task*> subs;
    for (int i = 0; i < subtaskNodes.size(); i++) {
        QDomNode n = subtaskNodes.item(i);
        if (!n.isElement()) {
            continue;
        }
        QDomElement subEl = n.toElement();
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
        auto lockTime = lockForLogListening ? AppResourceReadWriteLock::Write : AppResourceReadWriteLock::Read;
        addTaskResource(TaskResourceUsage(UGENE_RESOURCE_ID_TEST_LOG_LISTENER, lockTime, TaskResourceStage::Prepare));
        for (Task* t : qAsConst(subs)) {
            addSubTask(t);
        }
    }
}

Task::ReportResult XMLMultiTest::report() {
    if (!hasError()) {
        Task* t = getSubtaskWithErrors();
        if (t != nullptr) {
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
    } else {
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

}  // namespace U2
