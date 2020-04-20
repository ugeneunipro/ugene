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
#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "Peak2GeneSupport.h"
#include "Peak2GeneWorker.h"


namespace U2 {
namespace LocalWorkflow {

const QString Peak2GeneWorkerFactory::ACTOR_ID("peak2gene-id");

static const QString TREAT_SLOT_ID("_treat-ann");

static const QString IN_TYPE_ID("peak2gene-data");
static const QString OUT_TYPE_ID("macs-data-out");

static const QString IN_PORT_DESCR("in-data");
static const QString OUT_PORT_DESCR("out-data");

static const QString PEAK_ANNOTATION("peak-annotation");
static const QString GENE_ANNOTATION("gene-annotation");

static const QString PEAK_ANNOTATION_URL("peak-annotation-url");
static const QString GENE_ANNOTATION_URL("gene-annotation-url");

static const QString OUTPOS("outpos");
static const QString SYMBOL("symbol");
static const QString DISTANCE("distance");
static const QString GENOME("genome");


/************************************************************************/
/* Worker */
/************************************************************************/
Peak2GeneWorker::Peak2GeneWorker(Actor *p)
: BaseWorker(p)
, inChannel(NULL)
, output(NULL)
{

}

void Peak2GeneWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
    output = ports.value(OUT_PORT_DESCR);
}

Task *Peak2GeneWorker::tick() {
    if (inChannel->hasMessage()) {
        U2OpStatus2Log os;

        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        if (!data.contains(TREAT_SLOT_ID)) {
            os.setError("Treatment slot is empty");
            return new FailTask(os.getError());
        }

        const QList<SharedDbiDataHandler> treatData = StorageUtils::getAnnotationTableHandlers(data[TREAT_SLOT_ID]);

        Peak2GeneSettings settings = createPeak2GeneSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        Peak2GeneTask* t = new Peak2GeneTask(settings, context->getDataStorage(), treatData);
        t->addListeners(createLogListeners());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }else if (inChannel->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void Peak2GeneWorker::cleanup() {

}

void Peak2GeneWorker::sl_taskFinished() {
    Peak2GeneTask *task = qobject_cast<Peak2GeneTask*>(sender());
    if (!task->isFinished() || task->hasError() || task->isCanceled()) {
        return;
    }

    QVariantMap data;

    AnnotationTableObject * genesTable = task->getGenes();
    data[GENE_ANNOTATION] = QVariant::fromValue(context->getDataStorage()->putAnnotationTable(genesTable));

    AnnotationTableObject * peaksTable = task->getPeaks();
    data[PEAK_ANNOTATION] = QVariant::fromValue(context->getDataStorage()->putAnnotationTable(peaksTable));

    data[GENE_ANNOTATION_URL] = task->getGenesUrl();
    data[PEAK_ANNOTATION_URL] = task->getPeaksUrl();

    output->put(Message(output->getBusType(), data));

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
        output->setEnded();
    }
}

Peak2GeneSettings Peak2GeneWorker::createPeak2GeneSettings(U2OpStatus &os) {
    Q_UNUSED(os);
    Peak2GeneSettings settings;

    settings.outpos = actor->getParameter(OUTPOS)->getAttributeValue<QString>(context);
    settings.symbol = actor->getParameter(SYMBOL)->getAttributeValue<bool>(context);
    settings.distance = actor->getParameter(DISTANCE)->getAttributeValue<int>(context);
    settings.genomePath = actor->getParameter(GENOME)->getAttributeValue<QString>(context);

    return settings;
}

/************************************************************************/
/* Peak2GeneComboBoxWithUrlsDelegate */
/************************************************************************/
void Peak2GeneComboBoxWithUrlsDelegate::updateUgeneSettings() {
    updateDataPath(getDataPathName(), Peak2GeneSupport::REFGENE_DIR_NAME);
    updateDataPath(Peak2GeneSupport::ENTREZ_TRANSLATION_DATA_NAME, Peak2GeneSupport::TRANSLATIONS_DIR_NAME);
}

QString Peak2GeneComboBoxWithUrlsDelegate::getDataPathName() {
    return Peak2GeneSupport::REF_GENES_DATA_NAME;
}

QString Peak2GeneComboBoxWithUrlsDelegate::getAttributeName() {
    return GENOME;
}

/************************************************************************/
/* Factory */
/************************************************************************/


void Peak2GeneWorkerFactory::init() {

    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(Peak2GeneSupport::REF_GENES_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }

    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor treatDesc(TREAT_SLOT_ID,
        Peak2GeneWorker::tr("Treatment features"),
        Peak2GeneWorker::tr("Result peaks of MACS."));
    inTypeMap[treatDesc] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        Peak2GeneWorker::tr("Peak2gene data"),
        Peak2GeneWorker::tr("MACS peaks to get the refgenes near the peak summit/center."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);

    //out port
    QMap<Descriptor, DataTypePtr> outTypeMap;
    Descriptor geneRegDesc(GENE_ANNOTATION,
        Peak2GeneWorker::tr("Gene regions"),
        Peak2GeneWorker::tr("Annotation for each gene, containing all the peaks nearby."));
    Descriptor peakRegDesc(PEAK_ANNOTATION,
        Peak2GeneWorker::tr("Peak regions"),
        Peak2GeneWorker::tr("Annotation for each peak, containing all the genes nearby."));
    Descriptor geneRegUrlDesc(GENE_ANNOTATION_URL,
        Peak2GeneWorker::tr("Gene regions URL"),
        Peak2GeneWorker::tr("URL to file with annotation for each gene, containing all the peaks nearby."));
    Descriptor peakRegUrlDesc(PEAK_ANNOTATION_URL,
        Peak2GeneWorker::tr("Peak regions URL"),
        Peak2GeneWorker::tr("URL to file with annotation for each peak, containing all the genes nearby."));

    Descriptor outPortDesc(OUT_PORT_DESCR,
        Peak2GeneWorker::tr("Peak2gene output data"),
        Peak2GeneWorker::tr("Genes containing all the peaks nearby and peaks containing all the genes nearby."));

    outTypeMap[geneRegDesc] = BaseTypes::ANNOTATION_TABLE_TYPE();
    outTypeMap[peakRegDesc] = BaseTypes::ANNOTATION_TABLE_TYPE();
    outTypeMap[geneRegUrlDesc] = BaseTypes::STRING_TYPE();
    outTypeMap[peakRegUrlDesc] = BaseTypes::STRING_TYPE();

    DataTypePtr outTypeSet(new MapDataType(OUT_TYPE_ID, outTypeMap));
    portDescs << new PortDescriptor(outPortDesc, outTypeSet, false, true);

     QList<Attribute*> attrs;
     {
         Descriptor outpos(OUTPOS,
             Peak2GeneWorker::tr("Output type"),
             Peak2GeneWorker::tr("Select which type of genes need to output. "
             "<b>up</b> for genes upstream to peak summit, "
             "<b>down</b> for genes downstream to peak summit, "
             "<b>all</b> for both <b>up</b> and <b>down</b> (--op)."));
         Descriptor symbol(SYMBOL,
             Peak2GeneWorker::tr("Official gene symbols"),
             Peak2GeneWorker::tr("Output <b>official gene symbol</b> instead of <b>refseq name</b> (--symbol)."));
         Descriptor p2g_distance(DISTANCE,
             Peak2GeneWorker::tr("Distance"),
             Peak2GeneWorker::tr("Set a number which unit is base. It will get the refGenes in n bases from peak center (--distance)."));
         Descriptor p2g_genome(GENOME,
             Peak2GeneWorker::tr("Genome file"),
             Peak2GeneWorker::tr("Select a genome file (sqlite3 file) to search refGenes (--genome)."));

         Attribute* annGrAttr = NULL;
         if (dataPath){
             const QList<QString>& dataNames = dataPath->getDataNames();
             if (!dataNames.isEmpty()){
                 int hg19Idx = dataNames.indexOf("hg19");
                 if (hg19Idx == -1){
                     annGrAttr = new Attribute(p2g_genome, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
                 }else{
                     annGrAttr = new Attribute(p2g_genome, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.at(hg19Idx)));
                 }

             }else{
                 annGrAttr = new Attribute(p2g_genome, BaseTypes::STRING_TYPE(), true);
             }
         }else{
             annGrAttr = new Attribute(p2g_genome, BaseTypes::STRING_TYPE(), true);
        }

        attrs << annGrAttr;
        attrs << new Attribute(outpos, BaseTypes::STRING_TYPE(), false, QVariant(Peak2GeneSettings::OUT_TYPE_ALL));
        attrs << new Attribute(symbol, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(p2g_distance, BaseTypes::NUM_TYPE(), false, QVariant(3000));
     }

     QMap<QString, PropertyDelegate*> delegates;
     {
         {
            QVariantMap vm;
            vm["minimum"] = QVariant(0);
            vm["maximum"] = INT_MAX;
            vm["singleStep"] = QVariant(100);

            delegates[DISTANCE] = new SpinBoxDelegate(vm);
         }

         {
             QVariantMap vm;
             if (dataPath){
                 vm = dataPath->getDataItemsVariantMap();
             }
             delegates[GENOME] = new Peak2GeneComboBoxWithUrlsDelegate(vm);
         }


         QVariantMap contentMap;
         contentMap[Peak2GeneSettings::OUT_TYPE_ALL] = Peak2GeneSettings::OUT_TYPE_ALL;
         contentMap[Peak2GeneSettings::OUT_TYPE_DOWNSTREAM] = Peak2GeneSettings::OUT_TYPE_DOWNSTREAM;
         contentMap[Peak2GeneSettings::OUT_TYPE_UPSTREAM] = Peak2GeneSettings::OUT_TYPE_UPSTREAM;
         delegates[OUTPOS] = new ComboBoxDelegate(contentMap);

     }

    Descriptor protoDesc(Peak2GeneWorkerFactory::ACTOR_ID,
        Peak2GeneWorker::tr("Annotate Peaks with peak2gene"),
        Peak2GeneWorker::tr("Gets refGenes near the ChIP regions identified by a peak-caller."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new Peak2GenePrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(Peak2GeneSupport::ET_PEAK2GENE_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CHIP_SEQ(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new Peak2GeneWorkerFactory());
}

Worker *Peak2GeneWorkerFactory::createWorker(Actor *a) {
    return new Peak2GeneWorker(a);
}

QString Peak2GenePrompter::composeRichDoc() {
    QString res = "";

    Actor* treatProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(TREAT_SLOT_ID);

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString treatUrl = treatProducer ? treatProducer->getLabel() : unsetStr;

    QString dbUrl = getHyperlink(GENOME, getURL(GENOME));

    res.append(tr("Uses annotations from <u>%1</u> as peak regions ").arg(treatUrl));
    res.append(tr(" to annotate with genes nearby from <u>%1</u>.").arg(dbUrl));

    return res;
}

} // LocalWorkflow
} // U2
