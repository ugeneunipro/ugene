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

#include <QScopedPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "MACSSupport.h"
#include "MACSWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString MACSWorkerFactory::ACTOR_ID("macs-id");

static const QString TREATMENT_SLOT_ID("_treatment-ann");
static const QString CONTROL_SLOT_ID("control-ann");

static const QString PEAK_REGIONS_SLOT_ID("peak-regions");
static const QString PEAK_SUMMITS_SLOT_ID("peak-summits");
static const QString WIGGLE_TREAT_SLOT_ID("wiggle-treat");

static const QString IN_TYPE_ID("macs-data");
static const QString OUT_TYPE_ID("macs-data-out");

static const QString IN_PORT_DESCR("in-data");
static const QString OUT_PORT_DESCR("out-data");

static const QString OUTPUT_DIR("output-dir");
static const QString FILE_NAMES("file-names");
static const QString WIGGLE_OUTPUT("wiggle-output");
static const QString WIGGLE_SPACE("wiggle-space");

static const QString GENOME_SIZE_ATTR_ID("genome-size");
static const QString P_VALUE_ATTR_ID("p-value");
static const QString Q_VALUE_ATTR_ID("q-value");    //(MACS 2)
static const QString USE_MODEL_ATTR_ID("use-model");
static const QString MODEL_FOLD_ATTR_ID("model-fold");
static const QString SHIFT_SIZE_ATTR_ID("shift-size");
static const QString KEEP_DUBLICATES_ATTR_ID("keep-duplicates");
static const QString BAND_WIDTH_ATTR_ID("band-width");
static const QString EXT_FR_ATTR_ID("ext-fr");    //(???)
//optional
static const QString TAG_SIZE_ATTR_ID("tag-size");
//advanced
static const QString USE_LAMBDA_ATTR_ID("use-lambda");
static const QString SMALL_NEARBY_ATTR_ID("small-nearby");
static const QString LARGE_NEARBY_ATTR_ID("large-nearby");
static const QString AUTO_BIMODAL_ATTR_ID("auto_bimodal");
static const QString SCALE_LARGE_ATTR_ID("scale_large");
static const QString SHIFT_CONTROL_ATTR_ID("shift_control");    //(MACS 2)
static const QString HALF_EXTEND_ATTR_ID("half_extend");    //(MACS 2)
static const QString BROAD_ATTR_ID("broad");    //(MACS 2)
static const QString BROAD_CUTOFF_ATTR_ID("broad_cutoff");    //(MACS 2)


/************************************************************************/
/* Worker */
/************************************************************************/
MACSWorker::MACSWorker(Actor *p)
: BaseWorker(p)
, inChannel(NULL)
, output(NULL)
{

}

void MACSWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
    output = ports.value(OUT_PORT_DESCR);
}

Task *MACSWorker::tick() {
    if (inChannel->hasMessage()) {
        U2OpStatus2Log os;

        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        MACSSettings settings = createMACSSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        GUrl treatUrl(data.value(TREATMENT_SLOT_ID).toString());
        GUrl conUrl;
        if (data.contains(CONTROL_SLOT_ID)) {
            conUrl = data.value(CONTROL_SLOT_ID).toString();
        }

        MACSTask* t = new MACSTask(settings, treatUrl, conUrl);

        t->addListeners(createLogListeners());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }else if (inChannel->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void MACSWorker::cleanup() {

}

void MACSWorker::sl_taskFinished() {
    MACSTask *t = dynamic_cast<MACSTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    QVariantMap data;

    const QList<AnnotationTableObject *> peaksTables = t->getPeaks();
    data[PEAK_REGIONS_SLOT_ID] = QVariant::fromValue(context->getDataStorage()->putAnnotationTables(peaksTables));
    qDeleteAll(peaksTables);

    const QList<AnnotationTableObject *> summitsTables = t->getPeakSummits();
    data[PEAK_SUMMITS_SLOT_ID] = QVariant::fromValue(context->getDataStorage()->putAnnotationTables(summitsTables));
    qDeleteAll(summitsTables);

    if (t->getSettings().wiggleOut){
        data[WIGGLE_TREAT_SLOT_ID] = qVariantFromValue<QString>(t->getWiggleUrl());
    }else{
        data[WIGGLE_TREAT_SLOT_ID] = qVariantFromValue<QString>("");
    }

    output->put(Message(output->getBusType(), data));

    const QStringList& resFileNames = t->getOutputFiles();
    foreach(const QString& fn, resFileNames){
        QString url = t->getSettings().outDir + "/" + fn;
        context->getMonitor()->addOutputFile(url, getActor()->getId());
    }

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
        output->setEnded();
    }
}

U2::MACSSettings MACSWorker::createMACSSettings( U2OpStatus & /*os*/ ){
    MACSSettings settings;

    settings.outDir = getValue<QString>(OUTPUT_DIR);
    settings.fileNames = getValue<QString>(FILE_NAMES);
    settings.wiggleOut = getValue<bool>(WIGGLE_OUTPUT);
    settings.wiggleSpace = getValue<int>(WIGGLE_SPACE);

    settings.genomeSizeInMbp = getValue<int>(GENOME_SIZE_ATTR_ID);
    settings.pValue = getValue<double>(P_VALUE_ATTR_ID);
    settings.useModel = getValue<bool>(USE_MODEL_ATTR_ID);

    QString locStr = getValue<QString>(MODEL_FOLD_ATTR_ID);
    U2Location l;
    QStringList messages;
    Genbank::LocationParser::ParsingResult parsingResult = Genbank::LocationParser::parseLocation(qPrintable(locStr), locStr.size(), l, messages);
    if (Genbank::LocationParser::Failure == parsingResult || !messages.isEmpty()){
        algoLog.error(tr("Bad model fold region: %1. Default region is used").arg(messages.isEmpty() ? tr("unrecognized parsing error") : messages.last()));
    } else {
        if (!l->regions.isEmpty()) {
            settings.modelFold = l->regions.first();
        }
    }

    settings.shiftSize = getValue<int>(SHIFT_SIZE_ATTR_ID);

    settings.keepDublicates = getValue<QString>(KEEP_DUBLICATES_ATTR_ID);

    settings.bandWidth = getValue<int>(BAND_WIDTH_ATTR_ID);
    //settings.extFr = getValue<bool>(EXT_FR_ATTR_ID);    //(???)
    //optional
    settings.tagSize = getValue<int>(TAG_SIZE_ATTR_ID); //0 for default
    //advanced
    settings.useLambda = getValue<bool>(USE_LAMBDA_ATTR_ID);
    settings.smallNearby = getValue<int>(SMALL_NEARBY_ATTR_ID);
    settings.largeNearby = getValue<int>(LARGE_NEARBY_ATTR_ID);
    settings.autoBimodal = getValue<bool>(AUTO_BIMODAL_ATTR_ID);
    settings.scaleLarge = getValue<bool>(SCALE_LARGE_ATTR_ID);
    //settings.shiftControl = getValue<bool>(SHIFT_CONTROL_ATTR_ID);    //(MACS 2)
    //settings.halfExtend = getValue<bool>(HALF_EXTEND_ATTR_ID);    //(MACS 2)
    //settings.broad = getValue<bool>(BROAD_ATTR_ID);    //(MACS 2)
    //settings.broadCutoff = getValue<double>(BROAD_CUTOFF_ATTR_ID);    //(MACS 2)

    return settings;
}

/************************************************************************/
/* Factory */
/************************************************************************/

class MACSInputSlotsValidator : public PortValidator {
    public:

    bool validate(const IntegralBusPort *port, NotificationsList &notificationList) const {
        QVariant busMap = port->getParameter(Workflow::IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributePureValue();
        bool data = isBinded(busMap.value<StrStrMap>(), TREATMENT_SLOT_ID);
        if (!data){
            QString dataName = slotName(port, TREATMENT_SLOT_ID);
            notificationList.append(WorkflowNotification(IntegralBusPort::tr("The slot must be not empty: '%1'").arg(dataName)));
            return false;
        }


        QString slot1Val = busMap.value<StrStrMap>().value(TREATMENT_SLOT_ID);
        QString slot2Val = busMap.value<StrStrMap>().value(CONTROL_SLOT_ID);
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
            notificationList.append(WorkflowNotification(MACSWorker::tr("Input control and treatment annotations are the same")));
            return false;
        }

        return true;
    }
    };

void MACSWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor treatDesc(TREATMENT_SLOT_ID,
        MACSWorker::tr("Treatment features"),
        MACSWorker::tr("ChIP-seq treatment features."));
    Descriptor conDesc(CONTROL_SLOT_ID,
        MACSWorker::tr("Control features"),
        MACSWorker::tr("Control features (Optional)."));
    inTypeMap[treatDesc] = BaseTypes::STRING_TYPE();
    inTypeMap[conDesc] = BaseTypes::STRING_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        MACSWorker::tr("MACS data"),
        MACSWorker::tr("ChIP-seq treatment features and control features (optional) to call peaks with MACS."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);

    //out port
    QMap<Descriptor, DataTypePtr> outTypeMap;
    Descriptor peakRegDesc(PEAK_REGIONS_SLOT_ID,
        MACSWorker::tr("Peak regions"),
        MACSWorker::tr("Peak locations. Typically used in gene association study like CEAS, or correlation calculation."));
    Descriptor peakSummitsDescr(PEAK_SUMMITS_SLOT_ID,
        MACSWorker::tr("Peak summits"),
        MACSWorker::tr("Peak summits locations for every peaks. Typically used in DNA motif analysis or conservation check."));
    Descriptor wiggleTreatDescr(WIGGLE_TREAT_SLOT_ID,
        MACSWorker::tr("Treatment fragments pileup (wiggle)"),
        MACSWorker::tr("Wiggle format files which can be imported to UCSC genome browser/GMOD/Affy IGB."));

    Descriptor outPortDesc(OUT_PORT_DESCR,
        MACSWorker::tr("MACS output data"),
        MACSWorker::tr("ChIP-seq peaks and summits. Pileup data (optional)"));

    outTypeMap[peakRegDesc] = BaseTypes::ANNOTATION_TABLE_TYPE();
    outTypeMap[peakSummitsDescr] = BaseTypes::ANNOTATION_TABLE_TYPE();
    outTypeMap[wiggleTreatDescr] = BaseTypes::STRING_TYPE();

    DataTypePtr outTypeSet(new MapDataType(OUT_TYPE_ID, outTypeMap));
    portDescs << new PortDescriptor(outPortDesc, outTypeSet, false, true);


     QList<Attribute*> attrs;
     {
         Descriptor outDir(OUTPUT_DIR,
             MACSWorker::tr("Output folder"),
             MACSWorker::tr("Folder to save MACS output files."));
         Descriptor fileNames(FILE_NAMES,
             MACSWorker::tr("Name"),
             MACSWorker::tr("The name string of the experiment. MACS will use this string NAME"
             " to create output files like 'NAME_peaks.xls', 'NAME_negative_peaks.xls', 'NAME_peaks.bed',"
             " 'NAME_summits.bed', 'NAME_model.r' and so on. So please avoid any confliction between these filenames and your existing files (--name)."));
         Descriptor wiggleOut(WIGGLE_OUTPUT,
             MACSWorker::tr("Wiggle output"),
             MACSWorker::tr("If this flag is on, MACS will store the fragment pileup in wiggle format for the whole genome data instead of for every chromosomes (--wig) (--single-profile)."));
         Descriptor wiggleSpace(WIGGLE_SPACE,
             MACSWorker::tr("Wiggle space"),
             MACSWorker::tr("By default, the resolution for saving wiggle files is 10 bps,i.e.,"
             " MACS will save the raw tag count every 10 bps. You can change it along with '--wig' option (--space)."));
        Descriptor gsizeDesc(GENOME_SIZE_ATTR_ID,
            MACSWorker::tr("Genome size (Mbp)"),
            MACSWorker::tr("Homo sapience - 2700 Mbp<br>"
            "Mus musculus - 1870 Mbp<br>"
            "Caenorhabditis elegans - 90 Mbp<br>"
            "Drosophila melanogaster  - 120 Mbp<br>"
            " It's the mappable genome size or effective genome size"
            " which is defined as the genome size which can be sequenced."
            " Because of the repetitive features on the chromosomes, the actual mappable"
            " genome size will be smaller than the original size, about 90% or 70% of the genome size (--gsize)."));
        Descriptor pvalueDesc(P_VALUE_ATTR_ID,
            MACSWorker::tr("P-value"),
            MACSWorker::tr("P-value cutoff. Default is 0.00001, for looser results, try 0.001 instead (--pvalue)."));
        Descriptor qvalueDesc(Q_VALUE_ATTR_ID,
            MACSWorker::tr("Q-value"),
            MACSWorker::tr("Minimum FDR (q-value) cutoff for peak detection."));
        Descriptor useModelDesc(USE_MODEL_ATTR_ID,
            MACSWorker::tr("Use model"),
            MACSWorker::tr("Whether or not to use MACS paired peaks model (--nomodel)."));
        Descriptor modelFoldDesc(MODEL_FOLD_ATTR_ID,
            MACSWorker::tr("Model fold"),
            MACSWorker::tr("Select the regions within MFOLD range of high-confidence enrichment ratio against."
            " <b>Model fold</b> is available when <b>Use model</b> is true, which is the foldchange"
            " to chose paired peaks to build paired peaks model. Users need to set a lower(smaller)"
            " and upper(larger) number for fold change so that MACS will only use the peaks within these foldchange range to build model (--mfold)."));
        Descriptor shiftSizeDesc(SHIFT_SIZE_ATTR_ID,
            MACSWorker::tr("Shift size"),
            MACSWorker::tr("An arbitrary shift value used as a half of the fragment size when model is not built."
            " <b>Shift size</b> is available when <b>Use model</b> is false, which will represent the HALF of the fragment size of your sample."
            " If your sonication and size selection size is 300 bps, after you trim out nearly 100 bps adapters,"
            " the fragment size is about 200 bps, so you can specify 100 here (--shiftsize)."));
        Descriptor keepDupDesc(KEEP_DUBLICATES_ATTR_ID,
            MACSWorker::tr("Keep duplicates"),
            MACSWorker::tr("It controls the MACS behavior towards duplicate tags at the exact same location -- the same coordination and the same strand."
            " The default <b>auto</b> option makes MACS calculate the maximum tags at the exact same location based on binomal distribution using 1e-5 as "
            "pvalue cutoff; and the <b>all</b> option keeps every tags. If an <b>integer</b> is given, at most this number of tags will be kept at the same location (--keep-dup)."));
        Descriptor bandWDesc(BAND_WIDTH_ATTR_ID,
            MACSWorker::tr("Band width"),
            MACSWorker::tr("The band width which is used to scan the genome for model building."
            " You can set this parameter as the sonication fragment size expected from wet experiment."
            " Used only while building the shifting model (--bw)."));
        Descriptor extFrDesc(EXT_FR_ATTR_ID,
            MACSWorker::tr("Extended fragment pileup"),
            MACSWorker::tr("Whether or not to generate extended fragment pileup, local lambda and score tracks at every bp."));

        //optional
        Descriptor tagSizeDesc(TAG_SIZE_ATTR_ID,
            MACSWorker::tr("Tag size (optional)"),
            MACSWorker::tr("Length of reads. Determined from first 10 reads if not specified (input <b>0</b>) (--tsize)."));

        //advanced
        Descriptor useLambdaDesc(USE_LAMBDA_ATTR_ID,
            MACSWorker::tr("Use lambda"),
            MACSWorker::tr("Whether to use local lambda model which can use the local bias at peak regions to throw out false positives (--nolambda)."));
        Descriptor smallNearbyDesc(SMALL_NEARBY_ATTR_ID,
            MACSWorker::tr("Small nearby region"),
            MACSWorker::tr("The small nearby region in basepairs to calculate dynamic lambda."
            " This is used to capture the bias near the peak summit region. Invalid if there is no control data (--slocal)."));
        Descriptor LargeNearbyDesc(LARGE_NEARBY_ATTR_ID,
            MACSWorker::tr("Large nearby region"),
            MACSWorker::tr("The large nearby region in basepairs to calculate dynamic lambda. "
            " This is used to capture the surround bias (--llocal)."));
        Descriptor autoBimodalDesc(AUTO_BIMODAL_ATTR_ID,
            MACSWorker::tr("Auto bimodal"),
            MACSWorker::tr("Whether turn on the auto pair model process."
            "If set, when MACS failed to build paired model, it will use the nomodel"
            "settings, the “Shift size” parameter to shift and extend each tags (--on-auto)."));
        Descriptor scaleLargeDesc(SCALE_LARGE_ATTR_ID,
            MACSWorker::tr("Scale to large"),
            MACSWorker::tr(" When set, scale the small sample up to the bigger sample."
            "By default, the bigger dataset will be scaled down towards the smaller dataset,"
            "which will lead to smaller p/qvalues and more specific results."
            "Keep in mind that scaling down will bring down background noise more (--to-large)."));
        Descriptor shiftControlDesc(SHIFT_CONTROL_ATTR_ID,
            MACSWorker::tr("Shift control"),
            MACSWorker::tr("When set, control tags will be shifted just as ChIP tags according to their strand "
            "before the extension of d, slocal and llocal. By default, control tags are extended centered"
            "at their current positions regardless of strand. You may consider to turn this option on while"
            "comparing two ChIP datasets of different condition but the same factor."));
        Descriptor halfExtendDesc(HALF_EXTEND_ATTR_ID,
            MACSWorker::tr("Half-extend"),
            MACSWorker::tr("When set, MACS extends 1/2 d size for each fragment centered at its middle point."));
        Descriptor broadDesc(BROAD_ATTR_ID,
            MACSWorker::tr("Broad"),
            MACSWorker::tr("If set, MACS will try to call broad peaks by linking nearby highly enriched regions."
            "The linking region is controlled by another cutoff through “Broad cutoff”."
            "The maximum linking region length is 4 times of d from MACS."));
        Descriptor broadCutoffDesc(BROAD_CUTOFF_ATTR_ID,
            MACSWorker::tr("Broad cutoff"),
            MACSWorker::tr("Cutoff for broad region. This option is not available unless “Broad” is set."
            "If “P-value” is set, this is a pvalue cutoff, otherwise, it's a qvalue cutoff."));


        attrs << new Attribute(outDir, BaseTypes::STRING_TYPE(), true, QVariant(""));
        attrs << new Attribute(fileNames, BaseTypes::STRING_TYPE(), true, QVariant("Default"));
        attrs << new Attribute(wiggleOut, BaseTypes::BOOL_TYPE(), true, QVariant(true));

        Attribute* wspaceAttr = new Attribute(wiggleSpace, BaseTypes::NUM_TYPE(), true, QVariant(10));
        wspaceAttr ->addRelation(new VisibilityRelation(WIGGLE_OUTPUT, QVariant(true)));
        attrs << wspaceAttr;

        attrs << new Attribute(gsizeDesc, BaseTypes::NUM_TYPE(), false, QVariant(2700));
        attrs << new Attribute(pvalueDesc, BaseTypes::NUM_TYPE(), false, QVariant(0.00001));
        attrs << new Attribute(tagSizeDesc, BaseTypes::NUM_TYPE(), false, QVariant(0));
        attrs << new Attribute(keepDupDesc, BaseTypes::STRING_TYPE(), false, QVariant("1"));
        //attrs << new Attribute(qvalueDesc, BaseTypes::NUM_TYPE(), false, QVariant(0.1));    //(MACS 2)
        attrs << new Attribute(useModelDesc, BaseTypes::BOOL_TYPE(), false, QVariant(true));

        Attribute* foldAttr = new Attribute(modelFoldDesc, BaseTypes::STRING_TYPE(), false,
            QVariant(U1AnnotationUtils::buildLocationString(QVector<U2Region>()<<U2Region(9, 21))));
        foldAttr ->addRelation(new VisibilityRelation(USE_MODEL_ATTR_ID, QVariant(true)));
        attrs << foldAttr;

        Attribute* shiftAttr = new Attribute(shiftSizeDesc, BaseTypes::NUM_TYPE(), false, QVariant(100));
        shiftAttr ->addRelation(new VisibilityRelation(USE_MODEL_ATTR_ID, QVariant(false)));
        //TODO:Available if: “Use model” is False OR building model by MACSis failed
        attrs << shiftAttr;


        attrs << new Attribute(bandWDesc, BaseTypes::NUM_TYPE(), false, QVariant(300));
        //attrs << new Attribute(extFrDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));    //(???)
        //optional

        //advanced
        attrs << new Attribute(useLambdaDesc, BaseTypes::BOOL_TYPE(), false, QVariant(true));
        attrs << new Attribute(smallNearbyDesc, BaseTypes::NUM_TYPE(), false, QVariant(1000));
        attrs << new Attribute(LargeNearbyDesc, BaseTypes::NUM_TYPE(), false, QVariant(10000));
        attrs << new Attribute(autoBimodalDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(scaleLargeDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        //attrs << new Attribute(shiftControlDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));    //(MACS 2)
        //attrs << new Attribute(halfExtendDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));    //(MACS 2)
        //attrs << new Attribute(broadDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));    //(MACS 2)
        //attrs << new Attribute(broadCutoffDesc, BaseTypes::NUM_TYPE(), false, QVariant(0.1));    //(MACS 2)
     }

     QMap<QString, PropertyDelegate*> delegates;
     {
         delegates[OUTPUT_DIR] = new URLDelegate("", "", false, true);
         delegates[USE_MODEL_ATTR_ID] = new ComboBoxWithBoolsDelegate();
         delegates[WIGGLE_OUTPUT] = new ComboBoxWithBoolsDelegate();
         {
            QVariantMap vm;
            vm["minimum"] = QVariant(1);
            vm["maximum"] = INT_MAX;
            vm["singleStep"] = QVariant(10);

            delegates[WIGGLE_SPACE] = new SpinBoxDelegate(vm);
         }
         {
            QVariantMap vm;
            vm["minimum"] = QVariant(1);
            vm["maximum"] = QVariant(10000);
            vm["singleStep"] = QVariant(1);
            vm["suffix"] = QVariant("Mbp");
            delegates[GENOME_SIZE_ATTR_ID] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = 1;
             vm["singleStep"] = 0.00001;
             vm["decimals"] = 6;
             delegates[P_VALUE_ATTR_ID] = new DoubleSpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = 1;
             vm["singleStep"] = 0.1;
             delegates[Q_VALUE_ATTR_ID] = new DoubleSpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = QVariant(1);
             vm["maximum"] = INT_MAX;
             vm["singleStep"] = QVariant(1);
             delegates[SHIFT_SIZE_ATTR_ID] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = INT_MAX;
             vm["singleStep"] = 1;
             delegates[BAND_WIDTH_ATTR_ID] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = INT_MAX;
             vm["singleStep"] = 1;
             delegates[TAG_SIZE_ATTR_ID] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = INT_MAX;
             vm["singleStep"] = 100;
             delegates[SMALL_NEARBY_ATTR_ID] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = INT_MAX;
             vm["singleStep"] = 1000;
             delegates[LARGE_NEARBY_ATTR_ID] = new SpinBoxDelegate(vm);
         }
         {
             QVariantMap vm;
             vm["minimum"] = 0;
             vm["maximum"] = 1;
             vm["singleStep"] = 0.1;
             delegates[BROAD_CUTOFF_ATTR_ID] = new DoubleSpinBoxDelegate(vm);
         }

     }

    Descriptor protoDesc(MACSWorkerFactory::ACTOR_ID,
        MACSWorker::tr("Find Peaks with MACS"),
        MACSWorker::tr("Performs peak calling for ChIP-Seq data."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new MACSPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPortValidator(IN_PORT_DESCR, new MACSInputSlotsValidator());
    proto->addExternalTool(MACSSupport::ET_MACS_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CHIP_SEQ(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new MACSWorkerFactory());
}

Worker *MACSWorkerFactory::createWorker(Actor *a) {
    return new MACSWorker(a);
}

QString MACSPrompter::composeRichDoc() {
    QString res = "";

    Actor* treatProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(TREATMENT_SLOT_ID);
    Actor* controlProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(CONTROL_SLOT_ID);

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString treatUrl = treatProducer ? treatProducer->getLabel() : unsetStr;
    QString conUrl = controlProducer ? controlProducer->getLabel() : unsetStr;
    QString wiggleSpan = getHyperlink(WIGGLE_SPACE, getParameter(WIGGLE_SPACE).toInt());

    QString dir = getHyperlink(OUTPUT_DIR, getURL(OUTPUT_DIR));

    res.append(tr("Uses <u>%1</u> as treatment").arg(treatUrl));
    if (controlProducer){
        res.append(tr(" and <u>%1</u> as control").arg(conUrl));
    }

    res.append(tr(" to call peaks."));

    res.append(tr(" Outputs all files to <u>%1</u> folder").arg(dir));
    if (getParameter(WIGGLE_OUTPUT).toBool()){
        res.append(tr(" and pileup with <u>%1</u> span").arg(wiggleSpan));
    }
    res.append(".");


    return res;
}

} // LocalWorkflow
} // U2
