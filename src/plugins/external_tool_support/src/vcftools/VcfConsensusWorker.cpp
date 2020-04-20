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

#include "VcfConsensusWorker.h"
#include "VcfConsensusSupport.h"
#include "VcfConsensusSupportTask.h"
#include "samtools/TabixSupport.h"

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>


namespace U2 {
namespace LocalWorkflow {

static const QString IN_PORT_ID("in-data");
static const QString IN_FASTA_URL_SLOT_ID("fasta");
static const QString IN_VCF_URL_SLOT_ID("vcf");
static const QString OUT_PORT_ID("out-consensus");
static const QString OUT_FASTA_URL_ID("consensus-url");

// VcfConsensusWorker

VcfConsensusWorker::VcfConsensusWorker(Actor *a)
    : BaseWorker(a),
      inputFA(NULL),
      inputVcfBgzip(NULL),
      outputFA(NULL)
{
}

void VcfConsensusWorker::init() {
    inputFA = ports.value(IN_PORT_ID);
    inputVcfBgzip = ports.value(IN_VCF_URL_SLOT_ID);
    outputFA = ports.value(OUT_PORT_ID);
}

Task* VcfConsensusWorker::tick() {
    if (inputFA->hasMessage()) {
        const Message inputMessage = getMessageAndSetupScriptValues(inputFA);
        if (inputMessage.isEmpty()) {
            outputFA->transit();
            return NULL;
        }
        const QVariantMap data = inputMessage.getData().toMap();
        if ( !data.contains(IN_FASTA_URL_SLOT_ID) ) {
            return new FailTask(tr("Input fasta slot is empty"));
        }
        if ( !data.contains(IN_VCF_URL_SLOT_ID) ) {
            return new FailTask(tr("Input vcf slot is empty"));
        }

        GUrl fastaURL(data.value(IN_FASTA_URL_SLOT_ID).toString());
        GUrl vcfURL(data.value(IN_VCF_URL_SLOT_ID).toString());
        GUrl outputURL(getValue<QString>(OUT_FASTA_URL_ID));

        VcfConsensusSupportTask *t = NULL;
        t = new VcfConsensusSupportTask(fastaURL, vcfURL, outputURL);
        t->addListeners(createLogListeners(2));
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));

        return t;
    }
    else {
        setDone();
        outputFA->setEnded();
    }
    return NULL;
}

void VcfConsensusWorker::cleanup() {
}

void VcfConsensusWorker::sl_taskFinished() {
    VcfConsensusSupportTask *t = dynamic_cast<VcfConsensusSupportTask*>(sender());
    CHECK(t != NULL, );
    CHECK(t->isFinished() && !t->hasError(), );

    if (t->isCanceled()) {
        return;
    }
    QString outUrl = t->getResultUrl().getURLString();

    QVariantMap data;
    data[OUT_PORT_ID] = outUrl;
    outputFA->put(Message(ports[OUT_PORT_ID]->getBusType(), data));

    monitor()->addOutputFile(outUrl, getActorId());
}

// VcfConsensusWorkerFactory

const QString VcfConsensusWorkerFactory::ACTOR_ID("vcf-consensus");

void VcfConsensusWorkerFactory::init() {
    QList<PortDescriptor*> ports;
    {
        Descriptor inDesc(IN_PORT_ID, VcfConsensusWorker::tr("Input FASTA and VCF"), VcfConsensusWorker::tr("Input FASTA and VCF"));
        Descriptor inFastaDesc(IN_FASTA_URL_SLOT_ID, VcfConsensusWorker::tr("FASTA url"), VcfConsensusWorker::tr("FASTA url"));
        Descriptor inVcfDesc(IN_VCF_URL_SLOT_ID, VcfConsensusWorker::tr("VCF url"), VcfConsensusWorker::tr("VCF url"));
        QMap<Descriptor, DataTypePtr> inM;
        inM[inFastaDesc] = BaseTypes::STRING_TYPE();
        inM[inVcfDesc] = BaseTypes::STRING_TYPE();
        ports << new PortDescriptor(inDesc, DataTypePtr(new MapDataType("in.fasta_vcf", inM)), true/*input*/);


        Descriptor outDesc(OUT_PORT_ID, VcfConsensusWorker::tr("Fasta consensus url"), VcfConsensusWorker::tr("Fasta consensus url"));
        QMap<Descriptor, DataTypePtr> outM;
        outM[OUT_PORT_ID] = BaseTypes::STRING_TYPE();
        ports << new PortDescriptor(outDesc, DataTypePtr(new MapDataType("out.fasta", outM)), false/*input*/, true/*multi*/);
    }

    QList<Attribute*> attrs;
    {
        Descriptor outAttDesc(OUT_FASTA_URL_ID, VcfConsensusWorker::tr("Output FASTA consensus"),
                              VcfConsensusWorker::tr("The path to the output file with the result consensus."));
        attrs << new Attribute(outAttDesc, BaseTypes::STRING_TYPE(), true);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[OUT_FASTA_URL_ID] = new URLDelegate("", "", false/*multi*/, false/*isPath*/, true/*save*/);
    }

    Descriptor desc(ACTOR_ID,
                    VcfConsensusWorker::tr("Create VCF Consensus"),
                    VcfConsensusWorker::tr("Apply VCF variants to a fasta file to create consensus sequence."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attrs);
    proto->setPrompter(new VcfConsensusPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(VcfConsensusSupport::ET_VCF_CONSENSUS_ID);
    proto->addExternalTool(TabixSupport::ET_TABIX_ID);


    SAFE_POINT(WorkflowEnv::getProtoRegistry() != NULL, "Workflow proto registry is NULL", );
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_VARIATION_ANALYSIS(), proto);

    SAFE_POINT(WorkflowEnv::getDomainRegistry() != NULL, "Workflow domain registry is NULL", );
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new VcfConsensusWorkerFactory());
}

QString VcfConsensusPrompter::composeRichDoc() {
    IntegralBusPort *in = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_ID));
    SAFE_POINT( in != NULL, "NULL input port", "");
    QString fasta = getProducersOrUnset(IN_PORT_ID, IN_FASTA_URL_SLOT_ID);
    QString vcf = getProducersOrUnset(IN_PORT_ID, IN_VCF_URL_SLOT_ID);
    QString out = getHyperlink(OUT_FASTA_URL_ID, getURL(OUT_FASTA_URL_ID));

    return tr("Apply VCF variants from <u>%1</u> to fasta file <u>%2</u> and save consensus sequence to <u>%3</u>.")
              .arg(vcf)
              .arg(fasta)
              .arg(out);
}

} // namesapce LocalWorkflow
} // namespace U2
