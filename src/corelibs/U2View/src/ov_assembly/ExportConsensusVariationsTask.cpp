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
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2VariantUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Formats/DocumentFormatUtils.h>
#include <U2Gui/OpenViewTask.h>

#include "ExportConsensusVariationsTask.h"

namespace U2 {

ExportConsensusVariationsTask::ExportConsensusVariationsTask(const ExportConsensusVariationsTaskSettings &settings_)
    : DocumentProviderTask("", TaskFlags_NR_FOSE_COSC)
    , settings(settings_)
    , consensusTask(NULL)
    , varTrackObject(NULL)
{
    setTaskName(tr("Export consensus variations of assembly '%1' to '%2'")
                .arg(settings.model->getAssembly().visualName)
                .arg(QFileInfo(settings.fileName).fileName()));
    setMaxParallelSubtasks(1);
}

void ExportConsensusVariationsTask::prepare() {
     SAFE_POINT_EXT(!settings.fileName.isEmpty(), setError(tr("File name cannot be empty")),);

     DocumentFormat * df = AppContext::getDocumentFormatRegistry()->getFormatById(settings.formatId);
     SAFE_POINT_EXT(df != NULL, setError(tr("Internal: couldn't find document format with id '%1'").arg(settings.formatId)),);

     IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.fileName));
     resultDocument = df->createNewLoadedDocument(iof, settings.fileName, stateInfo);
     CHECK_OP(stateInfo, );

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

     U2VariantTrack track = U2VariationUtils::createVariantTrack(resultDocument->getDbiRef(), settings.seqObjName, stateInfo);
     CHECK_OP(stateInfo, );

     U2EntityRef trackRef(resultDocument->getDbiRef(), track.id);
     varTrackObject = new VariantTrackObject(settings.seqObjName, trackRef);

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

QList<Task*> ExportConsensusVariationsTask::onSubTaskFinished(Task *finished) {
    QList<Task*> newSubTasks;
    if(finished == consensusTask) {
        SAFE_POINT(varTrackObject!=NULL, "Variant track object is null", newSubTasks);
        resultDocument->addObject(varTrackObject);
    }
    return newSubTasks;
}

AssemblyConsensusTaskSettings ExportConsensusVariationsTask::getNextSettings() {
    AssemblyConsensusTaskSettings iterSettings = settings;
    iterSettings.region = consensusRegions.dequeue();
    return iterSettings;
}

void ExportConsensusVariationsTask::reportResult(const ConsensusInfo &result) {
    SAFE_POINT(varTrackObject!=NULL, "Variant track object is null", );
    SAFE_POINT(settings.refSeq.isValid(), "Reference to sequence is invalid", );

    QByteArray consensus = result.consensus;

    DbiConnection con(settings.refSeq.dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    QByteArray refSeqData = seqDbi->getSequenceData(settings.refSeq.entityId, result.region, stateInfo);
    CHECK_OP(stateInfo,);
    CHECK(!refSeqData.isEmpty(), );

    QList<U2Variant> variants = U2VariationUtils::getSNPFromSequences(refSeqData, consensus, settings.mode, !settings.keepGaps, settings.seqObjName, result.region.startPos);

    varTrackObject->addVariants(variants, stateInfo);
    CHECK_OP(stateInfo,);
}

} // namespace
