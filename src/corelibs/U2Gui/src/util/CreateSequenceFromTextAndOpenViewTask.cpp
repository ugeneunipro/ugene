/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "CreateSequenceFromTextAndOpenViewTask.h"

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

#include "ImportSequenceFromRawDataTask.h"

namespace U2 {

CreateSequenceFromTextAndOpenViewTask::CreateSequenceFromTextAndOpenViewTask(const QList<DNASequence>& sequences, const QString& formatId, const GUrl& saveToPath)
    : Task(tr("Create sequence from raw data"), TaskFlags_NR_FOSE_COSC),
      sequences(sequences),
      saveToPath(saveToPath),
      openProjectTask(nullptr),
      importedSequences(0),
      document(nullptr) {
    format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
    SAFE_POINT_EXT(format != nullptr, setError(QString("An unknown document format: %1").arg(formatId)), );
}

void CreateSequenceFromTextAndOpenViewTask::prepare() {
    Project* project = AppContext::getProject();
    if (project == nullptr) {
        openProjectTask = AppContext::getProjectLoader()->createNewProjectTask();
        CHECK_EXT(openProjectTask != nullptr, setError(tr("Can't create a project")), );
        addSubTask(openProjectTask);
    } else {
        prepareImportSequenceTasks();
        for (Task* task : qAsConst(importTasks)) {
            addSubTask(task);
        }
    }
}

QList<Task*> CreateSequenceFromTextAndOpenViewTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    if (openProjectTask == subTask) {
        res << prepareImportSequenceTasks();
    }

    if (importTasks.contains(subTask)) {
        importedSequences++;

        if (importedSequences == sequences.size()) {
            addDocument();
            CHECK_OP(stateInfo, res);

            res << new SaveDocumentTask(document);
            res << new OpenViewTask(document);
        }
    }

    return res;
}

QList<Task*> CreateSequenceFromTextAndOpenViewTask::prepareImportSequenceTasks() {
    for (const DNASequence& sequence : qAsConst(sequences)) {
        importTasks << new ImportSequenceFromRawDataTask(AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo), U2ObjectDbi::ROOT_FOLDER, sequence);
        CHECK_OP(stateInfo, QList<Task*>());
    }
    return importTasks;
}

Document* CreateSequenceFromTextAndOpenViewTask::createEmptyDocument() {
    IOAdapterFactory* ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(saveToPath));
    SAFE_POINT_EXT(ioAdapterFactory != nullptr, setError("IO adapter factory is NULL"), nullptr);
    return format->createNewLoadedDocument(ioAdapterFactory, saveToPath, stateInfo);
}

void CreateSequenceFromTextAndOpenViewTask::addDocument() {
    Project* project = AppContext::getProject();
    SAFE_POINT(project != nullptr, "Project is NULL", );

    document = createEmptyDocument();
    CHECK_OP(stateInfo, );

    for (Task* task : qAsConst(importTasks)) {
        auto importTask = qobject_cast<ImportSequenceFromRawDataTask*>(task);
        document->addObject(new U2SequenceObject(importTask->getSequenceName(), importTask->getEntityRef()));
    }

    // We do not need to add the document, if we already have one, associated with the current URL
    CHECK(project->findDocumentByURL(saveToPath) == nullptr, );

    project->addDocument(document);
}

}  // namespace U2
