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
#include <U2Core/DataPathRegistry.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "DiamondClassifyPrompter.h"
#include "DiamondClassifyTask.h"
#include "DiamondClassifyWorker.h"
#include "DiamondClassifyWorkerFactory.h"
#include "DiamondSupport.h"
#include "../ngs_reads_classification/src/DatabaseDelegate.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

const QString DiamondClassifyWorkerFactory::ACTOR_ID = "diamond-classify";

const QString DiamondClassifyWorkerFactory::INPUT_PORT_ID = "in";
const QString DiamondClassifyWorkerFactory::OUTPUT_PORT_ID = "out";

// Slot should be the same as in GetReadsListWorkerFactory
const QString DiamondClassifyWorkerFactory::INPUT_SLOT = "reads-url1";

const QString DiamondClassifyWorkerFactory::INPUT_DATA_ATTR_ID("input-data");
const QString DiamondClassifyWorkerFactory::DATABASE_ATTR_ID("database");
const QString DiamondClassifyWorkerFactory::GENCODE_ATTR_ID("genetic-code");
const QString DiamondClassifyWorkerFactory::SENSITIVE_ATTR_ID("sensitive-mode");
const QString DiamondClassifyWorkerFactory::TOP_ALIGNMENTS_PERCENTAGE_ATTR_ID("top-alignments-percentage");
const QString DiamondClassifyWorkerFactory::FSHIFT_ATTR_ID("frame-shift");
const QString DiamondClassifyWorkerFactory::EVALUE_ATTR_ID("e-value");
const QString DiamondClassifyWorkerFactory::MATRIX_ATTR_ID("matrix");
const QString DiamondClassifyWorkerFactory::GO_PEN_ATTR_ID("gap-open");
const QString DiamondClassifyWorkerFactory::GE_PEN_ATTR_ID("gap-extend");
const QString DiamondClassifyWorkerFactory::THREADS_ATTR_ID("threads");
const QString DiamondClassifyWorkerFactory::BSIZE_ATTR_ID("block-size");
const QString DiamondClassifyWorkerFactory::CHUNKS_ATTR_ID("index-chunks");
const QString DiamondClassifyWorkerFactory::OUTPUT_URL_ATTR_ID("output-url");

const QString DiamondClassifyWorkerFactory::WORKFLOW_CLASSIFY_TOOL_DIAMOND = "DIAMOND";

DiamondClassifyWorkerFactory::DiamondClassifyWorkerFactory()
    : DomainFactory(ACTOR_ID)
{

}

Worker *DiamondClassifyWorkerFactory::createWorker(Actor *actor) {
    return new DiamondClassifyWorker(actor);
}

void DiamondClassifyWorkerFactory::init() {
    QList<PortDescriptor *> ports;
    {
        const Descriptor inSlotDesc(INPUT_SLOT,
                                    DiamondClassifyPrompter::tr("Input URL"),
                                    DiamondClassifyPrompter::tr("Input URL."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[inSlotDesc] = BaseTypes::STRING_TYPE();

        QMap<Descriptor, DataTypePtr> outType;
        outType[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();

        const Descriptor inPortDesc(INPUT_PORT_ID,
                                    DiamondClassifyPrompter::tr("Input sequences"),
                                    DiamondClassifyPrompter::tr("URL(s) to FASTQ or FASTA file(s) should be provided.\n\n"
                                                                "The input files may contain single-end reads, contigs, or \"left\" reads in case of the paired-end sequencing (see \"Input data\" parameter of the element)."));

        const Descriptor outPortDesc(OUTPUT_PORT_ID,
                                     DiamondClassifyPrompter::tr("DIAMOND Classification"),
                                     DiamondClassifyPrompter::tr("A list of sequence names with the associated taxonomy IDs, classified by DIAMOND."));

        ports << new PortDescriptor(inPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);
        ports << new PortDescriptor(outPortDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-out", outType)), false /*input*/, true /*multi*/);
    }

    QList<Attribute *> attributes;
    {
        Descriptor databaseDesc(DATABASE_ATTR_ID, DiamondClassifyPrompter::tr("Database"),
                                      DiamondClassifyPrompter::tr("Input a binary DIAMOND database file."));

        Descriptor code(GENCODE_ATTR_ID, DiamondClassifyPrompter::tr("Genetic code"), DiamondClassifyPrompter::tr("Genetic code used for translation of query sequences (--query-gencode)."));
        Descriptor sense(SENSITIVE_ATTR_ID, DiamondClassifyPrompter::tr("Sensitive mode"), DiamondClassifyPrompter::tr("The sensitive modes (--sensitive, --more-sensitive) are generally recommended for aligning longer sequences. The default mode is mainly designed for short read alignment, i.e. finding significant matches of >50 bits on 30-40aa fragments."));
        Descriptor topAlignmentsPercentage(TOP_ALIGNMENTS_PERCENTAGE_ATTR_ID,
                                                 DiamondClassifyPrompter::tr("Top alignments percentage"),
                                                 DiamondClassifyPrompter::tr("DIAMOND uses the lowest common ancestor (LCA) algorithm for taxonomy classification of the input sequences. This parameter specifies what alignments should be taken into account during the calculations (--top)."
                                                                             "<br><br>"
                                                                             "For example, the default value \"10\" means to take top 10% of the best hits (i.e. sort all query/subject-alignments by score, take top 10% of the alignments with the best score, calculate the lowest common ancestor for them)."));
        Descriptor fshift(FSHIFT_ATTR_ID, DiamondClassifyPrompter::tr("Frameshift"), DiamondClassifyPrompter::tr("Penalty for frameshift in DNA-vs-protein alignments. Values around 15 are reasonable for this parameter. Enabling this feature will have the aligner tolerate missing bases in DNA sequences and is most recommended for long, error-prone sequences like MinION reads."));
        Descriptor evalue(EVALUE_ATTR_ID, DiamondClassifyPrompter::tr("Expected value"), DiamondClassifyPrompter::tr("Maximum expected value to report an alignment (--evalue/-e)."));
        Descriptor matrix(MATRIX_ATTR_ID, DiamondClassifyPrompter::tr("Matrix"), DiamondClassifyPrompter::tr("Scoring matrix (--matrix)."));
        Descriptor gapopen(GO_PEN_ATTR_ID, DiamondClassifyPrompter::tr("Gap open penalty"), DiamondClassifyPrompter::tr("Gap open penalty (--gapopen)."));
        Descriptor gapextend(GE_PEN_ATTR_ID, DiamondClassifyPrompter::tr("Gap extension penalty"), DiamondClassifyPrompter::tr("Gap extension penalty (--gapextend)."));
        Descriptor threads(THREADS_ATTR_ID, DiamondClassifyPrompter::tr("Number of threads"), DiamondClassifyPrompter::tr("Number of CPU threads (--treads)."));
        Descriptor bsize(BSIZE_ATTR_ID, DiamondClassifyPrompter::tr("Block size"), DiamondClassifyPrompter::tr("Block size in billions of sequence letters to be processed at a time (--block-size). This is the main parameter for controlling the program’s memory usage. Bigger numbers will increase the use of memory and temporary disk space, but also improve performance. The program can be expected to use roughly six times this number of memory (in GB)."));
        Descriptor chunks(CHUNKS_ATTR_ID, DiamondClassifyPrompter::tr("Index chunks"), DiamondClassifyPrompter::tr("The number of chunks for processing the seed index (--index-chunks). This option can be additionally used to tune the performance. It is recommended to set this to 1 on a high memory server, which will increase performance and memory usage, but not the usage of temporary disk space."));
        Descriptor outputUrlDesc(OUTPUT_URL_ATTR_ID, DiamondClassifyPrompter::tr("Output file"),
                                       DiamondClassifyPrompter::tr("Specify the output file name."
                                                                   "<br><br>"
                                                                   "The output file is a tab-delimited file with the following fields:"
                                                                   "<ul>"
                                                                   "<li>Query ID</li>"
                                                                   "<li>NCBI taxonomy ID (0 if unclassified)</li>"
                                                                   "<li>E-value of the best alignment with a known taxonomy ID found for the query (0 if unclassified)</li>"
                                                                   "</ul>"));

        Descriptor classifyToolDesc(NgsReadsClassificationPlugin::WORKFLOW_CLASSIFY_TOOL_ID,
                                          WORKFLOW_CLASSIFY_TOOL_DIAMOND,
                                          "Classify tool. Hidden attribute");

        QString diamondDatabasePath;
        U2DataPath *uniref50DataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::DIAMOND_UNIPROT_50_DATABASE_DATA_ID);
        if (NULL != uniref50DataPath && uniref50DataPath->isValid()) {
            diamondDatabasePath = uniref50DataPath->getPathByName(NgsReadsClassificationPlugin::DIAMOND_UNIPROT_50_DATABASE_ITEM_ID);
        } else {
            U2DataPath *clarkViralDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::DIAMOND_UNIPROT_90_DATABASE_DATA_ID);
            if (NULL != clarkViralDataPath && clarkViralDataPath->isValid()) {
                diamondDatabasePath = clarkViralDataPath->getPathByName(NgsReadsClassificationPlugin::DIAMOND_UNIPROT_90_DATABASE_ITEM_ID);
            }
        }

        attributes << new Attribute(databaseDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, diamondDatabasePath);
        attributes << new Attribute(code, BaseTypes::NUM_TYPE(), Attribute::None, 1);
        attributes << new Attribute(sense, BaseTypes::STRING_TYPE(), Attribute::None, DiamondClassifyTaskSettings::SENSITIVE_DEFAULT);
        attributes << new Attribute(topAlignmentsPercentage, BaseTypes::NUM_TYPE(), Attribute::None, 10);
        attributes << new Attribute(fshift, BaseTypes::NUM_TYPE(), Attribute::None, 0);
        attributes << new Attribute(evalue, BaseTypes::NUM_TYPE(), Attribute::None, 0.001);
        attributes << new Attribute(matrix, BaseTypes::STRING_TYPE(), Attribute::None, DiamondClassifyTaskSettings::BLOSUM62);
        attributes << new Attribute(gapopen, BaseTypes::NUM_TYPE(), Attribute::None, -1);
        attributes << new Attribute(gapextend, BaseTypes::NUM_TYPE(), Attribute::None, -1);
        attributes << new Attribute(bsize, BaseTypes::NUM_TYPE(), Attribute::None, 0.5); //NB: unless --very-sensitive supported
        attributes << new Attribute(chunks, BaseTypes::NUM_TYPE(), Attribute::None, 4); //NB: unless --very-sensitive supported
        attributes << new Attribute(threads, BaseTypes::NUM_TYPE(), Attribute::None, AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
        attributes << new Attribute(outputUrlDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);

        attributes << new Attribute(classifyToolDesc, BaseTypes::STRING_TYPE(),
                                    static_cast<Attribute::Flags>(Attribute::Hidden),
                                    WORKFLOW_CLASSIFY_TOOL_DIAMOND);
    }

    QMap<QString, PropertyDelegate *> delegates;
    {
        {
            QList<StrStrPair> dataPathItems;
            dataPathItems << StrStrPair(NgsReadsClassificationPlugin::DIAMOND_UNIPROT_50_DATABASE_DATA_ID, NgsReadsClassificationPlugin::DIAMOND_UNIPROT_50_DATABASE_ITEM_ID);
            dataPathItems << StrStrPair(NgsReadsClassificationPlugin::DIAMOND_UNIPROT_90_DATABASE_DATA_ID, NgsReadsClassificationPlugin::DIAMOND_UNIPROT_90_DATABASE_ITEM_ID);
            delegates[DATABASE_ATTR_ID] = new DatabaseDelegate(ACTOR_ID, DATABASE_ATTR_ID, dataPathItems, "diamond/database", false);
        }
        {
            QList<ComboItem> idMap;
            QList<DNATranslation*> TTs = AppContext::getDNATranslationRegistry()->
                lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()),
                DNATranslationType_NUCL_2_AMINO);
            int prefixLen = QString(DNATranslationID(1)).size() - 1;
            foreach(DNATranslation* tt, TTs) {
                QString id = tt->getTranslationId();
                idMap.append(qMakePair(tt->getTranslationName(), id.mid(prefixLen).toInt()));
            }
            delegates[GENCODE_ATTR_ID] = new ComboBoxDelegate(idMap);
        }
        {
            QList<ComboItem> items;
            items.append(qMakePair(DiamondClassifyPrompter::tr("Default"), DiamondClassifyTaskSettings::SENSITIVE_DEFAULT));
            items.append(qMakePair(DiamondClassifyPrompter::tr("Sensitive"), DiamondClassifyTaskSettings::SENSITIVE_HIGH));
            items.append(qMakePair(DiamondClassifyPrompter::tr("More sensitive"), DiamondClassifyTaskSettings::SENSITIVE_ULTRA));
            delegates[SENSITIVE_ATTR_ID] = new ComboBoxDelegate(items);
        }
        {
            QVariantMap map;
            map["minimum"] = 0;
            map["maximum"] = 100;
            map["suffix"] = "%";
            delegates[TOP_ALIGNMENTS_PERCENTAGE_ATTR_ID] = new SpinBoxDelegate(map);
        }
        {
            QVariantMap map;
            map[DiamondClassifyTaskSettings::BLOSUM45] = DiamondClassifyTaskSettings::BLOSUM45;
            map[DiamondClassifyTaskSettings::BLOSUM50] = DiamondClassifyTaskSettings::BLOSUM50;
            map[DiamondClassifyTaskSettings::BLOSUM62] = DiamondClassifyTaskSettings::BLOSUM62;
            map[DiamondClassifyTaskSettings::BLOSUM80] = DiamondClassifyTaskSettings::BLOSUM80;
            map[DiamondClassifyTaskSettings::BLOSUM90] = DiamondClassifyTaskSettings::BLOSUM90;
            map[DiamondClassifyTaskSettings::PAM30] = DiamondClassifyTaskSettings::PAM30;
            map[DiamondClassifyTaskSettings::PAM70] = DiamondClassifyTaskSettings::PAM70;
            map[DiamondClassifyTaskSettings::PAM250] = DiamondClassifyTaskSettings::PAM250;
            delegates[MATRIX_ATTR_ID] = new ComboBoxDelegate(map);
        }

        {
            QVariantMap map;
            map["minimum"] = -1;
            map["maximum"] = std::numeric_limits<int>::max();
            map["specialValueText"] = DiamondClassifyPrompter::tr("Default");
            delegates[GO_PEN_ATTR_ID] = new SpinBoxDelegate(map);
        }
        {
            QVariantMap map;
            map["minimum"] = -1;
            map["maximum"] = std::numeric_limits<int>::max();
            map["specialValueText"] = DiamondClassifyPrompter::tr("Default");
            delegates[GE_PEN_ATTR_ID] = new SpinBoxDelegate(map);
        }

        {
            QVariantMap map;
            map["minimum"] = 0;
            map["maximum"] = std::numeric_limits<int>::max();
            map["specialValueText"] = DiamondClassifyPrompter::tr("Skipped");
            delegates[FSHIFT_ATTR_ID] = new SpinBoxDelegate(map);
        }
        {
            QVariantMap map;
            map["minimum"] = 0;
            map["maximum"] = std::numeric_limits<int>::max();
            map["specialValueText"] = DiamondClassifyPrompter::tr("Default");
            delegates[CHUNKS_ATTR_ID] = new SpinBoxDelegate(map);
        }

        {
            QVariantMap map;
            map["minimum"] = 0;
            map["singleStep"] = 0.001;
            map["decimals"] = 4;
            delegates[EVALUE_ATTR_ID] = new DoubleSpinBoxDelegate(map);
        }

        {
            QVariantMap map;
            map["minimum"] = 0;
            map["singleStep"] = 0.1;
            map["decimals"] = 2;
            map["specialValueText"] = DiamondClassifyPrompter::tr("Default");
            delegates[BSIZE_ATTR_ID] = new DoubleSpinBoxDelegate(map);
        }

        QVariantMap threadsNumberProperties;
        threadsNumberProperties["minimum"] = 1;
        threadsNumberProperties["maximum"] = QThread::idealThreadCount();
        delegates[THREADS_ATTR_ID] = new SpinBoxDelegate(threadsNumberProperties);

        DelegateTags outputUrlTags;
        outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
        outputUrlTags.set(DelegateTags::FILTER, DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT, true, QStringList()));
        outputUrlTags.set(DelegateTags::FORMAT, BaseDocumentFormats::PLAIN_TEXT);
        delegates[OUTPUT_URL_ATTR_ID] = new URLDelegate(outputUrlTags, "diamond/output");
    }

    const Descriptor desc(ACTOR_ID,
                          DiamondClassifyPrompter::tr("Classify Sequences with DIAMOND"),
                          DiamondClassifyPrompter::tr("In general, DIAMOND is a sequence aligner for protein and translated DNA "
                                                      "searches similar to the NCBI BLAST software tools. However, it provides a "
                                                      "speedup of BLAST ranging up to x20,000."
                                                      "<br><br>"
                                                      "Using this workflow element one can use DIAMOND for taxonomic classification of "
                                                      "short DNA reads and longer sequences such as contigs. The lowest common "
                                                      "ancestor (LCA) algorithm is used for the classification."));

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new DiamondClassifyPrompter(NULL));
    proto->addExternalTool(DiamondSupport::TOOL_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new DiamondClassifyWorkerFactory());
}

void DiamondClassifyWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

}   // namespace LocalWorkflow
}   // namespace U2
