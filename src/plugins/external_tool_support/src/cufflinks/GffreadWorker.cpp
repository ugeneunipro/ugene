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
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "CufflinksSupport.h"
#include "GffreadWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString GffreadWorkerFactory::ACTOR_ID("gffread");
static const QString IN_PORT_ID("in-data");
static const QString GENOME_URL_SLOT_ID("genome");
static const QString TRANSCRIPTS_URL_SLOT_ID("transcripts");

static const QString OUT_PORT_ID("extracted-data");
static const QString OUT_URL_SLOT_ID("sequences");

static const QString OUT_URL_ATTR_ID("url-out");

/************************************************************************/
/* Worker */
/************************************************************************/
GffreadWorker::GffreadWorker(Actor *a)
: BaseWorker(a)
{

}

void GffreadWorker::init() {

}

Task * GffreadWorker::tick() {
    if (hasInput()) {
        U2OpStatusImpl os;
        GffreadSettings s = takeSettings(os);
        CHECK_OP(os, new FailTask(os.getError()));

        return runGffread(s);
    } else if (noMoreData()) {
        finalize();
    }
    return NULL;
}

void GffreadWorker::sl_taskFinished() {
    GffreadSupportTask *t = dynamic_cast<GffreadSupportTask*>(sender());
    CHECK(t->isFinished() && !t->hasError(), );

    if(t->isCanceled()){
        return;
    }

    sendResult(t->result());
    monitor()->addOutputFile(t->result(), getActorId());
}

void GffreadWorker::cleanup() {

}

bool GffreadWorker::hasInput() const {
    return ports[IN_PORT_ID]->hasMessage();
}

static void createUrlDir(const QString &url) {
    QFileInfo info(url);
    QDir dir = info.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
}

QString GffreadWorker::getOutUrl() {
    QString url = getValue<QString>(OUT_URL_ATTR_ID);
    if (counters.contains(url)) {
        counters[url]++;
        url = GUrlUtils::insertSuffix(url, "_" + QString::number(counters[url]));
    } else {
        counters[url] = 0;
    }

    createUrlDir(url);
    return url;
}

QVariantMap GffreadWorker::takeData(U2OpStatus &os) {
    Message m = getMessageAndSetupScriptValues(ports[IN_PORT_ID]);
    QVariantMap data = m.getData().toMap();
    if (!data.contains(GENOME_URL_SLOT_ID)) {
        os.setError("No sequence url");
    }
    if (!data.contains(TRANSCRIPTS_URL_SLOT_ID)) {
        os.setError("No transcripts url");
    }
    return data;
}

GffreadSettings GffreadWorker::takeSettings(U2OpStatus &os) {
    GffreadSettings settings;
    QVariantMap data = takeData(os);
    CHECK_OP(os, settings);

    settings.genomeUrl = data[GENOME_URL_SLOT_ID].toString();
    settings.transcriptsUrl = data[TRANSCRIPTS_URL_SLOT_ID].toString();
    settings.outputUrl = getOutUrl();
    return settings;
}

Task * GffreadWorker::runGffread(const GffreadSettings &s) {
    GffreadSupportTask *t = new GffreadSupportTask(s);
    t->addListeners(createLogListeners());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;
}

bool GffreadWorker::noMoreData() const {
    return ports[IN_PORT_ID]->isEnded();
}

void GffreadWorker::finalize() {
    setDone();
    ports[OUT_PORT_ID]->setEnded();
}

void GffreadWorker::sendResult(const QString &outUrl) {
    QVariantMap data;
    data[OUT_URL_SLOT_ID] = outUrl;
    ports[OUT_PORT_ID]->put(Message(ports[OUT_PORT_ID]->getBusType(), data));
}

/************************************************************************/
/* Validator */
/************************************************************************/
class GffreadInputSlotsValidator : public PortValidator {
public:
    virtual bool validate(const IntegralBusPort *port, NotificationsList &notificationList) const {
        bool genome = isBinded(port, GENOME_URL_SLOT_ID);
        bool transc = isBinded(port, TRANSCRIPTS_URL_SLOT_ID);

        QString genomeName = slotName(port, GENOME_URL_SLOT_ID);
        QString trancsName = slotName(port, TRANSCRIPTS_URL_SLOT_ID);
        if (!genome) {
            notificationList.append(WorkflowNotification(QObject::tr("Genome sequence slot is not binded : '%1'").arg(genomeName)));
        }
        if (!transc) {
            notificationList.append(WorkflowNotification(QObject::tr("Transcripts slot is not binded : '%1'").arg(trancsName)));
        }

        return genome && transc;
    }
};

/************************************************************************/
/* Factory */
/************************************************************************/
void GffreadWorkerFactory::init() {
    QList<PortDescriptor*> ports;
    {
        Descriptor inD(IN_PORT_ID, QObject::tr("Input transcripts"), QObject::tr("Input transcripts"));
        Descriptor genomeD(GENOME_URL_SLOT_ID, QObject::tr("Genomic sequence url"), QObject::tr("Genomic sequence url [FASTA]"));
        Descriptor transD(TRANSCRIPTS_URL_SLOT_ID, QObject::tr("Transcripts url"), QObject::tr("Transcripts url [GTF]"));
        QMap<Descriptor, DataTypePtr> inM;
        inM[genomeD] = BaseTypes::STRING_TYPE();
        inM[transD] = BaseTypes::STRING_TYPE();
        ports << new PortDescriptor(inD, DataTypePtr(new MapDataType("in.transcripts", inM)), true /*input*/);

        Descriptor outD(OUT_PORT_ID, QObject::tr("Extracted sequences url"), QObject::tr("Extracted sequences url"));
        QMap<Descriptor, DataTypePtr> outM;
        outM[OUT_URL_SLOT_ID] = BaseTypes::STRING_TYPE();
        ports << new PortDescriptor(outD, DataTypePtr(new MapDataType("out.sequences", outM)), false /*input*/, true);
    }

    QList<Attribute*> attrs;
    {
        Descriptor od(OUT_URL_ATTR_ID, QObject::tr("Output sequences"), QObject::tr("The url to the output file with the extracted sequences."));
        attrs << new Attribute(od, BaseTypes::STRING_TYPE(), true);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[OUT_URL_ATTR_ID] = new URLDelegate("", "", false, false, true);
    }

    Descriptor desc(ACTOR_ID,
        QObject::tr("Extract Transcript Sequences with gffread"),
        QObject::tr("Extract transcript sequences from the genomic sequence(s) with gffread."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attrs);
    proto->setPrompter(new GffreadPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPortValidator(IN_PORT_ID, new GffreadInputSlotsValidator());
    proto->addExternalTool(CufflinksSupport::ET_GFFREAD_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_RNA_SEQ(), proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new GffreadWorkerFactory());
}

Worker * GffreadWorkerFactory::createWorker(Actor *a) {
    return new GffreadWorker(a);
}

/************************************************************************/
/* Prompter */
/************************************************************************/
QString GffreadPrompter::composeRichDoc() {
    IntegralBusPort *in = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_ID));
    SAFE_POINT(NULL != in, "NULL input port", "");
    QString genome = getProducersOrUnset(IN_PORT_ID, GENOME_URL_SLOT_ID);
    QString transc = getProducersOrUnset(IN_PORT_ID, TRANSCRIPTS_URL_SLOT_ID);
    QString url = getHyperlink(OUT_URL_ATTR_ID, getURL(OUT_URL_ATTR_ID));

    return tr("Extract transcript sequences from the genomic sequence from <u>%1</u> using transcripts from <u>%2</u> and save them to the file %3.").arg(genome).arg(transc).arg(url);
}

} // LocalWorkflow
} // namespace
