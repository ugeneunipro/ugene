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

#include "InSilicoPcrWorker.h"

#include <QTextStream>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/FailTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultiTask.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "InSilicoPcrWorkflowTask.h"
#include "PrimersGrouperWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString InSilicoPcrWorkerFactory::ACTOR_ID = "in-silico-pcr";
namespace {
const QString OUT_PORT_ID = "out";
const QString PRIMERS_ATTR_ID = "primers-url";
const QString REPORT_ATTR_ID = "report-url";
const QString MISMATCHES_ATTR_ID = "mismatches";
const QString PERFECT_ATTR_ID = "perfect-match";
const QString MAX_PRODUCT_ATTR_ID = "max-product";
const QString EXTRACT_ANNOTATIONS_ATTR_ID = "extract-annotations";

const char *PAIR_NUMBER_PROP_ID = "pair-number";
}    // namespace

/************************************************************************/
/* InSilicoPcrWorkerFactory */
/************************************************************************/
InSilicoPcrWorkerFactory::InSilicoPcrWorkerFactory()
    : DomainFactory(ACTOR_ID) {
}

Worker *InSilicoPcrWorkerFactory::createWorker(Actor *a) {
    return new InSilicoPcrWorker(a);
}

void InSilicoPcrWorkerFactory::init() {
    QList<PortDescriptor *> ports;
    {
        Descriptor inDesc(BasePorts::IN_SEQ_PORT_ID(), InSilicoPcrWorker::tr("Input sequence"), InSilicoPcrWorker::tr("Input sequence."));
        Descriptor outDesc(OUT_PORT_ID, InSilicoPcrWorker::tr("PCR product"), InSilicoPcrWorker::tr("PCR product sequence."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        inType[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();

        QMap<Descriptor, DataTypePtr> outType;
        outType[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        outType[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();

        ports << new PortDescriptor(inDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)), true /*input*/);
        ports << new PortDescriptor(outDesc, DataTypePtr(new MapDataType(ACTOR_ID + "-out", outType)), false /*input*/, true /*multi*/);
    }
    QList<Attribute *> attributes;
    {
        Descriptor primersDesc(PRIMERS_ATTR_ID, InSilicoPcrWorker::tr("Primers URL"), InSilicoPcrWorker::tr("A URL to the input file with primer pairs."));
        Descriptor reportDesc(REPORT_ATTR_ID, InSilicoPcrWorker::tr("Report URL"), InSilicoPcrWorker::tr("A URL to the output file with the PCR report."));
        Descriptor mismatchesDesc(MISMATCHES_ATTR_ID, InSilicoPcrWorker::tr("Mismatches"), InSilicoPcrWorker::tr("Number of allowed mismatches."));
        Descriptor perfectDesc(PERFECT_ATTR_ID, InSilicoPcrWorker::tr("Min perfect match"), InSilicoPcrWorker::tr("Number of bases that match exactly on 3' end of primers."));
        Descriptor maxProductDesc(MAX_PRODUCT_ATTR_ID, InSilicoPcrWorker::tr("Max product size"), InSilicoPcrWorker::tr("Maximum size of amplified region."));
        Descriptor annotationsDesc(EXTRACT_ANNOTATIONS_ATTR_ID, InSilicoPcrWorker::tr("Extract annotations"), InSilicoPcrWorker::tr("Extract annotations within a product region."));

        attributes << new Attribute(primersDesc, BaseTypes::STRING_TYPE(), true);
        attributes << new Attribute(reportDesc, BaseTypes::STRING_TYPE(), true, "report.html");
        attributes << new Attribute(mismatchesDesc, BaseTypes::NUM_TYPE(), false, 3);
        attributes << new Attribute(perfectDesc, BaseTypes::NUM_TYPE(), false, 15);
        attributes << new Attribute(maxProductDesc, BaseTypes::NUM_TYPE(), false, 5000);
        attributes << new Attribute(annotationsDesc, BaseTypes::NUM_TYPE(), false, ExtractProductSettings::Inner);
    }
    QMap<QString, PropertyDelegate *> delegates;
    {
        delegates[PRIMERS_ATTR_ID] = new URLDelegate("", "", false, false, false);
        delegates[REPORT_ATTR_ID] = new URLDelegate("", "", false, false, true);
        {    // mismatches
            QVariantMap props;
            props["minimum"] = 0;
            props["maximum"] = 99;
            delegates[MISMATCHES_ATTR_ID] = new SpinBoxDelegate(props);
        }
        {    // perfect match
            QVariantMap props;
            props["minimum"] = 0;
            props["maximum"] = 99;
            delegates[PERFECT_ATTR_ID] = new SpinBoxDelegate(props);
        }
        {    // max product
            QVariantMap props;
            props["minimum"] = 0;
            props["maximum"] = 999999;
            delegates[MAX_PRODUCT_ATTR_ID] = new SpinBoxDelegate(props);
        }
        {    // extract annotations
            QVariantMap values;
            values[InSilicoPcrWorker::tr("Inner")] = ExtractProductSettings::Inner;
            values[InSilicoPcrWorker::tr("All intersected")] = ExtractProductSettings::All;
            values[InSilicoPcrWorker::tr("None")] = ExtractProductSettings::None;
            delegates[EXTRACT_ANNOTATIONS_ATTR_ID] = new ComboBoxDelegate(values);
        }
    }

    Descriptor desc(ACTOR_ID, InSilicoPcrWorker::tr("In Silico PCR"), InSilicoPcrWorker::tr("Simulates PCR for input sequences and primer pairs. Creates the table with the PCR statistics."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new InSilicoPcrPrompter(NULL));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new InSilicoPcrWorkerFactory());
}

/************************************************************************/
/* InSilicoPcrPrompter */
/************************************************************************/
InSilicoPcrPrompter::InSilicoPcrPrompter(Actor *a)
    : PrompterBase<InSilicoPcrPrompter>(a) {
}

QString InSilicoPcrPrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort *>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    SAFE_POINT(NULL != input, "No input port", "");
    const Actor *producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    const QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    const QString producerName = (NULL != producer) ? producer->getLabel() : unsetStr;
    const QString primersLink = getHyperlink(PRIMERS_ATTR_ID, getURL(PRIMERS_ATTR_ID));
    return tr("Simulates PCR for the sequences from <u>%1</u> and primer pairs from <u>%2</u>.").arg(producerName).arg(primersLink);
}

/************************************************************************/
/* InSilicoPcrWorker */
/************************************************************************/
InSilicoPcrWorker::InSilicoPcrWorker(Actor *a)
    : BaseThroughWorker(a, BasePorts::IN_SEQ_PORT_ID(), OUT_PORT_ID), reported(false) {
}

Task *InSilicoPcrWorker::createPrepareTask(U2OpStatus &os) const {
    QString primersUrl = getValue<QString>(PRIMERS_ATTR_ID);
    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(context->getDataStorage()->getDbiRef());
    LoadDocumentTask *task = LoadDocumentTask::getDefaultLoadDocTask(primersUrl, hints);
    if (NULL == task) {
        os.setError(tr("Can not read the primers file: ") + primersUrl);
    }
    return task;
}

void InSilicoPcrWorker::onPrepared(Task *task, U2OpStatus &os) {
    LoadDocumentTask *loadTask = qobject_cast<LoadDocumentTask *>(task);
    CHECK_EXT(NULL != loadTask, os.setError(L10N::internalError("Unexpected prepare task")), );

    QScopedPointer<Document> doc(loadTask->takeDocument());
    CHECK_EXT(!doc.isNull(), os.setError(tr("Can't read the file: ") + loadTask->getURLString()), );

    QList<GObject *> objects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    CHECK_EXT(!objects.isEmpty(), os.setError(tr("No primer sequences in the file: ") + loadTask->getURLString()), );
    CHECK_EXT(0 == objects.size() % 2, os.setError(tr("There is the odd number of primers in the file: ") + loadTask->getURLString()), );

    fetchPrimers(objects, os);
}

void InSilicoPcrWorker::fetchPrimers(const QList<GObject *> &objects, U2OpStatus &os) {
    for (int i = 0; i < objects.size() / 2; i++) {
        bool skipped = false;

        Primer forward = createPrimer(objects[2 * i], skipped, os);
        CHECK_OP(os, );

        Primer reverse = createPrimer(objects[2 * i + 1], skipped, os);
        CHECK_OP(os, );

        if (skipped) {
            continue;
        }

        primers << QPair<Primer, Primer>(forward, reverse);
    }
}

Primer InSilicoPcrWorker::createPrimer(GObject *object, bool &skipped, U2OpStatus &os) {
    Primer result;
    U2SequenceObject *primerSeq = qobject_cast<U2SequenceObject *>(object);
    CHECK_EXT(NULL != primerSeq, os.setError(L10N::nullPointerError("Primer sequence")), result);

    if (primerSeq->getSequenceLength() > Primer::MAX_LEN) {
        skipped = true;
        coreLog.details(tr("Primer sequence is too long: %1. The pair is skipped").arg(primerSeq->getSequenceName()));
        return result;
    }

    result.name = primerSeq->getSequenceName();
    result.sequence = primerSeq->getWholeSequenceData(os);
    CHECK_OP(os, Primer());
    return result;
}

QList<Message> InSilicoPcrWorker::fetchResult(Task *task, U2OpStatus &os) {
    QList<Message> result;
    InSilicoPcrReportTask *reportTask = qobject_cast<InSilicoPcrReportTask *>(task);
    if (NULL != reportTask) {
        monitor()->addOutputFile(getValue<QString>(REPORT_ATTR_ID), actor->getId(), true);
        return result;
    }

    MultiTask *multiTask = qobject_cast<MultiTask *>(task);
    CHECK_EXT(NULL != multiTask, os.setError(L10N::nullPointerError("MultiTask")), result);

    InSilicoPcrReportTask::TableRow tableRow;
    foreach (Task *t, multiTask->getTasks()) {
        InSilicoPcrWorkflowTask *pcrTask = qobject_cast<InSilicoPcrWorkflowTask *>(t);
        CHECK_EXT(NULL != multiTask, os.setError(L10N::nullPointerError("InSilicoPcrTask")), result);

        int pairNumber = pcrTask->property(PAIR_NUMBER_PROP_ID).toInt();
        SAFE_POINT_EXT(pairNumber >= 0 && pairNumber < primers.size(), os.setError(L10N::internalError("Out of range")), result);

        InSilicoPcrTaskSettings settings = pcrTask->getPcrSettings();
        tableRow.sequenceName = settings.sequenceName;
        QList<InSilicoPcrWorkflowTask::Result> pcrResults = pcrTask->takeResult();
        tableRow.productsNumber[pairNumber] = pcrResults.size();

        foreach (const InSilicoPcrWorkflowTask::Result &pcrResult, pcrResults) {
            QVariant sequence = fetchSequence(pcrResult.doc);
            QVariant annotations = fetchAnnotations(pcrResult.doc);
            pcrResult.doc->setDocumentOwnsDbiResources(false);
            delete pcrResult.doc;
            if (sequence.isNull() || annotations.isNull()) {
                continue;
            }

            QVariantMap data;
            data[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = sequence;
            data[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = annotations;
            int metadataId = createMetadata(settings, pcrResult.product.region, pairNumber);
            result << Message(output->getBusType(), data, metadataId);
        }
    }
    table << tableRow;
    return result;
}

QVariant InSilicoPcrWorker::fetchSequence(Document *doc) {
    QList<GObject *> seqObjects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (1 != seqObjects.size()) {
        reportError(L10N::internalError(tr("Wrong sequence objects count")));
        return QVariant();
    }
    SharedDbiDataHandler seqId = context->getDataStorage()->getDataHandler(seqObjects.first()->getEntityRef());
    return qVariantFromValue<SharedDbiDataHandler>(seqId);
}

QVariant InSilicoPcrWorker::fetchAnnotations(Document *doc) {
    QList<GObject *> annsObjects = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    if (1 != annsObjects.size()) {
        reportError(L10N::internalError(tr("Wrong annotations objects count")));
        return QVariant();
    }
    SharedDbiDataHandler annsId = context->getDataStorage()->getDataHandler(annsObjects.first()->getEntityRef());
    return qVariantFromValue<SharedDbiDataHandler>(annsId);
}

int InSilicoPcrWorker::createMetadata(const InSilicoPcrTaskSettings &settings, const U2Region &productRegion, int pairNumber) {
    MessageMetadata oldMetadata = context->getMetadataStorage().get(output->getContextMetadataId());
    QString primerName = primers[pairNumber].first.name;
    QString suffix = "_" + ExtractProductTask::getProductName(primerName, settings.sequence.length(), productRegion, true);
    QString newUrl = GUrlUtils::insertSuffix(oldMetadata.getFileUrl(), suffix);

    MessageMetadata metadata(newUrl, oldMetadata.getDatasetName());
    context->getMetadataStorage().put(metadata);
    return metadata.getId();
}

Task *InSilicoPcrWorker::onInputEnded() {
    CHECK(!reported, NULL);
    reported = true;
    return new InSilicoPcrReportTask(table, primers, getValue<QString>(REPORT_ATTR_ID));
}

Task *InSilicoPcrWorker::createTask(const Message &message, U2OpStatus &os) {
    QVariantMap data = message.getData().toMap();
    SharedDbiDataHandler seqId = data[BaseSlots::DNA_SEQUENCE_SLOT().getId()].value<SharedDbiDataHandler>();
    QScopedPointer<U2SequenceObject> seq(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
    SAFE_POINT(!seq.isNull(), L10N::nullPointerError("Sequence"), NULL);
    if (seq->getSequenceLength() > InSilicoPcrTaskSettings::MAX_SEQUENCE_LENGTH) {
        os.setError(tr("The sequence is too long: ") + seq->getSequenceName());
        return NULL;
    }

    ExtractProductSettings productSettings;
    productSettings.sequenceRef = seq->getEntityRef();
    QList<AnnotationTableObject *> anns = StorageUtils::getAnnotationTableObjects(context->getDataStorage(), data[BaseSlots::ANNOTATION_TABLE_SLOT().getId()]);
    foreach (AnnotationTableObject *annsObject, anns) {
        productSettings.annotationRefs << annsObject->getEntityRef();
        delete annsObject;
    }
    anns.clear();
    productSettings.targetDbiRef = context->getDataStorage()->getDbiRef();
    productSettings.annotationsExtraction = ExtractProductSettings::AnnotationsExtraction(getValue<int>(EXTRACT_ANNOTATIONS_ATTR_ID));

    InSilicoPcrTaskSettings pcrSettings;
    pcrSettings.sequence = seq->getWholeSequenceData(os);
    CHECK_OP(os, NULL);
    pcrSettings.isCircular = seq->isCircular();
    pcrSettings.forwardMismatches = getValue<int>(MISMATCHES_ATTR_ID);
    pcrSettings.reverseMismatches = pcrSettings.forwardMismatches;
    pcrSettings.maxProductSize = getValue<int>(MAX_PRODUCT_ATTR_ID);
    pcrSettings.perfectMatch = getValue<int>(PERFECT_ATTR_ID);
    pcrSettings.sequenceName = seq->getSequenceName();

    QList<Task *> tasks;
    for (int i = 0; i < primers.size(); i++) {
        pcrSettings.forwardPrimer = primers[i].first.sequence.toLocal8Bit();
        pcrSettings.reversePrimer = primers[i].second.sequence.toLocal8Bit();
        Task *pcrTask = new InSilicoPcrWorkflowTask(pcrSettings, productSettings);
        pcrTask->setProperty(PAIR_NUMBER_PROP_ID, i);
        tasks << pcrTask;
    }
    sequences << seqId;
    CHECK(!tasks.isEmpty(), new FailTask(tr("Primers specified in \"%1\" are too long.").arg(QFileInfo(getValue<QString>(PRIMERS_ATTR_ID)).fileName())));

    return new MultiTask(tr("Multiple In Silico PCR"), tasks);
}

/************************************************************************/
/* InSilicoPcrReportTask */
/************************************************************************/
InSilicoPcrReportTask::InSilicoPcrReportTask(const QList<TableRow> &table, const QList<QPair<Primer, Primer>> &primers, const QString &reportUrl)
    : Task(tr("Generate In Silico PCR report"), TaskFlag_None), table(table), primers(primers), reportUrl(reportUrl) {
}

void InSilicoPcrReportTask::run() {
    QScopedPointer<IOAdapter> io(IOAdapterUtils::open(reportUrl, stateInfo, IOAdapterMode_Write));
    CHECK_OP(stateInfo, );

    io->writeBlock(createReport());
}

QByteArray InSilicoPcrReportTask::createReport() const {
    QString html = readHtml();
    QStringList tokens = html.split("<body>");
    SAFE_POINT(2 == tokens.size(), "Wrong HTML base", "");
    QByteArray report = tokens[0].toLocal8Bit() + "<body>\n";

    report += productsTable();
    report += primerDetails();
    report += tokens[1];
    return report;
}

QByteArray InSilicoPcrReportTask::productsTable() const {
    QByteArray result;
    result += "<table bordercolor=\"gray\" border=\"1\" width=\"100%\">";
    result += "<tr>";
    result += PrimerGrouperTask::createColumn(tr("Sequence name"), "width=\"20%\"");
    for (int i = 0; i < primers.size(); i++) {
        result += PrimerGrouperTask::createColumn(primers[i].first.name + "<br/>" + primers[i].second.name);
    }
    result += "</tr>";
    foreach (const TableRow &tableRow, table) {
        result += "<tr>";
        result += PrimerGrouperTask::createCell(tableRow.sequenceName);
        for (int i = 0; i < primers.size(); i++) {
            QString elemClass = (tableRow.productsNumber[i] == 0) ? "red" : "green";
            QString classDef = QString("class=\"%1\"").arg(elemClass);
            result += PrimerGrouperTask::createCell(QString::number(tableRow.productsNumber[i]), true, classDef);
        }
        result += "</tr>";
    }
    result += "</table>\n";
    return chapterName(tr("Products count table")) + chapterContent(result);
}

QByteArray InSilicoPcrReportTask::primerDetails() const {
    QByteArray result;
    for (int i = 0; i < primers.size(); i++) {
        QPair<Primer, Primer> pair = primers[i];
        PrimersPairStatistics calc(pair.first.sequence.toLocal8Bit(), pair.second.sequence.toLocal8Bit());
        result += chapter(
            chapterName("<span class=\"span-closed\">&#9656;</span> " + pair.first.name + " / " + pair.second.name),
            chapterContent(calc.generateReport().toLocal8Bit()));
    }
    return chapterName(tr("Primer pair details")) + chapterContent(result);
}

QByteArray InSilicoPcrReportTask::chapterName(const QString &name) const {
    return "<h3>" + name.toLocal8Bit() + "</h3>\n";
}

QByteArray InSilicoPcrReportTask::chapterContent(const QByteArray &content) const {
    return "<div class=\"chapter-content\">\n" + content + "</div>\n";
}

QByteArray InSilicoPcrReportTask::chapter(const QByteArray &name, const QByteArray &content) const {
    QByteArray result;
    result += "<div>\n";
    result += "<a href=\"#\" onclick=\"showContent(this);\">" + name + "</a>\n";
    result += "<div class=\"hidden\">\n";
    result += content;
    result += "</div>\n";
    result += "</div>\n";
    return result;
}

QString InSilicoPcrReportTask::readHtml() const {
    static const QString htmlUrl = ":pcr/html/report.html";
    QFile file(htmlUrl);
    bool opened = file.open(QIODevice::ReadOnly);
    if (!opened) {
        coreLog.error("Can not load " + htmlUrl);
        return "";
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString result = stream.readAll();
    file.close();
    return result;
}

}    // namespace LocalWorkflow
}    // namespace U2
