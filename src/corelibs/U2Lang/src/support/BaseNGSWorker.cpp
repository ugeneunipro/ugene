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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/BAMUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "BaseNGSWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString BaseNGSWorker::INPUT_PORT = "in-file";
const QString BaseNGSWorker::OUTPUT_PORT = "out-file";
const QString BaseNGSWorker::OUT_MODE_ID = "out-mode";
const QString BaseNGSWorker::CUSTOM_DIR_ID = "custom-dir";
const QString BaseNGSWorker::OUT_NAME_ID = "out-name";
const QString BaseNGSWorker::DEFAULT_NAME = "Default";


//////////////////////////////////////////////////////////////////////////
//BaseNGSWorker
BaseNGSWorker::BaseNGSWorker(Actor *a)
:BaseWorker(a)
,inputUrlPort(NULL)
,outputUrlPort(NULL)
,outUrls("")
{

}

void BaseNGSWorker::init() {
    inputUrlPort = ports.value(INPUT_PORT);
    outputUrlPort = ports.value(OUTPUT_PORT);
}

Task * BaseNGSWorker::tick() {
    if (inputUrlPort->hasMessage()) {
        const QString url = takeUrl();
        CHECK(!url.isEmpty(), NULL);

        const QString outputDir = FileAndDirectoryUtils::createWorkingDir(url, getValue<int>(OUT_MODE_ID), getValue<QString>(CUSTOM_DIR_ID), context->workingDir());

        BaseNGSSetting setting;
        setting.outDir = outputDir;
        setting.outName = getTargetName(url, outputDir);
        setting.inputUrl = url;
        setting.customParameters = getCustomParameters();
        setting.listeners = createLogListeners();
        Task *t = getTask(setting);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return t;
    }

    if (inputUrlPort->isEnded()) {
        setDone();
        outputUrlPort->setEnded();
    }
    return NULL;
}

void BaseNGSWorker::cleanup() {
    outUrls.clear();
}

namespace {
    QString getTargetUrl(Task *task) {

        BaseNGSTask *NGSTask = dynamic_cast<BaseNGSTask*>(task);

        if (NULL != NGSTask) {
            return NGSTask->getResult();
        }
        return "";
    }
}

void BaseNGSWorker::sl_taskFinished(Task *task) {
    CHECK(!task->hasError(), );
    CHECK(!task->isCanceled(), );

    QString url = getTargetUrl(task);
    CHECK(!url.isEmpty(), );

    sendResult(url);
    monitor()->addOutputFile(url, getActorId());
}

QString BaseNGSWorker::getTargetName (const QString &fileUrl, const QString &outDir){
    QString name = getValue<QString>(OUT_NAME_ID);

    if(name == DEFAULT_NAME || name.isEmpty()){
        name = QFileInfo(fileUrl).fileName();
        name = name + getDefaultFileName();
    }
    QString rolledUrl = GUrlUtils::rollFileName(outDir + name, "_", outUrls.toSet());
    outUrls.append(rolledUrl);
    QFileInfo fi(rolledUrl);
    return fi.fileName();
}


QString BaseNGSWorker::takeUrl() {
    const Message inputMessage = getMessageAndSetupScriptValues(inputUrlPort);
    if (inputMessage.isEmpty()) {
        outputUrlPort->transit();
        return "";
    }

    const QVariantMap data = inputMessage.getData().toMap();
    return data[BaseSlots::URL_SLOT().getId()].toString();
}

void BaseNGSWorker::sendResult(const QString &url) {
    const Message message(BaseTypes::STRING_TYPE(), url);
    outputUrlPort->put(message);
}

//////////////////////////////////////////////////////////////////////////
//BaseNGSParser
BaseNGSParser::BaseNGSParser()
    :ExternalToolLogParser() {

}

void BaseNGSParser::parseOutput( const QString& partOfLog ){
    ExternalToolLogParser::parseOutput(partOfLog);
}

void BaseNGSParser::parseErrOutput( const QString& partOfLog ){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastErrLine+lastPartOfLog.first();
    lastErrLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
            if(buf.contains("ERROR", Qt::CaseInsensitive)){
                    coreLog.error("NGS: " + buf);
            }
    }
}


//////////////////////////////////////////////////////////////////////////
//BaseNGSTask
BaseNGSTask::BaseNGSTask(const BaseNGSSetting &settings)
:Task(QString("NGS for %1").arg(settings.inputUrl), TaskFlags_FOSE_COSC)
,settings(settings)
{

}

void BaseNGSTask::prepare(){
    if (settings.inputUrl.isEmpty()){
        setError(tr("No input URL"));
        return ;
    }

    if(FileAndDirectoryUtils::isFileEmpty(settings.inputUrl)){
        coreLog.info("File is empty: " + settings.inputUrl);
        return;
    }

    const QDir outDir = QFileInfo(settings.outDir).absoluteDir();
    if (!outDir.exists()) {
        setError(tr("Folder does not exist: ") + outDir.absolutePath());
        return ;
    }

    prepareStep();
}

void BaseNGSTask::run(){
    CHECK_OP(stateInfo, );
    if(FileAndDirectoryUtils::isFileEmpty(settings.inputUrl)){
        return;
    }

    runStep();

    resultUrl = settings.outDir + settings.outName;

    finishStep();
}

ExternalToolRunTask *BaseNGSTask::getExternalToolTask(const QString &toolId, ExternalToolLogParser *customParser) {
    const QStringList args = getParameters(stateInfo);
    CHECK_OP(stateInfo, NULL);

    ExternalToolRunTask* etTask = NULL;
    if (customParser == NULL) {
        etTask = new ExternalToolRunTask(toolId, args, new BaseNGSParser(), settings.outDir);
    } else {
        etTask = new ExternalToolRunTask(toolId, args, customParser, settings.outDir);
    }

    etTask->setStandartOutputFile(settings.outDir + settings.outName);
    if(!settings.listeners.isEmpty()){
        etTask->addOutputListener(settings.listeners.at(0));
    }
    return etTask;
}

} //LocalWorkflow
} //U2
