/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include "GenomeAlignerWorker.h"

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/FailTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Algorithm/OpenCLGpuRegistry.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Gui/DialogUtils.h>
#include <U2Lang/WorkflowMonitor.h>

#include "GenomeAlignerPlugin.h"

namespace U2 {
namespace LocalWorkflow {


static const QString IN_TYPE_ID("Bowtie2-data");
static const QString OUT_TYPE_ID("Bowtie2-data-out");
static const QString IN_PORT_DESCR("in-data");
static const QString OUT_PORT_DESCR("out-data");
static const QString INDEX_PORT_ID("in-gen-al-index");
static const QString INDEX_OUT_PORT_ID("out-gen-al-index");
static const QString OUTPUT_DIR("output-dir");
static const QString OUTPUT_NAME = "outname";

static const QString READS_URL_SLOT_ID("readsurl");
static const QString READS_PAIRED_URL_SLOT_ID("readspairedurl");

static const QString ASSEBLY_OUT_SLOT_ID("assembly-out");

static const QString BASE_GENOME_ALIGNER_SUBDIR("genome_aligner");
static const QString BASE_GENOME_ALIGNER_OUTFILE("out.sam");


const QString GenomeAlignerWorkerFactory::ACTOR_ID("genome-aligner");

static const QString ABS_OR_PERC_MISMATCHES_ATTR("if-absolute-mismatches-value");
static const QString MISMATCHES_ATTR("absolute-mismatches");
static const QString PERCENT_MISMATCHES_ATTR("percentage-mismatches");
static const QString REVERSE_ATTR("reverse");
static const QString BEST_ATTR("best");
static const QString GPU_ATTR("gpu");
static const QString QUAL_ATTR("quality-threshold");
static const QString REFERENCE_GENOME("reference");
static const QString REFERENCE_INPUT_TYPE = "reference-input-type";
static const QString INDEX_DIR("index-dir");
static const QString INDEX_BASENAME("index-basename");

/************************************************************************/
/* Genome aligner worker                                                */
/************************************************************************/

GenomeAlignerWorker::GenomeAlignerWorker(Actor* a)
: BaseWorker(a, false), inChannel(NULL), output(NULL)
{
}

void GenomeAlignerWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
    output = ports.value(OUT_PORT_DESCR);
}

Task* GenomeAlignerWorker::tick() {
    if (inChannel->hasMessage()) {
        U2OpStatus2Log os;
        if(inChannel->isEnded()) {
            algoLog.error(GenomeAlignerWorker::tr("Short reads list is empty."));
            return NULL;
        }
        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        //settings.indexFileName = getValue<QString>(REFERENCE_GENOME);
        DnaAssemblyToRefTaskSettings settings = getSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }
        QString readsUrl = data[READS_URL_SLOT_ID].toString();
        if(data.contains(READS_PAIRED_URL_SLOT_ID)){
            //paired
            QString readsPairedUrl = data[READS_PAIRED_URL_SLOT_ID].toString();
            settings.shortReadSets.append(ShortReadSet(readsUrl, ShortReadSet::PairedEndReads, ShortReadSet::UpstreamMate));
            settings.shortReadSets.append(ShortReadSet(readsPairedUrl, ShortReadSet::PairedEndReads, ShortReadSet::DownstreamMate));
            settings.pairedReads = true;
        }else {
            //single
            settings.shortReadSets.append(ShortReadSet(readsUrl, ShortReadSet::SingleEndReads, ShortReadSet::UpstreamMate));
            settings.pairedReads = false;
        }

        Task* t = new GenomeAlignerTask(settings);
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (inChannel->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}


void GenomeAlignerWorker::cleanup() {
}

void GenomeAlignerWorker::sl_taskFinished() {
    GenomeAlignerTask *t = dynamic_cast<GenomeAlignerTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    QString url = t->getSettings().resultFileName.getURLString();

    QVariantMap data;
    data[ASSEBLY_OUT_SLOT_ID] =  qVariantFromValue<QString>(url);
    output->put(Message(output->getBusType(), data));

    context->getMonitor()->addOutputFile(url, getActor()->getId());

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
        output->setEnded();
    }
}

DnaAssemblyToRefTaskSettings GenomeAlignerWorker::getSettings(U2OpStatus &os) {
    DnaAssemblyToRefTaskSettings settings;

    QString referenceInputType = getValue<QString>(REFERENCE_INPUT_TYPE);
    if (referenceInputType == DnaAssemblyToRefTaskSettings::INDEX) {
        settings.prebuiltIndex = true;
        settings.indexDir = getValue<QString>(INDEX_DIR);
        settings.indexBasename = getValue<QString>(INDEX_BASENAME);

        QString baseUrl = QDir(settings.indexDir).filePath(settings.indexBasename);
        settings.refSeqUrl = baseUrl;
        settings.indexFileName = baseUrl;
    } else {
        settings.refSeqUrl = getValue<QString>(REFERENCE_GENOME);
        settings.prebuiltIndex = false;

        settings.indexDir = "";
        settings.indexBasename = "";
        settings.indexFileName = QDir(settings.refSeqUrl.dirPath()).filePath(settings.refSeqUrl.baseFileName());
    }

    QString outDir = GUrlUtils::createDirectory(
        getValue<QString>(OUTPUT_DIR) + QDir::separator() + BASE_GENOME_ALIGNER_SUBDIR,
        "_", os);
    CHECK_OP(os, settings);

    if (!outDir.endsWith(QDir::separator())){
        outDir  = outDir + QDir::separator();
    }

    QString outFileName = getValue<QString>(OUTPUT_NAME);
    if(outFileName.isEmpty()){
        outFileName = BASE_GENOME_ALIGNER_OUTFILE;
    }
    settings.resultFileName = outDir + outFileName;

    bool absMismatches = actor->getParameter(ABS_OR_PERC_MISMATCHES_ATTR)->getAttributeValue<bool>(context);
    settings.setCustomValue(GenomeAlignerTask::OPTION_IF_ABS_MISMATCHES, absMismatches);
    int nMismatches = actor->getParameter(MISMATCHES_ATTR)->getAttributeValue<int>(context);
    settings.setCustomValue(GenomeAlignerTask::OPTION_MISMATCHES, nMismatches);
    int ptMismatches = actor->getParameter(PERCENT_MISMATCHES_ATTR)->getAttributeValue<int>(context);
    settings.setCustomValue(GenomeAlignerTask::OPTION_PERCENTAGE_MISMATCHES, ptMismatches);
    bool alignReverse = actor->getParameter(REVERSE_ATTR)->getAttributeValue<bool>(context);
    settings.setCustomValue(GenomeAlignerTask::OPTION_ALIGN_REVERSED, alignReverse);
    bool best = actor->getParameter(BEST_ATTR)->getAttributeValue<bool>(context);
    settings.setCustomValue(GenomeAlignerTask::OPTION_BEST, best);
    int qual = actor->getParameter(QUAL_ATTR)->getAttributeValue<int>(context);
    settings.setCustomValue(GenomeAlignerTask::OPTION_QUAL_THRESHOLD, qual);
    if(GenomeAlignerWorkerFactory::openclEnabled) {
        bool gpu = actor->getParameter(GPU_ATTR)->getAttributeValue<bool>(context);
        settings.setCustomValue(GenomeAlignerTask::OPTION_OPENCL, gpu);
    }
    return settings;
}

QString GenomeAlignerPrompter::composeRichDoc() {
    QString res = "";

    Actor* readsProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(READS_URL_SLOT_ID);

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString readsUrl = readsProducer ? readsProducer->getLabel() : unsetStr;

    res.append(tr("Maps input reads from <u>%1</u> ").arg(readsUrl));

    QVariant inputType = getParameter(REFERENCE_INPUT_TYPE);
    if (inputType == DnaAssemblyToRefTaskSettings::INDEX) {
        QString baseName = getHyperlink(INDEX_BASENAME, getURL(INDEX_BASENAME));
        res.append(tr(" to reference sequence with index <u>%1</u>.").arg(baseName));
    } else {
        QString genome = getHyperlink(REFERENCE_GENOME, getURL(REFERENCE_GENOME));
        res.append(tr(" to reference sequence <u>%1</u>.").arg(genome));
    }

    return res;
}


/************************************************************************/
/* Factory */
/************************************************************************/
bool GenomeAlignerWorkerFactory::openclEnabled(false);

class GenomeAlignerInputSlotsValidator : public PortValidator {
public:

    bool validate(const IntegralBusPort *port, NotificationsList &notificationList) const {
        QVariant busMap = port->getParameter(Workflow::IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributePureValue();
        bool data = isBinded(busMap.value<StrStrMap>(), READS_URL_SLOT_ID);
        if (!data){
            QString dataName = slotName(port, READS_URL_SLOT_ID);
            notificationList.append(WorkflowNotification(GenomeAlignerWorker::tr("The slot must be not empty: '%1'").arg(dataName)));
            return false;
        }

        QString slot1Val = busMap.value<StrStrMap>().value(READS_URL_SLOT_ID);
        QString slot2Val = busMap.value<StrStrMap>().value(READS_PAIRED_URL_SLOT_ID);
        U2OpStatusImpl os;
        const QList<IntegralBusSlot>& slots1 = IntegralBusSlot::listFromString(slot1Val, os);
        const QList<IntegralBusSlot>& slots2 = IntegralBusSlot::listFromString(slot2Val, os);

        bool hasCommonElements = false;

        foreach(const IntegralBusSlot& ibsl1, slots1){
            if (hasCommonElements){
                break;
            }
            foreach(const IntegralBusSlot& ibsl2, slots2){
                if (ibsl1 == ibsl2){
                    hasCommonElements = true;
                    break;
                }
            }
        }

        if (hasCommonElements){
            notificationList.append(WorkflowNotification(GenomeAlignerWorker::tr("Bowtie2 cannot recognize read pairs from the same file. Please, perform demultiplexing first.")));
            return false;
        }

        return true;
    }
};

void GenomeAlignerWorkerFactory::init() {
    QList<PortDescriptor*> p;
    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor readsDesc(READS_URL_SLOT_ID,
        GenomeAlignerWorker::tr("URL of a file with reads"),
        GenomeAlignerWorker::tr("Input reads to be aligned."));
    Descriptor readsPairedDesc(READS_PAIRED_URL_SLOT_ID,
        GenomeAlignerWorker::tr("URL of a file with mate reads"),
        GenomeAlignerWorker::tr("Input mate reads to be aligned."));

    inTypeMap[readsDesc] = BaseTypes::STRING_TYPE();
    inTypeMap[readsPairedDesc] = BaseTypes::STRING_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        GenomeAlignerWorker::tr("Genome aligner data"),
        GenomeAlignerWorker::tr("Input reads to be aligned with Bowtie2."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    p << new PortDescriptor(inPortDesc, inTypeSet, true);
    //out port
    QMap<Descriptor, DataTypePtr> outTypeMap;
    Descriptor assemblyOutDesc(ASSEBLY_OUT_SLOT_ID,
        GenomeAlignerWorker::tr("Assembly URL"),
        GenomeAlignerWorker::tr("Output assembly URL."));

    Descriptor outPortDesc(OUT_PORT_DESCR,
        GenomeAlignerWorker::tr("Genome aligner output data"),
        GenomeAlignerWorker::tr("Output assembly files."));

    outTypeMap[assemblyOutDesc] = BaseTypes::STRING_TYPE();

    DataTypePtr outTypeSet(new MapDataType(OUT_TYPE_ID, outTypeMap));
    p << new PortDescriptor(outPortDesc, outTypeSet, false, true);

    QList<Attribute*> attrs;
    {

        Descriptor outDir(OUTPUT_DIR,
            GenomeAlignerWorker::tr("Output folder"),
            GenomeAlignerWorker::tr("Folder to save UGENE genome aligner output files."));

        Descriptor outName(OUTPUT_NAME,
            GenomeAlignerWorker::tr("Output file name"),
            GenomeAlignerWorker::tr("Base name of the output file. 'out.sam' by default"));

        Descriptor referenceInputType(REFERENCE_INPUT_TYPE,
            GenomeAlignerWorker::tr("Reference input type"),
            GenomeAlignerWorker::tr("Select \"Sequence\" to input a reference genome as a sequence file. "
            "<br/>Note that any sequence file format, supported by UGENE, is allowed (FASTA, GenBank, etc.). "
            "<br/>The index will be generated automatically in this case. "
            "<br/>Select \"Index\" to input already generated index files, specific for the tool."));

        Descriptor refGenome(REFERENCE_GENOME,
            GenomeAlignerWorker::tr("Reference genome"),
            GenomeAlignerWorker::tr("Path to indexed reference genome."));

        Descriptor indexDir(INDEX_DIR,
            GenomeAlignerWorker::tr("Genome Aligner index folder"),
            GenomeAlignerWorker::tr("The folder with the index for the reference sequence."));

        Descriptor indexBasename(INDEX_BASENAME,
            GenomeAlignerWorker::tr("Genome Aligner index basename"),
            GenomeAlignerWorker::tr("The basename of the index for the reference sequence."));

        Descriptor absMismatches(ABS_OR_PERC_MISMATCHES_ATTR,
            GenomeAlignerWorker::tr("Is absolute mismatches values?"),
            GenomeAlignerWorker::tr("<html><body><p><b>true</b> - absolute mismatches mode is used</p><p><b>false</b> - percentage mismatches mode is used</p>\
                                    You can choose absolute or percentage mismatches values mode.</body></html>"));
        Descriptor mismatches(MISMATCHES_ATTR,
            GenomeAlignerWorker::tr("Absolute mismatches"),
            GenomeAlignerWorker::tr("<html><body>Number of mismatches allowed while aligning reads.</body></html>"));
        Descriptor ptMismatches(PERCENT_MISMATCHES_ATTR,
            GenomeAlignerWorker::tr("Percentage mismatches"),
            GenomeAlignerWorker::tr("<html><body>Percentage of mismatches allowed while aligning reads.</body></html>"));
        Descriptor reverse(REVERSE_ATTR,
            GenomeAlignerWorker::tr("Align reverse complement reads"),
            GenomeAlignerWorker::tr("<html><body>Set this option to align both direct and reverse complement reads.</body></html>"));
        Descriptor best(BEST_ATTR,
            GenomeAlignerWorker::tr("Use \"best\"-mode"),
            GenomeAlignerWorker::tr("<html><body>Report only the best alignment for each read (in terms of mismatches).</body></html>"));
        Descriptor qual(QUAL_ATTR,
            GenomeAlignerWorker::tr("Omit reads with qualities lower than"),
            GenomeAlignerWorker::tr("<html><body>Omit reads with qualities lower than the specified value. Reads that have no qualities are not omited.\
                                    <p>Set <b>\"0\"</b> to switch off this option.</p></body></html>"));

#ifdef OPENCL_SUPPORT
        openclEnabled = AppContext::getOpenCLGpuRegistry()->getEnabledGpu() != nullptr;
#endif
        if(openclEnabled) {
            Descriptor gpu(GPU_ATTR, GenomeAlignerWorker::tr("Use GPU-optimization"),
                GenomeAlignerWorker::tr("<html><body>Use GPU-calculatings while aligning reads. This option requires OpenCL-enable GPU-device.</body></html>"));

            attrs << new Attribute(gpu, BaseTypes::BOOL_TYPE(), false/*required*/, false);
        }

        attrs << new Attribute(referenceInputType, BaseTypes::STRING_TYPE(), true, QVariant(DnaAssemblyToRefTaskSettings::SEQUENCE));
        Attribute* attrRefGenom = new Attribute(refGenome, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrRefGenom->addRelation(new VisibilityRelation(REFERENCE_INPUT_TYPE, DnaAssemblyToRefTaskSettings::SEQUENCE));
        attrs << attrRefGenom;
        Attribute* attrIndexDir = new Attribute(indexDir, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrIndexDir->addRelation(new VisibilityRelation(REFERENCE_INPUT_TYPE, DnaAssemblyToRefTaskSettings::INDEX));
        attrs << attrIndexDir;
        Attribute* attrIndexBasename = new Attribute(indexBasename, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrIndexBasename->addRelation(new VisibilityRelation(REFERENCE_INPUT_TYPE, DnaAssemblyToRefTaskSettings::INDEX));
        attrs << attrIndexBasename;

        attrs << new Attribute(outDir, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrs << new Attribute(outName, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(BASE_GENOME_ALIGNER_OUTFILE));
        attrs << new Attribute(absMismatches, BaseTypes::BOOL_TYPE(), true/*required*/, true);
        Attribute* mismatchesAttr = new Attribute(mismatches, BaseTypes::NUM_TYPE(), false, 0);
        mismatchesAttr->addRelation(new VisibilityRelation(ABS_OR_PERC_MISMATCHES_ATTR, QVariant(true)));
        attrs << mismatchesAttr;
        Attribute* ptMismatchesAttr = new Attribute(ptMismatches, BaseTypes::NUM_TYPE(), false, 0);
        ptMismatchesAttr->addRelation(new VisibilityRelation(ABS_OR_PERC_MISMATCHES_ATTR, QVariant(false)));
        attrs << ptMismatchesAttr;
        attrs << new Attribute(reverse, BaseTypes::BOOL_TYPE(), false/*required*/, false);
        attrs << new Attribute(best, BaseTypes::BOOL_TYPE(), false/*required*/, true);
        attrs << new Attribute(qual, BaseTypes::NUM_TYPE(), false/*required*/, 0);
    }

    Descriptor desc(ACTOR_ID, GenomeAlignerWorker::tr("Map Reads with UGENE Genome Aligner"),
        GenomeAlignerWorker::tr("Genome Aligner is a program for mapping short DNA sequence reads"
                                " to a long reference sequence, developed by the UGENE team."
                                "<br/><br/>Provide URL(s) to FASTA or FASTQ file(s) with NGS reads to the input"
                                " port of the element, set up the reference sequence in the parameters."
                                " The result is saved to the specified SAM file, URL to the file is passed"
                                " to the output port."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, attrs);

    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap rip;
        rip[GenomeAlignerWorker::tr("Sequence")] = DnaAssemblyToRefTaskSettings::SEQUENCE;
        rip[GenomeAlignerWorker::tr("Index")] = DnaAssemblyToRefTaskSettings::INDEX;
        delegates[REFERENCE_INPUT_TYPE] = new ComboBoxDelegate(rip);

        delegates[REFERENCE_GENOME] = new URLDelegate("", "", false, false, false);
        delegates[INDEX_DIR] = new URLDelegate("", "", false, true, false, NULL, "", true);

        delegates[OUTPUT_DIR] = new URLDelegate("", "", false, true);
        delegates[ABS_OR_PERC_MISMATCHES_ATTR] = new ComboBoxWithBoolsDelegate();

        QVariantMap m;
        m["minimum"] = 0;
        m["maximum"] = 3;
        delegates[MISMATCHES_ATTR] = new SpinBoxDelegate(m);

        QVariantMap ptM;
        ptM["minimum"] = 0;
        ptM["maximum"] = 10;
        delegates[PERCENT_MISMATCHES_ATTR] = new SpinBoxDelegate(ptM);

        QVariantMap q;
        q["minimum"] = 0;
        q["maximum"] = 70;
        delegates[QUAL_ATTR] = new SpinBoxDelegate(q);
    }

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new GenomeAlignerPrompter());
    proto->setPortValidator(IN_PORT_DESCR, new GenomeAlignerInputSlotsValidator());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_MAP_ASSEMBLE_READS(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new GenomeAlignerWorkerFactory());
}
} //namespace LocalWorkflow
} //namespace U2
