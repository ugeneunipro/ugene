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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/Log.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

#include "ImportQualityScoresWorker.h"

namespace U2 {
namespace LocalWorkflow {

/*************************************
 * ImportPhredQualityWorkerFactory
 *************************************/
const QString ImportPhredQualityWorkerFactory::ACTOR_ID("import-phred-qualities");
const QString QUALITY_TYPE_ATTR("quality-type");
const QString QUALITY_FORMAT_ATTR("quality-format");

void ImportPhredQualityWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_SEQ_PORT_ID(), ImportPhredQualityWorker::tr("DNA sequences"),
        ImportPhredQualityWorker::tr("The PHRED scores will be imported to these sequences"));
    Descriptor oud(BasePorts::OUT_SEQ_PORT_ID(), ImportPhredQualityWorker::tr("DNA sequences with imported qualities"),
        ImportPhredQualityWorker::tr("These sequences have quality scores."));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("import.qual.in", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("import.qual.out", outM)), false /*input*/, true /*multi*/);

    Descriptor qualUrl(BaseAttributes::URL_IN_ATTRIBUTE().getId(), ImportPhredQualityWorker::tr("PHRED input"),
         ImportPhredQualityWorker::tr("Path to file with PHRED quality scores."));
    Descriptor qualType(QUALITY_TYPE_ATTR, ImportPhredQualityWorker::tr("Quality type"),
        ImportPhredQualityWorker::tr("Choose method to encode quality scores."));
    Descriptor qualFormat(QUALITY_FORMAT_ATTR, ImportPhredQualityWorker::tr("File format"),
        ImportPhredQualityWorker::tr("Quality values can be in specialized FASTA-like PHRED qual format or encoded similar as in FASTQ files."));

    a << new Attribute(qualUrl, BaseTypes::STRING_TYPE(), true /*required*/, QString());
    a << new Attribute(qualType, BaseTypes::STRING_TYPE(), false/*required*/, DNAQuality::getDNAQualityNameByType(DNAQualityType_Sanger) );
    a << new Attribute(qualFormat, BaseTypes::STRING_TYPE(), false, DNAQuality::QUAL_FORMAT);

    Descriptor desc(ACTOR_ID, ImportPhredQualityWorker::tr("Import PHRED Qualities"),
        ImportPhredQualityWorker::tr("Add corresponding PHRED quality scores to the sequences.\nYou can use this worker to convert .fasta and .qual pair to fastq format."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;

    delegates[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), true, false, false);


    {
        QVariantMap m;
        QStringList qualFormats = DNAQuality::getDNAQualityTypeNames();
        foreach( const QString& name, qualFormats  ) {
            m[name] = name;
        }
        delegates[QUALITY_TYPE_ATTR] = new ComboBoxDelegate(m);
    }

    {
        QVariantMap m;
        m[DNAQuality::QUAL_FORMAT] = DNAQuality::QUAL_FORMAT;
        m[DNAQuality::ENCODED] = DNAQuality::ENCODED;
        delegates[QUALITY_FORMAT_ATTR] = new ComboBoxDelegate(m);
    }

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ImportPhredQualityPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ImportPhredQualityWorkerFactory());
}

/*************************************
 * ImportPhredQualityPrompter
 *************************************/
QString ImportPhredQualityPrompter::composeRichDoc() {

    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_SEQ_PORT_ID());
    QString producerName = producer ? tr(" from <u>%1</u>").arg(producer->getLabel()) : "";
    QString qualUrl = getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId()).toString();
    QString qualSeq = (qualUrl.isEmpty() ? "<font color='red'>"+tr("unset")+"</font>" : QString("<u>%1</u>").arg(GUrl(qualUrl).fileName()) );
    qualSeq = getHyperlink(BaseAttributes::URL_IN_ATTRIBUTE().getId(), qualSeq);

    QString doc = tr("Import PHRED quality scores in file %1  to the sequences %2 and send the sequences to the output.")
        .arg(qualSeq).arg(producerName);

    return doc;
}

/*************************************
 * ImportPhredQualityWorker
 *************************************/
ImportPhredQualityWorker::ImportPhredQualityWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL), readTask(NULL) {
}

void ImportPhredQualityWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_SEQ_PORT_ID());
    fileName = actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>(context);
    type = DNAQuality::getDNAQualityTypeByName( actor->getParameter(QUALITY_TYPE_ATTR)->getAttributeValue<QString>(context) );
    format = actor->getParameter(QUALITY_FORMAT_ATTR)->getAttributeValue<QString>(context);
    if (format.isEmpty()) {
        format = DNAQuality::QUAL_FORMAT;
    }
}

Task* ImportPhredQualityWorker::tick() {

    if (input->hasMessage()) {

        if (readTask == NULL) {
            readTask = new ReadQualityScoresTask(fileName, type, format);
            return readTask;
        } else if (readTask->getState() != Task::State_Finished) {
            return NULL;
        }

        Message inputMessage = getMessageAndSetupScriptValues(input);
        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return NULL;
        }
        const QMap<QString,DNAQuality>& qualities = readTask->getResult();

        // It's OK to copy whole sequence because we do not to expect reads to be bigger than 1000 bp
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        const QString& seqName = seq.getName();
        if (qualities.contains(seqName)) {
            const DNAQuality& qual = qualities.value(seqName);
            if (seq.length() == qual.qualCodes.length()) {
                seq.quality = qual;
            }
        }
        SharedDbiDataHandler handler = context->getDataStorage()->putSequence(seq);
        output->put(Message(BaseTypes::DNA_SEQUENCE_TYPE(), qVariantFromValue<SharedDbiDataHandler>(handler)));
    } else if (input->isEnded()) {
        output->setEnded();
        setDone();
        algoLog.trace(tr("Import of qualities is finished.") );
    }

    return NULL;
}

void ImportPhredQualityWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
