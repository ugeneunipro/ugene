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

#include <QApplication>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

#include "WeightMatrixIOWorkers.h"
#include "WeightMatrixSearchTask.h"
#include "WeightMatrixWorkers.h"

namespace U2 {
namespace LocalWorkflow {

static const QString MODEL_PORT("in-wmatrix");
static const QString NAME_ATTR("result-name");
static const QString SCORE_ATTR("min-score");

const QString PWMatrixSearchWorker::ACTOR_ID("wmatrix-search");

void PWMatrixSearchWorker::registerProto() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    {
        Descriptor md(MODEL_PORT, PWMatrixSearchWorker::tr("Weight matrix"), PWMatrixSearchWorker::tr("Profile data to search with."));
        Descriptor sd(BasePorts::IN_SEQ_PORT_ID(), PWMatrixSearchWorker::tr("Sequence"), PWMatrixSearchWorker::tr("Input nucleotide sequence to search in."));
        Descriptor od(BasePorts::OUT_ANNOTATIONS_PORT_ID(), PWMatrixSearchWorker::tr("Weight matrix annotations"),
            PWMatrixSearchWorker::tr("Annotations marking found TFBS sites."));

        QMap<Descriptor, DataTypePtr> modelM;
        modelM[PWMatrixWorkerFactory::WMATRIX_SLOT] = PWMatrixWorkerFactory::WEIGHT_MATRIX_MODEL_TYPE();
        p << new PortDescriptor(md, DataTypePtr(new MapDataType("wmatrix.search.model", modelM)), true /*input*/, false, IntegralBusPort::BLIND_INPUT);
        QMap<Descriptor, DataTypePtr> seqM;
        seqM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(sd, DataTypePtr(new MapDataType("wmatrix.search.sequence", seqM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("wmatrix.search.out", outM)), false /*input*/, true /*multi*/);
    }
    {
        Descriptor nd(NAME_ATTR, PWMatrixSearchWorker::tr("Result annotation"),
            PWMatrixSearchWorker::tr("Annotation name for marking found regions."));
        Descriptor scd(SCORE_ATTR, PWMatrixSearchWorker::tr("Min score"),
            QApplication::translate("PWMSearchDialog", "Minimum score to detect transcription factor binding site", 0));

        a << new Attribute(nd, BaseTypes::STRING_TYPE(), true, "misc_feature");
        a << new Attribute(BaseAttributes::STRAND_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, BaseAttributes::STRAND_BOTH());
        a << new Attribute(scd, BaseTypes::NUM_TYPE(), false, 85);
    }

    Descriptor desc(ACTOR_ID, tr("Search for TFBS with Weight Matrix"),
        tr("Searches each input sequence for transcription factor binding sites significantly similar to specified weight matrices."
        " In case several profiles were supplied, searches with all profiles one by one and outputs merged set of annotations for each sequence.")
       );
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap m; m["minimum"] = 1; m["maximum"] = 100; m["suffix"] = "%";
        delegates[SCORE_ATTR] = new SpinBoxDelegate(m);
    }
    delegates[BaseAttributes::STRAND_ATTRIBUTE().getId()] = new ComboBoxDelegate(BaseAttributes::STRAND_ATTRIBUTE_VALUES_MAP());
    proto->setPrompter(new PWMatrixSearchPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":weight_matrix/images/weight_matrix.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_TRANSCRIPTION(), proto);
}

static int getStrand(const QString & s) {
    QString str = s.toLower();
    if(BaseAttributes::STRAND_BOTH().startsWith(str)) {
        return 0;
    } else if(BaseAttributes::STRAND_DIRECT().startsWith(str)) {
        return 1;
    } else if(BaseAttributes::STRAND_COMPLEMENTARY().startsWith(str)) {
        return -1;
    } else {
        bool ok = false;
        int num = str.toInt(&ok);
        if(ok) {
            return num;
        } else {
            return 0;
        }
    }
}

QString PWMatrixSearchPrompter::composeRichDoc() {
    Actor* modelProducer = qobject_cast<IntegralBusPort*>(target->getPort(MODEL_PORT))->getProducer(PWMatrixWorkerFactory::WMATRIX_SLOT.getId());
    Actor* seqProducer = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()))->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString seqName = tr("For each sequence from <u>%1</u>,").arg(seqProducer ? seqProducer->getLabel() : unsetStr);
    QString modelName = tr("with all profiles provided by <u>%1</u>,").arg(modelProducer ? modelProducer->getLabel() : unsetStr);

    QString resultName = getRequiredParam(NAME_ATTR);
    resultName = getHyperlink(NAME_ATTR, resultName);

    QString strandName;
    switch (getStrand(getParameter(BaseAttributes::STRAND_ATTRIBUTE().getId()).value<QString>())) {
    case 0: strandName = PWMatrixSearchWorker::tr("both strands"); break;
    case 1: strandName = PWMatrixSearchWorker::tr("direct strand"); break;
    case -1: strandName = PWMatrixSearchWorker::tr("complement strand"); break;
    }
    strandName = getHyperlink(BaseAttributes::STRAND_ATTRIBUTE().getId(), strandName);

    QString doc = tr("%1 search transcription factor binding sites (TFBS) %2."
        "<br>Recognize sites with <u>similarity %3%</u>, process <u>%4</u>."
        "<br>Output the list of found regions annotated as <u>%5</u>.")
        .arg(seqName)
        .arg(modelName)
        .arg(getHyperlink(SCORE_ATTR, getParameter(SCORE_ATTR).toInt()))
        .arg(strandName)
        .arg(resultName);

    return doc;
}

void PWMatrixSearchWorker::init() {
    modelPort = ports.value(MODEL_PORT);
    dataPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
    dataPort->addComplement(output);
    output->addComplement(dataPort);

    strand = getStrand(actor->getParameter(BaseAttributes::STRAND_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
    cfg.minPSUM = actor->getParameter(SCORE_ATTR)->getAttributeValue<int>(context);
    resultName = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
}

bool PWMatrixSearchWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    bool dataEnded = dataPort->isEnded();
    bool modelEnded = modelPort->isEnded();
    int dataHasMes = dataPort->hasMessage();
    int modelHasMes = modelPort->hasMessage();
    return modelHasMes || (modelEnded && (dataHasMes || dataEnded));
}

void PWMatrixSearchWorker::cleanup() {

}

Task* PWMatrixSearchWorker::tick() {
    while (modelPort->hasMessage()) {
        models << modelPort->get().getData().toMap().value(PWMatrixWorkerFactory::WMATRIX_SLOT.getId()).value<PWMatrix>();
    }
    if (!modelPort->isEnded()) {
        return NULL;
    }

    if (dataPort->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(dataPort);
        if (inputMessage.isEmpty() || models.isEmpty()) {
            output->transit();
            return NULL;
        }
        QVariantMap map = inputMessage.getData().toMap();
        SharedDbiDataHandler seqId = map.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if (!seq.isNull() && seq.alphabet->getType() == DNAAlphabet_NUCL) {
            WeightMatrixSearchCfg config(cfg);
            config.complOnly = (strand < 0);
            if (strand <= 0) {
                DNATranslation* compTT = AppContext::getDNATranslationRegistry()->
                    lookupComplementTranslation(seq.alphabet);
                if (compTT  != NULL) {
                    config.complTT = compTT  ;
                }
            }
            QList<Task*> subtasks;
            foreach(PWMatrix model, models) {
                subtasks << new WeightMatrixSingleSearchTask(model, seq.seq, config, 0);
            }
            Task* t = new MultiTask(tr("Find TFBS in %1").arg(seq.getName()), subtasks);
            connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
            return t;
        }
        QString err = tr("Bad sequence supplied to Weight Matrix Search: %1").arg(seq.getName());
        return new FailTask(err);
    } if (dataPort->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void PWMatrixSearchWorker::sl_taskFinished(Task *t) {
    QList<SharedAnnotationData> res;
    SAFE_POINT(NULL != t, "Invalid task is encountered",);
    if (t->isCanceled()) {
        return;
    }
    foreach (const QPointer<Task> &sub, t->getSubtasks()) {
        WeightMatrixSingleSearchTask *sst = qobject_cast<WeightMatrixSingleSearchTask *>(sub.data());
        res += WeightMatrixSearchResult::toTable(sst->takeResults(), U2FeatureTypes::MiscFeature, resultName);
    }
    const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(res);
    const QVariant v = qVariantFromValue<SharedDbiDataHandler>(tableId);
    output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), v));
    algoLog.info(tr("Found %1 TFBS").arg(res.size())); //TODO set task description for report
}

} //namespace LocalWorkflow
} //namespace U2
