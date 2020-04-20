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
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ConservationPlotSupport.h"
#include "R/RSupport.h"

#include "ConservationPlotTask.h"

namespace U2 {

const QString ConservationPlotTask::BASE_DIR_NAME("ConservationPlot_tmp");

ConservationPlotTask::ConservationPlotTask(const ConservationPlotSettings& _settings, Workflow::DbiDataStorage *storage, const QList<Workflow::SharedDbiDataHandler> &_plotData)
: ExternalToolSupportTask("ConservationPlot annotation", TaskFlag_CollectChildrenWarnings)
, settings(_settings)
, treatDoc(NULL)
, treatTask(NULL)
, storage(storage)
, plotData(_plotData)
, activeSubtasks(0)
, etTask(NULL)
{
    GCOUNTER(cvar, tvar, "NGS:ConservationPlotTask");
    SAFE_POINT_EXT(NULL != storage, setError(L10N::nullPointerError("workflow data storage")), );
}

ConservationPlotTask::~ConservationPlotTask() {
    cleanup();
}

void ConservationPlotTask::cleanup() {
    plotData.clear();

    delete treatDoc; treatDoc = NULL;

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

void ConservationPlotTask::prepare() {
    UserAppsSettings *appSettings = AppContext::getAppSettings()->getUserAppsSettings();
    workingDir = appSettings->createCurrentProcessTemporarySubDir(stateInfo, BASE_DIR_NAME);
    CHECK_OP(stateInfo, );

    foreach (const Workflow::SharedDbiDataHandler &annTableHandler, plotData) {
        Document *bedDoc = NULL;
        SaveDocumentTask *saveTask = NULL;

        QString name = getSettings().label;
        name = name.replace(' ','_');
        if (activeSubtasks != 0){
            name += QString("_%1").arg(activeSubtasks);
        }
        bedDoc = createDoc(annTableHandler, name);
        CHECK_OP(stateInfo, );

        saveTask = new SaveDocumentTask(bedDoc);

        docTaskMap.insert(bedDoc, saveTask);

        activeSubtasks++;
        addSubTask(saveTask);
    }
}

Document* ConservationPlotTask::createDoc(const Workflow::SharedDbiDataHandler &annTableHandler, const QString& name){
    Document* doc = NULL;

    QString docUrl = workingDir + "/" + name +".bed";

    DocumentFormat *bedFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::BED);
    CHECK_EXT(NULL != bedFormat, stateInfo.setError("NULL bed format"), doc);

    doc = bedFormat->createNewLoadedDocument(
        IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), docUrl, stateInfo);
    CHECK_OP(stateInfo, doc);
    doc->setDocumentOwnsDbiResources(false);

    AnnotationTableObject *annTable = Workflow::StorageUtils::getAnnotationTableObject(storage, annTableHandler);
    SAFE_POINT_EXT(NULL != annTable, setError(tr("An annotation table object wasn't found in the workflow data storage")), NULL);
    doc->addObject(annTable);

    return doc;
}

QList<Task*> ConservationPlotTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(!subTask->isCanceled(), result);
    CHECK(!subTask->hasError(), result);

    bool containsTask = false;
    foreach(SaveDocumentTask* sdt, docTaskMap.values()){
        if (sdt == subTask){
            containsTask = true;
            break;
        }
    }

    if (containsTask) {
        activeSubtasks--;
        if (activeSubtasks == 0){
            QList<QString> docNames;
            foreach(Document* bedDoc, docTaskMap.keys()){
                docNames.append(bedDoc->getURLString());
            }

            QStringList args = settings.getArguments(docNames);

            ExternalTool* rTool = AppContext::getExternalToolRegistry()->getById(RSupport::ET_R_ID);
            SAFE_POINT(NULL != rTool, "R script tool wasn't found in the registry", result);
            const QString rDir = QFileInfo(rTool->getPath()).dir().absolutePath();

            etTask = new ExternalToolRunTask(ConservationPlotSupport::ET_CONSERVATION_PLOT_ID, args, new ConservationPlotLogParser(), workingDir, QStringList() << rDir);
            setListenerForTask(etTask);
            result << etTask;
        }
    }
    return result;
}

bool ConservationPlotTask::copyFile(const QString &src, const QString &dst) {
    if (!QFile::exists(src)) {
        coreLog.error(tr("Conservation Plot error: Can not find a required output file %1.").arg(src));
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


void ConservationPlotTask::run() {
    QString tmpPdfFile = workingDir + "/tmp.bmp";
    if(!copyFile(tmpPdfFile, getSettings().outFile)){
        settings.outFile = "";
    }
    CHECK_OP(stateInfo, );
}

const ConservationPlotSettings& ConservationPlotTask::getSettings(){
    return settings;
}

//////////////////////////////////////////////////////////////////////////
//ConservationPlotLogParser

int ConservationPlotLogParser::getProgress(){
    return 0;
}

void ConservationPlotLogParser::parseOutput( const QString& partOfLog ){
    ExternalToolLogParser::parseOutput(partOfLog);
}

void ConservationPlotLogParser::parseErrOutput( const QString& partOfLog ){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastErrLine+lastPartOfLog.first();
    lastErrLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        if(buf.contains("ERROR", Qt::CaseInsensitive)
            || buf.contains("CRITICAL", Qt::CaseInsensitive)){
                coreLog.error("conservation plot: " + buf);
        }else if (buf.contains("WARNING", Qt::CaseInsensitive)){
            algoLog.info(buf);
        }else {
            algoLog.trace(buf);
        }
    }
}

} // U2
