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

#include <QFileInfo>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

#include <U2Gui/OpenViewTask.h>

#include "ExportConsensusTask.h"

namespace U2 {

ExportConsensusTask::ExportConsensusTask(const ExportConsensusTaskSettings &settings_)
    : DocumentProviderTask("", TaskFlags_NR_FOSE_COSC),
      settings(settings_),
      consensusTask(NULL)
{
    setTaskName(tr("Export consensus of assembly '%1' to '%2'")
                .arg(settings.model->getAssembly().visualName)
                .arg(QFileInfo(settings.fileName).fileName()));
    setMaxParallelSubtasks(1);
}

void ExportConsensusTask::prepare() {
    U2DbiRef dbiRef;
    if (settings.saveToFile) {
        SAFE_POINT_EXT(!settings.fileName.isEmpty(), setError(tr("File name cannot be empty")),);

        DocumentFormat * df = AppContext::getDocumentFormatRegistry()->getFormatById(settings.formatId);
        SAFE_POINT_EXT(df != NULL, setError(tr("Internal: couldn't find document format with id '%1'").arg(settings.formatId)),);

        IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.fileName));
        resultDocument = df->createNewLoadedDocument(iof, settings.fileName, stateInfo);
        CHECK_OP(stateInfo, );

        dbiRef = resultDocument->getDbiRef();
    } else {
        dbiRef = settings.targetDbi;
    }

    // If the input region length is more than REGION_TO_ANALAYZE,
    // divide the analysis into iterations
    int iterNum = 0;
    qint64 wholeRegionLength = settings.region.length;
    do
    {
        U2Region iterRegion;

        if (wholeRegionLength <= REGION_TO_ANALAYZE) {
            iterRegion = U2Region(settings.region.startPos + iterNum * REGION_TO_ANALAYZE, wholeRegionLength);
            wholeRegionLength = 0;
        } else {
            iterRegion = U2Region(settings.region.startPos + iterNum * REGION_TO_ANALAYZE, REGION_TO_ANALAYZE);
            wholeRegionLength -= REGION_TO_ANALAYZE;
        }

        consensusRegions.enqueue(iterRegion);

        iterNum++;
    } while (wholeRegionLength != 0);

    consensusTask = new AssemblyConsensusWorker(this);
    consensusTask->setSubtaskProgressWeight(100);
    addSubTask(consensusTask);

    seqImporter.startSequence(stateInfo, dbiRef, U2ObjectDbi::ROOT_FOLDER, settings.seqObjName, false);
    CHECK_OP(stateInfo, );

    if (settings.saveToFile) {
        SaveDocFlags saveFlags = SaveDoc_Overwrite;
        addSubTask(new SaveDocumentTask(resultDocument, saveFlags));

        Project * p = AppContext::getProject();
        if(p != NULL && p->findDocumentByURL(resultDocument->getURL()) != NULL) {
            // if already has such document in project, do not add
            settings.addToProject = false;
        }

        if(settings.addToProject) {
            addSubTask(new AddDocumentAndOpenViewTask(takeDocument()));
        }
    }
}

QList<Task*> ExportConsensusTask::onSubTaskFinished(Task *finished) {
    QList<Task*> newSubTasks;
    if (finished->hasError() || finished->isCanceled()) {
        return newSubTasks;
    }

    if(finished == consensusTask) {
        resultSequence = seqImporter.finalizeSequence(stateInfo);
        CHECK_OP(stateInfo, newSubTasks);
        if (settings.saveToFile) {
            U2SequenceObject * seqObj = new U2SequenceObject(resultSequence.visualName, U2EntityRef(resultDocument->getDbiRef(), resultSequence.id));
            resultDocument->addObject(seqObj);
        }
    }
    return newSubTasks;
}

AssemblyConsensusTaskSettings ExportConsensusTask::getNextSettings() {
    AssemblyConsensusTaskSettings iterSettings = settings;
    iterSettings.region = consensusRegions.dequeue();
    return iterSettings;
}

void ExportConsensusTask::reportResult(const ConsensusInfo &result) {
    QByteArray consensus = result.consensus;

    if(! settings.keepGaps) {
        consensus.replace(QString(AssemblyConsensusAlgorithm::EMPTY_CHAR), "");
    }
    seqImporter.addBlock(consensus.constData(), consensus.length(), stateInfo);
    CHECK_OP(stateInfo,);
}

U2Sequence ExportConsensusTask::getResult() const {
    return resultSequence;
}


} // namespace
