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

#include "FilterAnnotationsWorker.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ConfigurationEditor.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Gui/DialogUtils.h>

#include <U2Designer/DelegateEditors.h>

namespace U2 {
namespace LocalWorkflow {

const QString FilterAnnotationsWorkerFactory::ACTOR_ID("filter-annotations");

const static QString FILTER_NAMES_FILE_ATTR("annotation-names-file");
const static QString FILTER_NAMES_ATTR("annotation-names");
const static QString WHICH_FILTER_ATTR("accept-or-filter");

QString FilterAnnotationsPrompter::composeRichDoc() {
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString annName = getProducers(BasePorts::IN_ANNOTATIONS_PORT_ID(), BaseSlots::ANNOTATION_TABLE_SLOT().getId());
    annName = annName.isEmpty() ? unsetStr : annName;
    return tr("Filter annotations from <u>%1</u> by supplied names.").arg(annName);
}

void FilterAnnotationsWorker::init() {
    input = ports.value(BasePorts::IN_ANNOTATIONS_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* FilterAnnotationsWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }

        QVariantMap qm = inputMessage.getData().toMap();
        const QVariant annsVar = qm[BaseSlots::ANNOTATION_TABLE_SLOT().getId()];
        QList<SharedAnnotationData> inputAnns = StorageUtils::getAnnotationTable(context->getDataStorage(), annsVar);

        bool accept = getValue<bool>(WHICH_FILTER_ATTR);
        QString namesString = getValue<QString>(FILTER_NAMES_ATTR);
        QString namesFile = getValue<QString>(FILTER_NAMES_FILE_ATTR);

        Task* t = new FilterAnnotationsTask(inputAnns, namesString, namesFile, accept);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void FilterAnnotationsWorker::sl_taskFinished(Task *t) {
    FilterAnnotationsTask *task = dynamic_cast<FilterAnnotationsTask*>(t);
    CHECK(NULL != task, );
    CHECK(!task->getStateInfo().isCoR(), );

    const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(task->takeResult());
    output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), qVariantFromValue<SharedDbiDataHandler>(tableId)));
}

void FilterAnnotationsWorker::cleanup() {
}


void FilterAnnotationsWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    QList<Attribute*> attribs;

    //accept sequence and annotated regions as input
    QMap<Descriptor, DataTypePtr> inputMap;
    inputMap[ BaseSlots::ANNOTATION_TABLE_SLOT() ] = BaseTypes::ANNOTATION_TABLE_TYPE();

    { //Create input port descriptors
        Descriptor inDesc(BasePorts::IN_ANNOTATIONS_PORT_ID(), FilterAnnotationsWorker::tr("Input annotations"),
            FilterAnnotationsWorker::tr("Annotations to be filtered by name."));
        Descriptor outDesc(BasePorts::OUT_ANNOTATIONS_PORT_ID(), FilterAnnotationsWorker::tr("Result annotations"),
            FilterAnnotationsWorker::tr("Resulted annotations, filtered by name."));

        portDescs << new PortDescriptor(inDesc, DataTypePtr(new MapDataType("filter.anns", inputMap)), /*input*/ true);
        portDescs << new PortDescriptor(outDesc, DataTypePtr(new MapDataType("filter.anns", inputMap)), /*input*/false, /*multi*/true);
    }

    { //Create attributes descriptors
        Descriptor filterNamesDesc(FILTER_NAMES_ATTR,
            FilterAnnotationsWorker::tr("Annotation names"),
            FilterAnnotationsWorker::tr("List of annotation names, separated by spaces, that will be accepted or filtered."));
        Descriptor filterNamesFileDesc(FILTER_NAMES_FILE_ATTR,
            FilterAnnotationsWorker::tr("Annotation names file"),
            FilterAnnotationsWorker::tr("File with annotation names, separated by whitespaces, that will be accepted or filtered."));
        Descriptor whichFilterDesc(WHICH_FILTER_ATTR,
            FilterAnnotationsWorker::tr("Accept or filter"),
            FilterAnnotationsWorker::tr("Selects the name filter: accept specified names or accept all except specified."));

        attribs << new Attribute(filterNamesDesc, BaseTypes::STRING_TYPE(), /*required*/false);
        attribs << new Attribute(filterNamesFileDesc, BaseTypes::STRING_TYPE(), /*required*/false);
        attribs << new Attribute(whichFilterDesc, BaseTypes::BOOL_TYPE(), /*required*/ false, QVariant(true));
    }

    Descriptor desc(FilterAnnotationsWorkerFactory::ACTOR_ID,
        FilterAnnotationsWorker::tr("Filter Annotations by Name"),
        FilterAnnotationsWorker::tr("Filters annotations by name."));
    ActorPrototype * proto = new IntegralBusActorPrototype(desc, portDescs, attribs);

    proto->setPrompter(new FilterAnnotationsPrompter());
    {
        QMap<QString, PropertyDelegate*> delegateMap;
        delegateMap[FILTER_NAMES_FILE_ATTR] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), false, false, false);
        proto->setEditor(new DelegateEditor(delegateMap));
    }
    proto->setValidator(new FilterAnnotationsValidator());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new FilterAnnotationsWorkerFactory());
}

/************************************************************************/
/* FilterAnnotationsValidator */
/************************************************************************/
namespace {
    bool hasValue(Attribute *attr) {
        if (!attr->isEmpty() && !attr->isEmptyString()) {
            return true;
        }
        if (!attr->getAttributeScript().isEmpty()) {
            return true;
        }
        return false;
    }
}

bool FilterAnnotationsValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &/*options*/) const {
    Attribute *namesAttr = actor->getParameter(FILTER_NAMES_ATTR);
    Attribute *namesFileAttr = actor->getParameter(FILTER_NAMES_FILE_ATTR);

    if (hasValue(namesAttr) || hasValue(namesFileAttr)) {
        return true;
    }
    notificationList << WorkflowNotification(FilterAnnotationsWorker::tr("At least one of these parameters must be set: \"Annotation names\", \"Annotation names file\"."));
    return false;
}

/************************************************************************/
/* FilterAnnotationsTask */
/************************************************************************/
FilterAnnotationsTask::FilterAnnotationsTask(const QList<SharedAnnotationData> &annotations, const QString &namesString, const QString &namesUrl, bool accept)
: Task(tr("Filter annotations task"), TaskFlag_None), annotations(annotations), namesString(namesString), namesUrl(namesUrl), accept(accept)
{

}

void FilterAnnotationsTask::run() {
    QStringList names = namesString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    names << readAnnotationNames(stateInfo);
    CHECK_OP(stateInfo, );

    if (names.isEmpty()) {
        setError(tr("The list of annotation names to accept/filter is empty"));
    }

    QMutableListIterator<SharedAnnotationData> i(annotations);
    while (i.hasNext()) {
        SharedAnnotationData &ad = i.next();
        if (accept) {
            if (!names.contains(ad->name)) {
                i.remove();
            }
        } else {
            if (names.contains(ad->name)) {
                i.remove();
            }
        }
    }
}

QList<SharedAnnotationData> FilterAnnotationsTask::takeResult() {
    QList<SharedAnnotationData> result = annotations;
    annotations.clear();
    return result;
}

QStringList FilterAnnotationsTask::readAnnotationNames(U2OpStatus &os) const {
    CHECK(QFileInfo(namesUrl).exists(), QStringList());

    QFile file(namesUrl);
    bool opened = file.open(QFile::ReadOnly);
    CHECK(opened, QStringList());

    try {
        QString data = file.readAll();
        if (0 == data.size() && file.size() > 0) { // QFile::readAll() has no way of errors reporting.
            os.setError(tr("Too big annotation names file"));
            return QStringList();
        }
        return data.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    } catch (const std::bad_alloc &) {
        os.setError(tr("Not enough memory to load the file with annotation names"));
        return QStringList();
    }
}

} // U2 namespace
} // LocalWorkflow namespace
