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

#include "CufflinksSupportTask.h"
#include "CufflinksWorker.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

namespace U2 {
namespace LocalWorkflow {

/************************************************************************/
/* Validator */
/************************************************************************/
class InputSlotValidator : public PortValidator {
public:
    virtual bool validate(const IntegralBusPort *port, NotificationsList &notificationList) const {
        bool data = isBinded(port, BaseSlots::ASSEMBLY_SLOT().getId());
        bool url = isBinded(port, BaseSlots::URL_SLOT().getId());

        QString dataName = slotName(port, BaseSlots::ASSEMBLY_SLOT().getId());
        QString urlName = slotName(port, BaseSlots::URL_SLOT().getId());
        if (!data && !url) {
            notificationList.append(WorkflowNotification(IntegralBusPort::tr("One of these slots must be not empty: '%1', '%2'").arg(dataName).arg(urlName)));
            return false;
        }

        if (data && url) {
            notificationList.append(WorkflowNotification(IntegralBusPort::tr("Only one of these slots must be binded: '%1', '%2'").arg(dataName).arg(urlName)));
            return false;
        }
        return true;
    }
};

/*****************************
 * CufflinksWorkerFactory
 *****************************/
const QString CufflinksWorkerFactory::ACTOR_ID("cufflinks");

const QString CufflinksWorkerFactory::OUT_DIR("out-dir");
const QString CufflinksWorkerFactory::REF_ANNOTATION("ref-annotation");
const QString CufflinksWorkerFactory::RABT_ANNOTATION("rabt-annotation");
const QString CufflinksWorkerFactory::LIBRARY_TYPE("library-type");
const QString CufflinksWorkerFactory::MASK_FILE("mask-file");
const QString CufflinksWorkerFactory::MULTI_READ_CORRECT("multi-read-correct");
const QString CufflinksWorkerFactory::MIN_ISOFORM_FRACTION("min-isoform-fraction");
const QString CufflinksWorkerFactory::FRAG_BIAS_CORRECT("frag-bias-correct");
const QString CufflinksWorkerFactory::PRE_MRNA_FRACTION("pre-mrna-fraction");
const QString CufflinksWorkerFactory::EXT_TOOL_PATH("path");
const QString CufflinksWorkerFactory::TMP_DIR_PATH("tmp-dir");

const QString CufflinksWorkerFactory::OUT_MAP_DESCR_ID("out.annotations");
const QString CufflinksWorkerFactory::ISO_LEVEL_SLOT_DESCR_ID("isolevel.slot");

void CufflinksWorkerFactory::init()
{
    QList<PortDescriptor*> portDescriptors;
    QList<Attribute*> attributes;

    // Define ports and slots
    Descriptor inputPortDescriptor(BasePorts::IN_ASSEMBLY_PORT_ID(),
        CufflinksWorker::tr("Input reads"),
        CufflinksWorker::tr("Input RNA-Seq read alignments."));

    Descriptor outputPortDescriptor(BasePorts::OUT_ANNOTATIONS_PORT_ID(),
        CufflinksWorker::tr("Output annotations"),
        CufflinksWorker::tr("Assembled isoforms, estimated isoform-level expression values,"
        " and estimated gene-level expression values, produced by Cufflinks."));

    QMap<Descriptor, DataTypePtr> inputMap;
    inputMap[BaseSlots::ASSEMBLY_SLOT()] = BaseTypes::ASSEMBLY_TYPE();
    inputMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
    portDescriptors << new PortDescriptor(inputPortDescriptor,
        DataTypePtr(new MapDataType("in.assembly", inputMap)),
        true /* input */);

    QMap<Descriptor, DataTypePtr> outputMap;

    Descriptor isoformLevelExprDescriptor(ISO_LEVEL_SLOT_DESCR_ID,
        CufflinksWorker::tr("Isoform-level expression values"),
        CufflinksWorker::tr("A set of annotated regions"));

    outputMap[isoformLevelExprDescriptor] = BaseTypes::ANNOTATION_TABLE_TYPE();
    DataTypeRegistry* registry = WorkflowEnv::getDataTypeRegistry();
    assert(registry);

    DataTypePtr mapDataType(new MapDataType(OUT_MAP_DESCR_ID, outputMap));

    registry->registerEntry(mapDataType);

    portDescriptors << new PortDescriptor(outputPortDescriptor,
        mapDataType,
        false /* input */,
        true /* multi */);

    // Description of the element
    Descriptor cufflinksDescriptor(ACTOR_ID,
        CufflinksWorker::tr("Assembly Transcripts with Cufflinks"),
        CufflinksWorker::tr("Cufflinks accepts aligned RNA-Seq reads"
        " and assembles the alignments into a parsimonious set of"
        " transcripts. Cufflinks then estimates the relative abundances"
        " of these transcripts based on how many reads support each one,"
        " taking into account biases in library preparation protocols."));

    // Define parameters of the element
    Descriptor outDir(OUT_DIR,
        CufflinksWorker::tr("Output folder"),
        CufflinksWorker::tr("The base name of output folder. It could be modified with a suffix."));

    Descriptor refAnnotation(REF_ANNOTATION,
        CufflinksWorker::tr("Reference annotation"),
        CufflinksWorker::tr("Tells Cufflinks to use the supplied reference"
        " annotation to estimate isoform expression. Cufflinks will not"
        " assemble novel transcripts and the program will ignore alignments"
        " not structurally compatible with any reference transcript."));

    Descriptor rabtAnnotation(RABT_ANNOTATION,
        CufflinksWorker::tr("RABT annotation"),
        CufflinksWorker::tr("Tells Cufflinks to use the supplied reference"
        " annotation to guide Reference Annotation Based Transcript (RABT) assembly."
        " Reference transcripts will be tiled with faux-reads to provide additional"
        " information in assembly. Output will include all reference transcripts"
        " as well as any novel genes and isoforms that are assembled."));

    Descriptor libraryType(LIBRARY_TYPE,
        CufflinksWorker::tr("Library type"),
        CufflinksWorker::tr("Specifies RNA-Seq protocol."));

    Descriptor maskFile(MASK_FILE,
        CufflinksWorker::tr("Mask file"),
        CufflinksWorker::tr("Ignore all reads that could have come from transcripts"
        " in this file. It is recommended to include any annotated rRNA, mitochondrial"
        " transcripts or other abundant transcripts you wish to ignore in your analysis"
        " in this file. Due to variable efficiency of mRNA enrichment methods and rRNA"
        " depletion kits, masking these transcripts often improves the overall robustness"
        " of transcript abundance estimates."));

    Descriptor multiReadCorrect(MULTI_READ_CORRECT,
        CufflinksWorker::tr("Multi-read correct"),
        CufflinksWorker::tr("Tells Cufflinks to do an initial estimation procedure to more"
        " accurately weight reads mapping to multiple locations in the genome."));

    Descriptor minIsoformFraction(MIN_ISOFORM_FRACTION,
        CufflinksWorker::tr("Min isoform fraction"),
        CufflinksWorker::tr("After calculating isoform abundance for a gene, Cufflinks"
        " filters out transcripts that it believes are very low abundance, because"
        " isoforms expressed at extremely low levels often cannot reliably be assembled,"
        " and may even be artifacts of incompletely spliced precursors of processed transcripts."
        " This parameter is also used to filter out introns that have far fewer spliced"
        " alignments supporting them."));

    Descriptor fragBiasCorrect(FRAG_BIAS_CORRECT,
        CufflinksWorker::tr("Frag bias correct"),
        CufflinksWorker::tr("Providing Cufflinks with a multifasta file via this option"
        " instructs it to run the bias detection and correction algorithm which can"
        " significantly improve accuracy of transcript abundance estimates."));

    Descriptor preMrnaFraction(PRE_MRNA_FRACTION,
        CufflinksWorker::tr("Pre-mRNA fraction"),
        CufflinksWorker::tr("Some RNA-Seq protocols produce a significant amount of reads"
        " that originate from incompletely spliced transcripts, and these reads can"
        " confound the assembly of fully spliced mRNAs. Cufflinks uses this parameter"
        " to filter out alignments that lie within the intronic intervals implied"
        " by the spliced alignments. The minimum depth of coverage in the intronic"
        " region covered by the alignment is divided by the number of spliced reads,"
        " and if the result is lower than this parameter value, the intronic"
        " alignments are ignored."));

    Descriptor extToolPath(EXT_TOOL_PATH,
        CufflinksWorker::tr("Cufflinks tool path"),
        CufflinksWorker::tr("The path to the Cufflinks external tool in UGENE."));

    Descriptor tmpDir(TMP_DIR_PATH,
        CufflinksWorker::tr("Temporary folder"),
        CufflinksWorker::tr("The folder for temporary files."));

    attributes << new Attribute(outDir, BaseTypes::STRING_TYPE(), true, "");
    attributes << new Attribute(refAnnotation, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(rabtAnnotation, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(libraryType, BaseTypes::NUM_TYPE(), false, QVariant(0));
    attributes << new Attribute(maskFile, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(multiReadCorrect, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    attributes << new Attribute(minIsoformFraction, BaseTypes::NUM_TYPE(), false, QVariant(0.1));
    attributes << new Attribute(fragBiasCorrect, BaseTypes::STRING_TYPE(), false, QVariant(""));
    attributes << new Attribute(preMrnaFraction, BaseTypes::NUM_TYPE(), false, QVariant(0.15));
    attributes << new Attribute(extToolPath, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));
    attributes << new Attribute(tmpDir, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));

    // Create the actor prototype
    ActorPrototype* proto = new IntegralBusActorPrototype(cufflinksDescriptor,
        portDescriptors,
        attributes);

    // Values range of some parameters
    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap vm;
        vm["Standard Illumina"] = 0;
        vm["dUTP, NSR, NNSR"] = 1;
        vm["Ligation, Standard SOLiD"] = 2;
        delegates[LIBRARY_TYPE] = new ComboBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0.0;
        vm["maximum"] = 1.0;
        vm["singleStep"] = 0.1;
        delegates[MIN_ISOFORM_FRACTION] = new DoubleSpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0.0;
        vm["maximum"] = 1.0;
        vm["singleStep"] = 0.05;
        delegates[PRE_MRNA_FRACTION] = new DoubleSpinBoxDelegate(vm);
    }

    delegates[OUT_DIR] = new URLDelegate("", "", false, true /*path*/);
    delegates[REF_ANNOTATION] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", false, false, false);
    delegates[RABT_ANNOTATION] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", false, false, false);
    delegates[MASK_FILE] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", false, false, false);
    delegates[FRAG_BIAS_CORRECT] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", false, false, false);
    delegates[EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
    delegates[TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);

    // Init and register the actor prototype
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new CufflinksPrompter());
    proto->setPortValidator(BasePorts::IN_ASSEMBLY_PORT_ID(), new InputSlotValidator());

    { // external tools
        proto->addExternalTool(CufflinksSupport::ET_CUFFLINKS_ID, EXT_TOOL_PATH);
    }

    WorkflowEnv::getProtoRegistry()->registerProto(
        BaseActorCategories::CATEGORY_RNA_SEQ(),
        proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new CufflinksWorkerFactory());
}


/*****************************
 * CufflinksPrompter
 *****************************/
CufflinksPrompter::CufflinksPrompter(Actor* parent)
    : PrompterBase<CufflinksPrompter>(parent)
{
}


QString CufflinksPrompter::composeRichDoc()
{
    QString result = "Assembles transcripts and estimates their abundances.";

    return result;
}


/*****************************
 * CufflinksWorker
 *****************************/
CufflinksWorker::CufflinksWorker(Actor* actor)
    : BaseWorker(actor),
      input(NULL),
      output(NULL),
      settingsAreCorrect(false)
{
}

void CufflinksWorker::initSlotsState() {
    Port *port = actor->getPort(BasePorts::IN_ASSEMBLY_PORT_ID());
    IntegralBusPort *bus = dynamic_cast<IntegralBusPort*>(port);
    settings.fromFile = bus->getProducers(BaseSlots::ASSEMBLY_SLOT().getId()).isEmpty();
}

void CufflinksWorker::init() {
    input = ports.value(BasePorts::IN_ASSEMBLY_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());

    initSlotsState();

    // Init the parameters
    settingsAreCorrect = true;
    QString extToolPath = getValue<QString>(CufflinksWorkerFactory::EXT_TOOL_PATH);
    if (QString::compare(extToolPath, "default", Qt::CaseInsensitive) != 0) {
        AppContext::getExternalToolRegistry()->getById(CufflinksSupport::ET_CUFFLINKS_ID)->setPath(extToolPath);
    }

    QString tmpDirPath = getValue<QString>(CufflinksWorkerFactory::TMP_DIR_PATH);
    if (QString::compare(tmpDirPath, "default", Qt::CaseInsensitive) != 0) {
        AppContext::getAppSettings()->getUserAppsSettings()->setUserTemporaryDirPath(tmpDirPath);
    }

    settings.outDir = getValue<QString>(CufflinksWorkerFactory::OUT_DIR);
    settings.referenceAnnotation = getValue<QString>(CufflinksWorkerFactory::REF_ANNOTATION);
    settings.rabtAnnotation = getValue<QString>(CufflinksWorkerFactory::RABT_ANNOTATION);

    int libType = getValue<int>(CufflinksWorkerFactory::LIBRARY_TYPE);
    if (!settings.libraryType.setLibraryType(libType)) {
        algoLog.error(tr("Incorrect value of the library type parameter for Cufflinks!"));
        settingsAreCorrect = false;
    }

    settings.maskFile = getValue<QString>(CufflinksWorkerFactory::MASK_FILE);
    settings.multiReadCorrect = getValue<bool>(CufflinksWorkerFactory::MULTI_READ_CORRECT);
    settings.minIsoformFraction = getValue<double>(CufflinksWorkerFactory::MIN_ISOFORM_FRACTION);
    settings.fragBiasCorrect = getValue<QString>(CufflinksWorkerFactory::FRAG_BIAS_CORRECT);
    settings.preMrnaFraction = getValue<double>(CufflinksWorkerFactory::PRE_MRNA_FRACTION);
    settings.storage = context->getDataStorage();
}

Task * CufflinksWorker::tick() {
    if (false == settingsAreCorrect) {
        return NULL;
    }

    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        SAFE_POINT(!inputMessage.isEmpty(), "Internal error: message can't be NULL!", NULL);
        QVariantMap data = inputMessage.getData().toMap();

        if (settings.fromFile) {
            settings.url = data[BaseSlots::URL_SLOT().getId()].toString();
        } else {
            settings.assemblyId = data[BaseSlots::ASSEMBLY_SLOT().getId()].value<SharedDbiDataHandler>();
        }

        // Create the task
        CufflinksSupportTask* cufflinksSupportTask = new CufflinksSupportTask(settings);
        cufflinksSupportTask->addListeners(createLogListeners());
        connect(cufflinksSupportTask, SIGNAL(si_stateChanged()), SLOT(sl_cufflinksTaskFinished()));

        return cufflinksSupportTask;
    }
    else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }

    return NULL;
}

void CufflinksWorker::sl_cufflinksTaskFinished() {
    CufflinksSupportTask* cufflinksSupportTask = qobject_cast<CufflinksSupportTask*>(sender());
    CHECK(cufflinksSupportTask->isFinished(), );

    if (NULL != output) {
        DataTypePtr outputMapDataType = WorkflowEnv::getDataTypeRegistry()->getById(CufflinksWorkerFactory::OUT_MAP_DESCR_ID);
        SAFE_POINT(0 != outputMapDataType, "Internal error: can't get DataTypePtr for output map!",);

        QVariantMap messageData;
        QList<AnnotationTableObject *> isoformTables = cufflinksSupportTask->getIsoformAnnotationTables();
        messageData[CufflinksWorkerFactory::ISO_LEVEL_SLOT_DESCR_ID] = QVariant::fromValue(context->getDataStorage()->putAnnotationTables(isoformTables));
        output->put(Message(outputMapDataType, messageData));
        qDeleteAll(isoformTables);

        foreach (const QString &url, cufflinksSupportTask->getOutputFiles()) {
            context->getMonitor()->addOutputFile(url, getActor()->getId());
        }
    }
}

void CufflinksWorker::cleanup() {

}

} // namespace LocalWorkflow
} // namespace U2
