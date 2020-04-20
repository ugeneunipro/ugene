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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/ConvertSnpeffVariationsToAnnotationsTask.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/URLAttribute.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ConvertSnpeffVariationsToAnnotationsWorker.h"

namespace U2 {
namespace LocalWorkflow {

static const QString IN_VARIATIONS_URL_PORT_ID = "in-variations-url";

ConvertSnpeffVariationsToAnnotationsPrompter::ConvertSnpeffVariationsToAnnotationsPrompter(Actor *actor)
    : PrompterBase<ConvertSnpeffVariationsToAnnotationsPrompter>(actor)
{

}

QString ConvertSnpeffVariationsToAnnotationsPrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort*>(target->getPort(IN_VARIATIONS_URL_PORT_ID));
    SAFE_POINT(NULL != input, "No input port", "");
    const Actor *producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    const QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    const QString producerName = (NULL != producer) ? producer->getLabel() : unsetStr;
    return tr("Parses information in variations from <u>%1</u> into annotations.").arg(producerName);
}

const QString ConvertSnpeffVariationsToAnnotationsFactory::ACTOR_ID = "convert-snpeff-variations-to-annotations";

ConvertSnpeffVariationsToAnnotationsFactory::ConvertSnpeffVariationsToAnnotationsFactory()
    : DomainFactory(ACTOR_ID)
{

}

Worker * ConvertSnpeffVariationsToAnnotationsFactory::createWorker(Actor *actor) {
    return new ConvertSnpeffVariationsToAnnotationsWorker(actor);
}

void ConvertSnpeffVariationsToAnnotationsFactory::init() {
    QList<PortDescriptor *> ports;
    {
        Descriptor inDesc(IN_VARIATIONS_URL_PORT_ID,
                          ConvertSnpeffVariationsToAnnotationsWorker::tr("Input URL"),
                          ConvertSnpeffVariationsToAnnotationsWorker::tr("Input variation file URL."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();

        ports << new PortDescriptor(inDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);
    }

    DocumentFormatConstraints constraints;
    constraints.supportedObjectTypes.insert(GObjectTypes::ANNOTATION_TABLE);
    constraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    constraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> supportedFormats = AppContext::getDocumentFormatRegistry()->selectFormats(constraints);

    QList<Attribute *> attributes;
    {
        attributes << new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, "");

        const DocumentFormatId format = (supportedFormats.contains(BaseDocumentFormats::PLAIN_GENBANK) ? BaseDocumentFormats::PLAIN_GENBANK : supportedFormats.first());
        Attribute *documentFormatAttribute = new Attribute(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, format);
        documentFormatAttribute->addRelation(new FileExtensionRelation(BaseAttributes::URL_OUT_ATTRIBUTE().getId()));
        attributes << documentFormatAttribute;
    }

    Descriptor desc(ACTOR_ID,
                    ConvertSnpeffVariationsToAnnotationsWorker::tr("Convert SnpEff Variations to Annotations"),
                    ConvertSnpeffVariationsToAnnotationsWorker::tr("Parses information, added to variations by SnpEff, into standard annotations."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setPrompter(new ConvertSnpeffVariationsToAnnotationsPrompter(NULL));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_VARIATION_ANALYSIS(), proto);

    QMap<QString, PropertyDelegate *> delegates;
    {
        DelegateTags tags;
        tags.set(DelegateTags::PLACEHOLDER_TEXT, ConvertSnpeffVariationsToAnnotationsWorker::tr("Produced from the input file name"));
        delegates[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(tags, "", "");

        QVariantMap map;
        foreach (const DocumentFormatId &formatId, supportedFormats) {
            map[formatId] = formatId;
        }
        delegates[BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId()] = new ComboBoxDelegate(map);
    }
    proto->setEditor(new DelegateEditor(delegates));

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ConvertSnpeffVariationsToAnnotationsFactory());
}

ConvertSnpeffVariationsToAnnotationsWorker::ConvertSnpeffVariationsToAnnotationsWorker(Actor *actor)
    : BaseWorker(actor),
      input(NULL)
{

}

void ConvertSnpeffVariationsToAnnotationsWorker::init() {
    input = ports.value(IN_VARIATIONS_URL_PORT_ID);
}

Task * ConvertSnpeffVariationsToAnnotationsWorker::tick() {
    if (input->hasMessage()) {
        return createTask(getMessageAndSetupScriptValues(input));
    } else if (input->isEnded()) {
        setDone();
    }
    return NULL;
}

void ConvertSnpeffVariationsToAnnotationsWorker::cleanup() {

}

void ConvertSnpeffVariationsToAnnotationsWorker::sl_taskFinished(Task *task) {
    LoadConvertAndSaveSnpeffVariationsToAnnotationsTask *convertTask = qobject_cast<LoadConvertAndSaveSnpeffVariationsToAnnotationsTask *>(task);
    SAFE_POINT(NULL != convertTask, L10N::nullPointerError("LoadConvertAndSaveSnpeffVariationsToAnnotationsTask"), );
    CHECK(!convertTask->hasError() && !convertTask->isCanceled(), );
    monitor()->addOutputFile(convertTask->getResultUrl(), getActorId());
}

Task * ConvertSnpeffVariationsToAnnotationsWorker::createTask(const Message &message) {
    QVariantMap data = message.getData().toMap();
    const QString variationsFileurl = data[BaseSlots::URL_SLOT().getId()].toString();
    const QString formatId = actor->getParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId())->getAttributeValue<QString>(context);
    QString annotationsFileUrl = actor->getParameter(BaseAttributes::URL_OUT_ATTRIBUTE().getId())->getAttributeValue<QString>(context);
    if (annotationsFileUrl.isEmpty()) {
        annotationsFileUrl = context->getMetadataStorage().get(message.getMetadataId()).getFileUrl();
        const GUrl sourceUrl = GUrlUtils::changeFileExt(annotationsFileUrl, formatId);
        annotationsFileUrl = GUrlUtils::rollFileName(context->workingDir() + sourceUrl.baseFileName() + "_variants." + sourceUrl.completeFileSuffix(), "_");
    }
    Task *task = new LoadConvertAndSaveSnpeffVariationsToAnnotationsTask(variationsFileurl, context->getDataStorage()->getDbiRef(), annotationsFileUrl, formatId);
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
    return task;
}

}   // namespace LocalWorkflow
}   // namespace U2
