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

#include <QThread>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "WevotePrompter.h"
#include "WevoteSupport.h"
#include "WevoteValidator.h"
#include "WevoteWorker.h"
#include "WevoteWorkerFactory.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

const QString WevoteWorkerFactory::ACTOR_ID = "wevote-classify";

const QString WevoteWorkerFactory::INPUT_PORT_ID = "in";
const QString WevoteWorkerFactory::OUTPUT_PORT_ID = "out";

const QString WevoteWorkerFactory::PENALTY_ATTR_ID = "penalty";
const QString WevoteWorkerFactory::NUMBER_OF_AGREED_TOOLS_ATTR_ID = "number-of-agreed-tools";
const QString WevoteWorkerFactory::SCORE_THRESHOLD_ATTR_ID = "score-threshold";
const QString WevoteWorkerFactory::NUMBER_OF_THREADS_ATTR_ID = "threads";
const QString WevoteWorkerFactory::OUTPUT_FILE_ATTR_ID = "output-url";

const QString WevoteWorkerFactory::WORKFLOW_CLASSIFY_TOOL_WEVOTE = "WEVOTE";

WevoteWorkerFactory::WevoteWorkerFactory()
    : DomainFactory(ACTOR_ID)
{

}

Worker *WevoteWorkerFactory::createWorker(Actor *actor) {
    return new WevoteWorker(actor);
}

void WevoteWorkerFactory::init() {
    QList<PortDescriptor *> ports;
    {
        const Descriptor inSlotDesc(BaseSlots::URL_SLOT().getId(),
                                    WevotePrompter::tr("Input URL"),
                                    WevotePrompter::tr("Input URL."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[inSlotDesc] = BaseTypes::STRING_TYPE();

        QMap<Descriptor, DataTypePtr> outType;
        outType[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();

        Descriptor inPortDesc(INPUT_PORT_ID,
                                    WevotePrompter::tr("Input classification CSV file"),
                                    WevotePrompter::tr("Input a CSV file in the following format:\n"
                                                       "1) a sequence name\n"
                                                       "2) taxID from the first tool\n"
                                                       "3) taxID from the second tool\n"
                                                       "4) etc."));
        Descriptor outPortDesc(OUTPUT_PORT_ID, WevotePrompter::tr("WEVOTE Classification"), WevotePrompter::tr("A map of sequence names with the associated taxonomy IDs."));

        ports << new PortDescriptor(inPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);
        ports << new PortDescriptor(outPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-out", outType)), false /*input*/, true /*multi*/);
    }

    QList<Attribute *> attributes;
    {
        Descriptor penaltyDesc(PENALTY_ATTR_ID, WevotePrompter::tr("Penalty"),
                                     WevotePrompter::tr("Score penalty for disagreements (-k)"));

        Descriptor numberOfAgreedToolsDesc(NUMBER_OF_AGREED_TOOLS_ATTR_ID, WevotePrompter::tr("Number of agreed tools"),
                                                 WevotePrompter::tr("Specify the minimum number of tools agreed on WEVOTE decision (-a)."));

        Descriptor scoreThresholdDesc(SCORE_THRESHOLD_ATTR_ID, WevotePrompter::tr("Score threshold"),
                                            WevotePrompter::tr("Score threshold (-s)"));

        Descriptor numberOfThreadsDesc(NUMBER_OF_THREADS_ATTR_ID, WevotePrompter::tr("Number of threads"),
                                             WevotePrompter::tr("Use multiple threads (-n)."));

        Descriptor outputFileDesc(OUTPUT_FILE_ATTR_ID, WevotePrompter::tr("Output file"),
                                        WevotePrompter::tr("Specify the output text file name."));

        Descriptor classifyToolDesc(NgsReadsClassificationPlugin::WORKFLOW_CLASSIFY_TOOL_ID,
                                          WORKFLOW_CLASSIFY_TOOL_WEVOTE,
                                          "Classify tool. Hidden attribute");

        Attribute *penaltyAttribute = new Attribute(penaltyDesc, BaseTypes::NUM_TYPE(), Attribute::None, 2);
        Attribute *numberOfAgreedToolsAttribute = new Attribute(numberOfAgreedToolsDesc, BaseTypes::NUM_TYPE(), Attribute::None, 0);
        Attribute *scoreThresholdAttribute = new Attribute(scoreThresholdDesc, BaseTypes::NUM_TYPE(), Attribute::None, 0);
        Attribute *numberOfThreadsAttribute = new Attribute(numberOfThreadsDesc, BaseTypes::NUM_TYPE(), Attribute::None, AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
        Attribute *outputFileAttribute = new Attribute(outputFileDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);

        attributes << penaltyAttribute;
        attributes << numberOfAgreedToolsAttribute;
        attributes << scoreThresholdAttribute;
        attributes << numberOfThreadsAttribute;
        attributes << outputFileAttribute;

        attributes << new Attribute(classifyToolDesc, BaseTypes::STRING_TYPE(),
                                    static_cast<Attribute::Flags>(Attribute::Hidden),
                                    WORKFLOW_CLASSIFY_TOOL_WEVOTE);
    }

    QMap<QString, PropertyDelegate *> delegates;
    {
        QVariantMap penaltyProperties;
        penaltyProperties["minimum"] = 1;
        penaltyProperties["maximum"] = std::numeric_limits<int>::max();
        delegates[PENALTY_ATTR_ID] = new SpinBoxDelegate(penaltyProperties);

        QVariantMap numberOfAgreedToolsProperties;
        numberOfAgreedToolsProperties["minimum"] = 0;
        numberOfAgreedToolsProperties["maximum"] = std::numeric_limits<int>::max();
        delegates[NUMBER_OF_AGREED_TOOLS_ATTR_ID] = new SpinBoxDelegate(numberOfAgreedToolsProperties);

        QVariantMap scoreThresholdProperties;
        scoreThresholdProperties["minimum"] = 0;
        scoreThresholdProperties["maximum"] = std::numeric_limits<int>::max();
        delegates[SCORE_THRESHOLD_ATTR_ID] = new SpinBoxDelegate(scoreThresholdProperties);

        QVariantMap numberOfThreadsProperties;
        numberOfThreadsProperties["minimum"] = 1;
        numberOfThreadsProperties["maximum"] = QThread::idealThreadCount();
        delegates[NUMBER_OF_THREADS_ATTR_ID] = new SpinBoxDelegate(numberOfThreadsProperties);

        DelegateTags tags;
        tags.set(DelegateTags::PLACEHOLDER_TEXT, WevotePrompter::tr("Auto"));
        tags.set(DelegateTags::FILTER, DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT, true));
        delegates[OUTPUT_FILE_ATTR_ID] = new URLDelegate(tags, "wevote/output_file");
    }

    Descriptor desc(ACTOR_ID, WevotePrompter::tr("Improve Classification with WEVOTE"),
                          WevotePrompter::tr("WEVOTE (WEighted VOting Taxonomic idEntification) is a metagenome shortgun sequencing "
                                             "DNA reads classifier based on an ensemble of other classification methods (Kraken, CLARK, etc.)."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new WevotePrompter(NULL));
    proto->addExternalTool(WevoteSupport::TOOL_ID);
    proto->setValidator(new WevoteValidator());
    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new WevoteWorkerFactory());
}

void WevoteWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

}   // namespace LocalWorkflow
}   // namespace U2
