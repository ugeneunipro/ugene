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

#include <QThread>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/PairedReadsPortValidator.h>
#include <U2Lang/WorkflowEnv.h>

#include "TrimmomaticDelegate.h"
#include "TrimmomaticPrompter.h"
#include "TrimmomaticSupport.h"
#include "TrimmomaticWorker.h"
#include "TrimmomaticWorkerFactory.h"

#include "java/JavaSupport.h"

namespace U2 {
namespace LocalWorkflow {

const QString TrimmomaticWorkerFactory::ACTOR_ID = "trimmomatic";

const QString TrimmomaticWorkerFactory::INPUT_PORT_ID = "in";
const QString TrimmomaticWorkerFactory::OUTPUT_PORT_ID = "out";

// Slots should be the same as in GetReadsListWorkerFactory
const QString TrimmomaticWorkerFactory::INPUT_SLOT = "reads-url1";
const QString TrimmomaticWorkerFactory::PAIRED_INPUT_SLOT = "reads-url2";

const QString TrimmomaticWorkerFactory::OUT_SLOT = "reads-url1";
const QString TrimmomaticWorkerFactory::PAIRED_OUT_SLOT = "reads-url2";

const QString TrimmomaticWorkerFactory::INPUT_DATA_ATTR_ID = "input-data";
const QString TrimmomaticWorkerFactory::TRIMMING_STEPS_ATTR_ID = "trimming-steps";
const QString TrimmomaticWorkerFactory::OUTPUT_URL_ATTR_ID = "output-url";
const QString TrimmomaticWorkerFactory::PAIRED_URL_1_ATTR_ID = "paired-url-1";
const QString TrimmomaticWorkerFactory::PAIRED_URL_2_ATTR_ID = "paired-url-2";
const QString TrimmomaticWorkerFactory::UNPAIRED_URL_1_ATTR_ID = "unpaired-url-1";
const QString TrimmomaticWorkerFactory::UNPAIRED_URL_2_ATTR_ID = "unpaired-url-2";
const QString TrimmomaticWorkerFactory::GENERATE_LOG_ATTR_ID = "generate-log";
const QString TrimmomaticWorkerFactory::LOG_URL_ATTR_ID = "log-url";
const QString TrimmomaticWorkerFactory::THREADS_NUMBER_ATTR_ID = "threads";

const QString TrimmomaticWorkerFactory::SINGLE_END_TEXT = QObject::tr("SE reads");
const QString TrimmomaticWorkerFactory::PAIRED_END_TEXT = QObject::tr("PE reads");

TrimmomaticWorkerFactory::TrimmomaticWorkerFactory()
    : DomainFactory(ACTOR_ID)
{
}

Worker *TrimmomaticWorkerFactory::createWorker(Actor *actor) {
    return new TrimmomaticWorker(actor);
}

void TrimmomaticWorkerFactory::init() {
    QList<PortDescriptor *> ports;
    {
        // Input
        const Descriptor inSlot1Desc(INPUT_SLOT,
                             TrimmomaticPrompter::tr("Input FASTQ URL 1"),
                             TrimmomaticPrompter::tr("URL to a FASTQ file with SE reads or \"left\" PE reads."));

        const Descriptor inSlot2Desc(PAIRED_INPUT_SLOT,
                             TrimmomaticPrompter::tr("Input FASTQ URL 2"),
                             TrimmomaticPrompter::tr("URL to a FASTQ file with \"right\" PE reads."));

        const Descriptor inPortDesc(INPUT_PORT_ID,
                             TrimmomaticPrompter::tr("Input FASTQ file(s)"),
                             TrimmomaticPrompter::tr("URL(s) to FASTQ file(s) with Illumina reads should be provided."
                                                     "<br/><br/>In case of SE reads use the \"Input FASTQ URL 1\" slot only."
                                                     "<br/><br/>In case of PE reads input \"left\" reads to \"Input FASTQ URL 1\","
                                                     " \"right\" reads to \"Input FASTQ URL 2\"."
                                                     "<br/><br/>See also the \"Input data\" parameter of the element."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[inSlot1Desc] = BaseTypes::STRING_TYPE();
        inType[inSlot2Desc] = BaseTypes::STRING_TYPE();

        ports << new PortDescriptor(inPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);

        // Output
        const Descriptor outSlot1Desc(OUT_SLOT,
                             TrimmomaticPrompter::tr("Output FASTQ URL 1"),
                             TrimmomaticPrompter::tr("URL to a FASTQ file with SE reads or \"left\" PE reads, processed by Trimmomatic."));

        const Descriptor outSlot2Desc(PAIRED_OUT_SLOT,
                             TrimmomaticPrompter::tr("Output FASTQ URL 2"),
                             TrimmomaticPrompter::tr("URL to a FASTQ file with \"right\" reads, processed by Trimmomatic."));

        const Descriptor outPortDesc(OUTPUT_PORT_ID,
                             TrimmomaticPrompter::tr("Improved FASTQ file(s)"),
                             TrimmomaticPrompter::tr("The port outputs URLs to FASTQ files, produced by Trimmomatic."
                                                     "<br/><br/>In case of SE reads for each input FASTQ file one output file is produced."
                                                     " The file URL is passed to the output slot \"Output FASTQ URL 1\"."
                                                     "<br/><br/>In case of PE reads for each pair of input FASTQ files four output files are"
                                                     " produced: for paired \"left\" reads, for unpaired \"left\" reads, for paired \"right\" reads,"
                                                     " and for unpaired \"right\" reads. URLs of files with paired reads are passed to the output"
                                                     " slots \"Output FASTQ URL 1\" and \"Output FASTQ URL 2\"."));

        QMap<Descriptor, DataTypePtr> outType;
        outType[outSlot1Desc] = BaseTypes::STRING_TYPE();
        outType[outSlot2Desc] = BaseTypes::STRING_TYPE();

        ports << new PortDescriptor(outPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-out", outType)), false /*input*/, true /*multi*/);
    }

    QList<Attribute *> attributes;
    {
        const Descriptor inputDataDesc(INPUT_DATA_ATTR_ID,
                             TrimmomaticPrompter::tr("Input data"),
                             TrimmomaticPrompter::tr("Set the type of the input reads: single-end (SE) or paired-end (PE)."
                                                     "<br/><br/>One or two slots of the input port are used depending on the value"
                                                     " of the parameter. Pass URL(s) to data to these slots."
                                                     "<br/><br/>Note that the paired-end mode will use additional information"
                                                     " contained in paired reads to better find adapter or PCR primer fragments"
                                                     " introduced by the library preparation process."));

        const Descriptor trimmingStepsDesc(TRIMMING_STEPS_ATTR_ID,
                             TrimmomaticPrompter::tr("Trimming steps"),
                             TrimmomaticPrompter::tr("Configure trimming steps that should be performed by Trimmomatic."));

        const Descriptor seOutputUrlDesc(OUTPUT_URL_ATTR_ID,
                             TrimmomaticPrompter::tr("Output file"),
                             TrimmomaticPrompter::tr("Specify the output file name."));

        const Descriptor pairedOutputUrl1Desc(PAIRED_URL_1_ATTR_ID,
                             TrimmomaticPrompter::tr("Paired output file 1"),
                             TrimmomaticPrompter::tr("Specify the output file name for \"left\" reads that have paired \"right\" reads."));

        const Descriptor pairedOutputUrl2Desc(PAIRED_URL_2_ATTR_ID,
                             TrimmomaticPrompter::tr("Paired output file 2"),
                             TrimmomaticPrompter::tr("Specify the output file name for unpaired \"left\" reads."));

        const Descriptor unpairedOutputUrl1Desc(UNPAIRED_URL_1_ATTR_ID,
                             TrimmomaticPrompter::tr("Unpaired output file 1"),
                             TrimmomaticPrompter::tr("Specify the output file name for \"left\" reads that have no pair."));

        const Descriptor unpairedOutputUrl2Desc(UNPAIRED_URL_2_ATTR_ID,
                             TrimmomaticPrompter::tr("Unpaired output file 2"),
                             TrimmomaticPrompter::tr("Specify the output file name for \"right\" reads that have no pair."));

        const Descriptor generateLogDesc(GENERATE_LOG_ATTR_ID,
                             TrimmomaticPrompter::tr("Generate detailed log"),
                             TrimmomaticPrompter::tr("Select \"True\" to generate a file with log of all read trimmings,"
                                                     " indicating the following details (-trimlog):"
                                                     " <ul>"
                                                     "   <li>the read name</li>"
                                                     "   <li>the surviving sequence length</li>"
                                                     "   <li>the location of the first surviving base, aka. the amount trimmed from the start</li>"
                                                     "   <li>the location of the last surviving base in the original read</li>"
                                                     "   <li>the amount trimmed from the end</li>"
                                                     " </ul>"));

        const Descriptor logUrlDesc(LOG_URL_ATTR_ID,
                             TrimmomaticPrompter::tr("Log file"),
                             TrimmomaticPrompter::tr("Specify a text file to keep detailed information about reads trimming."));

        const Descriptor threadsDesc(THREADS_NUMBER_ATTR_ID,
                             TrimmomaticPrompter::tr("Number of threads"),
                             TrimmomaticPrompter::tr("Use multiple threads (-threads)."));

        Attribute *inputDataAttribute = new Attribute(inputDataDesc, BaseTypes::STRING_TYPE(), false, TrimmomaticTaskSettings::SINGLE_END);
        Attribute *trimmingStepsAttribute = new Attribute(trimmingStepsDesc, BaseTypes::STRING_LIST_TYPE(), Attribute::Required);
        Attribute *seOutputUrlAttribute = new Attribute(seOutputUrlDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::CanBeEmpty);
        Attribute *pairedOutputUrl1Attribute = new Attribute(pairedOutputUrl1Desc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::CanBeEmpty);
        Attribute *pairedOutputUrl2Attribute = new Attribute(pairedOutputUrl2Desc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::CanBeEmpty);
        Attribute *unpairedOutputUrl1Attribute = new Attribute(unpairedOutputUrl1Desc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::CanBeEmpty);
        Attribute *unpairedOutputUrl2Attribute = new Attribute(unpairedOutputUrl2Desc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::CanBeEmpty);
        Attribute *generateLogAttribute = new Attribute(generateLogDesc, BaseTypes::BOOL_TYPE(), Attribute::None, false);
        Attribute *logUrlAttribute = new Attribute(logUrlDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::CanBeEmpty);
        Attribute *threadsAttribute = new Attribute(threadsDesc, BaseTypes::NUM_TYPE(), Attribute::None, AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());

        seOutputUrlAttribute->addRelation(new VisibilityRelation(INPUT_DATA_ATTR_ID, TrimmomaticTaskSettings::SINGLE_END));
        pairedOutputUrl1Attribute->addRelation(new VisibilityRelation(INPUT_DATA_ATTR_ID, TrimmomaticTaskSettings::PAIRED_END));
        pairedOutputUrl2Attribute->addRelation(new VisibilityRelation(INPUT_DATA_ATTR_ID, TrimmomaticTaskSettings::PAIRED_END));
        unpairedOutputUrl1Attribute->addRelation(new VisibilityRelation(INPUT_DATA_ATTR_ID, TrimmomaticTaskSettings::PAIRED_END));
        unpairedOutputUrl2Attribute->addRelation(new VisibilityRelation(INPUT_DATA_ATTR_ID, TrimmomaticTaskSettings::PAIRED_END));
        logUrlAttribute->addRelation(new VisibilityRelation(GENERATE_LOG_ATTR_ID, true));

        inputDataAttribute->addSlotRelation(SlotRelationDescriptor(INPUT_PORT_ID, PAIRED_INPUT_SLOT, QVariantList() << TrimmomaticTaskSettings::PAIRED_END));
        inputDataAttribute->addSlotRelation(SlotRelationDescriptor(OUTPUT_PORT_ID, PAIRED_OUT_SLOT, QVariantList() << TrimmomaticTaskSettings::PAIRED_END));

        attributes << inputDataAttribute;
        attributes << trimmingStepsAttribute;
        attributes << seOutputUrlAttribute;
        attributes << pairedOutputUrl1Attribute;
        attributes << pairedOutputUrl2Attribute;
        attributes << unpairedOutputUrl1Attribute;
        attributes << unpairedOutputUrl2Attribute;
        attributes << generateLogAttribute;
        attributes << logUrlAttribute;
        attributes << threadsAttribute;
    }

    QMap<QString, PropertyDelegate *> delegates;
    {
        QVariantMap inputDataMap;
        inputDataMap[SINGLE_END_TEXT] = TrimmomaticTaskSettings::SINGLE_END;
        inputDataMap[PAIRED_END_TEXT] = TrimmomaticTaskSettings::PAIRED_END;
        delegates[INPUT_DATA_ATTR_ID] = new ComboBoxDelegate(inputDataMap);

        {
            DelegateTags outputUrlTags;
            outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
            outputUrlTags.set(DelegateTags::FILTER, DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTQ, true, QStringList()));
            outputUrlTags.set(DelegateTags::FORMAT, BaseDocumentFormats::FASTQ);
            delegates[OUTPUT_URL_ATTR_ID] = new URLDelegate(outputUrlTags, "trimmomatic/output");
            delegates[PAIRED_URL_1_ATTR_ID] = new URLDelegate(outputUrlTags, "trimmomatic/output");
            delegates[PAIRED_URL_2_ATTR_ID] = new URLDelegate(outputUrlTags, "trimmomatic/output");
            delegates[UNPAIRED_URL_1_ATTR_ID] = new URLDelegate(outputUrlTags, "trimmomatic/output");
            delegates[UNPAIRED_URL_2_ATTR_ID] = new URLDelegate(outputUrlTags, "trimmomatic/output");
            delegates[TRIMMING_STEPS_ATTR_ID] = new TrimmomaticDelegate();
        }

        delegates[GENERATE_LOG_ATTR_ID] = new ComboBoxWithBoolsDelegate();

        {
            DelegateTags outputUrlTags;
            outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
            outputUrlTags.set(DelegateTags::FILTER, DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT, true, QStringList()));
            outputUrlTags.set(DelegateTags::FORMAT, BaseDocumentFormats::PLAIN_TEXT);
            delegates[LOG_URL_ATTR_ID] = new URLDelegate(outputUrlTags, "trimmomatic/output");
        }

        QVariantMap threadsProperties;
        threadsProperties["minimum"] = 1;
        threadsProperties["maximum"] = QThread::idealThreadCount();
        delegates[THREADS_NUMBER_ATTR_ID] = new SpinBoxDelegate(threadsProperties);
    }

    const Descriptor desc(ACTOR_ID,
                             TrimmomaticPrompter::tr("Improve Reads with Trimmomatic"),
                             TrimmomaticPrompter::tr("Trimmomatic is a fast, multithreaded command line tool that can be used"
                                                  " to trim and crop Illumina (FASTQ) data as well as to remove adapters."));

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new TrimmomaticPrompter(NULL));
    proto->addExternalTool(ET_JAVA);
    proto->addExternalTool(ET_TRIMMOMATIC);
    proto->setPortValidator(INPUT_PORT_ID, new PairedReadsPortValidator(INPUT_SLOT, PAIRED_INPUT_SLOT));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new TrimmomaticWorkerFactory());
}

void TrimmomaticWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

} // namespace LocalWorkflow
} // namespace U2
