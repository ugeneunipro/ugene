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

#include "AddDocumentTask.h"

#include <U2Core/ProjectModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/RemoveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {


AddDocumentTask::AddDocumentTask(Document * _d, const AddDocumentTaskConfig& _conf) :
Task("Add document task", TaskFlags_NR_FOSE_COSC), document(_d), dpt(NULL), conf(_conf)
{
    CHECK_EXT(_d != NULL, setError("Document pointer ]is NULL"), );
    SAFE_POINT(document->isMainThreadObject(), QString("Document added to the project does not belong to the main application thread: %1 !").arg(document->getURLString()),);
    setTaskName(tr("Adding document to project: %1").arg(_d->getURLString()));
    if(AppContext::getProject() == NULL){
        addSubTask( AppContext::getProjectLoader()->createNewProjectTask());
    }else{
        setSubtaskProgressWeight(0);
    }
}

AddDocumentTask::AddDocumentTask(DocumentProviderTask * _dpt, const AddDocumentTaskConfig& c) :
Task("Add document task", TaskFlags_NR_FOSE_COSC), document(NULL), dpt(_dpt), conf(c)
{
    CHECK_EXT(_dpt != NULL, setError("Document provider task pointer is NULL"), );
    setTaskName(tr("Adding document to project: %1").arg(_dpt->getDocumentDescription()));
    addSubTask(dpt);
}

QList<Task*> AddDocumentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (propagateSubtaskError()) {
        return res;
    }

    if (subTask == dpt && dpt != NULL) {
        if(dpt->isCanceled()){
            return res;
        }
        document = dpt->takeDocument();
        if (AppContext::getProject() == NULL) {
            res << AppContext::getProjectLoader()->createNewProjectTask();
        } else if (conf.unloadExistingDocument) {
            Document* oldDoc = AppContext::getProject()->findDocumentByURL(document->getURL());
            if (oldDoc != NULL && oldDoc != document) {
                res << new RemoveMultipleDocumentsTask(AppContext::getProject(), QList<Document*>() << oldDoc, false, false);
            }
        }
    }
    return res;
}

Task::ReportResult AddDocumentTask::report() {
    Project * p = AppContext::getProject();
    if( p == NULL) { // no project is opened
        if (!hasError()) {
            setError(tr("No project is opened"));
        }
        return ReportResult_Finished;
    }

    if( p->isStateLocked() ) {
        return ReportResult_CallMeAgain;
    } else if (document != NULL) {
        Document* sameURLDoc = p->findDocumentByURL(document->getURL());
        if (sameURLDoc!=NULL) {
            stateInfo.setError(tr("Document is already added to the project %1").arg(document->getURL().getURLString()));
        } else {
            p->addDocument(document);
        }
    } else if (!stateInfo.isCanceled()) {
        stateInfo.setError(stateInfo.getError() + tr("Document was removed"));
    }
    return ReportResult_Finished;
}

}//namespace
