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

#include <U2Core/GObjectTypes.h>
#include <U2Core/DNASequence.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Gui/DialogUtils.h>
#include <U2Designer/DelegateEditors.h>

#include "DocActors.h"
#include "ImportAnnotationsWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ImportAnnotationsWorkerFactory::ACTOR_ID("import-annotations");

static const QString IMPORT_ANNOTATIONS_IN_TYPE_ID("import.annotations.in.port");
static const QString IMPORT_ANNOTATIONS_OUT_TYPE_ID("import.annotations.out.port");

/**************************
 * ImportAnnotationsWorker
 **************************/
void ImportAnnotationsWorker::init() {
    inPort = ports.value(BasePorts::IN_ANNOTATIONS_PORT_ID());
    outPort = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
    assert(inPort && outPort);
}

Task * ImportAnnotationsWorker::tick() {
    if (inPort->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(inPort);
        QList<QString> urls = WorkflowUtils::expandToUrls(
            actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>(context));

        QList<Task*> loadTasks;
        foreach(const QString & url, urls) {
            LoadDocumentTask * loadDocTask = LoadDocumentTask::getDefaultLoadDocTask(url);
            if(loadDocTask == NULL) {
                qDeleteAll(loadTasks);
                return new FailTask(L10N::errorOpeningFileRead(url));
            }
            loadTasks << loadDocTask;
        }
        Task * ret = new MultiTask(tr("Load documents with annotations"), loadTasks);
        connect(new TaskSignalMapper(ret), SIGNAL(si_taskFinished(Task*)), SLOT(sl_docsLoaded(Task*)));

        addTaskAnnotations(inputMessage.getData(), ret);
        return ret;
    } else if (inPort->isEnded()) {
        setDone();
        outPort->setEnded();
    }
    return NULL;
}

void ImportAnnotationsWorker::addTaskAnnotations(const QVariant &data, Task *t) {
    QVariantMap dataMap = data.toMap();
    if (dataMap.contains(BaseSlots::ANNOTATION_TABLE_SLOT().getId())) {
        const QList<SharedAnnotationData> result = StorageUtils::getAnnotationTable(context->getDataStorage(),
            dataMap[BaseSlots::ANNOTATION_TABLE_SLOT().getId()]);
        annsMap[t] = result;
    }
}

static QList<SharedAnnotationData> getAnnsFromDoc(Document *doc) {
    QList<SharedAnnotationData> ret;
    if (NULL == doc) {
        return ret;
    }
    QList<GObject *> objs = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    foreach (GObject *obj, objs) {
        AnnotationTableObject *annObj = qobject_cast<AnnotationTableObject *>(obj);
        if (NULL == annObj) {
            continue;
        }
        foreach (Annotation *a, annObj->getAnnotations()) {
            ret << a->getData();
        }
    }
    return ret;
}

void ImportAnnotationsWorker::sl_docsLoaded(Task *ta) {
    MultiTask *t = qobject_cast<MultiTask *>(ta);
    if (NULL == t || t->hasError()) {
        return;
    }

    QList<SharedAnnotationData> anns = annsMap.value(t);
    QList<Task *> loadSubs = t->getTasks();
    foreach (Task *s, loadSubs) {
        LoadDocumentTask *sub = qobject_cast<LoadDocumentTask *>(s);
        if (NULL == sub || sub->hasError()) {
            continue;
        }
        anns.append(getAnnsFromDoc(sub->getDocument()));
    }
    const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(anns);
    outPort->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), qVariantFromValue<SharedDbiDataHandler>(tableId)));
}

void ImportAnnotationsWorker::cleanup() {

}

/*********************************
 * ImportAnnotationsWorkerFactory
 *********************************/
void ImportAnnotationsWorkerFactory::init() {
    // ports description
    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();
        DataTypePtr inSet(new MapDataType(IMPORT_ANNOTATIONS_IN_TYPE_ID, inM));
        Descriptor inPortDesc(BasePorts::IN_ANNOTATIONS_PORT_ID(), ImportAnnotationsWorker::tr("Input annotations"),
            ImportAnnotationsWorker::tr("Input annotation table. Read annotations will be added to it"));
        portDescs << new PortDescriptor(inPortDesc, inSet, true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        DataTypePtr outSet(new MapDataType(IMPORT_ANNOTATIONS_OUT_TYPE_ID, outM));
        Descriptor outPortDesc(BasePorts::OUT_ANNOTATIONS_PORT_ID(), ImportAnnotationsWorker::tr("Output annotations"),
            ImportAnnotationsWorker::tr("Output annotation table"));
        portDescs << new PortDescriptor(outPortDesc, outSet, false);
    }
    // attributes description
    QList<Attribute*> attrs;
    {
        attrs << new Attribute(BaseAttributes::URL_IN_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
    }

    Descriptor protoDesc(ImportAnnotationsWorkerFactory::ACTOR_ID,
        ImportAnnotationsWorker::tr("Merge Annotations"),
        ImportAnnotationsWorker::tr("Read input annotation table and merge it with supplied annotation tables."));
    ActorPrototype * proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);

    //proto delegates
    QMap<QString, PropertyDelegate*> delegates;
    {
        delegates[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate(
            DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::ANNOTATION_TABLE, true), QString(), true, false, false);
    }
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ReadDocPrompter(ImportAnnotationsWorker::tr("Merge input annotations with annotations from <u>%1</u>.")));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new ImportAnnotationsWorkerFactory());
}

Worker * ImportAnnotationsWorkerFactory::createWorker(Actor* a) {
    return new ImportAnnotationsWorker(a);
}

} // LocalWorkflow
} // U2
