/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/URLAttribute.h>
#include <U2Lang/WorkflowEnv.h>

#include <U2Designer/DelegateEditors.h>
#include <U2Gui/GUIUtils.h>

#include "GetReadListWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString GetReadsListWorkerFactory::SE_ACTOR_ID("get-se-reads-list");
const QString GetReadsListWorkerFactory::PE_ACTOR_ID("get-pe-reads-list");

const QString GetReadsListWorkerFactory::SE_SLOT_ID("reads-url1");
const QString GetReadsListWorkerFactory::PE_SLOT_ID("reads-url2");

static const QString OUT_PORT_ID("out");

static const QString SE_URL_ATTR("url1");
static const QString PE_URL_ATTR("url2");

const Descriptor GetReadsListWorkerFactory::SE_SLOT()
{
    return Descriptor(SE_SLOT_ID, GetReadsListWorker::tr("Source URL 1"), GetReadsListWorker::tr("Source URL 1"));
}

const Descriptor GetReadsListWorkerFactory::PE_SLOT()
{
    return Descriptor(PE_SLOT_ID, GetReadsListWorker::tr("Source URL 2"), GetReadsListWorker::tr("Source URL 2"));
}

/************************************************************************/
/* Worker */
/************************************************************************/
GetReadsListWorker::GetReadsListWorker(Actor *p)
: BaseWorker(p), outChannel(NULL), files(NULL), pairedFiles(NULL)
{

}

void GetReadsListWorker::init() {
    outChannel = ports.value(OUT_PORT_ID);

    QList<Dataset> sets = getValue< QList<Dataset> >(SE_URL_ATTR);
    files = new DatasetFilesIterator(sets);

    algoLog.info(QString("GetReadsListWorker %1").arg(actor->getProto()->getId()));
    if (actor->getProto()->getId() == GetReadsListWorkerFactory::PE_ACTOR_ID) {
        QList<Dataset> sets2 = getValue< QList<Dataset> >(PE_URL_ATTR);
        pairedFiles = new DatasetFilesIterator(sets2);
    }
}

Task * GetReadsListWorker::tick() {
    if (files->hasNext()) {
        QVariantMap m;
        QString url = files->getNextFile();
        m[GetReadsListWorkerFactory::SE_SLOT_ID] = url;
        if (pairedFiles != NULL) {
            if (pairedFiles->hasNext()) {
                QString url2 = pairedFiles->getNextFile();
                m[GetReadsListWorkerFactory::PE_SLOT_ID] = url2;
            } else {
                reportError(tr("Missing right PE read for the left read: %1").arg(url));
                return NULL;
            }
        }
        QString datasetName = files->getLastDatasetName();
        MessageMetadata metadata(url, datasetName);
        context->getMetadataStorage().put(metadata);
        outChannel->put(Message(outChannel->getBusType(), m, metadata.getId()));
    } else {
        if (pairedFiles != NULL && pairedFiles->hasNext()) {
            reportError(tr("Missing left PE read for the right read: %1").arg(pairedFiles->getNextFile()));
        } else {
            setDone();
            outChannel->setEnded();
        }
    }
    return NULL;
}

void GetReadsListWorker::cleanup() {
    delete files;
}

/************************************************************************/
/* Factory */
/************************************************************************/
void GetReadsListWorkerFactory::init() {
    {
        QList<PortDescriptor*> portDescs;
        {
            QMap<Descriptor, DataTypePtr> outTypeMap;
            outTypeMap[SE_SLOT()] = BaseTypes::STRING_TYPE();
            DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_TEXT_PORT_ID(), outTypeMap));

            portDescs << new PortDescriptor(Descriptor(OUT_PORT_ID,
                                                       GetReadsListWorker::tr("Output File"),
                                                       GetReadsListWorker::tr("The port outputs one or several URLs.")), outTypeSet, false, true);
        }

        QList<Attribute*> attrs;
        {
            Descriptor seUrl(SE_URL_ATTR, GetReadsListWorker::tr("Input URL"), GetReadsListWorker::tr("Input URL"));

            attrs << new URLAttribute(seUrl, BaseTypes::URL_DATASETS_TYPE(), true);
        }
        Descriptor protoDesc(GetReadsListWorkerFactory::SE_ACTOR_ID,
            GetReadsListWorker::tr("Read FASTQ File with SE Reads"),
            GetReadsListWorker::tr("Input one or several files with NGS single-end reads in FASTQ format. The element outputs the file(s) URL(s)."));

        ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
        proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
        proto->setPrompter(new GetReadsListPrompter());
        if(AppContext::isGUIMode()) {
            proto->setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
        }
        WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASRC(), proto);
        WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new GetReadsListWorkerFactory(SE_ACTOR_ID));
    }
    ///////////////////////// PE reads //////////////////////////////
    {
        QList<PortDescriptor*> portDescs;
        {
            QMap<Descriptor, DataTypePtr> outTypeMap;
            outTypeMap[SE_SLOT()] = BaseTypes::STRING_TYPE();
            outTypeMap[PE_SLOT()] = BaseTypes::STRING_TYPE();
            DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_TEXT_PORT_ID(), outTypeMap));

            portDescs << new PortDescriptor(Descriptor(OUT_PORT_ID,
                                                       GetReadsListWorker::tr("Output File"),
                                                       GetReadsListWorker::tr("The port outputs one or several pairs of URL(s).")), outTypeSet, false, true);
        }

        QList<Attribute*> attrs;
        {
            Descriptor seUrl(SE_URL_ATTR, GetReadsListWorker::tr("Left PE reads"), GetReadsListWorker::tr("Left PE reads"));
            Descriptor peUrl(PE_URL_ATTR, GetReadsListWorker::tr("Right PE reads"), GetReadsListWorker::tr("Right PE reads"));

            attrs << new URLAttribute(seUrl, BaseTypes::URL_DATASETS_TYPE(), true);
            attrs << new URLAttribute(peUrl, BaseTypes::URL_DATASETS_TYPE(), true);
        }
        Descriptor protoDesc(GetReadsListWorkerFactory::PE_ACTOR_ID,
            GetReadsListWorker::tr("Read FASTQ Files with PE Reads"),
            GetReadsListWorker::tr("Input one or several pairs of files with NGS paired-end reads in FASTQ format. The element outputs the corresponding pairs of URLs."));

        ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
        proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
        proto->setPrompter(new GetReadsListPrompter());
        if(AppContext::isGUIMode()) {
            proto->setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
        }
        WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASRC(), proto);
        WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new GetReadsListWorkerFactory(PE_ACTOR_ID));
    }
}

void GetReadsListWorkerFactory::cleanup() {
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);

    delete WorkflowEnv::getProtoRegistry()->unregisterProto(SE_ACTOR_ID);
    delete localDomain->unregisterEntry(SE_ACTOR_ID);

    delete WorkflowEnv::getProtoRegistry()->unregisterProto(PE_ACTOR_ID);
    delete localDomain->unregisterEntry(PE_ACTOR_ID);
}

Worker *GetReadsListWorkerFactory::createWorker(Actor *a) {
    return new GetReadsListWorker(a);
}

/************************************************************************/
/* Prompter */
/************************************************************************/
QString GetReadsListPrompter::composeRichDoc() {
    return tr("Read and output the files URLs.");
}

/************************************************************************/
/* SeReadsListSplitter */
/************************************************************************/
const QString SeReadsListSplitter::ID = "se-reads";

SeReadsListSplitter::SeReadsListSplitter()
    : CandidatesSplitter(ID)
{

}

bool SeReadsListSplitter::canSplit(const Descriptor &toDesc, DataTypePtr toDatatype) {
    return ((BaseTypes::STRING_TYPE() == toDatatype) && (toDesc.getId() == GetReadsListWorkerFactory::SE_SLOT().getId()));
}

bool SeReadsListSplitter::isMain(const QString &candidateSlotId) {
    return (candidateSlotId == GetReadsListWorkerFactory::SE_SLOT().getId());
}

/************************************************************************/
/* PeReadsListSplitter */
/************************************************************************/
const QString PeReadsListSplitter::ID = "pe-reads";

PeReadsListSplitter::PeReadsListSplitter()
    : CandidatesSplitter(ID)
{

}

bool PeReadsListSplitter::canSplit(const Descriptor &toDesc, DataTypePtr toDatatype) {
    return ((BaseTypes::STRING_TYPE() == toDatatype) && (toDesc.getId() == GetReadsListWorkerFactory::PE_SLOT().getId()));
}

bool PeReadsListSplitter::isMain(const QString &candidateSlotId) {
    return (candidateSlotId == GetReadsListWorkerFactory::PE_SLOT().getId());
}

} // LocalWorkflow
} // U2
