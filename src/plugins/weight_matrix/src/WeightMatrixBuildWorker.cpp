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

#include <U2Algorithm/BuiltInPWMConversionAlgorithms.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/MultipleSequenceAlignment.h>
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

#include "PWMBuildDialogController.h"
#include "WeightMatrixIOWorkers.h"
#include "WeightMatrixPlugin.h"
#include "WeightMatrixWorkers.h"

namespace U2 {
namespace LocalWorkflow {

static const QString ALG_ATTR("weight-algorithm");
static const QString TYPE_ATTR("matrix-type");

static const QString FMATRIX_OUT_PORT_ID("out-fmatrix");
static const QString FMATRIX_IN_PORT_ID("in-fmatrix");
static const QString WMATRIX_OUT_PORT_ID("out-wmatrix");

const QString PWMatrixBuildWorker::ACTOR_ID("wmatrix-build");
const QString PFMatrixBuildWorker::ACTOR_ID("fmatrix-build");
const QString PFMatrixConvertWorker::ACTOR_ID("fmatrix-to-wmatrix");

//////////////////////////////////////////////////////////////////////////
// PWMatrix build worker
//////////////////////////////////////////////////////////////////////////

void PWMatrixBuildWorker::registerProto() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    QMap<Descriptor, DataTypePtr> m;
    Descriptor id(BasePorts::IN_MSA_PORT_ID(), PWMatrixBuildWorker::tr("Input alignment"), 
        PWMatrixBuildWorker::tr("Input multiple sequence alignment for building statistical model."));
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    DataTypePtr t(new MapDataType(Descriptor("build.pwmatrix.content"), m));

    Descriptor od(WMATRIX_OUT_PORT_ID, PWMatrixBuildWorker::tr("Weight matrix"), PWMatrixBuildWorker::tr("Produced statistical model of specified TFBS data."));
    p << new PortDescriptor(id, t, true /*input*/);
    
    QMap<Descriptor, DataTypePtr> outM;
    outM[PWMatrixWorkerFactory::WMATRIX_SLOT] = PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE();
    p << new PortDescriptor(od, DataTypePtr(new MapDataType("wmatrix.build.out", outM)), false /*input*/, true /*multi*/);
    
    {
        Descriptor ad(ALG_ATTR, PWMatrixBuildWorker::tr("Weight algorithm"), 
            QApplication::translate("PWMBuildDialog", "Different weight algorithms uses different functions to build weight matrices. It allows us to get better precision on different data sets. Log-odds, NLG and Match algorithms are sensitive to input matrices with zero values, so some of them may not work on those matrices.", 0));
        a << new Attribute(ad, BaseTypes::STRING_TYPE(), true, BuiltInPWMConversionAlgorithms::BVH_ALGO);
    }

    {
        Descriptor td(TYPE_ATTR, PWMatrixBuildWorker::tr("Matrix type"), PWMatrixBuildWorker::tr("Dinucleic matrices are more detailed, while mononucleic one are more useful for small input data sets."));
            a << new Attribute(td, BaseTypes::BOOL_TYPE(), true, false /* false = mononucleic, true = dinucleic */);
    }

    Descriptor desc(ACTOR_ID, tr("Build Weight Matrix"),
        tr("Builds weight matrix. Weight matrices are used for probabilistic recognition of transcription factor binding sites."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;    

    {
        QVariantMap modeMap;
        QStringList algo = AppContext::getPWMConversionAlgorithmRegistry()->getAlgorithmIds();
        foreach (QString curr, algo) {
            modeMap[curr] = QVariant(curr);
        }
        delegates[ALG_ATTR] = new ComboBoxDelegate(modeMap);
    }

    {
        QVariantMap modeMap;
        modeMap[tr("Mononucleic")] = QVariant(false);
        modeMap[tr("Dinucleic")] = QVariant(true);
        delegates[TYPE_ATTR] = new ComboBoxDelegate(modeMap);
    }
    
    proto->setPrompter(new PWMatrixBuildPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":weight_matrix/images/weight_matrix.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
}

QString PWMatrixBuildPrompter::composeRichDoc() {
    QString prod = getProducersOrUnset(BasePorts::IN_MSA_PORT_ID(), BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId());
    return  tr("For each MSA from <u>%1</u>, build weight matrix.").arg(prod);
}

void PWMatrixBuildWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(WMATRIX_OUT_PORT_ID);
}

Task* PWMatrixBuildWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        mtype = PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE();
        cfg.algo = actor->getParameter(ALG_ATTR)->getAttributeValue<QString>(context);
        cfg.type = actor->getParameter(TYPE_ATTR)->getAttributeValue<bool>(context) ? PM_DINUCLEOTIDE : PM_MONONUCLEOTIDE;

        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);

        Task* t = new PWMatrixBuildTask(cfg, msaObj->getMultipleAlignment());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void PWMatrixBuildWorker::sl_taskFinished() {
    PWMatrixBuildTask* t = qobject_cast<PWMatrixBuildTask*>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()){
        return;
    }
    PWMatrix model = t->getResult();
    QVariant v = qVariantFromValue<PWMatrix>(model);
    output->put(Message(mtype, v));
}

//////////////////////////////////////////////////////////////////////////
// PFMatrix build worker
//////////////////////////////////////////////////////////////////////////

void PFMatrixBuildWorker::registerProto() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    QMap<Descriptor, DataTypePtr> m;
    Descriptor id(BasePorts::IN_MSA_PORT_ID(), PFMatrixBuildWorker::tr("Input alignment"), 
        PFMatrixBuildWorker::tr("Input multiple sequence alignment for building statistical model."));
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    DataTypePtr t(new MapDataType(Descriptor("build.pfmatrix.content"), m));

    Descriptor od(FMATRIX_OUT_PORT_ID, PFMatrixBuildWorker::tr("Frequency matrix"), 
        PFMatrixBuildWorker::tr("Produced statistical model of specified TFBS data."));
    p << new PortDescriptor(id, t, true /*input*/);
    
    QMap<Descriptor, DataTypePtr> outM;
    outM[PFMatrixWorkerFactory::FMATRIX_SLOT] = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
    p << new PortDescriptor(od, DataTypePtr(new MapDataType("fmatrix.build.out", outM)), false /*input*/, true /*multi*/);
    
    {
        Descriptor td(TYPE_ATTR, PWMatrixBuildWorker::tr("Matrix type"), PWMatrixBuildWorker::tr("Dinucleic matrices are more detailed, while mononucleic one are more useful for small input data sets."));
        a << new Attribute(td, BaseTypes::BOOL_TYPE(), true, false /* false = mononucleic, true = dinucleic */);
    }

    Descriptor desc(ACTOR_ID, tr("Build Frequency Matrix"),
        tr("Builds frequency matrix. Frequency matrices are used for probabilistic recognition of transcription factor binding sites."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap modeMap;
        modeMap[tr("Mononucleic")] = QVariant(false);
        modeMap[tr("Dinucleic")] = QVariant(true);
        delegates[TYPE_ATTR] = new ComboBoxDelegate(modeMap);
    }

    proto->setPrompter(new PFMatrixBuildPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":weight_matrix/images/weight_matrix.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
}

QString PFMatrixBuildPrompter::composeRichDoc() {
    QString prod = getProducersOrUnset(BasePorts::IN_MSA_PORT_ID(), BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId());
    return  tr("For each MSA from <u>%1</u>, build Frequency Matrix.").arg(prod);
}

void PFMatrixBuildWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(FMATRIX_OUT_PORT_ID);
}

Task* PFMatrixBuildWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        mtype = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
        QVariantMap data = inputMessage.getData().toMap();
        cfg.type = actor->getParameter(TYPE_ATTR)->getAttributeValue<bool>(context) ? PM_DINUCLEOTIDE : PM_MONONUCLEOTIDE;

        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);

        Task* t = new PFMatrixBuildTask(cfg, msaObj->getMultipleAlignment());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void PFMatrixBuildWorker::sl_taskFinished() {
    PFMatrixBuildTask* t = qobject_cast<PFMatrixBuildTask*>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()){
        return;
    }
    PFMatrix model = t->getResult();
    QVariant v = qVariantFromValue<PFMatrix>(model);
    output->put(Message(mtype, v));
}

//////////////////////////////////////////////////////////////////////////
// PFMatrix convert worker
//////////////////////////////////////////////////////////////////////////

void PFMatrixConvertWorker::registerProto() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    QMap<Descriptor, DataTypePtr> m;
    Descriptor id(FMATRIX_IN_PORT_ID, PFMatrixConvertWorker::tr("Frequency matrix"), 
        PFMatrixConvertWorker::tr("Frequency matrix to convert."));
    m[PFMatrixWorkerFactory::FMATRIX_SLOT] = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
    DataTypePtr t(new MapDataType(Descriptor("convert.pfmatrix.content"), m));

    Descriptor od(WMATRIX_OUT_PORT_ID, PFMatrixConvertWorker::tr("Weight matrix"), 
        PFMatrixConvertWorker::tr("Produced statistical model of specified TFBS data."));
    p << new PortDescriptor(id, t, true /*input*/);
    
    QMap<Descriptor, DataTypePtr> outM;
    outM[PWMatrixWorkerFactory::WMATRIX_SLOT] = PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE();
    p << new PortDescriptor(od, DataTypePtr(new MapDataType("fmatrix.convert.out", outM)), false /*input*/, true /*multi*/);
    
    {
        Descriptor ad(ALG_ATTR, PWMatrixBuildWorker::tr("Weight algorithm"), PWMatrixBuildWorker::tr("Different weight algorithms uses different functions to build weight matrices. It allows us to get better precision on different data sets. Log-odds, NLG and Match algorithms are sensitive to input matrices with zero values, so some of them may not work on those matrices."));
        a << new Attribute(ad, BaseTypes::STRING_TYPE(), true, BuiltInPWMConversionAlgorithms::BVH_ALGO);
    }

    {
        Descriptor td(TYPE_ATTR, PWMatrixBuildWorker::tr("Matrix type"), PWMatrixBuildWorker::tr("Dinucleic matrices are more detailed, while mononucleic one are more useful for small input data sets."));
        a << new Attribute(td, BaseTypes::BOOL_TYPE(), true, false /* false = mononucleic, true = dinucleic */);
    }

    Descriptor desc(ACTOR_ID, tr("Convert Frequency Matrix"),
        tr("Converts frequency matrix to weight matrix. Weight matrices are used for probabilistic recognition of transcription factor binding sites."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;    

    {
        QVariantMap modeMap;
        QStringList algo = AppContext::getPWMConversionAlgorithmRegistry()->getAlgorithmIds();
        foreach (QString curr, algo) {
            modeMap[curr] = QVariant(curr);
        }
        delegates[ALG_ATTR] = new ComboBoxDelegate(modeMap);
    }

    {
        QVariantMap modeMap;
        modeMap[tr("Mononucleic")] = QVariant(false);
        modeMap[tr("Dinucleic")] = QVariant(true);
        delegates[TYPE_ATTR] = new ComboBoxDelegate(modeMap);
    }

    proto->setPrompter(new PFMatrixConvertPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":weight_matrix/images/weight_matrix.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
}

QString PFMatrixConvertPrompter::composeRichDoc() {
    QString prod = getProducersOrUnset(FMATRIX_IN_PORT_ID, PFMatrixWorkerFactory::FMATRIX_SLOT.getId());
    return  tr("For each frequency matrix from <u>%1</u>, build weight matrix.").arg(prod);
}

void PFMatrixConvertWorker::init() {
    input = ports.value(FMATRIX_IN_PORT_ID);
    output = ports.value(WMATRIX_OUT_PORT_ID);
}

Task* PFMatrixConvertWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        mtype = PFMatrixWorkerFactory::FREQUENCY_MATRIX_MODEL_TYPE();
        QVariantMap data = inputMessage.getData().toMap();
        PWMatrix model = data.value(PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE_ID).value<PWMatrix>();
        QString url = data.value(BaseSlots::URL_SLOT().getId()).toString();
        cfg.algo = actor->getParameter(ALG_ATTR)->getAttributeValue<QString>(context);
        cfg.type = actor->getParameter(TYPE_ATTR)->getAttributeValue<bool>(context) ? PM_DINUCLEOTIDE : PM_MONONUCLEOTIDE;
        const PFMatrix& ma = data.value(PFMatrixWorkerFactory::FMATRIX_SLOT.getId()).value<PFMatrix>();
        Task* t = new PWMatrixBuildTask(cfg, ma);
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void PFMatrixConvertWorker::sl_taskFinished() {
    PWMatrixBuildTask* t = qobject_cast<PWMatrixBuildTask*>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()){
        return;
    }
    PWMatrix model = t->getResult();
    QVariant v = qVariantFromValue<PWMatrix>(model);
    output->put(Message(mtype, v));
}

} //namespace LocalWorkflow
} //namespace U2
