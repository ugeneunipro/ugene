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

#include "SeqPosSupport.h"
#include "SeqPosTask.h"
#include "R/RSupport.h"


namespace U2 {

const QString SeqPosTask::BASE_DIR_NAME("SeqPos_tmp");
const QString SeqPosTask::BASE_SUBDIR_NAME("SeqPos");
const QString SeqPosTask::TREAT_NAME("treatment");

SeqPosTask::SeqPosTask(const SeqPosSettings& _settings, Workflow::DbiDataStorage *storage, const QList<Workflow::SharedDbiDataHandler> &_treatAnn)
: ExternalToolSupportTask("SeqPos annotation", TaskFlag_CollectChildrenWarnings)
, settings(_settings)
, storage(storage)
, treatAnn(_treatAnn)
, treatDoc(NULL)
, treatTask(NULL)
, etTask(NULL)
{
    GCOUNTER(cvar, tvar, "NGS:SeqPosTask");
    SAFE_POINT_EXT(NULL != storage, setError(L10N::nullPointerError("workflow data storage")), );
}

SeqPosTask::~SeqPosTask() {
    cleanup();
}

void SeqPosTask::cleanup() {
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

void SeqPosTask::prepare() {
    UserAppsSettings *appSettings = AppContext::getAppSettings()->getUserAppsSettings();
    workingDir = appSettings->createCurrentProcessTemporarySubDir(stateInfo, BASE_DIR_NAME);
    CHECK_OP(stateInfo, );

    settings.outDir = GUrlUtils::createDirectory(
        settings.outDir + "/" + BASE_SUBDIR_NAME,
        "_", stateInfo);
    CHECK_OP(stateInfo, );

    treatDoc = createDoc(treatAnn, TREAT_NAME);
    CHECK_OP(stateInfo, );
    treatAnn.clear();

    treatTask = new SaveDocumentTask(treatDoc);
    addSubTask(treatTask);
}

Document* SeqPosTask::createDoc( const QList<Workflow::SharedDbiDataHandler>& annTableHandlers, const QString& name){
    Document* doc = NULL;

    QString docUrl = workingDir + "/" + name +".bed";

    DocumentFormat *bedFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::BED);
    CHECK_EXT(NULL != bedFormat, stateInfo.setError("NULL bed format"), doc);

    doc = bedFormat->createNewLoadedDocument(
        IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), docUrl, stateInfo);
    CHECK_OP(stateInfo, doc);
    doc->setDocumentOwnsDbiResources(false);

    QList<AnnotationTableObject *> annTables = Workflow::StorageUtils::getAnnotationTableObjects(storage, annTableHandlers);
    foreach (AnnotationTableObject *annTable, annTables) {
        doc->addObject(annTable);
    }

    return doc;
}

QList<Task*> SeqPosTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(!subTask->isCanceled(), result);
    CHECK(!subTask->hasError(), result);

    if (treatTask == subTask) {
            QStringList args = settings.getArguments(treatDoc->getURLString());

            ExternalTool* rTool = AppContext::getExternalToolRegistry()->getById(RSupport::ET_R_ID);
            SAFE_POINT(NULL != rTool, "R script tool wasn't found in the registry", result);
            const QString rDir = QFileInfo(rTool->getPath()).dir().absolutePath();

            etTask = new ExternalToolRunTask(SeqPosSupport::ET_SEQPOS_ID, args, new ExternalToolLogParser(), getSettings().outDir, QStringList() << rDir);
            setListenerForTask(etTask);
            result << etTask;
    }
    return result;
}

void SeqPosTask::run() {

}

const SeqPosSettings& SeqPosTask::getSettings(){
    return settings;
}

QStringList SeqPosTask::getOutputFiles(){
    QStringList result;

    QString current;

    current = getSettings().outDir + "/results/" + "mdseqpos_index.html";
    if (QFile::exists(current)){
        result << current;
    }

//index.html contains table.html
//    current = getSettings().outDir + "/results/" + "table.html";
//    if (QFile::exists(current)){
//        result << current;
//    }


    return result;
}

} // U2
