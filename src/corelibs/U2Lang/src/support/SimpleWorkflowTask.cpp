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

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/WorkflowRunTask.h>

#include "SimpleWorkflowTask.h"

namespace U2 {

using namespace Workflow;

SimpleInOutWorkflowTaskConfig::SimpleInOutWorkflowTaskConfig()
        : emptyResultPossible(false) {
}

/***************************
 * WorkflowRunSchemaForTask
 ***************************/
static QString SCHEMA_DIR_PATH = QString("%1:cmdline/").arg(PATH_PREFIX_DATA);

static QString findWorkflowPath(const QString& schemaName) {
    foreach(const QString& ext, WorkflowUtils::WD_FILE_EXTENSIONS) {
        QString candidate = SCHEMA_DIR_PATH + schemaName + "." + ext;
        if (QFile::exists(candidate)) {
            return candidate;
        }
    }
    return QString();
}

SimpleInOutWorkflowTask::SimpleInOutWorkflowTask(const SimpleInOutWorkflowTaskConfig& _conf)
        : DocumentProviderTask(tr("Run workflow: %1").arg(_conf.schemaName), TaskFlags_NR_FOSCOE), conf(_conf) {
    inDoc = new Document(BaseDocumentFormats::get(conf.inFormat), IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE),
                         GUrl("unused"), U2DbiRef(), conf.objects, conf.inDocHints);
    inDoc->setParent(this);
}

void SimpleInOutWorkflowTask::prepareTmpFile(QTemporaryFile& tmpFile, const QString& tmpl) {
    tmpFile.setFileTemplate(tmpl);
    if (!tmpFile.open()) {
        setError(tr("Cannot create temporary file for writing"));
        return;
    }
#ifdef _DEBUG
    tmpFile.setAutoRemove(false);
#endif
    tmpFile.close();
}

void SimpleInOutWorkflowTask::prepare() {
    prepareTmpFile(inputTmpFile, QString("%1/XXXXXX.%2").arg(QDir::tempPath()).arg(conf.inFormat));
    CHECK_OP(stateInfo,);

    prepareTmpFile(resultTmpFile, QString("%1/XXXXXX.%2").arg(QDir::tempPath()).arg(conf.outFormat));
    CHECK_OP(stateInfo,);

    schemaPath = findWorkflowPath(conf.schemaName);
    CHECK_EXT(!schemaPath.isEmpty(), setError(tr("Internal error: cannot find workflow %1").arg(conf.schemaName)),);

    saveInputTask = new SaveDocumentTask(inDoc, IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), inputTmpFile.fileName());
    addSubTask(saveInputTask);
}

QList<Task*> SimpleInOutWorkflowTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    if (subTask == saveInputTask) {
        // run workflow
        conf.extraArgs << "--in=" + inputTmpFile.fileName();
        conf.extraArgs << "--out=" + resultTmpFile.fileName();
        conf.extraArgs << "--format=" + conf.outFormat;

        CmdlineTaskConfig monitorConf;
        monitorConf.command = "--task=" + schemaPath;
        monitorConf.arguments = conf.extraArgs;
#ifdef _DEBUG
        monitorConf.logLevel = LogLevel_TRACE;
#else
        monitorConf.logLevel = LogLevel_DETAILS;
#endif
        runWorkflowTask = new CmdlineTaskRunner(monitorConf);
        res << runWorkflowTask;
    } else if (subTask == runWorkflowTask) {
        if (QFileInfo(resultTmpFile.fileName()).size() == 0) {
            if (!conf.emptyResultPossible) {
                setError(tr("An error occurred during the task. See the log for details."));
            }
            return res;
        }
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        ioLog.details(tr("Loading result file '%1'").arg(resultTmpFile.fileName()));
        loadResultTask = new LoadDocumentTask(conf.outFormat, resultTmpFile.fileName(), iof, conf.outDocHints);
        res << loadResultTask;
    } else {
        assert(subTask == loadResultTask);
        resultDocument = loadResultTask->takeDocument();
    }

    return res;
}

//////////////////////////////////////////////////////////////////////////
// RunSimpleMSAWorkflow4GObject
SimpleMSAWorkflow4GObjectTask::SimpleMSAWorkflow4GObjectTask(const QString& taskName,
                                                             MultipleSequenceAlignmentObject* msaObj,
                                                             const SimpleMSAWorkflowTaskConfig& conf)
        : Task(taskName, TaskFlags_NR_FOSCOE),
          msaObjectPointer(msaObj),
          conf(conf) {
    SAFE_POINT(msaObj != nullptr, "NULL MultipleSequenceAlignmentObject!",);

    U2OpStatus2Log os;
    MultipleSequenceAlignment al = MSAUtils::setUniqueRowNames(msaObjectPointer->getMultipleAlignment());

    MultipleSequenceAlignmentObject* msaObject = MultipleSequenceAlignmentImporter::createAlignment(msaObjectPointer->getEntityRef().dbiRef, al, os);
    SAFE_POINT_OP(os,);

    SimpleInOutWorkflowTaskConfig sioConf;
    sioConf.objects << msaObject;
    sioConf.inFormat = BaseDocumentFormats::FASTA;
    sioConf.outFormat = BaseDocumentFormats::FASTA;
    sioConf.outDocHints = conf.resultDocHints;
    sioConf.outDocHints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    sioConf.extraArgs = conf.schemaArgs;
    sioConf.schemaName = conf.schemaName;

    runWorkflowTask = new SimpleInOutWorkflowTask(sioConf);
    addSubTask(runWorkflowTask);

    setUseDescriptionFromSubtask(true);
    setVerboseLogMode(true);
    docName = msaObjectPointer->getDocument()->getName();
}

void SimpleMSAWorkflow4GObjectTask::prepare() {
    CHECK_EXT(!msaObjectPointer.isNull(), setError(tr("Object '%1' removed").arg(docName)),);
}

Task::ReportResult SimpleMSAWorkflow4GObjectTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);
    CHECK_EXT(!msaObjectPointer.isNull(), setError(tr("Object '%1' removed").arg(docName)), ReportResult_Finished);
    CHECK_EXT(!msaObjectPointer->isStateLocked(), setError(tr("Object '%1' is locked").arg(docName)), ReportResult_Finished);

    MultipleSequenceAlignment resultMsa = getResult();
    const MultipleSequenceAlignment& originalMsa = msaObjectPointer->getMultipleAlignment();
    bool isAllRowsRestored = MSAUtils::restoreRowNames(resultMsa, originalMsa->getRowNames());
    if (!isAllRowsRestored) {
        setError(tr("MSA has incompatible changes during the alignment. Ignoring the alignment result: '%1'").arg(docName));
        return ReportResult_Finished;
    }
    resultMsa->setName(originalMsa->getName());

    U2OpStatus2Log os;
    if (resultMsa != originalMsa) {
        U2UseCommonUserModStep userModStep(msaObjectPointer->getEntityRef(), os);
        msaObjectPointer->setMultipleAlignment(resultMsa);
    }

    return ReportResult_Finished;
}

MultipleSequenceAlignment SimpleMSAWorkflow4GObjectTask::getResult() {
    MultipleSequenceAlignment res;
    CHECK_OP(stateInfo, res);

    SAFE_POINT(runWorkflowTask != NULL, "SimpleMSAWorkflow4GObjectTask::getResult. No task has been created.", res);

    Document* d = runWorkflowTask->getDocument();
    CHECK_EXT(d != NULL, setError(tr("Result document not found!")), res);
    CHECK_EXT(d->getObjects().size() == 1, setError(tr("Result document content not matched! %1").arg(d->getURLString())), res);

    MultipleSequenceAlignmentObject* maObj = qobject_cast<MultipleSequenceAlignmentObject*>(d->getObjects().first());
    CHECK_EXT(maObj != NULL, setError(tr("Result document contains no MSA! %1").arg(d->getURLString())), res);
    return maObj->getMsaCopy();
}

}    // namespace U2
