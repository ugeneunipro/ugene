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

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/BAMUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/URLAttribute.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ClarkSupport.h"
#include "ClarkBuildWorker.h"
#include "ClarkClassifyWorker.h"
#include "../../ngs_reads_classification/src/GenomicLibraryDelegate.h"
#include "../../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//ClarkBuild
const QString ClarkBuildWorkerFactory::ACTOR_ID("clark-build");

static const QString OUTPUT_PORT("out");

static const QString DB_URL("database");
static const QString GENOMIC_LIBRARY("genomic-library");
static const QString TAXONOMY_RANK("taxonomy-rank");

/************************************************************************/
/* ClarkBuildValidator */
/************************************************************************/
bool ClarkBuildValidator::validate(const Actor *actor, ProblemList &problemList, const QMap<QString, QString> &) const {
    return validateTaxonomy(actor, problemList);
}

bool ClarkBuildValidator::validateTaxonomy(const Actor *actor, ProblemList &problemList) const {
    U2DataPath *taxonomyDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(),
              problemList << Problem(tr("Taxonomy classification data are not available."), actor->getId()), false);

    bool isValid = true;

    const QString nuclGbAccession2Taxid = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NUCL_GB_ACCESSION_2_TAXID_ITEM_ID);
    const QString nuclWgsAccession2Taxid = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NUCL_WGS_ACCESSION_2_TAXID_ITEM_ID);
    const QString mergedDmp = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_MERGED_ITEM_ID);
    const QString namesDmp = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID);
    const QString nodesDmp = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID);
    const QString taxDump = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_TAXDUMP_ITEM_ID);

    const QString missingFileMessage = tr("Taxonomy classification data are not full: file '%1' is missing.");

    if (nuclGbAccession2Taxid.isEmpty()) {
        problemList << Problem(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_NUCL_GB_ACCESSION_2_TAXID_ITEM_ID), actor->getId());
        isValid = false;
    }

    if (nuclWgsAccession2Taxid.isEmpty()) {
        problemList << Problem(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_NUCL_WGS_ACCESSION_2_TAXID_ITEM_ID), actor->getId());
        isValid = false;
    }

    if (mergedDmp.isEmpty() && taxDump.isEmpty()) {
        problemList << Problem(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_MERGED_ITEM_ID), actor->getId());
        isValid = false;
    }

    if (namesDmp.isEmpty() && taxDump.isEmpty()) {
        problemList << Problem(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID), actor->getId());
        isValid = false;
    }

    if (nodesDmp.isEmpty() && taxDump.isEmpty()) {
        problemList << Problem(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID), actor->getId());
        isValid = false;
    }

    return isValid;
}

/************************************************************************/
/* ClarkBuildPrompter */
/************************************************************************/
QString ClarkBuildPrompter::composeRichDoc() {
    QString doc = tr("Use custom data to build %1 CLARK database.").arg(getHyperlink(DB_URL, getURL(DB_URL)));
    return doc;
}

/************************************************************************/
/* ClarkBuildWorkerFactory */
/************************************************************************/
void ClarkBuildWorkerFactory::init() {

    Descriptor desc( ACTOR_ID, ClarkBuildWorker::tr("Build CLARK Database"),
        ClarkBuildWorker::tr("Build a CLARK database from a set of reference sequences (\"targets\").\n"
                             "NCBI taxonomy data are used to map the accession number found in each reference sequence to its taxonomy ID.") );

    QList<PortDescriptor*> p;
    {
        Descriptor outD(OUTPUT_PORT, ClarkBuildWorker::tr("Output CLARK database"), ClarkBuildWorker::tr("URL to the folder with the CLARK database."));

        Descriptor outSlotDescription(BaseSlots::URL_SLOT().getId(), ClarkBuildWorker::tr("Output URL"), ClarkBuildWorker::tr("Output URL."));

        QMap<Descriptor, DataTypePtr> outM;
        outM[outSlotDescription] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("clark.db-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor dbUrl(DB_URL, ClarkBuildWorker::tr("Database"),
            ClarkBuildWorker::tr("A folder that should be used to store the database files."));

        Descriptor taxonomy(GENOMIC_LIBRARY, ClarkBuildWorker::tr("Genomic library"),
            ClarkBuildWorker::tr("Genomes that should be used to build the database (\"targets\").<br><br>"
                                 "The genomes should be specified in FASTA format. There should be one FASTA file per reference sequence. A sequence header must contain an accession number (i.e., &gt;accession.number ... or &gt;gi|number|ref|accession.number| ...)."));

        Descriptor rank(TAXONOMY_RANK, ClarkBuildWorker::tr("Taxonomy rank"),
            ClarkBuildWorker::tr("Set the taxonomy rank for the database.<br><br>"
                                    "CLARK classifies metagenomic samples by using only one taxonomy rank. So as a general rule, "
                                    "consider first the genus or species rank, then if a high proportion of reads "
                                    "cannot be classified, reset your targets definition at a higher taxonomy rank (e.g., family or phylum)."));

        a << new Attribute( dbUrl, BaseTypes::STRING_TYPE(), true);

        a << new Attribute( taxonomy, BaseTypes::URL_DATASETS_TYPE(), true);
        a << new Attribute( rank, BaseTypes::NUM_TYPE(), false, ClarkClassifySettings::Species);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap rankMap;
        rankMap[ClarkBuildWorker::tr("Species")] = ClarkClassifySettings::Species;
        rankMap[ClarkBuildWorker::tr("Genus")] = ClarkClassifySettings::Genus;
        rankMap[ClarkBuildWorker::tr("Family")] = ClarkClassifySettings::Family;
        rankMap[ClarkBuildWorker::tr("Order")] = ClarkClassifySettings::Order;
        rankMap[ClarkBuildWorker::tr("Class")] = ClarkClassifySettings::Class;
        rankMap[ClarkBuildWorker::tr("Phylum")] = ClarkClassifySettings::Phylum;
        delegates[TAXONOMY_RANK] = new ComboBoxDelegate(rankMap);

        const URLDelegate::Options options = URLDelegate::AllowSelectOnlyExistingDir |
                                             URLDelegate::SelectFileToSave |
                                             URLDelegate::DoNotUseWorkflowOutputFolder;
        DelegateTags tags;
        tags.set(DelegateTags::PLACEHOLDER_TEXT, L10N::required());
        delegates[DB_URL] = new URLDelegate(tags, "clark/database", options);

        delegates[GENOMIC_LIBRARY] = new GenomicLibraryDelegate();
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ClarkBuildPrompter());
    proto->addExternalTool(ET_CLARK_getAccssnTaxID);
    proto->addExternalTool(ET_CLARK_getfilesToTaxNodes);
    proto->addExternalTool(ET_CLARK_getTargetsDef);
    proto->addExternalTool(ET_CLARK_buildScript);
    proto->setValidator(new ClarkBuildValidator());

    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ClarkBuildWorkerFactory());
}

void ClarkBuildWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}


/************************************************************************/
/* ClarkBuildWorker */
/************************************************************************/
ClarkBuildWorker::ClarkBuildWorker(Actor *a)
:BaseWorker(a), output(NULL)
{
}

void ClarkBuildWorker::init() {
    output = ports.value(OUTPUT_PORT);
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(OUTPUT_PORT), );
}

Task * ClarkBuildWorker::tick() {
    if (!isDone()) {
        QString databaseUrl = getValue<QString>(DB_URL);
        int rank = getValue<int>(TAXONOMY_RANK);
        QStringList genUrls;

        U2DataPath *taxonomyDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
        CHECK(NULL != taxonomyDataPath && taxonomyDataPath->isValid(), new FailTask(tr("Taxonomy classification data are not available.")));
        QString taxdataUrl = taxonomyDataPath->getPath();

        const QList<Dataset> datasets = getValue<QList<Dataset> >(GENOMIC_LIBRARY);
        DatasetFilesIterator it(datasets);
        while(it.hasNext()) {
            genUrls << it.getNextFile();
        }

        ClarkBuildTask *task = new ClarkBuildTask(databaseUrl, genUrls, rank, taxdataUrl);
        task->addListeners(createLogListeners(1));
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        setDone();
        return task;
    }
    return NULL;
}

void ClarkBuildWorker::sl_taskFinished(Task* t) {
    ClarkBuildTask *task = qobject_cast<ClarkBuildTask *>(t);
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (!task->isFinished() || task->hasError() || task->isCanceled()) {
        return;
    }

    const QString dbUrl = task->getDbUrl();
    MessageMetadata metadata("Dataset 1");
    context->getMetadataStorage().put(metadata);

    QVariantMap data;
    data[BaseSlots::URL_SLOT().getId()] = dbUrl;
    output->put(Message(output->getBusType(), data, metadata.getId()));
    context->getMonitor()->addOutputFile(dbUrl, getActor()->getId());

    algoLog.trace(tr("Built Clark database"));
}

ClarkBuildTask::ClarkBuildTask(const QString &dbUrl, const QStringList &genomeUrls, int rank, const QString &taxdataUrl)
    : ExternalToolSupportTask(tr("Build Clark database"), TaskFlags_NR_FOSE_COSC),
    dbUrl(dbUrl), taxdataUrl(taxdataUrl), genomeUrls(genomeUrls), rank(rank)
{
  GCOUNTER(cvar, tvar, "ClarkBuildTask");

  SAFE_POINT_EXT(!dbUrl.isEmpty(), setError(tr("CLARK database URL is undefined")), );
  SAFE_POINT_EXT(!taxdataUrl.isEmpty(), setError(tr("Taxdata URL is undefined")), );
  SAFE_POINT_EXT(!genomeUrls.isEmpty(), setError(tr("Genomic library set is empty")), );
  SAFE_POINT_EXT(rank >= 0 && rank <= 5, setError(tr("Failed to recognize the rank. Please provide a number between 0 and 5, according to the following:\n"
                                                     "0: species, 1: genus, 2: family, 3: order, 4:class, and 5: phylum.")), );

}

class ClarkBuildLogParser : public ExternalToolLogParser {
public:
    ClarkBuildLogParser() {}

private:
    bool isError(const QString &line) const {
        foreach (const QString &wellKnownError, wellKnownErrors) {
            if (line.contains(wellKnownError)) {
                return true;
            }
        }
        return false;
    }

    static const QStringList wellKnownErrors;
};

const QStringList ClarkBuildLogParser::wellKnownErrors = QStringList() << "abort" << "core dumped";

void ClarkBuildTask::prepare() {
    const QString db("custom");// = QString("custom_%1").arg(rank);
    const QString reflist = dbUrl + "/.custom";
    QDir dir(dbUrl);
    if (!dir.mkpath(db)){
        setError(tr("Failed to create folder for CLARK database: %1/%2").arg(dbUrl).arg(db));
        return;
    }

    QFile refdata(reflist);
    if (refdata.open(QIODevice::WriteOnly))
    {
        refdata.write(genomeUrls.join("\n").toLocal8Bit());
        refdata.close();
    }
    else
    {
        setError(refdata.errorString());
        CHECK_OP(stateInfo, );
    }

  QString toolName = ET_CLARK_buildScript;
  QScopedPointer<ExternalToolRunTask> task(new ExternalToolRunTask(toolName, getArguments(), new ClarkBuildLogParser()));
  CHECK_OP(stateInfo, );
  setListenerForTask(task.data());
  addSubTask(task.take());
}

QStringList ClarkBuildTask::getArguments() {

  QStringList arguments;

  arguments << dbUrl << taxdataUrl << "custom" << QString::number(rank);

  return arguments;
}

} //LocalWorkflow
} //U2
