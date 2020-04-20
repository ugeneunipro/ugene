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

#include "Metaphlan2Prompter.h"
#include "Metaphlan2Support.h"
#include "Metaphlan2Validator.h"
#include "Metaphlan2Worker.h"
#include "Metaphlan2WorkerFactory.h"
#include "Metaphlan2Task.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DataPathRegistry.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/PairedReadsPortValidator.h>
#include <U2Lang/WorkflowEnv.h>

#include <../ngs_reads_classification/src/DatabaseDelegate.h>

namespace U2 {
namespace LocalWorkflow {

const QString Metaphlan2WorkerFactory::ACTOR_ID = "metaphlan2-classify";

const QString Metaphlan2WorkerFactory::INPUT_PORT_ID = "in";

// Slots should be the same as in GetReadsListWorkerFactory
const QString Metaphlan2WorkerFactory::INPUT_SLOT = "reads-url1";
const QString Metaphlan2WorkerFactory::PAIRED_INPUT_SLOT = "reads-url2";

const QString Metaphlan2WorkerFactory::SEQUENCING_READS = "input-data";
const QString Metaphlan2WorkerFactory::DB_URL = "database";
const QString Metaphlan2WorkerFactory::NUM_THREADS = "threads";
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE = "analysis-type";
const QString Metaphlan2WorkerFactory::TAX_LEVEL = "tax-level";
const QString Metaphlan2WorkerFactory::NORMALIZE = "normalize-by-size";
const QString Metaphlan2WorkerFactory::PRESENCE_THRESHOLD = "presence-threshold";
const QString Metaphlan2WorkerFactory::BOWTIE2_OUTPUT_URL = "bowtie2-output-url";
const QString Metaphlan2WorkerFactory::OUTPUT_URL = "output-url";

const QString Metaphlan2WorkerFactory::SINGLE_END_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "SE reads or contigs");
const QString Metaphlan2WorkerFactory::PAIRED_END_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "PE reads");

const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_REL_AB_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "Relative abundance");
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_REL_AB_W_READ_STATS_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "Relative abundance with reads statistics");
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_READS_MAP_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "Reads mapping");
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_CLADE_PROFILES_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "Clade profiles");
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_MARKER_AB_TABLE_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "Marker abundance table");
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_MARKER_PRES_TABLE_TEXT = QCoreApplication::translate("Metaphlan2WorkerFactory", "Marker presence table");

const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_REL_AB_VALUE = "rel-ab";
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_REL_AB_W_READ_STATS_VALUE = "rel-ab-w-read-stats";
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_READS_MAP_VALUE = "reads-map";
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_CLADE_PROFILES_VALUE = "clade-profiles";
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_MARKER_AB_TABLE_VALUE = "marker-ab-table";
const QString Metaphlan2WorkerFactory::ANALYSIS_TYPE_MARKER_PRES_TABLE_VALUE = "marker-pres-table";

const QString Metaphlan2WorkerFactory::TAX_LEVEL_ALL_TEXT = "All";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_KINGDOMS_TEXT = "Kingdoms";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_PHYLA_TEXT = "Phyla";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_CLASSES_TEXT = "Classes";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_ORDERS_TEXT = "Orders";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_FAMILIES_TEXT = "Families";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_GENERA_TEXT = "Genera";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_SPECIES_TEXT = "Species";

const QString Metaphlan2WorkerFactory::TAX_LEVEL_ALL_VALUE = "a";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_KINGDOMS_VALUE = "k";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_PHYLA_VALUE = "p";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_CLASSES_VALUE = "c";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_ORDERS_VALUE = "o";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_FAMILIES_VALUE = "f";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_GENERA_VALUE = "g";
const QString Metaphlan2WorkerFactory::TAX_LEVEL_SPECIES_VALUE = "s";

const QString Metaphlan2WorkerFactory::SKIP_NORMILIZE_BY_SIZE = "skip";
const QString Metaphlan2WorkerFactory::NOT_SKIP_NORMILIZE_BY_SIZE = "normalize";

const QString Metaphlan2WorkerFactory::SINGLE_END = "single-end";
const QString Metaphlan2WorkerFactory::PAIRED_END = "paired-end";

Metaphlan2WorkerFactory::Metaphlan2WorkerFactory() :
                         DomainFactory(ACTOR_ID) {}

Worker* Metaphlan2WorkerFactory::createWorker(Actor *actor) {
    return new Metaphlan2Worker(actor);
}

void Metaphlan2WorkerFactory::init() {
    QList<PortDescriptor *> ports;
    {
        const Descriptor inPortDescriptor(INPUT_PORT_ID,
                                          tr("Input sequences"),
                                          tr("URL(s) to FASTQ or FASTA file(s) should be provided. "
                                             "In case of SE reads or contigs use the \"Input URL 1\" slot only. "
                                             "In case of PE reads input \"left\" reads to \"Input URL 1\", "
                                             "\"right\" reads to \"Input URL 2\". "
                                             "See also the \"Input data\" parameter of the element."));

        Descriptor inSlot1Descriptor(INPUT_SLOT,
                                     tr("Input URL 1"),
                                     tr("Input URL 1."));
        Descriptor inSlot2Descriptor(PAIRED_INPUT_SLOT,
                                     tr("Input URL 2"),
                                     tr("Input URL 2."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[inSlot1Descriptor] = BaseTypes::STRING_TYPE();
        inType[inSlot2Descriptor] = BaseTypes::STRING_TYPE();
        ports << new PortDescriptor(inPortDescriptor, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true);
    }

    QList<Attribute*> attributes;
    {
        Descriptor sequencingReadsDescriptor(SEQUENCING_READS,
                                             tr("Input data"),
                                             tr("To classify single-end (SE) reads or contigs, received by reads de novo assembly, "
                                                "set this parameter to \"SE reads or contigs\".<br><br>"
                                                "To classify paired-end (PE) reads, set the value to \"PE reads\".<br><br>"
                                                "One or two slots of the input port are used depending on the value of the parameter. "
                                                "Pass URL(s) to data to these slots.<br><br>"
                                                "The input files should be in FASTA or FASTQ formats. See \"Input file format\" parameter."));


        Descriptor databaseDescriptor(DB_URL,
                                      tr("Database"),
                                      tr("A path to a folder with MetaPhlAn2 database: "
                                         "BowTie2 index files, built from reference genomes, "
                                         "and *.pkl file (--mpa-pkl, --bowtie2db).<br><br>"
                                         "By default, \"mpa_v20_m200\" database is provided (if it has been downloaded). "
                                         "The database was built on ~1M unique clade-specific marker genes identified from ~17,000 "
                                         "reference genomes (~13,500 bacterial and archaeal, ~3,500 viral, and ~110 eukaryotic)."));

        Descriptor numberOfThreadsDescriptor(NUM_THREADS,
                                             tr("Number of threads"),
                                             tr("The number of CPUs to use for parallelizing the mapping (--nproc)."));

        Descriptor analysisTypeDescriptor(ANALYSIS_TYPE,
                                          tr("Analysis type"),
                                          tr("Specify type of analysis to perform:"
                                             "<ul>"
                                             "<li>Relative abundance - profiling of metagenomes in terms of "
                                             "relative abundances (corresponds to \"-t rel_ab\")</li>"
                                             "<li>Relative abundance with reads statistics - "
                                             "profiling of metagenomes in terms of relative abundances and "
                                             "estimate the number of reads coming from each clade (\"-t rel_ab_w_read_stats\")</li>"
                                             "<li>Reads mapping - mapping from reads to clades, "
                                             "the output contains reads that hit a marker only (\"-t reads_map\")</li>"
                                             "<li>Clade profiles - normalized marker counts for clades with at least a non-null marker(\"-t clade_profiles\")</li>"
                                             "<li>Marker abundance table - normalized marker counts: "
                                             "only when > 0.0 and optionally normalized by metagenome size "
                                             "(\"-t marker_ab_table\"), see also \"Normalize by metagenome size\" parameter</li>"
                                             "<li>Marker presence table - list of markers present in the sample "
                                             "(\"-t marker_pres_table\"), see also \"Presence threshold\" parameter<li>"
                                             "</ul>"));

        Descriptor taxLevelDescriptor(TAX_LEVEL,
                                      tr("Tax level"),
                                      tr("The taxonomic level for the relative abundance output: "
                                      "all, kingdoms (Bacteria and Archaea) only, phyla only, etc. (--tax_lev)."));

        Descriptor normalizeBySizeDescriptor(NORMALIZE,
                                             tr("Normalize by metagenome size"),
                                             tr("If \"Normalize\" is selected, "
                                                 "the total number of reads in the original metagenome is taken into account for normlization: "
                                                 "UGENE calculates the number of reads in an input "
                                                 "FASTA/FASTQ file and passes \"--nreads\" parameter to MetaPhlAn2."));

        Descriptor precenceThresholdDescriptor(PRESENCE_THRESHOLD,
                                               tr("Presence threshold"),
                                               tr("Specify a threshold for calling a marker."));

        Descriptor bowtie2OutputDescriptor(BOWTIE2_OUTPUT_URL,
                                           tr("Bowtie2 output file"),
                                           tr("The file for saving the output of BowTie2 (--bowtie2out). "
                                                                  "In case of PE reads one file is created per each pair of files."));
        Descriptor outputUrlDescriptor(OUTPUT_URL,
                                       tr("Output file"),
                                       tr("MetaPhlAn2 output depends on the \"Analysis type\" parameter. "
                                          "By default, it is a tab-delimited file with the predicted taxon relative abundances."));

        Attribute* sequencingReadsAttribute = new Attribute(sequencingReadsDescriptor,
                                                            BaseTypes::STRING_TYPE(),
                                                            Attribute::None,
                                                            SINGLE_END);
        sequencingReadsAttribute->addSlotRelation(new SlotRelationDescriptor(INPUT_PORT_ID,
                                                                             PAIRED_INPUT_SLOT,
                                                                             QVariantList() << PAIRED_END));

        QString databasePath;
        U2DataPath* metaphlan2DataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::METAPHLAN2_DATABASE_DATA_ID);
        if (NULL != metaphlan2DataPath && metaphlan2DataPath->isValid()) {
            databasePath = metaphlan2DataPath->getPathByName(NgsReadsClassificationPlugin::METAPHLAN2_DATABASE_ITEM_ID);
        }
        Attribute* databaseAttribute = new Attribute(databaseDescriptor,
                                                     BaseTypes::STRING_TYPE(),
                                                     Attribute::Required | Attribute::NeedValidateEncoding,
                                                     databasePath);

        Attribute* numberOfThreadsAttribute = new Attribute(numberOfThreadsDescriptor,
                                                            BaseTypes::NUM_TYPE(),
                                                            Attribute::None,
                                                            AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());

        Attribute* analysisTypeAttribute = new Attribute(analysisTypeDescriptor,
                                                         BaseTypes::STRING_TYPE(),
                                                         Attribute::None,
                                                         ANALYSIS_TYPE_REL_AB_VALUE);

        Attribute* taxLevelAttribute = new Attribute(taxLevelDescriptor,
                                                     BaseTypes::STRING_TYPE(),
                                                     Attribute::None,
                                                     TAX_LEVEL_ALL_VALUE);

        Attribute* normalizeBySizeAttribute = new Attribute(normalizeBySizeDescriptor,
                                                            BaseTypes::STRING_TYPE(),
                                                            Attribute::None,
                                                            SKIP_NORMILIZE_BY_SIZE);

        Attribute* precenceThresholdAttribute = new Attribute(precenceThresholdDescriptor,
                                                              BaseTypes::NUM_TYPE(),
                                                              Attribute::None,
                                                              1);


        Attribute* bowtie2OutputAttribute = new Attribute(bowtie2OutputDescriptor,
                                                          BaseTypes::STRING_TYPE(),
                                                          Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);

        Attribute* outputUrlAttribute = new Attribute(outputUrlDescriptor,
                                                      BaseTypes::STRING_TYPE(),
                                                      Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);

        taxLevelAttribute->addRelation(new VisibilityRelation(ANALYSIS_TYPE, QVariantList() << ANALYSIS_TYPE_REL_AB_VALUE << ANALYSIS_TYPE_REL_AB_W_READ_STATS_VALUE));
        normalizeBySizeAttribute->addRelation(new VisibilityRelation(ANALYSIS_TYPE, QVariantList() << ANALYSIS_TYPE_MARKER_AB_TABLE_VALUE));
        precenceThresholdAttribute->addRelation(new VisibilityRelation(ANALYSIS_TYPE, QVariantList() << ANALYSIS_TYPE_MARKER_PRES_TABLE_VALUE));

        attributes << sequencingReadsAttribute;
        attributes << databaseAttribute;
        attributes << numberOfThreadsAttribute;
        attributes << analysisTypeAttribute;
        attributes << taxLevelAttribute;
        attributes << normalizeBySizeAttribute;
        attributes << precenceThresholdAttribute;
        attributes << bowtie2OutputAttribute;
        attributes << outputUrlAttribute;
        }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap sequencingReadsMap;
        sequencingReadsMap[SINGLE_END_TEXT] = SINGLE_END;
        sequencingReadsMap[PAIRED_END_TEXT] = PAIRED_END;
        delegates[SEQUENCING_READS] = new ComboBoxDelegate(sequencingReadsMap);

        QList<StrStrPair> databaseMap;
        databaseMap << StrStrPair(NgsReadsClassificationPlugin::METAPHLAN2_DATABASE_DATA_ID,
                                  NgsReadsClassificationPlugin::METAPHLAN2_DATABASE_ITEM_ID);
        delegates[DB_URL] = new DatabaseDelegate(ACTOR_ID, DB_URL, databaseMap, "metaphlan2/database", true);

        QVariantMap threadsProperties;
        threadsProperties["minimum"] = 1;
        threadsProperties["maximum"] = QThread::idealThreadCount();
        delegates[NUM_THREADS] = new SpinBoxDelegate(threadsProperties);

        QVariantMap analysisTypeMap;
        analysisTypeMap[ANALYSIS_TYPE_REL_AB_TEXT] = ANALYSIS_TYPE_REL_AB_VALUE;
        analysisTypeMap[ANALYSIS_TYPE_REL_AB_W_READ_STATS_TEXT] = ANALYSIS_TYPE_REL_AB_W_READ_STATS_VALUE;
        analysisTypeMap[ANALYSIS_TYPE_READS_MAP_TEXT] = ANALYSIS_TYPE_READS_MAP_VALUE;
        analysisTypeMap[ANALYSIS_TYPE_CLADE_PROFILES_TEXT] = ANALYSIS_TYPE_CLADE_PROFILES_VALUE;
        analysisTypeMap[ANALYSIS_TYPE_MARKER_AB_TABLE_TEXT] = ANALYSIS_TYPE_MARKER_AB_TABLE_VALUE;
        analysisTypeMap[ANALYSIS_TYPE_MARKER_PRES_TABLE_TEXT] = ANALYSIS_TYPE_MARKER_PRES_TABLE_VALUE;
        delegates[ANALYSIS_TYPE] = new ComboBoxDelegate(analysisTypeMap);

        QVariantMap taxLevelMap;
        taxLevelMap[TAX_LEVEL_ALL_TEXT] = TAX_LEVEL_ALL_VALUE;
        taxLevelMap[TAX_LEVEL_KINGDOMS_TEXT] = TAX_LEVEL_KINGDOMS_VALUE;
        taxLevelMap[TAX_LEVEL_PHYLA_TEXT] = TAX_LEVEL_PHYLA_VALUE;
        taxLevelMap[TAX_LEVEL_CLASSES_TEXT] = TAX_LEVEL_CLASSES_VALUE;
        taxLevelMap[TAX_LEVEL_ORDERS_TEXT] = TAX_LEVEL_ORDERS_VALUE;
        taxLevelMap[TAX_LEVEL_FAMILIES_TEXT] = TAX_LEVEL_FAMILIES_VALUE;
        taxLevelMap[TAX_LEVEL_GENERA_TEXT] = TAX_LEVEL_GENERA_VALUE;
        taxLevelMap[TAX_LEVEL_SPECIES_TEXT] = TAX_LEVEL_SPECIES_VALUE;
        delegates[TAX_LEVEL] = new ComboBoxDelegate(taxLevelMap);

        QVariantMap normalizeBySizeMap;
        normalizeBySizeMap[tr("Skip")] = SKIP_NORMILIZE_BY_SIZE;
        normalizeBySizeMap[tr("Normalize")] = NOT_SKIP_NORMILIZE_BY_SIZE;
        delegates[NORMALIZE] = new ComboBoxDelegate(normalizeBySizeMap);

        QVariantMap precenceThresholdMap;
        precenceThresholdMap["minimum"] = 0;
        precenceThresholdMap["maximum"] = std::numeric_limits<int>::max();
        delegates[PRESENCE_THRESHOLD] = new SpinBoxDelegate(precenceThresholdMap);

        DelegateTags bowtie2OutputTags;
        bowtie2OutputTags.set(DelegateTags::PLACEHOLDER_TEXT,
                              tr("Auto"));
        bowtie2OutputTags.set(DelegateTags::FILTER,
                              DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT,
                                                                      true,
                                                                      QStringList()));
        bowtie2OutputTags.set(DelegateTags::FORMAT,
                              BaseDocumentFormats::PLAIN_TEXT);
        delegates[BOWTIE2_OUTPUT_URL] = new URLDelegate(bowtie2OutputTags, "metaphlan2/bowtie2Output");

        DelegateTags outputUrlTags;
        outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT,
                          tr("Auto"));
        outputUrlTags.set(DelegateTags::FILTER,
                          DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT,
                                                                  true,
                                                                  QStringList()));
        outputUrlTags.set(DelegateTags::FORMAT,
                          BaseDocumentFormats::PLAIN_TEXT);
        delegates[OUTPUT_URL] = new URLDelegate(outputUrlTags, "metaphlan2/output");
    }

    const Descriptor actorDescription(ACTOR_ID,
        tr("Classify Sequences with MetaPhlAn2"),
        tr("MetaPhlAn2 (METAgenomic PHyLogenetic ANalysis) is a tool "
                               "for profiling the composition of microbial communities "
                               "(bacteria, archaea, eukaryotes, and viruses) from "
                               "whole-metagenome shotgun sequencing data.<br><br>"
                               "The tool relies on ~1M unique clade - specific marker genes "
                               "identified from ~17,000 reference genomes "
                               "(~13,500 bacterial and archaeal, ~3,500 viral, and ~110 eukaryotic)"));
    ActorPrototype *proto = new IntegralBusActorPrototype(actorDescription,
                                                          ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new Metaphlan2Prompter(nullptr));
    proto->addExternalTool(Metaphlan2Support::TOOL_ID);
    proto->setPortValidator(INPUT_PORT_ID, new PairedReadsPortValidator(INPUT_SLOT, PAIRED_INPUT_SLOT));
    proto->setValidator(new Metaphlan2Validator());
    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new Metaphlan2WorkerFactory());
}

void Metaphlan2WorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

}
}
