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
#include <U2Core/L10n.h>
#include <U2Core/QVariantUtils.h>
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

#include "ConductGOSupport.h"
#include "ConductGOWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ConductGOWorkerFactory::ACTOR_ID("conduct-go-id");

static const QString ANNOT_SLOT_ID("in-ann");

static const QString IN_TYPE_ID("conduct-go-data");

static const QString IN_PORT_DESCR("in-data");

static const QString OUTPUT_DIR("output-dir");
static const QString TITLE("title");
static const QString GENE_UNIVERSE("gene-universe");


/************************************************************************/
/* Worker */
/************************************************************************/
ConductGOWorker::ConductGOWorker(Actor *p)
: BaseWorker(p)
, inChannel(NULL)
{

}

void ConductGOWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
}

Task *ConductGOWorker::tick() {
    if (inChannel->hasMessage()) {
        U2OpStatus2Log os;

        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap data = m.getData().toMap();

        if (!data.contains(ANNOT_SLOT_ID)) {
            os.setError("Annotations slot is empty");
            return new FailTask(os.getError());
        }

        ConductGOSettings settings = createConductGOSettings(data[ANNOT_SLOT_ID].toString());

        ConductGOTask* t = new ConductGOTask(settings);
        t->addListeners(createLogListeners());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }else if (inChannel->isEnded()) {
        setDone();
    }
    return NULL;
}

void ConductGOWorker::cleanup() {

}

void ConductGOWorker::sl_taskFinished() {
    ConductGOTask *t = dynamic_cast<ConductGOTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    const QStringList& resFileNames = t->getResultFileNames();
    foreach(const QString& fn, resFileNames){
        QString url = t->getSettings().outDir + "/" + fn;
        context->getMonitor()->addOutputFile(url, getActor()->getId(), QFileInfo(url).suffix() == "html");
    }

    if (inChannel->isEnded() && !inChannel->hasMessage()) {
        setDone();
    }
}

ConductGOSettings ConductGOWorker::createConductGOSettings(const QString &treatUrl) {
    ConductGOSettings settings;

    settings.outDir = getValue<QString>(OUTPUT_DIR);
    settings.title = getValue<QString>(TITLE);
    settings.treatUrl = treatUrl;
    settings.geneUniverse = getValue<QString>(GENE_UNIVERSE);

    return settings;
}

/************************************************************************/
/* Factory */
/************************************************************************/


void ConductGOWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    Descriptor treatDesc(ANNOT_SLOT_ID,
        ConductGOWorker::tr("Target genes"),
        ConductGOWorker::tr("URL to file with genes to identify over represented GO terms."));
    inTypeMap[treatDesc] = BaseTypes::STRING_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        ConductGOWorker::tr("Conduct GO data"),
        ConductGOWorker::tr("URL to file with genes to identify over represented GO terms."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    portDescs << new PortDescriptor(inPortDesc, inTypeSet, true);

    QList<Attribute*> attrs;
    {
         Descriptor outDir(OUTPUT_DIR,
             ConductGOWorker::tr("Output folder"),
             ConductGOWorker::tr("The folder to store Conduct GO results."));
         Descriptor titleDescr(TITLE,
             ConductGOWorker::tr("Title"),
             ConductGOWorker::tr("Title is used to name the output files - so make it meaningful."));
         Descriptor guDescr(GENE_UNIVERSE,
             ConductGOWorker::tr("Gene Universe"),
             ConductGOWorker::tr("Select a gene universe."));


        attrs << new Attribute(outDir, BaseTypes::STRING_TYPE(), true, QVariant(""));
        attrs << new Attribute(titleDescr, BaseTypes::STRING_TYPE(), true, QVariant("Default"));
        attrs << new Attribute(guDescr, BaseTypes::STRING_TYPE(), true, ConductGOSettings::UNIVERSE_HGU133A);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
          delegates[OUTPUT_DIR] = new URLDelegate("", "", false, true);


          {
              QVariantMap vm;

              vm[ConductGOSettings::UNIVERSE_HGU133A] = ConductGOSettings::UNIVERSE_HGU133A;
              vm[ConductGOSettings::UNIVERSE_HGU133B] = ConductGOSettings::UNIVERSE_HGU133B;
              vm[ConductGOSettings::UNIVERSE_HGU133PLUS2] = ConductGOSettings::UNIVERSE_HGU133PLUS2;
              vm[ConductGOSettings::UNIVERSE_HGU95AV2] = ConductGOSettings::UNIVERSE_HGU95AV2;
              vm[ConductGOSettings::UNIVERSE_MOUSE_430a2] = ConductGOSettings::UNIVERSE_MOUSE_430a2;
              vm[ConductGOSettings::UNIVERSE_CELEGANS] = ConductGOSettings::UNIVERSE_CELEGANS;
              vm[ConductGOSettings::UNIVERSE_DROSOPHILA2] = ConductGOSettings::UNIVERSE_DROSOPHILA2;
              vm[ConductGOSettings::UNIVERSE_ORG_HS_EG] = ConductGOSettings::UNIVERSE_ORG_HS_EG;
              vm[ConductGOSettings::UNIVERSE_ORG_MM_EG] = ConductGOSettings::UNIVERSE_ORG_MM_EG;
              vm[ConductGOSettings::UNIVERSE_ORG_CE_EG] = ConductGOSettings::UNIVERSE_ORG_CE_EG;
              vm[ConductGOSettings::UNIVERSE_ORG_DM_EG] = ConductGOSettings::UNIVERSE_ORG_DM_EG;

              delegates[GENE_UNIVERSE] = new ComboBoxDelegate(vm);
          }


    }

    Descriptor protoDesc(ConductGOWorkerFactory::ACTOR_ID,
    ConductGOWorker::tr("Conduct GO"),
    ConductGOWorker::tr("Given a list of genes, using Bioconductor (GO, GOstats) and  DAVID at NIH."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new ConductGOPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(ConductGOSupport::ET_GO_ANALYSIS_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CHIP_SEQ(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new ConductGOWorkerFactory());
}

Worker *ConductGOWorkerFactory::createWorker(Actor *a) {
    return new ConductGOWorker(a);
}

QString ConductGOPrompter::composeRichDoc() {
    QString res = "";

    Actor* annProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(ANNOT_SLOT_ID);

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString annUrl = annProducer ? annProducer->getLabel() : unsetStr;

    QString dir = getHyperlink(OUTPUT_DIR, getURL(OUTPUT_DIR));

    res.append(tr("Uses annotations from <u>%1</u> as gene list to conduct GO.").arg(annUrl));

    res.append(tr(" Outputs all result files to <u>%1</u> folder").arg(dir));
    res.append(".");

    return res;
}

} // LocalWorkflow
} // U2
