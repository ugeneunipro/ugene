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

#include "GetFileListWorker.h"
#include "util/DatasetValidator.h"

namespace U2 {
namespace LocalWorkflow {

const QString GetFileListWorkerFactory::ACTOR_ID("get-file-list");
static const QString OUT_PORT_ID("out-url");

static const QString URL_ATTR("url-in");

/************************************************************************/
/* Worker */
/************************************************************************/
GetFileListWorker::GetFileListWorker(Actor *p)
: BaseWorker(p), outChannel(NULL), files(NULL)
{

}

void GetFileListWorker::init() {
    outChannel = ports.value(OUT_PORT_ID);

    QList<Dataset> sets = getValue< QList<Dataset> >(BaseAttributes::URL_IN_ATTRIBUTE().getId());
    files = new DatasetFilesIterator(sets);
}

Task * GetFileListWorker::tick() {
    if (files->hasNext()) {
        QVariantMap m;
        QString url = files->getNextFile();
        QString datasetName = files->getLastDatasetName();
        m[BaseSlots::URL_SLOT().getId()] = url;
        m[BaseSlots::DATASET_SLOT().getId()] = datasetName;
        MessageMetadata metadata(url, datasetName);
        context->getMetadataStorage().put(metadata);
        outChannel->put(Message(outChannel->getBusType(), m, metadata.getId()));
    } else {
        setDone();
        outChannel->setEnded();
    }
    return NULL;
}

void GetFileListWorker::cleanup() {
    delete files;
}

/************************************************************************/
/* Factory */
/************************************************************************/
void GetFileListWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> outTypeMap;
        outTypeMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        outTypeMap[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_TEXT_PORT_ID(), outTypeMap));

        portDescs << new PortDescriptor(Descriptor(OUT_PORT_ID, GetFileListWorker::tr("Output URL"), GetFileListWorker::tr("Paths read by the element.")), outTypeSet, false, true);
    }

    QList<Attribute*> attrs;
    {
        Descriptor inUrl(URL_ATTR,
            GetFileListWorker::tr("Input URL"),
            GetFileListWorker::tr("Input URL"));

        attrs << new URLAttribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::URL_DATASETS_TYPE(), true);
    }

    Descriptor protoDesc(GetFileListWorkerFactory::ACTOR_ID,
        GetFileListWorker::tr("Read File URL(s)"),
        GetFileListWorker::tr("Input one or several files in any format. The element outputs the file(s) URL(s)."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
    proto->setPrompter(new GetFileListPrompter());
    proto->setValidator(new DatasetValidator());
    if(AppContext::isGUIMode()) {
        proto->setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
    }

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASRC(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new GetFileListWorkerFactory());
}

Worker *GetFileListWorkerFactory::createWorker(Actor *a) {
    return new GetFileListWorker(a);
}

/************************************************************************/
/* Prompter */
/************************************************************************/
QString GetFileListPrompter::composeRichDoc() {
    QString url = getHyperlink(URL_ATTR, getURL(URL_ATTR));
    return tr("Gets paths of files: <u>%1</u>.")
        .arg(url);
}

} // LocalWorkflow
} // U2
