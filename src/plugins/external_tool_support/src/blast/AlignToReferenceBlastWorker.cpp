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

#include "AlignToReferenceBlastWorker.h"

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "BlastSupport.h"
#include "align_worker_subtasks/BlastAlignToReferenceTask.h"
#include "align_worker_subtasks/ComposeResultSubtask.h"
#include "align_worker_subtasks/MakeBlastDbAlignerSubtask.h"
#include "align_worker_subtasks/PrepareReferenceSequenceTask.h"

namespace U2 {
namespace LocalWorkflow {

const QString AlignToReferenceBlastWorkerFactory::ACTOR_ID("align-to-reference");
const QString AlignToReferenceBlastWorkerFactory::ROW_NAMING_SEQUENCE_NAME = QObject::tr("Sequence name from file");
const QString AlignToReferenceBlastWorkerFactory::ROW_NAMING_FILE_NAME = QObject::tr("File name");
const QString AlignToReferenceBlastWorkerFactory::ROW_NAMING_SEQUENCE_NAME_VALUE = "sequence name";
const QString AlignToReferenceBlastWorkerFactory::ROW_NAMING_FILE_NAME_VALUE = "file name";

namespace {
const QString OUT_PORT_ID = "out";
const QString REF_ATTR_ID = "reference";
const QString RESULT_URL_ATTR_ID = "result-url";
const QString IDENTITY_ID = "identity";
const QString ROW_NAMING_ID = "row-naming-policy";
}  // namespace

/************************************************************************/
/* AlignToReferenceBlastWorkerFactory */
/************************************************************************/
AlignToReferenceBlastWorkerFactory::AlignToReferenceBlastWorkerFactory()
    : DomainFactory(ACTOR_ID) {
}

Worker* AlignToReferenceBlastWorkerFactory::createWorker(Actor* a) {
    return new AlignToReferenceBlastWorker(a);
}

void AlignToReferenceBlastWorkerFactory::init() {
    QList<PortDescriptor*> ports;
    {
        Descriptor inDesc(BasePorts::IN_SEQ_PORT_ID(), AlignToReferenceBlastPrompter::tr("Input sequence"), AlignToReferenceBlastPrompter::tr("Input sequence."));
        Descriptor outDesc(OUT_PORT_ID, AlignToReferenceBlastPrompter::tr("Aligned data"), AlignToReferenceBlastPrompter::tr("Aligned data."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();

        QMap<Descriptor, DataTypePtr> outType;
        outType[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        outType[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        outType[Descriptor(BaseSlots::URL_SLOT().getId(),
                           AlignToReferenceBlastPrompter::tr("Multiple Chromatogram Alignment URL"),
                           AlignToReferenceBlastPrompter::tr("Location of a result file with a Multiple Chromatogram Alignment."))] = BaseTypes::STRING_TYPE();

        ports << new PortDescriptor(inDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);
        ports << new PortDescriptor(outDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-out", outType)), false /*input*/, true /*multi*/);
    }
    QList<Attribute*> attributes;
    {
        Descriptor refDesc(REF_ATTR_ID, AlignToReferenceBlastPrompter::tr("Reference URL"), AlignToReferenceBlastPrompter::tr("A URL to the file with a reference sequence."));
        attributes << new Attribute(refDesc, BaseTypes::STRING_TYPE(), true);

        Descriptor outputUrlDesc(RESULT_URL_ATTR_ID, AlignToReferenceBlastPrompter::tr("Result alignment URL"), AlignToReferenceBlastPrompter::tr("An URL to write the result alignment."));
        attributes << new Attribute(outputUrlDesc, BaseTypes::STRING_TYPE(), true);

        Descriptor identityDesc(IDENTITY_ID, AlignToReferenceBlastPrompter::tr("Mapping min similarity"), AlignToReferenceBlastPrompter::tr("Reads, whose similarity with the reference is less than the stated value, will be ignored."));
        attributes << new Attribute(identityDesc, BaseTypes::NUM_TYPE(), false, 80);

        Descriptor rowNamingDesc(ROW_NAMING_ID, AlignToReferenceBlastPrompter::tr("Read name in result alignment"), AlignToReferenceBlastPrompter::tr("Reads in the result alignment can be named either by names of the sequences in the input files or by the input files names. "
                                                                                                                                                      "For example, if the sequences have the same name, set this value to \"File name\" to be able to distinguish the reads in the result alignment."));
        attributes << new Attribute(rowNamingDesc, BaseTypes::STRING_TYPE(), false, ROW_NAMING_SEQUENCE_NAME_VALUE);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[REF_ATTR_ID] = new URLDelegate("", "", false, false, false);
        delegates[RESULT_URL_ATTR_ID] = new URLDelegate(FileFilters::createFileFilterByObjectTypes({BaseDocumentFormats::UGENEDB}, true), "", false, false, true, nullptr, BaseDocumentFormats::UGENEDB);
        QVariantMap m;
        m["minimum"] = 0;
        m["maximum"] = 100;
        m["suffix"] = "%";
        delegates[IDENTITY_ID] = new SpinBoxDelegate(m);

        QVariantMap rowNamingMap;
        rowNamingMap.insert(ROW_NAMING_SEQUENCE_NAME, ROW_NAMING_SEQUENCE_NAME_VALUE);
        rowNamingMap.insert(ROW_NAMING_FILE_NAME, ROW_NAMING_FILE_NAME_VALUE);
        delegates[ROW_NAMING_ID] = new ComboBoxDelegate(rowNamingMap);
    }

    Descriptor desc(ACTOR_ID, AlignToReferenceBlastWorker::tr("Map to Reference"), AlignToReferenceBlastWorker::tr("Align input sequences (e.g. Sanger reads) to the reference sequence."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new AlignToReferenceBlastPrompter(nullptr));
    proto->addExternalTool(BlastSupport::ET_BLASTN_ID);
    proto->addExternalTool(BlastSupport::ET_MAKEBLASTDB_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new AlignToReferenceBlastWorkerFactory());
}

/************************************************************************/
/* AlignToReferenceBlastPrompter */
/************************************************************************/
AlignToReferenceBlastPrompter::AlignToReferenceBlastPrompter(Actor* a)
    : PrompterBase<AlignToReferenceBlastPrompter>(a) {
}

QString AlignToReferenceBlastPrompter::composeRichDoc() {
    auto input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    SAFE_POINT(input != nullptr, "No input port", "");
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    const QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    const QString producerName = producer != nullptr ? producer->getLabel() : unsetStr;
    const QString refLink = getHyperlink(REF_ATTR_ID, getURL(REF_ATTR_ID));
    return tr("Aligns each sequence from <u>%1</u> to the reference sequence from <u>%2</u>.").arg(producerName).arg(refLink);
}

/************************************************************************/
/* AlignToReferenceBlastWorker */
/************************************************************************/
AlignToReferenceBlastWorker::AlignToReferenceBlastWorker(Actor* a)
    : BaseDatasetWorker(a, BasePorts::IN_SEQ_PORT_ID(), OUT_PORT_ID) {
}

Task* AlignToReferenceBlastWorker::createPrepareTask(U2OpStatus& /*os*/) const {
    QString referenceUrlValue = getValue<QString>(REF_ATTR_ID);
    return new PrepareReferenceSequenceTask(referenceUrlValue, context->getDataStorage()->getDbiRef());
}

void AlignToReferenceBlastWorker::onPrepared(Task* task, U2OpStatus& os) {
    auto prepareTask = qobject_cast<PrepareReferenceSequenceTask*>(task);
    CHECK_EXT(prepareTask != nullptr, os.setError(L10N::internalError("Unexpected prepare task")), );
    reference = context->getDataStorage()->getDataHandler(prepareTask->getReferenceEntityRef());
    referenceUrl = prepareTask->getPreparedReferenceUrl();
}

Task* AlignToReferenceBlastWorker::createTask(const QList<Message>& messages) const {
    QList<SharedDbiDataHandler> reads;
    QMap<SharedDbiDataHandler, QString> readNameById;
    for (const Message& message : qAsConst(messages)) {
        QVariantMap data = message.getData().toMap();
        if (data.contains(BaseSlots::DNA_SEQUENCE_SLOT().getId())) {
            SharedDbiDataHandler read = data[BaseSlots::DNA_SEQUENCE_SLOT().getId()].value<SharedDbiDataHandler>();
            reads << read;
            QString customReadName = getReadName(message);
            if (!customReadName.isEmpty()) {
                readNameById.insert(read, customReadName);
            }
        }
    }
    int readIdentity = getValue<int>(IDENTITY_ID);
    return new AlignToReferenceBlastTask(referenceUrl, getValue<QString>(RESULT_URL_ATTR_ID), reference, reads, readNameById, readIdentity, context->getDataStorage());
}

QVariantMap AlignToReferenceBlastWorker::getResult(Task* task, U2OpStatus& os) const {
    auto alignTask = qobject_cast<AlignToReferenceBlastTask*>(task);
    CHECK_EXT(alignTask != nullptr, os.setError(L10N::internalError("Unexpected task")), QVariantMap());

    QList<QPair<QString, QPair<int, bool>>> acceptedReads = alignTask->getAcceptedReads();
    QList<QPair<QString, int>> discardedReads = alignTask->getDiscardedReads();

    algoLog.details(QString("Reads discarded by the mapper: %1").arg(discardedReads.count()));
    for (const auto& discardedPair : qAsConst(discardedReads)) {
        algoLog.details(discardedPair.first);
    }
    algoLog.trace(QString("Reads accepted by the mapper: %1").arg(acceptedReads.count()));
    for (const auto& pair : qAsConst(acceptedReads)) {
        algoLog.details((pair.second.second ? "&#x2190;&nbsp;&nbsp;" : "&#x2192;&nbsp;&nbsp;") + pair.first);
    }
    algoLog.details(QString("Total reads processed by the mapper: %1").arg(acceptedReads.count() + discardedReads.count()));

    if (!discardedReads.isEmpty()) {
        monitor()->addInfo(QString("%1 %2 not mapped")
                               .arg(discardedReads.count())
                               .arg(discardedReads.count() == 1 ? "read was" : "reads were"),
                           actor->getId(),
                           WorkflowNotification::U2_WARNING);
    }

    QString resultUrl = alignTask->getResultUrl();
    if (QFileInfo::exists(resultUrl)) {
        monitor()->addOutputFile(resultUrl, actor->getId());
    } else {
        os.setError(tr("The result file was not produced"));
    }

    QVariantMap result;
    result[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(reference);
    result[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(alignTask->getAnnotations());
    if (QFileInfo::exists(resultUrl)) {
        result[BaseSlots::URL_SLOT().getId()] = resultUrl;
    }
    return result;
}

MessageMetadata AlignToReferenceBlastWorker::generateMetadata(const QString& datasetName) const {
    return MessageMetadata(getValue<QString>(REF_ATTR_ID), datasetName);
}

QString AlignToReferenceBlastWorker::getReadName(const Message& message) const {
    CHECK(getValue<QString>(ROW_NAMING_ID) == AlignToReferenceBlastWorkerFactory::ROW_NAMING_FILE_NAME_VALUE, "");
    const int metadataId = message.getMetadataId();
    const MessageMetadata metadata = context->getMetadataStorage().get(metadataId);
    return GUrlUtils::getUncompressedCompleteBaseName(metadata.getFileUrl());
}

/************************************************************************/
/* AlignToReferenceBlastTask */
/************************************************************************/
AlignToReferenceBlastTask::AlignToReferenceBlastTask(const QString& _referenceUrl,
                                                     const QString& _resultUrl,
                                                     const SharedDbiDataHandler& _reference,
                                                     const QList<SharedDbiDataHandler>& _reads,
                                                     const QMap<SharedDbiDataHandler, QString>& _readNameById,
                                                     int _minIdentityPercent,
                                                     DbiDataStorage* _storage)
    : Task(tr("Map to reference"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      referenceUrl(_referenceUrl),
      resultUrl(_resultUrl),
      reference(_reference),
      reads(_reads),
      readNameById(_readNameById),
      minIdentityPercent(_minIdentityPercent),
      storage(_storage) {
    GCOUNTER(cvar, "AlignToReferenceBlastTask");
}

void AlignToReferenceBlastTask::prepare() {
    formatDbSubTask = new U2::Workflow::MakeBlastDbAlignerSubtask(referenceUrl, reference, storage);
    addSubTask(formatDbSubTask);
}

QList<Task*> AlignToReferenceBlastTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(subTask != nullptr, result);
    CHECK(!subTask->isCanceled() && !subTask->hasError(), result);

    if (subTask == formatDbSubTask) {
        QString dbPath = formatDbSubTask->getResultPath();
        blastTask = new BlastAlignToReferenceMuxTask(dbPath, reads, reference, readNameById, storage);
        result << blastTask;
    } else if (subTask == blastTask) {
        const QList<AlignToReferenceResult>& alignmentResults = blastTask->getAlignmentResults();
        QList<AlignToReferenceResult> acceptedResults;
        for (const auto& alignmentResult : qAsConst(alignmentResults)) {
            if (alignmentResult.identityPercent >= minIdentityPercent) {
                acceptedResults << alignmentResult;
            }
        }
        composeSubTask = new ComposeResultSubtask(reference, acceptedResults, storage);
        composeSubTask->setSubtaskProgressWeight(0.5f);
        result << composeSubTask;
    } else if (subTask == composeSubTask) {
        DocumentFormat* ugenedbFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::UGENEDB);
        QScopedPointer<Document> document(ugenedbFormat->createNewLoadedDocument(IOAdapterUtils::get(IOAdapterUtils::url2io(resultUrl)), resultUrl, stateInfo));
        CHECK_OP(stateInfo, result);

        document->setDocumentOwnsDbiResources(false);

        MultipleChromatogramAlignmentObject* mcaObject = composeSubTask->takeMcaObject();
        SAFE_POINT_EXT(mcaObject != nullptr, setError("Result MCA object is NULL"), result);
        document->addObject(mcaObject);

        U2SequenceObject* referenceSequenceObject = composeSubTask->takeReferenceSequenceObject();
        SAFE_POINT_EXT(referenceSequenceObject != nullptr, setError("Result reference sequence object is NULL"), result);
        document->addObject(referenceSequenceObject);

        mcaObject->addObjectRelation(GObjectRelation(GObjectReference(referenceSequenceObject), ObjectRole_ReferenceSequence));

        saveTask = new SaveDocumentTask(document.take(), SaveDocFlags(SaveDoc_DestroyAfter) | SaveDoc_Roll);
        result << saveTask;
    }
    return result;
}

Task::ReportResult AlignToReferenceBlastTask::report() {
    if (formatDbSubTask != nullptr && !formatDbSubTask->getResultPath().isEmpty()) {
        QFileInfo(formatDbSubTask->getResultPath()).dir().removeRecursively();
    }
    return ReportResult_Finished;
}

QString AlignToReferenceBlastTask::generateReport() const {
    QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, reference));
    CHECK(refObject != nullptr, "");

    const QList<QPair<QString, QPair<int, bool>>> acceptedReads = getAcceptedReads();
    const QList<QPair<QString, int>> filteredReads = getDiscardedReads();

    QString result = "<br><table><tr><td><b>" + tr("Details") + "</b></td></tr></table>\n";
    result += "<u>" + tr("Reference sequence:") + QString("</u> %1<br>").arg(refObject->getSequenceName());
    result += "<u>" + tr("Mapped reads (%1):").arg(acceptedReads.size()) + "</u>";
    result += "<table>";
    for (const auto& acceptedPair : qAsConst(acceptedReads)) {
        QString arrow = acceptedPair.second.second ? "&#x2190;" : "&#x2192;";
        QString read = QString(acceptedPair.first).replace("-", "&#8209;");
        QString readIdentity = tr("similarity") + "&nbsp;&nbsp;" + QString::number(acceptedPair.second.first) + "%";
        result += "<tr><td align=right width=50>" + arrow + QString("</td><td><nobr>") + read + "</nobr></td><td><div style=\"margin-left:7px;\">" + readIdentity + "</div></td></tr>";
    }
    if (!filteredReads.isEmpty()) {
        result += "<tr><td colspan=3><u>" + tr("Filtered by low similarity (%1):").arg(filteredReads.size()) + "</u></td></tr>";
        for (const auto& filteredPair : qAsConst(filteredReads)) {
            QString readIdentity = tr("similarity") + "&nbsp;&nbsp;" + QString::number(filteredPair.second) + "%";
            result += QString("<tr><td></td><td style=white-space:nowrap>") + QString(filteredPair.first).replace("-", "&#8209;") + "&nbsp; &nbsp;" + "</td><td><div style=\"margin-left:7px;\">" + readIdentity + "</div></td></tr>";
        }
    }
    result += "</table>";

    return result;
}

QString AlignToReferenceBlastTask::getResultUrl() const {
    CHECK(saveTask != nullptr, "");
    return saveTask->getURL().getURLString();
}

SharedDbiDataHandler AlignToReferenceBlastTask::getAnnotations() const {
    CHECK(composeSubTask != nullptr, {});
    return composeSubTask->getAnnotations();
}

QList<QPair<QString, QPair<int, bool>>> AlignToReferenceBlastTask::getAcceptedReads() const {
    SAFE_POINT(blastTask != nullptr, "Task is not finished!", {});
    QList<QPair<QString, QPair<int, bool>>> acceptedReads;
    const QList<AlignToReferenceResult>& alignmentResults = blastTask->getAlignmentResults();
    for (const auto& alignmentResult : qAsConst(alignmentResults)) {
        if (alignmentResult.identityPercent >= minIdentityPercent) {
            QPair<int, bool> identityAndStrandPair(alignmentResult.identityPercent, alignmentResult.isOnComplementaryStrand);
            acceptedReads.append({alignmentResult.readName, identityAndStrandPair});
        }
    }
    return acceptedReads;
}

QList<QPair<QString, int>> AlignToReferenceBlastTask::getDiscardedReads() const {
    SAFE_POINT(blastTask != nullptr, "Task is not finished!", {});
    QList<QPair<QString, int>> discardedReads;
    const QList<AlignToReferenceResult>& alignmentResults = blastTask->getAlignmentResults();
    for (const auto& alignmentResult : qAsConst(alignmentResults)) {
        if (alignmentResult.identityPercent < minIdentityPercent) {
            discardedReads.append({alignmentResult.readName, alignmentResult.identityPercent});
        }
    }
    return discardedReads;
}

}  // namespace LocalWorkflow
}  // namespace U2
