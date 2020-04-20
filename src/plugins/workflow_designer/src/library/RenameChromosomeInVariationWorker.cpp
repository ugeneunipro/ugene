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

#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "RenameChromosomeInVariationWorker.h"
#include "tasks/RenameChromosomeInVariationTask.h"

namespace U2 {
namespace LocalWorkflow {

namespace {
const QString SEPARATOR = ";";
const QString TO_REPLACE_ATTR = "prefixes-to-replace";
const QString REPLACE_WITH_ATTR = "prefix-replace-with";

const QString INPUT_PORT = "in-file";
const QString OUTPUT_PORT = "out-file";
}

/************************************************************************/
/* Prompter */
/************************************************************************/
RenameChomosomeInVariationPrompter::RenameChomosomeInVariationPrompter(Actor *actor) :
    RenameChomosomeInVariationBase(actor)
{

}

QString RenameChomosomeInVariationPrompter::composeRichDoc() {
    const QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";

    const QStringList prefixesToReplace = getHyperlink(TO_REPLACE_ATTR, getRequiredParam(TO_REPLACE_ATTR)).split(SEPARATOR, QString::SkipEmptyParts);
    const QString prefixReplaceWith = getHyperlink(REPLACE_WITH_ATTR, getRequiredParam(REPLACE_WITH_ATTR));

    IntegralBusPort *input = qobject_cast<IntegralBusPort *>(target->getPort(INPUT_PORT));
    const Actor *producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    const QString producerName = tr("<u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    return tr("For each variation from %1 replace chromosome prefix%2 %3 with %4 prefix.")
            .arg(producerName).arg(prefixesToReplace.size() > 1 ? "es" : "").arg(prefixesToReplace.join(", ")).arg(prefixReplaceWith);
}

const QString RenameChomosomeInVariationWorkerFactory::ACTOR_ID("rename-chromosome-in-variation");

/************************************************************************/
/* Worker */
/************************************************************************/

RenameChomosomeInVariationWorker::RenameChomosomeInVariationWorker(Actor *actor)
    : BaseThroughWorker(actor, INPUT_PORT, OUTPUT_PORT),
      inputUrlPort(NULL),
      outputUrlPort(NULL)
{

}

QList<Message> RenameChomosomeInVariationWorker::fetchResult(Task *task, U2OpStatus &os) {
    QList<Message> messages;
    RenameChromosomeInVariationFileTask *renameTask = qobject_cast<RenameChromosomeInVariationFileTask *>(task);
    SAFE_POINT_EXT(NULL != renameTask, os.setError(tr("An unexpected task type")), messages);

    const QString resultFileUrl = renameTask->getDstFileUrl();
    monitor()->addOutputFile(resultFileUrl, actor->getId(), false);

    QVariantMap data;
    data[BaseSlots::URL_SLOT().getId()] = resultFileUrl;
    messages << Message(output->getBusType(), data);

    return messages;
}

Task * RenameChomosomeInVariationWorker::createTask(const Message &message, U2OpStatus &os) {
    const QStringList prefixesToReplace = getValue<QString>(TO_REPLACE_ATTR).split(SEPARATOR, QString::SkipEmptyParts);
    const QString prefixReplaceWith = getValue<QString>(REPLACE_WITH_ATTR);
    QString dstFileUrl = monitor()->outputDir() + QFileInfo(context->getMetadataStorage().get(message.getMetadataId()).getFileUrl()).fileName();

    const QVariantMap dataMap = message.getData().toMap();
    const QString srcFileUrl = dataMap.value(BaseSlots::URL_SLOT().getId()).toString();
    CHECK_EXT(!srcFileUrl.isEmpty(), os.setError(tr("Source file URL is empty")), NULL);

    if (dstFileUrl.isEmpty()) {
        dstFileUrl = monitor()->outputDir() + QFileInfo(srcFileUrl).fileName();
    }

    return new RenameChromosomeInVariationFileTask(srcFileUrl, dstFileUrl, prefixesToReplace, prefixReplaceWith);
}

/************************************************************************/
/* Factory */
/************************************************************************/
RenameChomosomeInVariationWorkerFactory::RenameChomosomeInVariationWorkerFactory() :
    DomainFactory(ACTOR_ID)
{

}

void RenameChomosomeInVariationWorkerFactory::init() {
    Descriptor desc(ACTOR_ID, RenameChomosomeInVariationWorker::tr("Change Chromosome Notation for VCF"),
        RenameChomosomeInVariationWorker::tr("Changes chromosome notation for each variant from the input VCF or other variation files."));

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, RenameChomosomeInVariationWorker::tr("Input file URL"),
            RenameChomosomeInVariationWorker::tr("Input files(s) with variations, usually in VCF format."));
        Descriptor outD(OUTPUT_PORT, RenameChomosomeInVariationWorker::tr("Output file URL"),
            RenameChomosomeInVariationWorker::tr("Output file(s) with annotated variations."));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("renameChr.input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        Descriptor outSlotDescriptor("url", RenameChomosomeInVariationWorker::tr("Produced URL"), RenameChomosomeInVariationWorker::tr("Location of a corresponding produced file."));
        outM[outSlotDescriptor] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("renameChr.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor prefixesToReplace(TO_REPLACE_ATTR, RenameChomosomeInVariationWorker::tr("Replace prefixes"),
            RenameChomosomeInVariationWorker::tr("Input the list of chromosome prefixes that you would like to replace, for example \"NC_000\". Separate different prefixes by semicolons."));

        Descriptor prefixReplaceWith(REPLACE_WITH_ATTR, RenameChomosomeInVariationWorker::tr("Replace by"),
            RenameChomosomeInVariationWorker::tr("Input the prefix that should be set instead, for example \"chr\"."));

        a << new Attribute(prefixesToReplace, BaseTypes::STRING_TYPE(), true);
        a << new Attribute(prefixReplaceWith, BaseTypes::STRING_TYPE(), false);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setPrompter(new RenameChomosomeInVariationPrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_VARIATION_ANALYSIS(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new RenameChomosomeInVariationWorkerFactory());
}

Worker * RenameChomosomeInVariationWorkerFactory::createWorker(Actor *actor) {
    return new RenameChomosomeInVariationWorker(actor);
}

}   // namespace LocalWorkflow
}   // namespace U2
