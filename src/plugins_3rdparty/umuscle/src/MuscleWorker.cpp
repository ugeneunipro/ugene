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
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/NoFailTaskWrapper.h>
#include <U2Lang/WorkflowEnv.h>

#include "MuscleAlignDialogController.h"
#include "MuscleConstants.h"
#include "MuscleWorker.h"
#include "TaskLocalStorage.h"

namespace U2 {
namespace LocalWorkflow {


/****************************
 * MuscleWorkerFactory
 ****************************/
const QString MuscleWorkerFactory::ACTOR_ID("muscle");
static const QString MODE_ATTR("mode");
static const QString STABLE_ATTR("stable");
static const QString MAX_ITERATIONS_ATTR("max-iterations");
static const QString RANGE_ATTR("range");
static const QString RANGE_ATTR_DEFAULT_VALUE("Whole alignment");

void MuscleWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_MSA_PORT_ID(), MuscleWorker::tr("Input MSA"), 
        MuscleWorker::tr("Multiple sequence alignment to be processed."));
    Descriptor oud(BasePorts::OUT_MSA_PORT_ID(), MuscleWorker::tr("Multiple sequence alignment"), MuscleWorker::tr("Result of alignment."));
    
    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("muscle.in.msa", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("muscle.out.msa", outM)), false /*input*/, true /*multi*/);
    
    Descriptor mod(MODE_ATTR, MuscleWorker::tr("Mode"), 
        MuscleWorker::tr("Selector of preset configurations, that give you the choice of optimizing accuracy,"
                            " speed, or some compromise between the two. The default favors accuracy."));
    Descriptor sd(STABLE_ATTR, MuscleWorker::tr("Stable order"), 
        MuscleWorker::tr("Do not rearrange aligned sequences (-stable switch of MUSCLE). "
                            "<p>Otherwise, MUSCLE re-arranges sequences so that similar sequences are adjacent in the output file."
                            " This makes the alignment easier to evaluate by eye. "));
    Descriptor mi(MAX_ITERATIONS_ATTR, MuscleWorker::tr("Max iterations"), 
        MuscleWorker::tr("Maximum number of iterations."));
    Descriptor ra(RANGE_ATTR, MuscleWorker::tr("Region to align"), 
        MuscleWorker::tr("Whole alignment or column range e.g. <b>1..100</b>."));
    
    a << new Attribute(mod, BaseTypes::NUM_TYPE(), false, 0);
    a << new Attribute(sd, BaseTypes::BOOL_TYPE(), false, true);
    a << new Attribute(mi, BaseTypes::NUM_TYPE(), false, -1);
    a << new Attribute(ra, BaseTypes::STRING_TYPE(), false, RANGE_ATTR_DEFAULT_VALUE);
    
    Descriptor desc(ACTOR_ID, MuscleWorker::tr("Align with MUSCLE"), 
        MuscleWorker::tr("MUSCLE is public domain multiple alignment software for protein and nucleotide sequences."
                            "<p><dfn>MUSCLE stands for MUltiple Sequence Comparison by Log-Expectation.</dfn></p>"));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    
    QMap<QString, PropertyDelegate*> delegates;    
    {
        QVariantMap vm; 
        vm[DefaultModePreset().name] = 0;
        vm[LargeModePreset().name] = 1;
        vm[RefineModePreset().name] = 2;
        delegates[MODE_ATTR] = new ComboBoxDelegate(vm);
        
        QVariantMap nMap; 
        nMap["minimum"] = 2;
        nMap["maximum"] = INT_MAX;
        delegates[MAX_ITERATIONS_ATTR] = new SpinBoxDelegate(nMap);
    }
    
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new MusclePrompter());
    proto->setIconPath(":umuscle/images/muscle_16.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new MuscleWorkerFactory());
}

/****************************
* MusclePrompter
****************************/
QString MusclePrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_MSA_PORT_ID());
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";
    QString preset;
    switch (getParameter(MODE_ATTR).toInt()) {
        case 0: preset = DefaultModePreset().name; break;
        case 1: preset = LargeModePreset().name; break;
        case 2: preset = RefineModePreset().name; break;
    }

    QString doc = tr("Aligns each MSA supplied <u>%1</u> with MUSCLE using \"<u>%2</u>\" mode.")
        .arg(producerName).arg(preset);

    return doc;
}

/****************************
* MuscleWorker
****************************/
MuscleWorker::MuscleWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void MuscleWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(BasePorts::OUT_MSA_PORT_ID());
}

Task* MuscleWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        int mode = actor->getParameter(MODE_ATTR)->getAttributeValue<int>(context);
        switch(mode) {
            case 0: DefaultModePreset().apply(cfg); break;
            case 1: LargeModePreset().apply(cfg); break;
            case 2: RefineModePreset().apply(cfg); break;
        }
        cfg.stableMode = actor->getParameter(STABLE_ATTR)->getAttributeValue<bool>(context);
        int maxIterations = actor->getParameter(MAX_ITERATIONS_ATTR)->getAttributeValue<int>(context);
        if(maxIterations >= 2) {
            cfg.maxIterations = maxIterations;
        }
        
        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
        const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();

        if( msa->isEmpty() ) {
            algoLog.error(tr("An empty MSA '%1' has been supplied to MUSCLE.").arg(msa->getName()));
            return NULL;
        }
        QString range = actor->getParameter(RANGE_ATTR)->getAttributeValue<QString>(context);
        if( range.isEmpty() || range == RANGE_ATTR_DEFAULT_VALUE ) {
            cfg.alignRegion = false;
            cfg.regionToAlign = U2Region(0, msa->getLength());
        } else {
            QStringList words = range.split(".", QString::SkipEmptyParts);
            if( words.size() != 2 ) {
                return new FailTask(QObject::tr("Region should be set as 'start..end', start should be less than end, e.g. '1..100'"));
            }
            bool ok = false;
            int start = words.at(0).toInt(&ok) - 1;
            if(!ok) {
                return new FailTask(QObject::tr("Region should be set as 'start..end', start should be less than end, e.g. '1..100'"));
            }
            start = qMax(1, start);
            ok = false;
            int end = words.at(1).toInt(&ok) - 1;
            if(!ok) {
                return new FailTask(QObject::tr("Region should be set as 'start..end', start should be less than end, e.g. '1..100'"));
            }
            if(end < start) {
                return new FailTask(tr("Region end position should be greater than start position"));
            }
            end = qMin(end, msa->getLength());
            
            cfg.alignRegion = true;
            cfg.regionToAlign = U2Region(start,  end - start + 1);
        }
        
        Task *t = new NoFailTaskWrapper(new MuscleTask(msa, cfg));
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void MuscleWorker::sl_taskFinished() {
    NoFailTaskWrapper *wrapper = qobject_cast<NoFailTaskWrapper*>(sender());
    CHECK(wrapper->isFinished(), );
    MuscleTask* t = qobject_cast<MuscleTask*>(wrapper->originalTask());
    if (t->hasError()) {
        reportError(t->getError());
        return;
    } else if ( t->isCanceled( ) ) {
        return;
    }

    SAFE_POINT(NULL != output, "NULL output!", );
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(t->resultMA);
    QVariantMap msgData;
    msgData[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(msaId);
    output->put(Message(BaseTypes::MULTIPLE_ALIGNMENT_TYPE(), msgData));
    algoLog.info(tr("Aligned %1 with MUSCLE").arg(t->resultMA->getName()));
}

void MuscleWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
