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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "CEASReportWorker.h"
#include "CEASSupport.h"

namespace U2 {
namespace LocalWorkflow {

const QString CEASReportWorkerFactory::ACTOR_ID("ceas-report");

static const QString BED_SLOT_ID("peak-regions");
static const QString WIG_SLOT_ID("enrichment-signal");
static const QString IN_TYPE_ID("ceas-data");
static const QString IN_PORT_DESCR("in-data");

static const QString IMAGE_FILE_ATTR_ID("image-file");
static const QString IMAGE_FORMAT_ATTR_ID("image-format");
static const QString OUT_ANNS_ATTR_ID("anns-file");

static const QString ANNS_TABLE_ATTR_ID("anns-table");
static const QString SPAN_ATTR_ID("span");
static const QString PROF_RES_ATTR_ID("profiling-resolution");
static const QString SIZES_ATTR_ID("promoter-sizes");
static const QString BISIZES_ATTR_ID("promoter-bisizes");
static const QString REL_DIST_ATTR_ID("relative-distance");
static const QString GROUP_FILES_ATTR_ID("group-files");
static const QString GROUP_NAMES_ATTR_ID("group-names");

/************************************************************************/
/* Worker */
/************************************************************************/
CEASReportWorker::CEASReportWorker(Actor *p)
: BaseWorker(p), inChannel(NULL)
{

}

void CEASReportWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
}

Task *CEASReportWorker::tick() {
    if (inChannel->hasMessage()) {
        U2OpStatus2Log os;
        CEASTaskSettings settings = createTaskSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        CEASSupportTask *t = new CEASSupportTask(settings);
        t->addListeners(createLogListeners());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }if (inChannel->isEnded()) {
        setDone();
    }
    return NULL;
}

void CEASReportWorker::cleanup() {

}

void CEASReportWorker::sl_taskFinished() {
    CEASSupportTask *t = dynamic_cast<CEASSupportTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    QString imageUrl = t->getSettings().getCeasSettings().getImageFilePath();
    QString annsUrl = t->getSettings().getCeasSettings().getAnnsFilePath();
    context->getMonitor()->addOutputFile(imageUrl, getActor()->getId());
    context->getMonitor()->addOutputFile(annsUrl, getActor()->getId());

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
    }
}

CEASTaskSettings CEASReportWorker::createTaskSettings(U2OpStatus &os) {
    Message m = getMessageAndSetupScriptValues(inChannel);
    QVariantMap data = m.getData().toMap();

    QList<SharedDbiDataHandler> bedData;
    QString wigData = "";

    if (!(data.contains(BED_SLOT_ID) || data.contains(WIG_SLOT_ID))) {
        os.setError("There must be peak data(bed) or signal data (wiggle)");
        return CEASTaskSettings();
    }else{
        if (data.contains(BED_SLOT_ID)) {
            bedData = StorageUtils::getAnnotationTableHandlers(data[BED_SLOT_ID]);
        }

        if (data.contains(WIG_SLOT_ID)){
            wigData = data[WIG_SLOT_ID].toString();
        }
    }

    CEASSettings ceas = createCEASSettings(os);
    CHECK_OP(os, CEASTaskSettings());

    CEASTaskSettings settings(
        ceas,
        context->getDataStorage(),
        bedData,
        wigData);

    return settings;
}

CEASSettings CEASReportWorker::createCEASSettings(U2OpStatus &/*os*/) {
    CEASSettings settings;
    settings.setImagePath(getValue<QString>(IMAGE_FILE_ATTR_ID));
//     settings.setImageFormat(
//         actor->getParameter(IMAGE_FORMAT_ATTR_ID)->getAttributeValue<QString>(context));
    settings.setAnnsFilePath(getValue<QString>(OUT_ANNS_ATTR_ID));
    settings.setGdbFile(
        actor->getParameter(ANNS_TABLE_ATTR_ID)->getAttributeValue<QString>(context));
    settings.setSpan(
        actor->getParameter(SPAN_ATTR_ID)->getAttributeValue<int>(context));
    settings.setProfilingRes(
        actor->getParameter(PROF_RES_ATTR_ID)->getAttributeValue<int>(context));
    settings.setSizes(
        actor->getParameter(SIZES_ATTR_ID)->getAttributeValue<int>(context));
    settings.setBisizes(
        actor->getParameter(BISIZES_ATTR_ID)->getAttributeValue<int>(context));
    settings.setRelativeDist(
        actor->getParameter(REL_DIST_ATTR_ID)->getAttributeValue<int>(context));

    QString groupFiles =
        actor->getParameter(GROUP_FILES_ATTR_ID)->getAttributeValue<QString>(context);
    foreach (const QString &group, groupFiles.split(",", QString::SkipEmptyParts)) {
        settings.addGeneGroup(group.simplified());
    }

    QString groupNames =
        actor->getParameter(GROUP_NAMES_ATTR_ID)->getAttributeValue<QString>(context);
    foreach (const QString &name, groupNames.split(",", QString::SkipEmptyParts)) {
        settings.addGeneGroup(name.simplified());
    }

    return settings;
}

/************************************************************************/
/* CeasComboBoxWithUrlsDelegate */
/************************************************************************/
void CeasComboBoxWithUrlsDelegate::updateUgeneSettings() {
    updateDataPath(getDataPathName(), CEASSupport::REFGENE_DIR_NAME);
}

QString CeasComboBoxWithUrlsDelegate::getDataPathName() {
    return CEASSupport::REF_GENES_DATA_NAME;
}

QString CeasComboBoxWithUrlsDelegate::getAttributeName() {
    return ANNS_TABLE_ATTR_ID;
}

/************************************************************************/
/* Factory */
/************************************************************************/
void CEASReportWorkerFactory::init() {

    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(CEASSupport::REF_GENES_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }

    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> inTypeMap;
        Descriptor bedDesc(BED_SLOT_ID,
            CEASReportWorker::tr("Peak regions"),
            CEASReportWorker::tr("Peak regions."));
        Descriptor wigDesc(WIG_SLOT_ID,
            CEASReportWorker::tr("Enrichment signal"),
            CEASReportWorker::tr("Enrichment signal."));
        inTypeMap[bedDesc] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();
        inTypeMap[wigDesc] = BaseTypes::STRING_TYPE();

        Descriptor inPortDesc(IN_PORT_DESCR,
            CEASReportWorker::tr("CEAS data"),
            CEASReportWorker::tr("Peak regions and enrichment signal profile for generating CEAS report."));

        DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
        portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);
    }

    QList<Attribute*> attrs;
    {
        Descriptor imageFileDesc(IMAGE_FILE_ATTR_ID,
            CEASReportWorker::tr("Output report file"),
            CEASReportWorker::tr("Path to the report output file. Result for CEAS analysis."));
//         Descriptor formatDesc(IMAGE_FORMAT_ATTR_ID,
//             CEASReportWorker::tr("Image format"),
//             CEASReportWorker::tr("Output image format."));
        Descriptor annsDesc(OUT_ANNS_ATTR_ID,
            CEASReportWorker::tr("Output annotations file"),
            CEASReportWorker::tr("Name of tab-delimited output text file,"
            " containing a row of annotations for every RefSeq gene. (file is not generated if no peak location data are supplied)."));
        Descriptor gdbDesc(ANNS_TABLE_ATTR_ID,
            CEASReportWorker::tr("Gene annotations table"),
            CEASReportWorker::tr("Path to gene annotation table (e.g. a"
            " refGene table in sqlite3 db format (--gt)."));
        Descriptor spanDesc(SPAN_ATTR_ID,
            CEASReportWorker::tr("Span size"),
            CEASReportWorker::tr("Span from TSS and TTS in the gene-centered"
            " annotation (base pairs). ChIP regions within this range from TSS"
            " and TTS are considered when calculating the coverage rates in"
            " promoter and downstream (--span)."));
        Descriptor profResDesc(PROF_RES_ATTR_ID,
            CEASReportWorker::tr("Wiggle profiling resolution"),
            CEASReportWorker::tr("Wiggle profiling resolution. WARNING: Value"
            " smaller than the wig interval (resolution) may cause aliasing"
            " error (--pf-res)."));
        Descriptor sizesDesc(SIZES_ATTR_ID,
            CEASReportWorker::tr("Promoter/downstream interval"),
            CEASReportWorker::tr("Promoter/downstream intervals for ChIP"
            " region annotation are three values or a single value can be"
            " given. If a single value is given, it will be segmented into"
            " three equal fractions (e.g. 3000 is equivalent to"
            " 1000,2000,3000) (--rel-dist)."));
        Descriptor bisizesDesc(BISIZES_ATTR_ID,
            CEASReportWorker::tr("BiPromoter ranges"),
            CEASReportWorker::tr("Bidirectional-promoter sizes for ChIP region"
            " annotation. It's two values or a single value can be given. If a"
            " single value is given, it will be segmented into two equal"
            " fractions (e.g. 5000 is equivalent to 2500,5000) (--bisizes)."));
        Descriptor relDistDesc(REL_DIST_ATTR_ID,
            CEASReportWorker::tr("Relative distance"),
            CEASReportWorker::tr("Relative distance to TSS/TTS in WIGGLE file."
            " profiling. (--rel-dist)"));
        Descriptor groupsDesc(GROUP_FILES_ATTR_ID,
            CEASReportWorker::tr("Gene group files"),
            CEASReportWorker::tr("Gene groups of particular interest in wig"
            " profiling. Each gene group file must have gene names in the 1st"
            " column. The file names are separated by commas (--gn-groups)."));
        Descriptor namesDesc(GROUP_NAMES_ATTR_ID,
            CEASReportWorker::tr("Gene group names"),
            CEASReportWorker::tr("<i>Set this parameter empty for using default"
            " values.</i><br>The names of the gene groups from \"Gene group"
            " files\" parameter. These names appear in the legends of the wig"
            " profiling plots.<br> Values range: comma-separated list of"
            " strings. Default value: 'Group 1, Group 2,...Group n' (--gn-group-names)."));

        attrs << new Attribute(imageFileDesc, BaseTypes::STRING_TYPE(), true);
        //attrs << new Attribute(formatDesc, BaseTypes::STRING_TYPE(), false, CEASTaskSettings::PDF_FORMAT);
        attrs << new Attribute(annsDesc, BaseTypes::STRING_TYPE(), true);
        Attribute* annGrAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                int hg19Idx = dataNames.indexOf("hg19");
                if (hg19Idx == -1){
                    annGrAttr = new Attribute(gdbDesc, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
                }else{
                    annGrAttr = new Attribute(gdbDesc, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.at(hg19Idx)));
                }

            }else{
                annGrAttr = new Attribute(gdbDesc, BaseTypes::STRING_TYPE(), true);
            }
        }else{
            annGrAttr = new Attribute(gdbDesc, BaseTypes::STRING_TYPE(), true);
        }
        attrs << annGrAttr;

        attrs << new Attribute(spanDesc, BaseTypes::NUM_TYPE(), false, QVariant(3000));
        attrs << new Attribute(profResDesc, BaseTypes::NUM_TYPE(), false, QVariant(50));
        attrs << new Attribute(sizesDesc, BaseTypes::NUM_TYPE(), false, QVariant(3000));
        attrs << new Attribute(bisizesDesc, BaseTypes::STRING_TYPE(), false, QVariant(5000));
        attrs << new Attribute(relDistDesc, BaseTypes::STRING_TYPE(), false, QVariant(3000));
        attrs << new Attribute(groupsDesc, BaseTypes::STRING_TYPE(), false, "");
        attrs << new Attribute(namesDesc, BaseTypes::STRING_TYPE(), false, "");
    }

    QMap<QString, PropertyDelegate*> delegates;
    {

        delegates[IMAGE_FILE_ATTR_ID] = new URLDelegate(DialogUtils::prepareFileFilter(CEASReportWorker::tr("Report file"), QStringList("pdf"), true), "", false);
//         {
//             QVariantMap vm;
//             vm[CEASTaskSettings::PDF_FORMAT] = CEASTaskSettings::PDF_FORMAT;
//             vm[CEASTaskSettings::PNG_FORMAT] = CEASTaskSettings::PNG_FORMAT;
//             delegates[IMAGE_FORMAT_ATTR_ID] = new ComboBoxDelegate(vm);
//         }
        delegates[OUT_ANNS_ATTR_ID] = new URLDelegate(DialogUtils::prepareFileFilter(CEASReportWorker::tr("Annotations file"), QStringList("xls"), true), "");
        {
            QVariantMap vm;
            if (dataPath){
                vm = dataPath->getDataItemsVariantMap();
            }
            delegates[ANNS_TABLE_ATTR_ID] = new CeasComboBoxWithUrlsDelegate(vm);
        }
        {
            QVariantMap vm;
            vm["minimum"] = 1;
            vm["maximum"] = 999999999;
            vm["singleStep"] = 1;
            delegates[SPAN_ATTR_ID] = new SpinBoxDelegate(vm);
            delegates[PROF_RES_ATTR_ID] = new SpinBoxDelegate(vm);
            delegates[SIZES_ATTR_ID] = new SpinBoxDelegate(vm);
            delegates[BISIZES_ATTR_ID] = new SpinBoxDelegate(vm);
            delegates[REL_DIST_ATTR_ID] = new SpinBoxDelegate(vm);
        }
        delegates[GROUP_FILES_ATTR_ID] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), "", true, false, false);
    }

    Descriptor protoDesc(CEASReportWorkerFactory::ACTOR_ID,
        CEASReportWorker::tr("Create CEAS Report"),
        CEASReportWorker::tr("Provides summary statistics on ChIP enrichment"
        " in important genomic regions such as individual chromosomes,"
        " promoters, gene bodies or exons, and infers the genes most likely to"
        " be regulated by the binding factor under study."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new CEASReportPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(CEASSupport::ET_CEAS_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CHIP_SEQ(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new CEASReportWorkerFactory());
}

Worker *CEASReportWorkerFactory::createWorker(Actor *a) {
    return new CEASReportWorker(a);
}

QString CEASReportPrompter::composeRichDoc() {
    QString file = getHyperlink(IMAGE_FILE_ATTR_ID, getURL(IMAGE_FILE_ATTR_ID));
    return tr("Creates summary statistics on ChIP enrichment"
        " and saves it to %1.").arg(file);
}

} // LocalWorkflow
} // U2
