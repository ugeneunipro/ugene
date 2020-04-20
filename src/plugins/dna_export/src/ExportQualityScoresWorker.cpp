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
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Gui/DialogUtils.h>

#include "ExportQualityScoresWorker.h"


namespace U2 {
namespace LocalWorkflow {

/*************************************
 * ExportPhredQualityWorkerFactory
 *************************************/
const QString ExportPhredQualityWorkerFactory::ACTOR_ID("export-phred-qualities");
//const QString QUALITY_TYPE_ATTR("quality-format");

void ExportPhredQualityWorkerFactory::init() {

    QList<PortDescriptor*> portDescriptors;

    {
        Descriptor ind(BasePorts::IN_SEQ_PORT_ID(), ExportPhredQualityWorker::tr("DNA sequences"),
            ExportPhredQualityWorker::tr("The PHRED scores from these sequences will be exported"));
        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        portDescriptors << new PortDescriptor(ind, DataTypePtr(new MapDataType("Export.qual.in", inM)), true /*input*/);
    }


    QList<Attribute*> attributes;
    {
        Descriptor qualUrl(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), ExportPhredQualityWorker::tr("PHRED output"),
             ExportPhredQualityWorker::tr("Path to file with PHRED quality scores."));
        attributes << new Attribute(qualUrl, BaseTypes::STRING_TYPE(), true /*required*/, QString());
    }

    QMap<QString, PropertyDelegate*> delegates;
    {

        delegates[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), false, false);

    }


    Descriptor actorDesc(ACTOR_ID, ExportPhredQualityWorker::tr("Export PHRED Qualities"),
        ExportPhredQualityWorker::tr("Export corresponding PHRED quality scores from input sequences."));

    ActorPrototype* proto = new IntegralBusActorPrototype(actorDesc, portDescriptors, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ExportPhredQualityPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ExportPhredQualityWorkerFactory());

}

Worker* ExportPhredQualityWorkerFactory::createWorker(Actor *a) {
    return new ExportPhredQualityWorker(a);
}


/*************************************
 * ExportPhredQualityPrompter
 *************************************/
QString ExportPhredQualityPrompter::composeRichDoc() {

    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_SEQ_PORT_ID());
    QString producerName = producer ? tr(" from <u>%1</u>").arg(producer->getLabel()) : "";
    QString qualUrl = getParameter(BaseAttributes::URL_OUT_ATTRIBUTE().getId()).toString();
    QString qualSeq = qualUrl.isEmpty () ? "<font color='red'>"+tr("unset")+"</font>"
                                          : getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), GUrl(qualUrl).fileName());

    QString doc = tr("Export PHRED quality scores%1 to %2.")
        .arg(producerName).arg(qualSeq);

    return doc;
}



/*************************************
 * ExportPhredQualityWorker
 *************************************/
ExportPhredQualityWorker::ExportPhredQualityWorker(Actor* a) : BaseWorker(a), input(NULL), currentTask(NULL) {

}

void ExportPhredQualityWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    fileName = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
}

Task* ExportPhredQualityWorker::tick() {

    if (currentTask != NULL && currentTask->getState() != Task::State_Finished) {
        return NULL;
    }

    if ( input->hasMessage() ) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        U2SequenceObject* seqObj = StorageUtils::getSequenceObject(context->getDataStorage(), seqId);
        if ( NULL == seqObj ) {
            return NULL;
        }

        // Optimize clean up
        seqObjList.append(seqObj);

        ExportQualityScoresConfig cfg;
        cfg.dstFilePath = fileName;
        if (currentTask == NULL) {
            cfg.appendData = false;
        }

        currentTask = new ExportPhredQualityScoresTask(seqObj, cfg);
        return currentTask;
    }

    setDone();
    return NULL;
}


void ExportPhredQualityWorker::cleanup() {
    qDeleteAll(seqObjList);
}


} // U2
} // LocalWorkflow

