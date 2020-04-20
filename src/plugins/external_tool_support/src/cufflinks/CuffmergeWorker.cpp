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

#include "CufflinksSupport.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/L10n.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "CuffmergeWorker.h"

namespace U2 {
namespace LocalWorkflow {

/*****************************
 * CuffmergeWorkerFactory
 *****************************/
const QString CuffmergeWorkerFactory::ACTOR_ID("cuffmerge");

const QString OUT_DIR("out-dir");
const QString MIN_ISOFORM_FRACTION("min-isoform-fraction");
const QString REF_ANNOTATION("ref-annotation");
const QString REF_SEQ("ref-seq");
const QString CUFFCOMPARE_TOOL_PATH("cuffcompare-tool-path");
const QString EXT_TOOL_PATH("path");
const QString TMP_DIR_PATH("tmp-dir");

void CuffmergeWorkerFactory::init()
{
    // Description of the element
    Descriptor cuffmergeDescriptor(ACTOR_ID,
        CuffmergeWorker::tr("Merge Assemblies with Cuffmerge"),
        CuffmergeWorker::tr("Cuffmerge merges together several assemblies."
        " It also handles running Cuffcompare for you, and automatically"
        " filters a number of transfrags that are probably artifacts."
        " If you have a reference file available, you can provide it"
        " to Cuffmerge in order to gracefully merge input (e.g. novel) isoforms and"
        " known isoforms and maximize overall assembly quality."));

    QList<Attribute*> attributes;
    { // Define parameters of the element
        Descriptor outDir(OUT_DIR,
            CuffmergeWorker::tr("Output folder"),
            CuffmergeWorker::tr("The base name of output folder. It could be modified with a suffix."));

        Descriptor refAnnotation(REF_ANNOTATION,
            CuffmergeWorker::tr("Reference annotation"),
            CuffmergeWorker::tr("Merge the input assemblies together with"
            " this reference annotation."));

        Descriptor refSeq(REF_SEQ,
            CuffmergeWorker::tr("Reference sequence"),
            CuffmergeWorker::tr("The genomic DNA sequences for the reference."
            " It is used to assist in classifying transfrags and excluding"
            " artifacts (e.g. repeats). For example, transcripts consisting"
            " mostly of lower-case bases are classified as repeats."));

        Descriptor minIso(MIN_ISOFORM_FRACTION,
            CuffmergeWorker::tr("Minimum isoform fraction"),
            CuffmergeWorker::tr("Discard isoforms with abundance below this."));

        Descriptor cuffcompareToolPath(CUFFCOMPARE_TOOL_PATH,
            CuffmergeWorker::tr("Cuffcompare tool path"),
            CuffmergeWorker::tr("The path to the Cuffcompare external tool in UGENE."));

        Descriptor extToolPath(EXT_TOOL_PATH,
            CuffmergeWorker::tr("Cuffmerge tool path"),
            CuffmergeWorker::tr("The path to the Cuffmerge external tool in UGENE."));

        Descriptor tmpDir(TMP_DIR_PATH,
            CuffmergeWorker::tr("Temporary folder"),
            CuffmergeWorker::tr("The folder for temporary files."));

        attributes << new Attribute(outDir, BaseTypes::STRING_TYPE(), true, "");
        attributes << new Attribute(refAnnotation, BaseTypes::STRING_TYPE(), false, QVariant(""));
        attributes << new Attribute(refSeq, BaseTypes::STRING_TYPE(), false, QVariant(""));
        attributes << new Attribute(minIso, BaseTypes::NUM_TYPE(), false, QVariant(0.05));
        attributes << new Attribute(cuffcompareToolPath, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));
        attributes << new Attribute(extToolPath, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));
        attributes << new Attribute(tmpDir, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));
    }

    QList<PortDescriptor*> portDescriptors;
    { // Define ports of the element
        Descriptor inDesc(BasePorts::IN_ANNOTATIONS_PORT_ID(),
            CuffmergeWorker::tr("Set of annotations"),
            CuffmergeWorker::tr("Annotations for merging"));
        Descriptor outDesc(BasePorts::OUT_ANNOTATIONS_PORT_ID(),
            CuffmergeWorker::tr("Set of annotations"),
            CuffmergeWorker::tr("Merged annotations"));

        QMap<Descriptor, DataTypePtr> inTypeMap;
        inTypeMap[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();
        DataTypePtr inType(new MapDataType(inDesc.getId(), inTypeMap));

        QMap<Descriptor, DataTypePtr> outTypeMap;
        outTypeMap[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        DataTypePtr outType(new MapDataType(outDesc.getId(), outTypeMap));

        portDescriptors << new PortDescriptor(inDesc, inType, true, false, IntegralBusPort::BLIND_INPUT);
        portDescriptors << new PortDescriptor(outDesc, outType, false, true);
    }

    // Create the actor prototype
    ActorPrototype* proto = new IntegralBusActorPrototype(cuffmergeDescriptor,
        portDescriptors,
        attributes);

    // Values range of some parameters
    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[OUT_DIR] = new URLDelegate("", "", false, true /*path*/);
        delegates[REF_ANNOTATION] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", false, false, false);
        delegates[REF_SEQ] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", false, false, false);
        QVariantMap vm;
        vm["minimum"] = 0.0;
        vm["maximum"] = 1.0;
        delegates[MIN_ISOFORM_FRACTION] = new DoubleSpinBoxDelegate(vm);
        delegates[CUFFCOMPARE_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
        delegates[EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
        delegates[TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);
    }

    // Init and register the actor prototype
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new CuffmergePrompter());

    { // external tools
        proto->addExternalTool(CufflinksSupport::ET_CUFFMERGE_ID, EXT_TOOL_PATH);
        proto->addExternalTool(CufflinksSupport::ET_CUFFCOMPARE_ID, CUFFCOMPARE_TOOL_PATH);
    }

    WorkflowEnv::getProtoRegistry()->registerProto(
        BaseActorCategories::CATEGORY_RNA_SEQ(),
        proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new CuffmergeWorkerFactory());
}


/*****************************
 * CuffmergePrompter
 *****************************/
CuffmergePrompter::CuffmergePrompter(Actor* parent)
    : PrompterBase<CuffmergePrompter>(parent)
{
}


QString CuffmergePrompter::composeRichDoc()
{
    QString result = CuffmergeWorker::tr("Merges together supplied assemblies.");

    return result;
}


/*****************************
 * CuffmergeWorker
 *****************************/
CuffmergeWorker::CuffmergeWorker(Actor* actor)
    : BaseWorker(actor, false /*autoTransit*/),
      input(NULL),
      output(NULL)
{

}

void CuffmergeWorker::init() {
    WorkflowUtils::updateExternalToolPath(CufflinksSupport::ET_CUFFMERGE_ID, getValue<QString>(EXT_TOOL_PATH));
    WorkflowUtils::updateExternalToolPath(CufflinksSupport::ET_CUFFCOMPARE_ID, getValue<QString>(CUFFCOMPARE_TOOL_PATH));

    input = ports[BasePorts::IN_ANNOTATIONS_PORT_ID()];
    output = ports[BasePorts::OUT_ANNOTATIONS_PORT_ID()];
}

Task * CuffmergeWorker::tick() {
    while (input->hasMessage()) {
        takeAnnotations();
    }
    if (input->isEnded()) {
        Task *t = createCuffmergeTask();
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }
    return NULL;
}

void CuffmergeWorker::sl_taskFinished() {
    CuffmergeSupportTask *task = dynamic_cast<CuffmergeSupportTask *>(sender());
    if (!task->isFinished() || task->isCanceled() || task->hasError()) {
        return;
    }

    QVariantMap data;
    QList<AnnotationTableObject *> annTables = task->takeResult();
    data[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = QVariant::fromValue(context->getDataStorage()->putAnnotationTables(annTables));
    Message m(output->getBusType(), data);
    output->put(m);
    qDeleteAll(annTables);

    output->setEnded();
    foreach (const QString &url, task->getOutputFiles()) {
        context->getMonitor()->addOutputFile(url, getActor()->getId());
    }
    setDone();
}

void CuffmergeWorker::cleanup() {
    annTableHandlers.clear();
}

void CuffmergeWorker::takeAnnotations() {
    Message m = getMessageAndSetupScriptValues(input);
    QVariantMap data = m.getData().toMap();
    SAFE_POINT(data.contains(BaseSlots::ANNOTATION_TABLE_SLOT().getId()), "No annotations in a message",);
    QVariant annsVar = data[BaseSlots::ANNOTATION_TABLE_SLOT().getId()];
    annTableHandlers << StorageUtils::getAnnotationTableHandlers(annsVar);
}

CuffmergeSettings CuffmergeWorker::scanParameters() const {
    CuffmergeSettings result;
    result.outDir = getValue<QString>(OUT_DIR);
    result.minIsoformFraction = getValue<double>(MIN_ISOFORM_FRACTION);
    result.refAnnsUrl = getValue<QString>(REF_ANNOTATION);
    result.refSeqUrl = getValue<QString>(REF_SEQ);
    result.workingDir = getValue<QString>(TMP_DIR_PATH);
    return result;
}

Task * CuffmergeWorker::createCuffmergeTask() {
    CuffmergeSettings result = scanParameters();
    result.storage = context->getDataStorage();
    result.annotationTables = annTableHandlers;
    CuffmergeSupportTask* supportTask = new CuffmergeSupportTask(result);
    supportTask->addListeners(createLogListeners());
    return supportTask;
}

} // namespace LocalWorkflow
} // namespace U2
