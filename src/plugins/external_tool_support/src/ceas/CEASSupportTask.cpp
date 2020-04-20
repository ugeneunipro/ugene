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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/FailTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Lang/DbiDataStorage.h>

#include "CEASSupport.h"
#include "CEASSupportTask.h"
#include "R/RSupport.h"

namespace U2 {

/************************************************************************/
/* Settings */
/************************************************************************/
const QString CEASTaskSettings::PDF_FORMAT("PDF");
const QString CEASTaskSettings::PNG_FORMAT("PNG");

CEASTaskSettings::CEASTaskSettings() :
    storage(NULL)
{
}

CEASTaskSettings::CEASTaskSettings(const CEASSettings &_ceas, Workflow::DbiDataStorage *storage, const QList<Workflow::SharedDbiDataHandler> &_bedData, const QString &_wigData)
: ceas(_ceas), storage(storage), bedData(_bedData), wigData(_wigData)
{

}

CEASSettings & CEASTaskSettings::getCeasSettings() {
    return ceas;
}

const CEASSettings & CEASTaskSettings::getCeasSettings() const {
    return ceas;
}

Workflow::DbiDataStorage *CEASTaskSettings::getStorage() const {
    return storage;
}

const QList<Workflow::SharedDbiDataHandler> &CEASTaskSettings::getBedData() const {
    return bedData;
}

const QString & CEASTaskSettings::getWigData() const {
    return wigData;
}

/************************************************************************/
/* Task */
/************************************************************************/
const QString CEASSupportTask::BASE_DIR_NAME("ceas_report");

CEASSupportTask::CEASSupportTask(const CEASTaskSettings &_settings)
: ExternalToolSupportTask("Running CEAS report task", TaskFlag_None),
settings(_settings), bedDoc(NULL),
bedTask(NULL), wigTask(NULL), etTask(NULL), activeSubtasks(0)
{
    GCOUNTER(cvar, tvar, "NGS:CEASTask");
    SAFE_POINT_EXT(NULL != settings.getStorage() || settings.getBedData().isEmpty(), setError(L10N::nullPointerError("workflow data storage")), );
}

CEASSupportTask::~CEASSupportTask() {
    cleanup();
}

void CEASSupportTask::cleanup() {
    delete bedDoc; bedDoc = NULL;

    //remove tmp files
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(BASE_DIR_NAME);
    QDir tmpDir(tmpDirPath);
    if(tmpDir.exists()){
        foreach(QString file, tmpDir.entryList()){
            tmpDir.remove(file);
        }
        if(!tmpDir.rmdir(tmpDir.absolutePath())){
            //stateInfo.setError(tr("Subdir for temporary files exists. Can not remove this folder."));
            //return;
        }
    }

}

void CEASSupportTask::prepare() {
    UserAppsSettings *appSettings = AppContext::getAppSettings()->getUserAppsSettings();
    workingDir = appSettings->createCurrentProcessTemporarySubDir(stateInfo, BASE_DIR_NAME);
    CHECK_OP(stateInfo, );

    createBedDoc();
    CHECK_OP(stateInfo, );

    if (bedDoc){
        bedTask = new SaveDocumentTask(bedDoc);
        addSubTask(bedTask);
        activeSubtasks++;
    }else{
        addSubTask(createETTask());
    }
}

void CEASSupportTask::createBedDoc() {
    if (settings.getBedData().isEmpty()){
        bedDoc = NULL;
    }else{
        QString bedUrl = workingDir + "/" + "tmp.bed";

        DocumentFormat *bedFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::BED);
        CHECK_EXT(NULL != bedFormat, stateInfo.setError("NULL bed format"), );

        bedDoc = bedFormat->createNewLoadedDocument(
            IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), bedUrl, stateInfo);
        CHECK_OP(stateInfo, );
        bedDoc->setDocumentOwnsDbiResources(false);

        const QList<AnnotationTableObject *> annTables = Workflow::StorageUtils::getAnnotationTableObjects(settings.getStorage(), settings.getBedData());
        foreach (AnnotationTableObject *annTable, annTables) {
            bedDoc->addObject(annTable);
        }
    }
}

bool CEASSupportTask::canStartETTask() const {
    return (0 == activeSubtasks);
}

Task* CEASSupportTask::createETTask(){
    Task* res;
    if (bedDoc){
        settings.getCeasSettings().setBedFile(bedDoc->getURLString());
    }

    settings.getCeasSettings().setWigFile(settings.getWigData());
    QStringList args = settings.getCeasSettings().getArgumentList();

    ExternalTool* rTool = AppContext::getExternalToolRegistry()->getById(RSupport::ET_R_ID);
    SAFE_POINT(NULL != rTool, "R script tool wasn't found in the registry", new FailTask("R script tool wasn't found in the registry"));
    const QString rDir = QFileInfo(rTool->getPath()).dir().absolutePath();

    ExternalToolRunTask* runTask = new ExternalToolRunTask(CEASSupport::ET_CEAS_ID, args, new CEASLogParser(), workingDir, QStringList() << rDir);
    setListenerForTask(runTask);
    res = runTask;

    return res;
}

QList<Task*> CEASSupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(!subTask->isCanceled(), result);
    CHECK(!subTask->hasError(), result);

    if (bedTask == subTask || wigTask == subTask) {
        activeSubtasks--;
        if (canStartETTask()) {
            result << createETTask();
        }
    }

    return result;
}

bool CEASSupportTask::copyFile(const QString &src, const QString &dst) {
    if (!QFile::exists(src)) {
        coreLog.error(tr("CEAS error: Can not find a required output file %1.").arg(src));
        return false;
    }

    QSet<QString> excludeFileNames = DocumentUtils::getNewDocFileNameExcludesHint();
    if (!GUrlUtils::renameFileWithNameRoll(dst, stateInfo, excludeFileNames, &taskLog)) {
        return false;
    }

    bool copied = QFile::copy(src, dst);
    if (!copied) {
        setError(tr("Can not copy the result file to: %1").arg(dst));
        return true;
    }

    return true;
}

void CEASSupportTask::run() {
    QString tmpPdfFile = workingDir + "/tmp.pdf";
    if (!copyFile(tmpPdfFile, settings.getCeasSettings().getImageFilePath())){
        settings.getCeasSettings().setImagePath("");
    }

    CHECK_OP(stateInfo, );

    if (!settings.getBedData().isEmpty()){ //no annotation file if no bed data
        QString tmpXlsFile = workingDir + "/tmp.xls";
        if(!copyFile(tmpXlsFile, settings.getCeasSettings().getAnnsFilePath())){
            settings.getCeasSettings().setAnnsFilePath("");
        }
        CHECK_OP(stateInfo, );
    }
}

const CEASTaskSettings & CEASSupportTask::getSettings() const {
    return settings;
}

//////////////////////////////////////////////////////////////////////////
//CEASLogParser
CEASLogParser::CEASLogParser() :
    ExternalToolLogParser(),
    progress(-1)
{

}

int CEASLogParser::getProgress(){
    //parsing INFO  @ Fri, 07 Dec 2012 19:30:16: #1 read tag files...
    static const int max_step = 8;
    if(!lastPartOfLog.isEmpty()){
        QString lastMessage=lastPartOfLog.last();
        QRegExp rx(" #(\\d+) \\w");
        if(lastMessage.contains(rx)){
            SAFE_POINT(rx.indexIn(lastMessage) > -1, "bad progress index", 0);
            int step = rx.cap(1).toInt();
            return static_cast<int>((100 * step)/ float(qMax(step, max_step)));
        }
    }
    return progress;
}

void CEASLogParser::parseOutput( const QString& partOfLog ){
    ExternalToolLogParser::parseOutput(partOfLog);
}

void CEASLogParser::parseErrOutput( const QString& partOfLog ){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastErrLine+lastPartOfLog.first();
    lastErrLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        if(buf.contains("ERROR", Qt::CaseInsensitive)
            || buf.contains("CRITICAL", Qt::CaseInsensitive)){
                coreLog.error("CEAS: " + buf);
        }else if (buf.contains("WARNING", Qt::CaseInsensitive)){
            algoLog.info(buf);
        }else {
            algoLog.trace(buf);
        }
    }
}

} // U2
