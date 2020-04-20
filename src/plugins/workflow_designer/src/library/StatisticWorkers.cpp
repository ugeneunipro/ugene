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

#include <U2Core/AnnotationData.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/ConfigurationEditor.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/WorkflowEnv.h>

#include "StatisticWorkers.h"

namespace U2 {
namespace LocalWorkflow {

const QString DNAStatWorkerFactory::ACTOR_ID("dna-stats");

const QString GCCONTENT("gc-content");
const QString GC1CONTENT("gc1-content");
const QString GC2CONTENT("gc2-content");
const QString GC3CONTENT("gc3-content");

void DNAStatWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    QList<Attribute*> attribs;

    //accept sequence and annotated regions as input
    QMap<Descriptor, DataTypePtr> inputMap;
    QMap<Descriptor, DataTypePtr> outputMap;
    inputMap[ BaseSlots::DNA_SEQUENCE_SLOT() ] = BaseTypes::DNA_SEQUENCE_TYPE();
    outputMap[ BaseSlots::ANNOTATION_TABLE_SLOT() ] = BaseTypes::ANNOTATION_TABLE_TYPE();

    { //Create input port descriptors
        Descriptor inDesc( BasePorts::IN_SEQ_PORT_ID(), DNAStatWorker::tr("Input sequence"),
            DNAStatWorker::tr("Sequence for which GC-content and GC3-content will be evaluated.") );
        Descriptor outDesc( BasePorts::OUT_ANNOTATIONS_PORT_ID(), DNAStatWorker::tr("Result annotation"),
            DNAStatWorker::tr("Resulted annotations, with GC-content and GC3-content.") );

        portDescs << new PortDescriptor( inDesc, DataTypePtr(new MapDataType("filter.anns", inputMap)), /*input*/ true );
        portDescs << new PortDescriptor( outDesc, DataTypePtr(new MapDataType("filter.anns", outputMap)), /*input*/false, /*multi*/true );
    }

    attribs << new Attribute(Descriptor(GCCONTENT, DNAStatWorker::tr("GC-content"), DNAStatWorker::tr("Evaluate GC-content.")),
        BaseTypes::BOOL_TYPE(),false, true);
    attribs << new Attribute(Descriptor(GC1CONTENT, DNAStatWorker::tr("GC1-content"), DNAStatWorker::tr("Evaluate GC1-content.")),
        BaseTypes::BOOL_TYPE(),false, true);
    attribs << new Attribute(Descriptor(GC2CONTENT, DNAStatWorker::tr("GC2-content"), DNAStatWorker::tr("Evaluate GC2-content.")),
        BaseTypes::BOOL_TYPE(),false, true);
    attribs << new Attribute(Descriptor(GC3CONTENT, DNAStatWorker::tr("GC3-content"), DNAStatWorker::tr("Evaluate GC3-content.")),
        BaseTypes::BOOL_TYPE(),false, true);

    Descriptor desc( ACTOR_ID,
        DNAStatWorker::tr("DNA Statistics"),
        DNAStatWorker::tr("Evaluates statistics for DNA sequences.") );
    ActorPrototype * proto = new IntegralBusActorPrototype( desc, portDescs, attribs );
    proto->setPrompter( new DNAStatWorkerPrompter() );
    proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
    WorkflowEnv::getProtoRegistry()->registerProto( BaseActorCategories::CATEGORY_STATISTIC(), proto );

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById( LocalDomainFactory::ID );
    localDomain->registerEntry( new DNAStatWorkerFactory() );
}

QString DNAStatWorkerPrompter::composeRichDoc() {
    return tr("Evaluates GC-Content and GC3-Content.");
}


void DNAStatWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* DNAStatWorker::tick() {
    while (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler seqId = qm.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (NULL == seqObj.data()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence dna = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if(!dna.alphabet->isNucleic()) {
            return new FailTask(tr("Sequence must be nucleotide"));
        }

        QList<SharedAnnotationData> res;
        SharedAnnotationData gcAnn(new AnnotationData);
        gcAnn->name = "statistics";
        gcAnn->location->regions << U2Region( 0, dna.seq.size());

        if (actor->getParameter(GCCONTENT)->getAttributeValue<bool>(context)) {
            float gcContent = calcGCContent(dna.seq);
            gcAnn->qualifiers.push_back(U2Qualifier("gc-content", QString::number(gcContent*100) + "%"));
        }

        if (actor->getParameter(GC1CONTENT)->getAttributeValue<bool>(context)) {
            float gc1Content = calcGC1Content(dna.seq);
            gcAnn->qualifiers.push_back(U2Qualifier("gc1-content", QString::number(gc1Content*100) + "%"));
        }

        if (actor->getParameter(GC2CONTENT)->getAttributeValue<bool>(context)) {
            float gc2Content = calcGC2Content(dna.seq);
            gcAnn->qualifiers.push_back(U2Qualifier("gc2-content", QString::number(gc2Content*100) + "%"));
        }

        if (actor->getParameter(GC3CONTENT)->getAttributeValue<bool>(context)) {
            float gc3Content = calcGC3Content(dna.seq);
            gcAnn->qualifiers.push_back(U2Qualifier("gc3-content", QString::number(gc3Content*100) + "%"));
        }

        if (gcAnn->qualifiers.isEmpty()) {
            return new FailTask(tr("No statistics was selected"));
        }

        res << gcAnn;

        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(res);
        const QVariant v = qVariantFromValue<SharedDbiDataHandler>(tableId);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), v));
    }
    if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

float DNAStatWorker::calcGCContent(const QByteArray &seq) {
    int gc = 0;
    for(int i = 0; i < seq.size(); i++) {
        if(QRegExp("[cCgG]").exactMatch(QString(seq.at(i)))){
            gc++;
        }
    }
    float gcContent = (float)gc/seq.size();
    return gcContent;
}

float DNAStatWorker::calcGC1Content(const QByteArray &seq) {
    int gc1 = 0;
    for(int i = 0; i < seq.size(); i+=3) {
        if(QRegExp("[cCgG]").exactMatch(QString(seq.at(i)))){
            gc1++;
        }
    }
    float gc1Content = (float)gc1/seq.size()*3;
    return gc1Content;
}

float DNAStatWorker::calcGC2Content(const QByteArray &seq) {
    int gc2 = 0;
    for(int i = 1; i < seq.size(); i+=3) {
        if(QRegExp("[cCgG]").exactMatch(QString(seq.at(i)))){
            gc2++;
        }
    }
    float gc2Content = (float)gc2/seq.size()*3;
    return gc2Content;
}

float DNAStatWorker::calcGC3Content(const QByteArray &seq) {
    int gc3 = 0;
    for(int i = 2; i < seq.size(); i+=3) {
        if(QRegExp("[cCgG]").exactMatch(QString(seq.at(i)))){
            gc3++;
        }
    }
    float gc3Content = (float)gc3/seq.size()*3;
    return gc3Content;
}

} //LocalWorkflow
} //U2
