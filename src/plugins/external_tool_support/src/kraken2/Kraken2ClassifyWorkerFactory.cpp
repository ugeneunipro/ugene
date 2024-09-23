/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Kraken2ClassifyWorkerFactory.h"

#include <QThread>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/FileFilters.h>
#include <U2Core/L10n.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/PairedReadsPortValidator.h>
#include <U2Lang/WorkflowEnv.h>

#include "Kraken2ClassifyPrompter.h"
#include "Kraken2ClassifyValidator.h"
#include "Kraken2ClassifyWorker.h"
#include "Kraken2Support.h"

namespace U2 {
namespace LocalWorkflow {

const QString Kraken2ClassifyWorkerFactory::ACTOR_ID = "kraken-classify";

const QString Kraken2ClassifyWorkerFactory::INPUT_PORT_ID = "in";
const QString Kraken2ClassifyWorkerFactory::OUTPUT_PORT_ID = "out";

// Slots should be the same as in GetReadsListWorkerFactory
const QString Kraken2ClassifyWorkerFactory::INPUT_SLOT = "reads-url1";
const QString Kraken2ClassifyWorkerFactory::PAIRED_INPUT_SLOT = "reads-url2";

const QString Kraken2ClassifyWorkerFactory::INPUT_DATA_ATTR_ID = "input-data";
const QString Kraken2ClassifyWorkerFactory::DATABASE_ATTR_ID = "database";
const QString Kraken2ClassifyWorkerFactory::OUTPUT_URL_ATTR_ID = "output-url";
const QString Kraken2ClassifyWorkerFactory::QUICK_OPERATION_ATTR_ID = "quick-operation";
const QString Kraken2ClassifyWorkerFactory::MIN_HITS_NUMBER_ATTR_ID = "min-hits";
const QString Kraken2ClassifyWorkerFactory::THREADS_NUMBER_ATTR_ID = "threads";
const QString Kraken2ClassifyWorkerFactory::PRELOAD_DATABASE_ATTR_ID = "preload";

const QString Kraken2ClassifyWorkerFactory::SINGLE_END_TEXT = "SE reads or contigs";
const QString Kraken2ClassifyWorkerFactory::PAIRED_END_TEXT = "PE reads";

const QString Kraken2ClassifyWorkerFactory::WORKFLOW_CLASSIFY_TOOL_KRAKEN = "Kraken 2";

Kraken2ClassifyWorkerFactory::Kraken2ClassifyWorkerFactory()
    : DomainFactory(ACTOR_ID) {
}

Worker *Kraken2ClassifyWorkerFactory::createWorker(Actor *actor) {
    return new Kraken2ClassifyWorker(actor);
}

void Kraken2ClassifyWorkerFactory::init() {
    QList<PortDescriptor *> ports;
    {
        const Descriptor inSlotDesc(INPUT_SLOT,
                                    Kraken2ClassifyPrompter::tr("Input URL 1"),
                                    Kraken2ClassifyPrompter::tr("Input URL 1."));

        const Descriptor inPairedSlotDesc(PAIRED_INPUT_SLOT,
                                          Kraken2ClassifyPrompter::tr("Input URL 2"),
                                          Kraken2ClassifyPrompter::tr("Input URL 2."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[inSlotDesc] = BaseTypes::STRING_TYPE();
        inType[inPairedSlotDesc] = BaseTypes::STRING_TYPE();

        QMap<Descriptor, DataTypePtr> outType;
        outType[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();

        const Descriptor inPortDesc(INPUT_PORT_ID,
                                    Kraken2ClassifyPrompter::tr("Input sequences"),
                                    Kraken2ClassifyPrompter::tr("URL(s) to FASTQ or FASTA file(s) should be provided.\n\n"
                                                               "In case of SE reads or contigs use the \"Input URL 1\" slot only.\n\n"
                                                               "In case of PE reads input \"left\" reads to \"Input URL 1\", \"right\" reads to \"Input URL 2\".\n\n"
                                                               "See also the \"Input data\" parameter of the element."));
        Descriptor outPortDesc(OUTPUT_PORT_ID, Kraken2ClassifyPrompter::tr("Kraken Classification"), Kraken2ClassifyPrompter::tr("A map of sequence names with the associated taxonomy IDs, classified by Kraken."));

        ports << new PortDescriptor(inPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);
        ports << new PortDescriptor(outPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-out", outType)), false /*input*/, true /*multi*/);
    }

    QList<Attribute *> attributes;
    {
        Descriptor inputDataDesc(INPUT_DATA_ATTR_ID, Kraken2ClassifyPrompter::tr("Input data"), Kraken2ClassifyPrompter::tr("To classify single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to \"SE reads or contigs\".<br><br>"
                                                                                                                          "To classify paired-end (PE) reads, set the value to \"PE reads\".<br><br>"
                                                                                                                          "One or two slots of the input port are used depending on the value of the parameter. Pass URL(s) to data to these slots.<br><br>"
                                                                                                                          "The input files should be in FASTA or FASTQ formats."));

        Descriptor databaseDesc(DATABASE_ATTR_ID, Kraken2ClassifyPrompter::tr("Database"), Kraken2ClassifyPrompter::tr("A path to the folder with the Kraken database files."));

        Descriptor outputUrlDesc(OUTPUT_URL_ATTR_ID, Kraken2ClassifyPrompter::tr("Output file"), Kraken2ClassifyPrompter::tr("Specify the output file name."));

        Descriptor quickOperationDesc(QUICK_OPERATION_ATTR_ID, Kraken2ClassifyPrompter::tr("Quick operation"), Kraken2ClassifyPrompter::tr("Stop classification of an input read after the certain number of hits.<br><br>"
                                                                                                                                         "The value can be specified in the \"Minimum number of hits\" parameter."));

        Descriptor minHitsDesc(MIN_HITS_NUMBER_ATTR_ID, Kraken2ClassifyPrompter::tr("Minimum number of hits"), Kraken2ClassifyPrompter::tr("The number of hits that are required to declare an input sequence classified.<br><br>"
                                                                                                                                         "This can be especially useful with custom databases when testing to see if sequences either do or do not belong to a particular genome."));

        Descriptor threadsDesc(THREADS_NUMBER_ATTR_ID, Kraken2ClassifyPrompter::tr("Number of threads"), Kraken2ClassifyPrompter::tr("Use multiple threads (--threads)."));

        Descriptor preloadDatabaseDesc(PRELOAD_DATABASE_ATTR_ID, Kraken2ClassifyPrompter::tr("Load database into memory"), Kraken2ClassifyPrompter::tr("Load the Kraken database into RAM (--preload).<br><br>"
                                                                                                                                                     "This can be useful to improve the speed. The database size should be less than the RAM size.<br><br>"
                                                                                                                                                     "The other option to improve the speed is to store the database on ramdisk. Set this parameter to \"False\" in this case."));

        Attribute *inputDataAttribute = new Attribute(inputDataDesc, BaseTypes::STRING_TYPE(), false, Kraken2ClassifyTaskSettings::SINGLE_END);
        inputDataAttribute->addSlotRelation(new SlotRelationDescriptor(INPUT_PORT_ID, PAIRED_INPUT_SLOT, QVariantList() << Kraken2ClassifyTaskSettings::PAIRED_END));
        attributes << inputDataAttribute;

        

        attributes << new Attribute(quickOperationDesc, BaseTypes::BOOL_TYPE(), Attribute::None, false);

        Attribute *minHitsAttribute = new Attribute(minHitsDesc, BaseTypes::NUM_TYPE(), Attribute::None, 1);
        attributes << minHitsAttribute;

        attributes << new Attribute(preloadDatabaseDesc, BaseTypes::BOOL_TYPE(), Attribute::None, true);
        attributes << new Attribute(threadsDesc, BaseTypes::NUM_TYPE(), Attribute::None, AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
        attributes << new Attribute(outputUrlDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);

        minHitsAttribute->addRelation(new VisibilityRelation(QUICK_OPERATION_ATTR_ID, "true"));
    }

    QMap<QString, PropertyDelegate *> delegates;
    {
        QVariantMap inputDataMap;
        inputDataMap[SINGLE_END_TEXT] = Kraken2ClassifyTaskSettings::SINGLE_END;
        inputDataMap[PAIRED_END_TEXT] = Kraken2ClassifyTaskSettings::PAIRED_END;
        delegates[INPUT_DATA_ATTR_ID] = new ComboBoxDelegate(inputDataMap);

        DelegateTags outputUrlTags;
        outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
        outputUrlTags.set(DelegateTags::FILTER, FileFilters::createFileFilterByObjectTypes({BaseDocumentFormats::PLAIN_TEXT}, true));
        outputUrlTags.set(DelegateTags::FORMAT, BaseDocumentFormats::PLAIN_TEXT);
        delegates[OUTPUT_URL_ATTR_ID] = new URLDelegate(outputUrlTags, "kraken/output");

        delegates[QUICK_OPERATION_ATTR_ID] = new ComboBoxWithBoolsDelegate();

        QVariantMap threadsProperties;
        threadsProperties["minimum"] = 1;
        threadsProperties["maximum"] = QThread::idealThreadCount();
        delegates[THREADS_NUMBER_ATTR_ID] = new SpinBoxDelegate(threadsProperties);

        delegates[PRELOAD_DATABASE_ATTR_ID] = new ComboBoxWithBoolsDelegate();
    }

    Descriptor desc(ACTOR_ID, Kraken2ClassifyPrompter::tr("Classify Sequences with Kraken"), Kraken2ClassifyPrompter::tr("Kraken is a taxonomic sequence classifier that assigns taxonomic labels to short DNA reads. "
                                                                                                                       "It does this by examining the k-mers within a read and querying a database with those."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new Kraken2ClassifyPrompter(nullptr));
    proto->addExternalTool(Kraken2Support::CLASSIFY_TOOL_ID);
    proto->setValidator(new Kraken2ClassifyValidator());
    proto->setPortValidator(INPUT_PORT_ID, new PairedReadsPortValidator(INPUT_SLOT, PAIRED_INPUT_SLOT));
    WorkflowEnv::getProtoRegistry()->registerProto(Kraken2ClassifyPrompter::tr("NGS: Metagenomics Classification"), proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new Kraken2ClassifyWorkerFactory());
}

void Kraken2ClassifyWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

}  // namespace LocalWorkflow
}  // namespace U2
