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
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/OpenViewTask.h>

#include "CreateSequenceFromTextAndOpenViewTask.h"
#include "ImportSequenceFromRawDataTask.h"

namespace U2 {

CreateSequenceFromTextAndOpenViewTask::CreateSequenceFromTextAndOpenViewTask(const QList<DNASequence> &sequences, const QString &formatId, const GUrl &saveToPath, bool saveImmediately) :
    Task(tr("Create sequence from raw data"), TaskFlags_NR_FOSE_COSC),
    sequences(sequences),
    saveToPath(saveToPath),
    saveImmediately(saveImmediately),
    openProjectTask(NULL),
    importedSequences(0),
    document(NULL)
{
    format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
    SAFE_POINT_EXT(NULL != format, setError(QString("An unknown document format: %1").arg(formatId)), );
}

void CreateSequenceFromTextAndOpenViewTask::prepare() {
    Project *project = AppContext::getProject();
    if (NULL == project) {
        openProjectTask = AppContext::getProjectLoader()->createNewProjectTask();
        CHECK_EXT(NULL != openProjectTask, setError(tr("Can't create a project")), );
        addSubTask(openProjectTask);
    } else {
        prepareImportSequenceTasks();
        foreach (Task *task, importTasks) {
            addSubTask(task);
        }
    }
}

QList<Task *> CreateSequenceFromTextAndOpenViewTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;
    CHECK_OP(stateInfo, res);

    if (openProjectTask == subTask) {
        res << prepareImportSequenceTasks();
    }

    if (importTasks.contains(subTask)) {
        importedSequences++;

        if (importedSequences == sequences.size()) {
            addDocument();
            CHECK_OP(stateInfo, res);
            if (saveImmediately && !saveToPath.isEmpty()) {
                res << new SaveDocumentTask(document);
            }
            res << new OpenViewTask(document);
        }
    }

    return res;
}

QList<Task *> CreateSequenceFromTextAndOpenViewTask::prepareImportSequenceTasks() {
    foreach (const DNASequence &sequence, sequences) {
        importTasks << new ImportSequenceFromRawDataTask(AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo), U2ObjectDbi::ROOT_FOLDER, sequence);
        CHECK_OP(stateInfo, QList<Task *>());
    }
    return importTasks;
}

Document * CreateSequenceFromTextAndOpenViewTask::createEmptyDocument() {
    IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(saveToPath));
    SAFE_POINT_EXT(NULL != ioAdapterFactory, setError("IO adapter factory is NULL"), NULL);
    return format->createNewLoadedDocument(ioAdapterFactory, saveToPath, stateInfo);
}

void CreateSequenceFromTextAndOpenViewTask::addDocument() {
    document = createEmptyDocument();
    CHECK_OP(stateInfo, );

    foreach (Task *task, importTasks) {
        ImportSequenceFromRawDataTask *importTask = qobject_cast<ImportSequenceFromRawDataTask *>(task);
        document->addObject(new U2SequenceObject(importTask->getSequenceName(), importTask->getEntityRef()));
    }

    Project *project = AppContext::getProject();
    SAFE_POINT(NULL != project, "Project is NULL", );
    project->addDocument(document);
}

}   // namespace U2
