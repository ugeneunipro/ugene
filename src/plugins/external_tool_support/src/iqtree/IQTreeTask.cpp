/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "IQTreeTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "IQTreeSupport.h"

namespace U2 {

IQTreeTaskContext::IQTreeTaskContext(const MultipleSequenceAlignment& _msa, const CreatePhyTreeSettings& _settings)
    : msa(_msa), settings(_settings) {
}

//////////////////////////////////////////////////////////////////////////
/// IQTreeTask

IQTreeTask::IQTreeTask(const MultipleSequenceAlignment& msa, const CreatePhyTreeSettings& settings)
    : PhyTreeGeneratorTask(msa, settings), context(msa, settings) {
    GCOUNTER(cvar, "IQTreeTask");
    setTaskName(tr("IQTree tree calculation"));

    auto prepareWorkDirTask = new PrepareIQTreeWorkDirTask(&context);
    prepareWorkDirTask->setSubtaskProgressWeight(1);
    addSubTask(prepareWorkDirTask);

    auto externalToolTask = new RunIQTreeExternalToolTask(&context);
    externalToolTask->setSubtaskProgressWeight(99);
    addSubTask(externalToolTask);
}

Task::ReportResult IQTreeTask::report() {
    if (!context.tmpDirUrl.isEmpty()) {
        U2OpStatus2Log os;
        ExternalToolSupportUtils::removeTmpDir(context.tmpDirUrl, os);
    }
    result = context.resultTree;
    return ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
/// PrepareIQTreeWorkDirTask

PrepareIQTreeWorkDirTask::PrepareIQTreeWorkDirTask(IQTreeTaskContext* _context)
    : Task(tr("Prepare IQTree work dir"), TaskFlag_None), context(_context) {
}

void PrepareIQTreeWorkDirTask::run() {
    // Create work dir.
    context->tmpDirUrl = ExternalToolSupportUtils::createTmpDir("iqtree", stateInfo);
    CHECK_OP(stateInfo, );

    // Save input MSA into a file in work dir.
    QString alignmentFilePath = context->tmpDirUrl + "/input.phy";
    IOAdapterFactory* ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat* documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PHYLIP_INTERLEAVED);

    QScopedPointer<Document> document(documentFormat->createNewLoadedDocument(ioAdapterFactory, alignmentFilePath, stateInfo));
    auto* msaObject = MultipleSequenceAlignmentImporter::createAlignment(document->getDbiRef(), context->msa, stateInfo);
    CHECK_OP(stateInfo, );

    document->addObject(msaObject);
    documentFormat->storeDocument(document.data(), stateInfo, ioAdapterFactory, alignmentFilePath);
    CHECK_OP(stateInfo, );

    context->alignmentFilePath = alignmentFilePath;
}

//////////////////////////////////////////////////////////////////////////
/// RunIQTreeExternalToolTask

RunIQTreeExternalToolTask::RunIQTreeExternalToolTask(IQTreeTaskContext* _context)
    : Task(tr("Run IQTree tool"), TaskFlags_NR_FOSE_COSC), context(_context) {
}

void RunIQTreeExternalToolTask::prepare() {
    SAFE_POINT_EXT(!context->alignmentFilePath.isEmpty(), stateInfo.setError(L10N::internalError("alignmentFilePath is empty")), );

    QStringList arguments;
    arguments << "-s";
    arguments << context->alignmentFilePath;
    arguments << context->settings.extToolArguments;

    auto externalToolTask = new ExternalToolRunTask(IQTreeSupport::IQTREE_ID, arguments, new ExternalToolLogParser());
    externalToolTask->setSubtaskProgressWeight(99);
    addSubTask(externalToolTask);

    IOAdapterFactory* ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    auto loadResultTask = new LoadDocumentTask(BaseDocumentFormats::NEWICK, context->alignmentFilePath + ".treefile", ioAdapterFactory);
    loadResultTask->setSubtaskProgressWeight(1);
    addSubTask(loadResultTask);
}

Task::ReportResult RunIQTreeExternalToolTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    auto loadTask = qobject_cast<LoadDocumentTask*>(getSubtasks().last());
    SAFE_POINT_EXT(loadTask != nullptr, L10N::internalError("Invalid subtasks order"), ReportResult_Finished);

    auto treeDocument = loadTask->getDocument(false);
    auto treeObject = qobject_cast<PhyTreeObject*>(treeDocument->getObjects().first());
    SAFE_POINT_EXT(treeObject != nullptr, L10N::internalError("Result file has no tree object"), ReportResult_Finished);

    context->resultTree = treeObject->getTree();
    return ReportResult_Finished;
}

}  // namespace U2
