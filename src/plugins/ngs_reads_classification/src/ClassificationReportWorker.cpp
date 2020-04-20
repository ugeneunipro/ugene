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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
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
#include <U2Formats/FastaFormat.h>
#include <U2Formats/FastqFormat.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/ActorValidator.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ClassificationReportWorker.h"
#include "NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//ClassificationReport
const QString ClassificationReportWorkerFactory::ACTOR_ID("classification-report");

static const QString INPUT_PORT("in");

static const QString OUT_FILE("output-url");
static const QString ALL_TAXA("all-taxa");
static const QString SORT_BY("sort-by");

/************************************************************************/
/* ClassificationReportValidator */
/************************************************************************/
bool ClassificationReportValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &) const {
    return validateTaxonomyTree(actor, notificationList);
}

bool ClassificationReportValidator::validateTaxonomyTree(const Actor *actor, NotificationsList &notificationList) const {
    bool valid = true;
    if (!TaxonomyTree::getInstance()->isValid()) {
        notificationList << WorkflowNotification(tr("Taxonomy classification data from NCBI are not available."), actor->getId());
        valid = false;
    }
    return valid;
}

/************************************************************************/
/* ClassificationReportPrompter */
/************************************************************************/
QString ClassificationReportPrompter::composeRichDoc() {
    return tr("Generate a detailed classification report.");
}

/************************************************************************/
/* ClassificationReportWorkerFactory */
/************************************************************************/
void ClassificationReportWorkerFactory::init() {

    Descriptor desc( ACTOR_ID, ClassificationReportWorker::tr("Classification Report"),
        ClassificationReportWorker::tr("Based on the input taxonomy classification data the element generates a detailed report and saves it in a tab-delimited text format.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, ClassificationReportWorker::tr("Input taxonomy data"),
                       ClassificationReportWorker::tr("Input taxonomy data from one of the classification elements (Kraken, CLARK, etc.)."
                ));

        QMap<Descriptor, DataTypePtr> inM;
        inM[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("report.input", inM)), true);
    }

    QList<Attribute*> a;
    {
        Descriptor outputDesc(OUT_FILE, ClassificationReportWorker::tr("Output file"),
            ClassificationReportWorker::tr("Specify the output text file name."));

        Descriptor allTaxa(ALL_TAXA, ClassificationReportWorker::tr("All taxa"),
            ClassificationReportWorker::tr("By default, taxa with no sequences (reads or scaffolds) assigned are not included into the output. This option specifies to include all taxa.\
                                           <br><br>This may be useful when output from several samples is compared.Set \"Sort by\" to \"Tax ID\" in this case."));

        Descriptor sortBy(SORT_BY, ClassificationReportWorker::tr("Sort by"),
            ClassificationReportWorker::tr("It is possible to sort rows in the output file in two ways: \
            <ul><li>by the number of reads, covered by the clade rooted at the taxon(i.e. \"clade_num\" for this taxID)</li> \
            <li>by taxIDs</li></ul> \
            The second option may be useful when output from different samples is compared."));

        a << new Attribute(outputDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);
        a << new Attribute(allTaxa, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute(sortBy, BaseTypes::STRING_TYPE(), false, QVariant(ClassificationReportTask::NUMBER_OF_READS));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        const URLDelegate::Options options = URLDelegate::SelectFileToSave;
        DelegateTags tags;
        tags.set(DelegateTags::PLACEHOLDER_TEXT, ClassificationReportPrompter::tr("Auto"));
        tags.set(DelegateTags::FILTER, DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT, true));
        delegates[OUT_FILE] = new URLDelegate(tags, "classify/report", options);

        delegates[ALL_TAXA] = new ComboBoxWithBoolsDelegate();

        QVariantMap sortByMap;
        sortByMap[ClassificationReportWorkerFactory::tr("Number of reads")] = ClassificationReportTask::NUMBER_OF_READS;
        sortByMap[ClassificationReportWorkerFactory::tr("Tax ID")] = ClassificationReportTask::TAX_ID;
        delegates[SORT_BY] = new ComboBoxDelegate(sortByMap);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ClassificationReportPrompter());
    proto->setValidator(new ClassificationReportValidator());

    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ClassificationReportWorkerFactory());
}

void ClassificationReportWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}


/************************************************************************/
/* ClassificationReportWorker */
/************************************************************************/
ClassificationReportWorker::ClassificationReportWorker(Actor *a)
:BaseWorker(a, false), input(NULL)
{
}

const QString ClassificationReportWorker::getProducerClassifyToolName() const {
    Port* port = actor->getPort(input->getPortId());
    IntegralBusPort * inPort = qobject_cast<IntegralBusPort*>(port);
    Actor* ac = inPort->getProducer(TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT().getId());
    CHECK(ac != NULL, "UNKNOWN_CLASSIFY_TOOL");

    Attribute* a = ac->getParameter(NgsReadsClassificationPlugin::WORKFLOW_CLASSIFY_TOOL_ID);
    CHECK(a != NULL, ac->getId());

    return a->getAttributeValueWithoutScript<QString>();
}

void ClassificationReportWorker::init() {
    input = ports.value(INPUT_PORT);
    SAFE_POINT(NULL != input, QString("Port with id '%1' is NULL").arg(INPUT_PORT), );

    producerClassifyToolName = getProducerClassifyToolName();
}

const QString ClassificationReportWorker::getReportFilePrefix(const Message& message) const {
    QString prefix;

    const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
    QString metadataFileUrl = metadata.getFileUrl();
    prefix = GUrlUtils::getPairedFastqFilesBaseName(metadataFileUrl, true);

    return prefix;
}

Task * ClassificationReportWorker::tick() {
    if (input->hasMessage()) {
        const Message message = getMessageAndSetupScriptValues(input);

        QString outputFileUrl = getValue<QString>(OUT_FILE);
        if (outputFileUrl.isEmpty()) {
            QString reportFilePrefix = getReportFilePrefix(message);
            outputFileUrl = FileAndDirectoryUtils::createWorkingDir(context->workingDir(),
                                    FileAndDirectoryUtils::WORKFLOW_INTERNAL_CUSTOM,
                                    "Classification_Report/",
                                    context->workingDir());
            if (!reportFilePrefix.isEmpty()) {
                outputFileUrl += reportFilePrefix + "_";
            }
            outputFileUrl += producerClassifyToolName +
                    "_report.txt";
        }
        outputFileUrl = GUrlUtils::rollFileName(QFileInfo(outputFileUrl).absoluteFilePath(), "_");

        QVariantMap m = message.getData().toMap();
        TaxonomyClassificationResult tax = m[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT().getId()].value<U2::LocalWorkflow::TaxonomyClassificationResult>();

        QMap<TaxID, uint> data;
        foreach (TaxID id, tax) {
            data[id]++;
        }

        U2OpStatusImpl os;

        bool allTaxa = getValue<bool>(ALL_TAXA);
        ClassificationReportTask::SortBy sortBy = (ClassificationReportTask::SortBy)getValue<int>(SORT_BY);

        ClassificationReportTask *task = new ClassificationReportTask(data, tax.size(), outputFileUrl, allTaxa, sortBy);
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    }

    if (input->isEnded()) {
        setDone();
        algoLog.info("Report worker is done as input has ended");
    }

    return NULL;
}

void ClassificationReportWorker::sl_taskFinished(Task *t) {
    ClassificationReportTask *task = qobject_cast<ClassificationReportTask *>(t);
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (!task->isFinished() || task->hasError() || task->isCanceled()) {
        return;
    }

    context->getMonitor()->addOutputFile(task->getUrl(), getActor()->getId());
}

ClassificationReportTask::ClassificationReportTask(const QMap<TaxID,uint> &data,
                                                   uint totalCount,
                                                   const QString &reportUrl,
                                                   bool _allTaxa,
                                                   SortBy _sortBy)
    : Task(tr("Compose classification report"), TaskFlag_None),
      data(data), totalCount(totalCount), url(reportUrl), allTaxa(_allTaxa), sortBy(_sortBy)
{
    GCOUNTER(cvar, tvar, "ClassificationReportTask");

    SAFE_POINT_EXT(!reportUrl.isEmpty(), setError("Report URL is empty"), );
}

struct ClassificationReportLine {

    ClassificationReportLine() {
        tax_id = 0;
        directly_num = 0;
        directly_proportion_all = 0.;
        directly_proportion_classified = 0.;
        clade_num = 0;
        clade_proportion_all = 0.;
        clade_proportion_classified = 0.;

        superkingdom_tax_id = 0;
        phylum_tax_id = 0;
        class_tax_id = 0;
        order_tax_id = 0;
        family_tax_id = 0;
        genus_tax_id = 0;
        species_tax_id = 0;
    }

    TaxID tax_id;
    QString tax_name;
    QString rank;
    QString lineage;
    TaxID superkingdom_tax_id;
    QString superkingdom_name;
    TaxID phylum_tax_id;
    QString phylum_name;
    TaxID class_tax_id;
    QString class_name;
    TaxID order_tax_id;
    QString order_name;
    TaxID family_tax_id;
    QString family_name;
    TaxID genus_tax_id;
    QString genus_name;
    TaxID species_tax_id;
    QString species_name;
    uint directly_num;
    double directly_proportion_all;
    double directly_proportion_classified;
    uint clade_num;
    double clade_proportion_all;
    double clade_proportion_classified;

    static QString fmt(QString s) {
        return s.isEmpty() ? "-" : s;
    }

    // TODO it might be better to write to stream instead of bytearray buffer
    QByteArray toString() const {
        QByteArray line;
        line.reserve(400);
        return line.append(QByteArray::number(tax_id)).append('\t').append(tax_name).append('\t').append(rank).append('\t').append(lineage).append('\t')
                .append(QByteArray::number(superkingdom_tax_id)).append('\t').append(fmt(superkingdom_name)).append('\t').append(QByteArray::number(phylum_tax_id)).append('\t').append(fmt(phylum_name)).append('\t')
                .append(QByteArray::number(class_tax_id)).append('\t').append(fmt(class_name)).append('\t').append(QByteArray::number(order_tax_id)).append('\t').append(fmt(order_name)).append('\t')
                .append(QByteArray::number(family_tax_id)).append('\t').append(fmt(family_name)).append('\t').append(QByteArray::number(genus_tax_id)).append('\t').append(fmt(genus_name)).append('\t')
                .append(QByteArray::number(species_tax_id)).append('\t').append(fmt(species_name)).append('\t').append(QByteArray::number(directly_num)).append('\t')
                .append(QByteArray::number(directly_proportion_all * 100, 'f', 3)).append('\t').append(QByteArray::number(directly_proportion_classified * 100, 'f', 3)).append('\t')
                .append(QByteArray::number(clade_num)).append('\t').append(QByteArray::number(clade_proportion_all * 100, 'f', 3)).append('\t').append(QByteArray::number(clade_proportion_classified * 100, 'f', 3));
    }
};

static QString write(QString path, QHash<TaxID, ClassificationReportLine> report, ClassificationReportTask::SortBy);

static const QString SPECIES("species");
static const QString GENUS("genus");
static const QString FAMILY("family");
static const QString ORDER("order");
static const QString CLASS("class");
static const QString PHYLUM("phylum");
static const QString SUPERKINGDOM("superkingdom");
static const QString header("tax_id\ttax_name\trank\tlineage\tsuperkingdom_tax_id\tsuperkingdom_name\tphylum_tax_id\tphylum_name\tclass_tax_id\tclass_name\torder_tax_id\torder_name\tfamily_tax_id\tfamily_name\tgenus_tax_id\tgenus_name\tspecies_tax_id\tspecies_name\tdirectly_num\tdirectly_proportion_all(%)\tdirectly_proportion_classified(%)\tclade_num\tclade_proportion_all(%)\tclade_proportion_classified(%)");


static void fill(QHash<TaxID, uint> &claded, QHash<TaxID, ClassificationReportLine> &report, int count, TaxID id, int totalCount, int classifiedCount) {
    TaxonomyTree *tree = TaxonomyTree::getInstance();
    CHECK(tree->contains(id), );

    ClassificationReportLine &line = report[id];
    line.tax_id = id;
    line.directly_num = count;
    line.directly_proportion_all = double(count) / totalCount;
    line.directly_proportion_classified = double(count) / classifiedCount;

    QString rank = line.rank = tree->getRank(id);
    QString name = line.tax_name = tree->getName(id);
    do {
        claded[id] += line.directly_num;

        if (SPECIES.compare(rank) == 0) {
            line.species_tax_id = id;
            line.species_name = name;
        } else if (GENUS.compare(rank) == 0) {
            line.genus_tax_id = id;
            line.genus_name = name;
        } else if (FAMILY.compare(rank) == 0) {
            line.family_tax_id = id;
            line.family_name = name;
        } else if (ORDER.compare(rank) == 0) {
            line.order_tax_id = id;
            line.order_name = name;
        } else if (CLASS.compare(rank) == 0) {
            line.class_tax_id = id;
            line.class_name = name;
        } else if (PHYLUM.compare(rank) == 0) {
            line.phylum_tax_id = id;
            line.phylum_name = name;
        } else if (SUPERKINGDOM.compare(rank) == 0) {
            line.superkingdom_tax_id = id;
            line.superkingdom_name = name;
        }

        id = tree->getParent(id);
        if (id <= 1) {
            break;
        }

        rank = tree->getRank(id);
        name = tree->getName(id);
        line.lineage.prepend(name).prepend(';');
    } while (1);

    if (!line.lineage.isEmpty()) {
        line.lineage = line.lineage.mid(1);
    }
}

void ClassificationReportTask::run()
{
    uint classifiedCount = totalCount - data.remove(TaxonomyTree::UNCLASSIFIED_ID);
    QHash<TaxID, ClassificationReportLine> report;
    QHash<TaxID, uint> claded;

    if (!allTaxa) {
        report.reserve(data.size());
        claded.reserve(data.size() * 8);

        QMapIterator<TaxID, uint> i(data);
        while (i.hasNext()) {
            i.next();
            TaxID id = i.key();
            int count = i.value();
            fill(claded, report, count, id, totalCount, classifiedCount);
        }
    } else {
        TaxonomyTree *tree = TaxonomyTree::getInstance();
        int elementsCount = tree->getNamesListSize();
        report.reserve(elementsCount);
        claded.reserve(elementsCount * 8);
        //0 index is empty, 1 is root element
        for (int index = 2; index < elementsCount; index++) {
            if (tree->getName(index).isEmpty()) {
                continue;
            }
            TaxID id = index;
            int count = data.value(index, 0);
            fill(claded, report, count, id, totalCount, classifiedCount);
        }
    }

    QHashIterator<TaxID, uint> i2(claded);
    while (i2.hasNext()) {
        i2.next();
        TaxID id = i2.key();
        uint count = i2.value();
        QHash<TaxID, ClassificationReportLine>::iterator line = report.find(id);
        if (line != report.end()) {
            line.value().clade_num = count;
            line.value().clade_proportion_all = double(count) / totalCount;
            line.value().clade_proportion_classified = double(count) / classifiedCount;
        }
    }

    setError(write(url, report, sortBy));
}

static bool compareByCladeNum(const ClassificationReportLine* first, const ClassificationReportLine* second) {
    if (first->clade_num == second->clade_num) {
        return first->tax_id < second->tax_id;
    } else {
        return first->clade_num > second->clade_num;
    }
}

static bool compareByTaxId(const ClassificationReportLine* first, const ClassificationReportLine* second) {
    return first->tax_id < second->tax_id;
}

QString write(QString fileName, QHash<TaxID, ClassificationReportLine> report, ClassificationReportTask::SortBy sortBy) {
    QList<ClassificationReportLine*> sorted;
    for (QHash<TaxID, ClassificationReportLine>::iterator i = report.begin(); i != report.end(); ++i) {
        sorted << &*i;
    }

    switch (sortBy) {
    case ClassificationReportTask::NUMBER_OF_READS:
        std::sort(sorted.begin(), sorted.end(), compareByCladeNum);
        break;
    case ClassificationReportTask::TAX_ID:
        std::sort(sorted.begin(), sorted.end(), compareByTaxId);
    break;
    default:
        break;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly) ) {
        file.write(header.toLocal8Bit());
        file.putChar('\n');
        foreach(ClassificationReportLine *line, sorted) {
            file.write(line->toString());
            file.putChar('\n');
        }
        file.close();
        return QString();
    } else {
        return file.errorString();
    }
}

} //LocalWorkflow
} //U2
