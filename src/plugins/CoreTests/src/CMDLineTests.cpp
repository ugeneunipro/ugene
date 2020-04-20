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

#include <QApplication>
#include <QCoreApplication>
#include <QDomElement>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>

#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "CMDLineTests.h"

namespace U2 {

#define COMMON_DATA_DIR_ENV_ID "COMMON_DATA_DIR"
#define LOCAL_DATA_DIR_ENV_ID "LOCAL_DATA_DIR"
#define SAMPLE_DATA_DIR_ENV_ID "SAMPLE_DATA_DIR"
#define WORKFLOW_SAMPLES_ENV_ID "WORKFLOW_SAMPLES_DIR"
#define WORKFLOW_OUTPUT_ENV_ID "WORKFLOW_OUTPUT_DIR"
#define TEMP_DATA_DIR_ENV_ID   "TEMP_DATA_DIR"
#define CONFIG_FILE_ENV_ID "CONFIG_FILE"
#define CONFIG_PROTOTYPE "PROTOTYPE"
#define WORKING_DIR_ATTR "working-dir"

/************************
 * GTest_RunCMDLine
 ************************/
#ifndef _DEBUG
const QString GTest_RunCMDLine::UGENECL_PATH    = "/ugenecl";
#else
const QString GTest_RunCMDLine::UGENECL_PATH    = "/ugenecld";
#endif // _DEBUG

void GTest_RunCMDLine::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    customIniSet = false;
    setUgeneclPath();
    setArgs(el);
    proc = new QProcess(this);
    if (el.hasAttribute(WORKING_DIR_ATTR)) {
        workingDir = el.attribute(WORKING_DIR_ATTR);
        XMLTestUtils::replacePrefix(env, workingDir);
        if (QUrl(workingDir).isRelative()) {
            workingDir = env->getVar(TEMP_DATA_DIR_ENV_ID) + "/" + workingDir;
        }
    }

    autoRemoveWorkingDir = false;
    if (workingDir.isEmpty()) {
        workingDir = GUrlUtils::rollFileName(env->getVar(TEMP_DATA_DIR_ENV_ID) + "/workingDir", "_");
        autoRemoveWorkingDir = true;
        taskLog.trace(QString("Working dir is not defined, the foolowing dir will be used as working: %1").arg(workingDir));
    }

    QString protosPath = env->getVar(COMMON_DATA_DIR_ENV_ID) + "/" +  env->getVar(CONFIG_PROTOTYPE);
    QDir protoDir(protosPath), userScriptsDir(WorkflowSettings::getUserDirectory());
    QStringList filters;
    filters << "*.usa";
    protoDir.setNameFilters(filters);

    QFileInfoList list = protoDir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fIdest = list.at(i);
        QFileInfo fItarget(userScriptsDir.path() + "/" + fIdest.fileName());
        if(!fItarget.exists()){
            QFile::copy(fIdest.absoluteFilePath(), fItarget.absoluteFilePath());
        }else if(fIdest.size() != fItarget.size()){
            QFile::copy(fIdest.absoluteFilePath(), fItarget.absoluteFilePath());
        }
    }
}

void GTest_RunCMDLine::setArgs(const QDomElement & el) {
    QString commandLine;
    QDomNamedNodeMap map = el.attributes();
    int mapSz = map.length();
    for( int i = 0; i < mapSz; ++i ) {
        QDomNode node = map.item(i);
        if(node.nodeName() == "message"){
            expectedMessage = node.nodeValue();
            continue;
        }
        if(node.nodeName() == "nomessage"){
            unexpectedMessage = node.nodeValue();
            continue;
        }
        if(node.nodeName() == WORKING_DIR_ATTR){
            continue;
        }
        if (node.nodeName() == CMDLineCoreOptions::INI_FILE) {
            customIniSet = true;
        }

        QString argument = "--" + node.nodeName() + "=" + getVal(node.nodeValue());
         if( argument.startsWith("--task") ) {
            args.prepend(argument);
            commandLine.prepend(argument + " ");
        } else {
            args.append(argument);
            commandLine.append(argument + " ");
        }
    }

    if (!customIniSet) {
        args.append("--" + CMDLineCoreOptions::INI_FILE + "=" + AppContext::getSettings()->fileName());
    }

    args.append("--log-level-details");
    args.append("--lang=en");
    args.append("--log-no-task-progress");
    commandLine.append(QString(" --log-level-details --lang=en --log-no-task-progress"));
    cmdLog.info(commandLine);
}

QString GTest_RunCMDLine::splitVal(const QString & val, QString prefValue, const QString & prefix, bool isTmp) {
    int midSize = prefValue.size();
    const QString splitter = WorkflowUtils::getDatasetSplitter(val);
    QStringList dsVals = val.split(splitter + splitter);
    QStringList result;
    foreach (const QString &dsVal, dsVals) {
        QStringList realVals = dsVal.split(splitter);
        QStringList dsResult;
        foreach(QString s, realVals) {
            if(s.startsWith(prefValue)){
                s = s.mid(midSize);
            }
            QString filename = prefix + s;
            dsResult << filename;
            if(isTmp) {
                tmpFiles << filename;
            }
        }
        result << dsResult.join(";");
    }
    return result.join(";;");
}

QString GTest_RunCMDLine::getVal( const QString & val ) {
    if (val.isEmpty()) {
        return val;
    }
    if (val.startsWith(XMLTestUtils::COMMON_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::COMMON_DATA_DIR_PREFIX, env->getVar(COMMON_DATA_DIR_ENV_ID) + "/", false);
    }
    if (val.startsWith(XMLTestUtils::TMP_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::TMP_DATA_DIR_PREFIX, env->getVar(TEMP_DATA_DIR_ENV_ID) + "/", true);
    }
    if (val == XMLTestUtils::CONFIG_FILE_PATH) {
        return env->getVar(COMMON_DATA_DIR_ENV_ID) + "/" + env->getVar(CONFIG_FILE_ENV_ID);
    }
    if (val.startsWith(XMLTestUtils::LOCAL_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::LOCAL_DATA_DIR_PREFIX, env->getVar(LOCAL_DATA_DIR_ENV_ID), false);
    }
    if (val.startsWith(XMLTestUtils::SAMPLE_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::SAMPLE_DATA_DIR_PREFIX, env->getVar(SAMPLE_DATA_DIR_ENV_ID), false);
    }
    if (val.startsWith(XMLTestUtils::WORKFLOW_SAMPLES_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::WORKFLOW_SAMPLES_DIR_PREFIX, env->getVar(WORKFLOW_SAMPLES_ENV_ID), false);
    }
    if (val.startsWith(XMLTestUtils::WORKFLOW_OUTPUT_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::WORKFLOW_OUTPUT_DIR_PREFIX, env->getVar(WORKFLOW_OUTPUT_ENV_ID) + "/", false);
    }
    return val;
}

void GTest_RunCMDLine::setUgeneclPath() {
    ugeneclPath = AppContext::getMainWindow() ? QApplication::applicationDirPath() : QCoreApplication::applicationDirPath();
    assert(!ugeneclPath.isEmpty());
    ugeneclPath += UGENECL_PATH;
}

void GTest_RunCMDLine::prepare() {
    if (!workingDir.isEmpty()) {
        QDir().mkpath(workingDir);
        proc->setWorkingDirectory(workingDir);
    }

    QString argsStr = args.join(" ");
    coreLog.trace( "Starting UGENE with arguments: " + argsStr );
    proc->start( ugeneclPath, args );
}

static const QString ERROR_LABEL_TRY1 = "finished with error";
static QString getErrorMsg(const QString & str) {
    int ind = str.indexOf(ERROR_LABEL_TRY1);
    if (ind != -1) {
        return str.mid(ind + ERROR_LABEL_TRY1.size());
    }
    return QString();
}

Task::ReportResult GTest_RunCMDLine::report() {
    if( hasError() || isCanceled() ) {
        return ReportResult_Finished;
    }
    assert( proc != NULL );
    if( proc->state() != QProcess::NotRunning ) {
        return ReportResult_CallMeAgain;
    }
    //QProcess::ProcessError err = proc->error();
    QString output(proc->readAllStandardOutput());
    //QByteArray outputErr = proc->readAllStandardError();
    cmdLog.trace(output);

    if(!expectedMessage.isEmpty()){
        cmdLog.error(output);
        if(!output.contains(expectedMessage, Qt::CaseSensitive)){
            stateInfo.setError(QString("Expected message not found in output"));
        }
        return ReportResult_Finished;
    }
    if(!unexpectedMessage.isEmpty()){
        if(output.contains(unexpectedMessage, Qt::CaseSensitive)){
            stateInfo.setError(QString("Unexpected message is found in output"));
        }
        return ReportResult_Finished;
    }

    QString err = getErrorMsg(output);
    if (!err.isEmpty()) {
        int eofIdx = err.indexOf("\n");
        if (eofIdx > 0) {
            err = err.left(eofIdx - 1);
        }
        setError("Process finished with error" + err);
    }

    if (proc->exitStatus() == QProcess::CrashExit) {
        setError("Process is crashed!");
    }

    return ReportResult_Finished;
}

void GTest_RunCMDLine::cleanup() {
    if (!XMLTestUtils::parentTasksHaveError(this)) {
        foreach(const QString & file, tmpFiles) {
            taskLog.trace(QString("Temporary file removed: %1").arg(file));
            QFile::remove(file);
        }

        if (autoRemoveWorkingDir) {
            taskLog.trace(QString("Temporary working dir autoremoved: %1").arg(workingDir));
            QDir(workingDir).removeRecursively();
        }
    }

    XmlTest::cleanup();
}

/************************
* GTest_RunCMDLine
************************/
QList<XMLTestFactory*> CMDLineTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_RunCMDLine::createFactory());
    return res;
}

} // U2
