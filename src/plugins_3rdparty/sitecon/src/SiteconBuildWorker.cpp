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

#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

#include "SiteconBuildDialogController.h"
#include "SiteconIOWorkers.h"
#include "SiteconPlugin.h"
#include "SiteconWorkers.h"

namespace U2 {
namespace LocalWorkflow {

static const QString WINDOW_ATTR("window-size");
static const QString LEN_ATTR("calibrate-length");
static const QString SEED_ATTR("seed");
static const QString ALG_ATTR("weight-algorithm");

static const QString OUT_SITECON_PORT_ID("out-sitecon");

const QString SiteconBuildWorker::ACTOR_ID("sitecon-build");

void SiteconBuildWorker::registerProto() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    QMap<Descriptor, DataTypePtr> m;
    Descriptor id(BasePorts::IN_MSA_PORT_ID(), SiteconBuildWorker::tr("Input alignment"), SiteconBuildWorker::tr("Input multiple sequence alignment for building statistical model."));
    Descriptor ud(BaseSlots::URL_SLOT().getId(), SiteconBuildWorker::tr("Origin"), SiteconBuildWorker::tr("Location of input alignment, used as optional hint for model description."));
    m[ud] = BaseTypes::STRING_TYPE();
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    DataTypePtr t(new MapDataType(Descriptor("build.sitecon.content"), m));

    Descriptor od(OUT_SITECON_PORT_ID, SiteconBuildWorker::tr("Sitecon model"), SiteconBuildWorker::tr("Produced statistical model of specified TFBS data."));
    p << new PortDescriptor(id, t, true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[SiteconWorkerFactory::SITECON_SLOT] = SiteconWorkerFactory::SITECON_MODEL_TYPE();
    p << new PortDescriptor(od, DataTypePtr(new MapDataType("sitecon.build.out", outM)), false /*input*/, true /*multi*/);
    
    {
        Descriptor wd(WINDOW_ATTR, SiteconBuildWorker::tr("Window size, bp"), SiteconBuildWorker::tr("Window size."));
        Descriptor ld(LEN_ATTR, SiteconBuildWorker::tr("Calibration length"), SiteconBuildWorker::tr("Calibration length."));
        Descriptor sd(SEED_ATTR, SiteconBuildWorker::tr("Random seed"), SiteconBuildWorker::tr("Random seed."));
        Descriptor ad(ALG_ATTR, SiteconBuildWorker::tr("Weight algorithm"), SiteconBuildWorker::tr("Weight algorithm."));

        a << new Attribute(wd, BaseTypes::NUM_TYPE(), false, 40);
        a << new Attribute(ld, BaseTypes::NUM_TYPE(), false, 1000*1000);
        a << new Attribute(sd, BaseTypes::NUM_TYPE(), false, 0);
        a << new Attribute(ad, BaseTypes::BOOL_TYPE(), false, int(SiteconWeightAlg_None));
    }

    Descriptor desc(ACTOR_ID, tr("Build SITECON model"),
        tr("Builds statistical profile for SITECON. The SITECON is a program for probabilistic recognition of transcription factor binding sites."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;    
    
    {
        QVariantMap m2; m2["minimum"] = 0; m2["maximum"] = INT_MAX;
        delegates[SEED_ATTR] = new SpinBoxDelegate(m2);
    }
    {
        QVariantMap m2; m2["minimum"] = 1; m2["maximum"] = 1000;
        delegates[WINDOW_ATTR] = new SpinBoxDelegate(m2);
    }
    {
        QVariantMap modeMap; 
        modeMap["100K"] = 100*1000;
        modeMap["500K"] = 500*1000;
        modeMap["1M"] = 1000*1000;
        modeMap["5M"] = 5*1000*1000;
        delegates[LEN_ATTR] = new ComboBoxDelegate(modeMap);
    }
    {
        QVariantMap modeMap; 
        modeMap[tr("None")] = QVariant(SiteconWeightAlg_None);
        modeMap[tr("Algorithm2")] = QVariant(SiteconWeightAlg_Alg2);
        delegates[ALG_ATTR] = new ComboBoxDelegate(modeMap);
    }

    proto->setPrompter(new SiteconBuildPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":sitecon/images/sitecon.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
}

QString SiteconBuildPrompter::composeRichDoc() {
    QString prod = getProducersOrUnset(BasePorts::IN_MSA_PORT_ID(), BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId());
    return  tr("For each MSA from <u>%1</u>, build SITECON model.").arg(prod);
}

void SiteconBuildWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(OUT_SITECON_PORT_ID);
}

Task* SiteconBuildWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.props = SiteconPlugin::getDinucleotiteProperties();
        cfg.randomSeed = actor->getParameter(SEED_ATTR)->getAttributeValue<int>(context);
        if(cfg.randomSeed<0){
            return new FailTask(tr("Random seed can not be less zero"));
        }
        cfg.secondTypeErrorCalibrationLen = actor->getParameter(LEN_ATTR)->getAttributeValue<int>(context);
        if(cfg.secondTypeErrorCalibrationLen<0){
            return new FailTask(tr("Calibration length can not be less zero"));
        }
        cfg.weightAlg = SiteconWeightAlg(actor->getParameter(ALG_ATTR)->getAttributeValue<int>(context));
        cfg.windowSize = actor->getParameter(WINDOW_ATTR)->getAttributeValue<int>(context);
        if(cfg.windowSize<0){
            return new FailTask(tr("Window size can not be less zero"));
        }
        mtype = SiteconWorkerFactory::SITECON_MODEL_TYPE();
        QVariantMap data = inputMessage.getData().toMap();
        QString url = data.value(BaseSlots::URL_SLOT().getId()).toString();
        
        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
        const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();

        Task* t = new SiteconBuildTask(cfg, msa, url);
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void SiteconBuildWorker::sl_taskFinished() {
    SiteconBuildTask* t = qobject_cast<SiteconBuildTask*>(sender());
    if ( t->isCanceled( ) ) {
        return;
    }
    if (t->getState() != Task::State_Finished) return;
    SiteconModel model = t->getResult();
    QVariant v = qVariantFromValue<SiteconModel>(model);
    output->put(Message(mtype, v));
    algoLog.info(tr("Built SITECON model from: %1").arg(model.aliURL));
}

} //namespace LocalWorkflow
} //namespace U2
