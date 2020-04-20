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

#include "FilterAnnotationsByQualifierWorker.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/TaskSignalMapper.h>

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

const QString FilterAnnotationsByQualifierWorkerFactory::ACTOR_ID("filter-annotations-by-qualifier");

const static QString QUALIFER_NAME_ATTR("qualifier-name");
const static QString QUALIFER_VALUE_ATTR("qualifier-value");
const static QString WHICH_FILTER_ATTR("accept-or-filter");

QString FilterAnnotationsByQualifierPrompter::composeRichDoc() {
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString annName = getProducers(BasePorts::IN_ANNOTATIONS_PORT_ID(), BaseSlots::ANNOTATION_TABLE_SLOT().getId());
    annName = annName.isEmpty() ? unsetStr : annName;
    return tr("Filter annotations from <u>%1</u> by given qualifier name and value.").arg(annName);
}

void FilterAnnotationsByQualifierWorker::init() {
    input = ports.value(BasePorts::IN_ANNOTATIONS_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* FilterAnnotationsByQualifierWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }

        QVariantMap qm = inputMessage.getData().toMap();
        const QVariant annsVar = qm[BaseSlots::ANNOTATION_TABLE_SLOT().getId()];
        inputAnns = StorageUtils::getAnnotationTable(context->getDataStorage(), annsVar);

        bool accept = actor->getParameter(WHICH_FILTER_ATTR)->getAttributeValue<bool>(context);
        QString qualName = actor->getParameter(QUALIFER_NAME_ATTR)->getAttributeValue<QString>(context);
        QString qualValue = actor->getParameter(QUALIFER_VALUE_ATTR)->getAttributeValue<QString>(context);

        Task *t = new FilterAnnotationsByQualifierTask(inputAnns, qualName, qualValue, accept);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void FilterAnnotationsByQualifierWorker::sl_taskFinished(Task *t) {
    if (t->isCanceled() || t->hasError()) {
        return;
    }
    const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(inputAnns);
    output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(),
        qVariantFromValue<SharedDbiDataHandler>(tableId)));
}

void FilterAnnotationsByQualifierWorker::cleanup() {
}


void FilterAnnotationsByQualifierWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    QList<Attribute*> attribs;

    //accept sequence and annotated regions as input
    QMap<Descriptor, DataTypePtr> inputMap;
    inputMap[ BaseSlots::ANNOTATION_TABLE_SLOT() ] = BaseTypes::ANNOTATION_TABLE_TYPE();

    { //Create input port descriptors
        Descriptor inDesc(BasePorts::IN_ANNOTATIONS_PORT_ID(), FilterAnnotationsByQualifierWorker::tr("Input annotations"),
            FilterAnnotationsByQualifierWorker::tr("Annotations to be filtered by name."));
        Descriptor outDesc(BasePorts::OUT_ANNOTATIONS_PORT_ID(), FilterAnnotationsByQualifierWorker::tr("Result annotations"),
            FilterAnnotationsByQualifierWorker::tr("Resulted annotations, filtered by name."));

        portDescs << new PortDescriptor(inDesc, DataTypePtr(new MapDataType("filter.anns", inputMap)), /*input*/ true);
        portDescs << new PortDescriptor(outDesc, DataTypePtr(new MapDataType("filter.anns", inputMap)), /*input*/false, /*multi*/true);
    }

    { //Create attributes descriptors
        Descriptor qualifierNameDesc(QUALIFER_NAME_ATTR,
            FilterAnnotationsByQualifierWorker::tr("Qualifier name"),
            FilterAnnotationsByQualifierWorker::tr("Name of the qualifier to use for filtering."));
        Descriptor qualifierValDesc(QUALIFER_VALUE_ATTR,
            FilterAnnotationsByQualifierWorker::tr("Qualifier value"),
            FilterAnnotationsByQualifierWorker::tr("Text value of the qualifier to apply as filtering criteria"));
        Descriptor whichFilterDesc(WHICH_FILTER_ATTR,
            FilterAnnotationsByQualifierWorker::tr("Accept or filter"),
            FilterAnnotationsByQualifierWorker::tr("Selects the name filter: accept specified names or accept all except specified."));

        attribs << new Attribute(qualifierNameDesc, BaseTypes::STRING_TYPE(), /*required*/true);
        attribs << new Attribute(qualifierValDesc, BaseTypes::STRING_TYPE(), /*required*/true);
        attribs << new Attribute(whichFilterDesc, BaseTypes::BOOL_TYPE(), /*required*/ false, QVariant(true));
    }

    Descriptor desc(FilterAnnotationsByQualifierWorkerFactory::ACTOR_ID,
        FilterAnnotationsByQualifierWorker::tr("Filter Annotations by Qualifier"),
        FilterAnnotationsByQualifierWorker::tr("Filters annotations by Qualifier."));
    ActorPrototype * proto = new IntegralBusActorPrototype(desc, portDescs, attribs);

    proto->setPrompter(new FilterAnnotationsByQualifierPrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new FilterAnnotationsByQualifierWorkerFactory());
}

void FilterAnnotationsByQualifierTask::run() {

    //TODO: add reg exp option and tests!

    QMutableListIterator<SharedAnnotationData> i(anns);

    while (i.hasNext()) {
        SharedAnnotationData &ad = i.next();
        QVector<U2Qualifier> quals;
        ad->findQualifiers(qualName, quals);

        bool matchFound = false;
        foreach (const U2Qualifier &qual, quals) {
            if (qual.value == qualFilterVal) {
                matchFound = true;
                break;
            }
        }

        if (accept) {
            if (!matchFound) {
                i.remove();
            }
        } else {
            if (matchFound) {
                i.remove();
            }
        }
    }

}


} // U2 namespace
} // LocalWorkflow namespace
