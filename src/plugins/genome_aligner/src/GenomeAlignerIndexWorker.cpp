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

#include "GenomeAlignerIndexWorker.h"

#include <U2Core/Log.h>
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
#include <U2Gui/DialogUtils.h>

#include "GenomeAlignerPlugin.h"

namespace U2 {
namespace LocalWorkflow {


static const QString INDEX_PORT_ID("in-gen-al-index");
static const QString INDEX_OUT_PORT_ID("out-gen-al-index");
static const QString INDEX_SLOT("index-slot");

static const QString REFSEQ_URL_ATTR("url-reference");
static const QString INDEX_URL_ATTR("url-index");
static const QString REF_SIZE_ATTR("ref-size");


const QString GenomeAlignerBuildWorkerFactory::ACTOR_ID("gen-al-build-index");
const QString GenomeAlignerIndexReaderWorkerFactory::ACTOR_ID("gen-al-read-index");

/************************************************************************/
/* Genome aligner index build                                           */
/************************************************************************/
void GenomeAlignerBuildWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor oud(INDEX_OUT_PORT_ID, QString("Genome aligner index"), QString("Result genome aligner index of reference sequence."));

    QMap<Descriptor, DataTypePtr> outM;
    outM[INDEX_SLOT] = GenomeAlignerPlugin::GENOME_ALIGNER_INDEX_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("gen.al.build.index.out", outM)), false /*input*/, true /*multi*/);

    Descriptor refseq(REFSEQ_URL_ATTR, GenomeAlignerBuildWorker::tr("Reference"),
        GenomeAlignerBuildWorker::tr("Reference sequence url. The short reads will be aligned to this reference genome."));
    Descriptor desc(ACTOR_ID, GenomeAlignerBuildWorker::tr("Genome aligner index builder"),
        GenomeAlignerBuildWorker::tr("GenomeAlignerBuild builds an index from a set of DNA sequences. GenomeAlignerBuild outputs a set of 3 files with suffixes .idx, .ref, .sarr. These files together constitute the index: they are all that is needed to align reads to that reference."));
    Descriptor index(INDEX_URL_ATTR, GenomeAlignerBuildWorker::tr("Index"),
        GenomeAlignerBuildWorker::tr("Output index url."));
    Descriptor refSize(REF_SIZE_ATTR, GenomeAlignerBuildWorker::tr("Reference fragmentation"),
        GenomeAlignerBuildWorker::tr("Reference fragmentation size"));

    a << new Attribute(refseq, BaseTypes::STRING_TYPE(), true /*required*/, QString());
    a << new Attribute(index, BaseTypes::STRING_TYPE(), true /*required*/, QString());
    a << new Attribute(refSize, BaseTypes::NUM_TYPE(), true /*required*/, 10);

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;

    delegates[REFSEQ_URL_ATTR] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), true);
    delegates[INDEX_URL_ATTR] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), false);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new GenomeAlignerBuildPrompter());
    proto->setIconPath(":core/images/align.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ASSEMBLY(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new GenomeAlignerBuildWorkerFactory());
}

void GenomeAlignerBuildWorker::init() {
    output = ports.value(INDEX_OUT_PORT_ID);
    refSeqUrl = actor->getParameter(REFSEQ_URL_ATTR)->getAttributeValue<QString>(context);
    indexUrl = actor->getParameter(INDEX_URL_ATTR)->getAttributeValue<QString>(context);

    settings.prebuiltIndex = false;
}

bool GenomeAlignerBuildWorker::isReady() const {
    return !isDone();
}

Task* GenomeAlignerBuildWorker::tick() {
    if( refSeqUrl.isEmpty()) {
        algoLog.trace(GenomeAlignerBuildWorker::tr("Reference sequence URL is empty"));
        return NULL;
    }
    if( indexUrl.isEmpty()) {
        algoLog.trace(GenomeAlignerBuildWorker::tr("Result index URL is empty"));
        return NULL;
    }


    settings.refSeqUrl = refSeqUrl;
    settings.indexFileName = indexUrl.getURLString();
    Task* t = new GenomeAlignerTask(settings, true);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;
}

void GenomeAlignerBuildWorker::sl_taskFinished() {
    GenomeAlignerTask* t = qobject_cast<GenomeAlignerTask*>(sender());
    if (t->getState() != Task::State_Finished) {
        return;
    }

    done = true;

    QVariant v = qVariantFromValue<QString>(t->getIndexPath());
    output->put(Message(GenomeAlignerPlugin::GENOME_ALIGNER_INDEX_TYPE(), v));
    output->setEnded();
    algoLog.trace(tr("Genome aligner index building finished. Result name is %1").arg(t->getIndexPath()));
}

bool GenomeAlignerBuildWorker::isDone() const {
    return done;
}

void GenomeAlignerBuildWorker::cleanup() {

}

QString GenomeAlignerBuildPrompter::composeRichDoc() {
    QString refSeqUrl = getParameter(REFSEQ_URL_ATTR).toString();
    QString refSeq = (refSeqUrl.isEmpty() ? "" : QString("<u>%1</u>").arg(GUrl(refSeqUrl).fileName()) );

    QString doc = tr("Build genome aligner index from %1 and send it url to output.").arg(refSeq);

    return doc;
}

/************************************************************************/
/* Genome aligner index read                                            */
/************************************************************************/
void GenomeAlignerIndexReaderWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor oud(INDEX_OUT_PORT_ID, GenomeAlignerIndexReaderWorker::tr("Genome aligner index"), GenomeAlignerIndexReaderWorker::tr("Result of genome aligner index builder."));

    QMap<Descriptor, DataTypePtr> outM;
    outM[INDEX_SLOT] = GenomeAlignerPlugin::GENOME_ALIGNER_INDEX_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("gen.al.index.reader.out", outM)), false /*input*/, true /*multi*/);

    Descriptor desc(ACTOR_ID, GenomeAlignerIndexReaderWorker::tr("Genome aligner index reader"),
       GenomeAlignerIndexReaderWorker::tr("Read a set of several files with extensions .idx, .ref, .X.sarr. These files together constitute the index: they are all that is needed to align reads to that reference."));
    Descriptor index(INDEX_URL_ATTR, GenomeAlignerIndexReaderWorker::tr("Index"),
        GenomeAlignerIndexReaderWorker::tr("Select an index file with the .idx extension"));

    a << new Attribute(index, BaseTypes::STRING_TYPE(), true /*required*/, QString());

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;

    delegates[INDEX_URL_ATTR] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), false, false, false);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new GenomeAlignerIndexReaderPrompter());
    proto->setIconPath(":core/images/align.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ASSEMBLY(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new GenomeAlignerIndexReaderWorkerFactory());
}

void GenomeAlignerIndexReaderWorker::init() {
    output = ports.value(INDEX_OUT_PORT_ID);
    indexUrl = actor->getParameter(INDEX_URL_ATTR)->getAttributeValue<QString>(context);
}

bool GenomeAlignerIndexReaderWorker::isReady() const {
    return !isDone();
}

Task *GenomeAlignerIndexReaderWorker::tick() {

    if(indexUrl.isEmpty()) {
        algoLog.trace(GenomeAlignerIndexReaderWorker::tr("Index URL is empty"));
        return NULL;
    }
    Task* t = new Task("Genome aligner index reader", TaskFlags_NR_FOSCOE);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;
}

void GenomeAlignerIndexReaderWorker::sl_taskFinished() {
    QVariant v = qVariantFromValue<QString>(indexUrl.getURLString());
    output->put(Message(GenomeAlignerPlugin::GENOME_ALIGNER_INDEX_TYPE(), v));
    output->setEnded();
    done = true;
    algoLog.trace(tr("Reading genome aligner index finished. Result name is %1").arg(indexUrl.getURLString()));
}

bool GenomeAlignerIndexReaderWorker::isDone() const {
    return done;
}

void GenomeAlignerIndexReaderWorker::cleanup() {

}

QString GenomeAlignerIndexReaderPrompter::composeRichDoc() {
    QString indexUrl = getParameter(INDEX_URL_ATTR).toString();
    QString index = (indexUrl.isEmpty() ? "" : QString("<u>%1</u>").arg(GUrl(indexUrl).fileName()));

    QString doc = tr("Read genome aligner index from %1 and send it url to output.").arg(index);

    return doc;
}
} //namespace LocalWorkflow
} //namespace U2
