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
#include <U2Core/L10n.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ConservationPlotSupport.h"
#include "ConservationPlotWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ConservationPlotWorkerFactory::ACTOR_ID("conservation_plot-id");

static const QString ANNOT_SLOT_ID("cp_treat-ann");

static const QString IN_TYPE_ID("conservation_plot-data");

static const QString IN_PORT_DESCR("in-data");

static const QString OUTPUT_FILE("output-file");
static const QString TITLE("title");
static const QString LABEL("label");
static const QString ASSEMBLY_VER("assembly_version");
static const QString WINDOW_S("windos_s");
static const QString HEIGHT("height");
static const QString WIDTH("width");


/************************************************************************/
/* Worker */
/************************************************************************/
ConservationPlotWorker::ConservationPlotWorker(Actor *p)
: BaseWorker(p)
, inChannel(NULL)
{

}

void ConservationPlotWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
}

Task *ConservationPlotWorker::tick() {
    U2OpStatus2Log os;

    while (inChannel->hasMessage()) {

        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        if (!data.contains(ANNOT_SLOT_ID)) {
            os.setError("Annotations slot is empty");
            return new FailTask(os.getError());
        }

        plotData = StorageUtils::getAnnotationTableHandlers(data[ANNOT_SLOT_ID]);
    }

    if (!inChannel->isEnded()) {
        return NULL;
    }

    ConservationPlotSettings settings = createConservationPlotSettings(os);
    if (os.hasError()) {
        return new FailTask(os.getError());
    }

    ConservationPlotTask* t = new ConservationPlotTask(settings, context->getDataStorage(), plotData);
    t->addListeners(createLogListeners());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;

    if (inChannel->isEnded()) {
        setDone();
    }

    return NULL;
}

void ConservationPlotWorker::cleanup() {
    plotData.clear();
}

void ConservationPlotWorker::sl_taskFinished() {
    ConservationPlotTask *t = dynamic_cast<ConservationPlotTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    context->getMonitor()->addOutputFile(t->getSettings().outFile, getActor()->getId(), true);

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
    }
}

U2::ConservationPlotSettings ConservationPlotWorker::createConservationPlotSettings( U2OpStatus &os ){
    Q_UNUSED(os);
    ConservationPlotSettings settings;

    settings.outFile = getValue<QString>(OUTPUT_FILE);
    settings.title = getValue<QString>(TITLE);
    settings.label = getValue<QString>(LABEL);
    settings.assemblyVersion= getValue<QString>(ASSEMBLY_VER);
    settings.windowSize = getValue<int>(WINDOW_S);
    settings.height = getValue<int>(HEIGHT);
    settings.width = getValue<int>(WIDTH);

    return settings;
}


/************************************************************************/
/* ConservationPlotComboBoxWithUrlsDelegate */
/************************************************************************/
void ConservationPlotComboBoxWithUrlsDelegate::updateUgeneSettings() {
    updateDataPath(getDataPathName(), ConservationPlotSupport::CONSERVATION_DIR_NAME, true);
}

QString ConservationPlotComboBoxWithUrlsDelegate::getDataPathName() {
    return ConservationPlotSupport::CONSERVATION_DATA_NAME;
}

QString ConservationPlotComboBoxWithUrlsDelegate::getAttributeName() {
    return ASSEMBLY_VER;
}

/************************************************************************/
/* Factory */
/************************************************************************/


void ConservationPlotWorkerFactory::init() {
    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(ConservationPlotSupport::CONSERVATION_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }

    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor treatDesc(ANNOT_SLOT_ID,
        ConservationPlotWorker::tr("Input regions"),
        ConservationPlotWorker::tr("Regions (centered at peak summits for better performance)."));
    inTypeMap[treatDesc] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        ConservationPlotWorker::tr("conservation_plot data"),
        ConservationPlotWorker::tr("Regions to plot the PhastCons scores profiles."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);

    QList<Attribute*> attrs;
    {
         Descriptor outFile(OUTPUT_FILE,
             ConservationPlotWorker::tr("Output file"),
             ConservationPlotWorker::tr("File to store phastcons results (BMP)."));
         Descriptor titleDescr(TITLE,
             ConservationPlotWorker::tr("Title"),
             ConservationPlotWorker::tr("Title of the figure (--title)."));
         Descriptor labelDescr(LABEL,
             ConservationPlotWorker::tr("Label"),
             ConservationPlotWorker::tr("Label of data in the figure (--bed-label)."));
         Descriptor assemblyVersion(ASSEMBLY_VER,
             ConservationPlotWorker::tr("Assembly version"),
             ConservationPlotWorker::tr("The folder to store phastcons scores (--phasdb)."));
         Descriptor windowSizeDescr(WINDOW_S,
             ConservationPlotWorker::tr("Window width"),
             ConservationPlotWorker::tr("Window width centered at middle of regions (-w)."));
         Descriptor heightDescr(HEIGHT,
             ConservationPlotWorker::tr("Height"),
             ConservationPlotWorker::tr("Height of plot (--height)."));
         Descriptor widthDescr(WIDTH,
             ConservationPlotWorker::tr("Width"),
             ConservationPlotWorker::tr("Width of plot (--width)."));


        attrs << new Attribute(outFile, BaseTypes::STRING_TYPE(), true, QVariant(""));
        attrs << new Attribute(titleDescr, BaseTypes::STRING_TYPE(), true, QVariant("Average Phastcons around the Center of Sites"));
        attrs << new Attribute(labelDescr, BaseTypes::STRING_TYPE(), true, QVariant("Conservation_at_peak_summits"));
        Attribute* assemblyVerAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                assemblyVerAttr = new Attribute(assemblyVersion, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
            }else{
                assemblyVerAttr = new Attribute(assemblyVersion, BaseTypes::STRING_TYPE(), true);
            }
        }else{
            assemblyVerAttr = new Attribute(assemblyVersion, BaseTypes::STRING_TYPE(), true);
        }
        attrs << assemblyVerAttr;
        attrs << new Attribute(windowSizeDescr, BaseTypes::NUM_TYPE(), false, QVariant(1000));
        attrs << new Attribute(heightDescr, BaseTypes::NUM_TYPE(), false, QVariant(1000));
        attrs << new Attribute(widthDescr, BaseTypes::NUM_TYPE(), false, QVariant(1000));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
          delegates[OUTPUT_FILE] = new URLDelegate(DialogUtils::prepareFileFilter(ConservationPlotWorker::tr("Conservation plot file"), QStringList("bmp"), true), "", false, false);
         {
            QVariantMap vm;
            vm["minimum"] = QVariant(0);
            vm["maximum"] = INT_MAX;
            vm["singleStep"] = QVariant(1000);

            delegates[WINDOW_S] = new SpinBoxDelegate(vm);
            delegates[HEIGHT] = new SpinBoxDelegate(vm);
            delegates[WIDTH] = new SpinBoxDelegate(vm);
         }

         {
             QVariantMap vm;
             if (dataPath){
                 vm = dataPath->getDataItemsVariantMap();
             }
             delegates[ASSEMBLY_VER] = new ConservationPlotComboBoxWithUrlsDelegate(vm, true);
         }
    }

    Descriptor protoDesc(ConservationPlotWorkerFactory::ACTOR_ID,
    ConservationPlotWorker::tr("Build Conservation Plot"),
    ConservationPlotWorker::tr("Plots the PhastCons scores profiles."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new ConservationPlotPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(ConservationPlotSupport::ET_CONSERVATION_PLOT_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CHIP_SEQ(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new ConservationPlotWorkerFactory());
}

Worker *ConservationPlotWorkerFactory::createWorker(Actor *a) {
    return new ConservationPlotWorker(a);
}

QString ConservationPlotPrompter::composeRichDoc() {
    QString res = "";

     Actor* annProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(ANNOT_SLOT_ID);

     QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
     QString annUrl = annProducer ? annProducer->getLabel() : unsetStr;

     QString file = getHyperlink(OUTPUT_FILE, getURL(OUTPUT_FILE));
     QString assembly = getHyperlink(ASSEMBLY_VER, getURL(ASSEMBLY_VER));

     res.append(tr("Uses annotations from <u>%1</u> as peak regions for conservation plot.").arg(annUrl));
     res.append(tr(" Conservations scores from <u>%1</u>.").arg(assembly));
     res.append(tr(" Outputs the result to <u>%1</u>").arg(file));
     res.append(".");

    return res;
}

} // LocalWorkflow
} // U2
