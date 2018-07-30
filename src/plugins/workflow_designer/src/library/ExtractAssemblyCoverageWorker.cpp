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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/FailTask.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include <U2View/ExportCoverageTask.h>

#include "ExtractAssemblyCoverageWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ExtractAssemblyCoverageWorkerFactory::ACTOR_ID("extract-assembly-coverage");
const QString ExtractAssemblyCoverageWorkerFactory::EXPORT_COVERAGE = QObject::tr("coverage");
const QString ExtractAssemblyCoverageWorkerFactory::EXPORT_BASES_QUANTITY = QObject::tr("bases count");

namespace {
const QString FORMAT_ATTR_ID("format");
const QString EXPORT_TYPE_ATTR_ID("export-type");
const QString THRESHOLD_ATTR_ID("threshold");
}

ExtractAssemblyCoverageWorker::ExtractAssemblyCoverageWorker(Actor *actor) :
    BaseWorker(actor)
{
}

void ExtractAssemblyCoverageWorker::init() {
}

Task *ExtractAssemblyCoverageWorker::tick() {
    if (hasAssembly()) {
        U2OpStatusImpl os;
        const U2EntityRef assembly = takeAssembly(os);
        CHECK_OP(os, new FailTask(os.getError()));

        return createTask(assembly);
    } else {
        finish();
        return NULL;
    }
}

void ExtractAssemblyCoverageWorker::cleanup() {
}

void ExtractAssemblyCoverageWorker::sl_taskFinished() {
    ExportCoverageTask *task = dynamic_cast<ExportCoverageTask *>(sender());
    CHECK(NULL != task, );
    CHECK(task->isFinished() && !task->hasError() && !task->isCanceled(), );

    monitor()->addOutputFile(task->getUrl(), getActorId());
}

bool ExtractAssemblyCoverageWorker::hasAssembly() const {
    const IntegralBus *port = ports[BasePorts::IN_ASSEMBLY_PORT_ID()];
    SAFE_POINT(NULL != port, "NULL assembly port", false);
    return port->hasMessage();
}

U2EntityRef ExtractAssemblyCoverageWorker::takeAssembly(U2OpStatus &os) {
    const Message m = getMessageAndSetupScriptValues(ports[BasePorts::IN_ASSEMBLY_PORT_ID()]);
    const QVariantMap data = m.getData().toMap();
    if (!data.contains(BaseSlots::ASSEMBLY_SLOT().getId())) {
        os.setError(tr("Empty assembly slot"));
        return U2EntityRef();
    }

    const SharedDbiDataHandler dbiId = data[BaseSlots::ASSEMBLY_SLOT().getId()].value<SharedDbiDataHandler>();
    const AssemblyObject *obj = StorageUtils::getAssemblyObject(context->getDataStorage(), dbiId);
    if (NULL == obj) {
        os.setError(tr("Error with assembly object"));
        return U2EntityRef();
    }
    return obj->getEntityRef();
}

ExportCoverageSettings ExtractAssemblyCoverageWorker::getSettings() const {
    ExportCoverageSettings settings;
    const QString exportTypeString = getValue<QString>(EXPORT_TYPE_ATTR_ID);
    settings.exportCoverage = exportTypeString.contains(ExtractAssemblyCoverageWorkerFactory::EXPORT_COVERAGE);
    settings.exportBasesCount = exportTypeString.contains(ExtractAssemblyCoverageWorkerFactory::EXPORT_BASES_QUANTITY);
    settings.threshold = getValue<int>(THRESHOLD_ATTR_ID);
    settings.url = GUrlUtils::rollFileName(getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId()), "_", QSet<QString>());
    settings.compress = (settings.url.endsWith(".gz"));
    return settings;
}

Task *ExtractAssemblyCoverageWorker::createTask(const U2EntityRef &assembly) {
    const ExportCoverageSettings::Format format = static_cast<ExportCoverageSettings::Format>(getValue<int>(FORMAT_ATTR_ID));
    Task *task = NULL;
    switch (format) {
    case ExportCoverageSettings::Histogram:
        task = new ExportCoverageHistogramTask(assembly.dbiRef, assembly.entityId, getSettings());
        break;
    case ExportCoverageSettings::PerBase:
        task = new ExportCoveragePerBaseTask(assembly.dbiRef, assembly.entityId, getSettings());
        break;
    case ExportCoverageSettings::Bedgraph:
        task = new ExportCoverageBedgraphTask(assembly.dbiRef, assembly.entityId, getSettings());
        break;
    }

    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return task;
}

void ExtractAssemblyCoverageWorker::finish() {
    IntegralBus *inPort = ports[BasePorts::IN_ASSEMBLY_PORT_ID()];
    SAFE_POINT(NULL != inPort, "NULL assembly port", );
    SAFE_POINT(inPort->isEnded(), "The assembly is not ended", );

    setDone();
}

/************************************************************************/
/* ExportAssemblyCoverageWorkerFactory */
/************************************************************************/
ExtractAssemblyCoverageWorkerFactory::ExtractAssemblyCoverageWorkerFactory() :
    DomainFactory(ACTOR_ID)
{
}

Worker *ExtractAssemblyCoverageWorkerFactory::createWorker(Actor *actor) {
    return new ExtractAssemblyCoverageWorker(actor);
}

void ExtractAssemblyCoverageWorkerFactory::init() {
    const Descriptor desc(ACTOR_ID,
        ExtractAssemblyCoverageWorker::tr("Extract Coverage from Assembly"),
        ExtractAssemblyCoverageWorker::tr("Extract the coverage and bases quantity from the incoming assembly."));

    QList<PortDescriptor*> ports;
    {
        QMap<Descriptor, DataTypePtr> inData;
        inData[BaseSlots::ASSEMBLY_SLOT()] = BaseTypes::ASSEMBLY_TYPE();
        DataTypePtr inType(new MapDataType(BasePorts::IN_ASSEMBLY_PORT_ID(), inData));
        ports << new PortDescriptor(BasePorts::IN_ASSEMBLY_PORT_ID(), inType, true);
    }

    QList<Attribute*> attrs;
    {
        const Descriptor formatDesc(FORMAT_ATTR_ID,
                                        ExtractAssemblyCoverageWorker::tr("Format"),
                                        ExtractAssemblyCoverageWorker::tr("Format to store the output."));
        const Descriptor exportTypeDesc(EXPORT_TYPE_ATTR_ID,
                                        ExtractAssemblyCoverageWorker::tr("Export"),
                                        ExtractAssemblyCoverageWorker::tr("Data type to export."));
        const Descriptor thresholdDesc(THRESHOLD_ATTR_ID,
                                       ExtractAssemblyCoverageWorker::tr("Threshold"),
                                       ExtractAssemblyCoverageWorker::tr("The minimum coverage value to export."));
        attrs << new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true, "assembly_coverage." + ExportCoverageSettings::BEDGRAPH_EXTENSION);

        Attribute *formatAttribute = new Attribute(formatDesc, BaseTypes::NUM_TYPE(), false, ExportCoverageSettings::Bedgraph);
        formatAttribute->addRelation(new ExtractAssemblyCoverageFileExtensionRelation(BaseAttributes::URL_OUT_ATTRIBUTE().getId()));
        attrs << formatAttribute;

        Attribute *exportTypeAttribute = new Attribute(exportTypeDesc, BaseTypes::STRING_TYPE(), true, EXPORT_COVERAGE);
        exportTypeAttribute->addRelation(new VisibilityRelation(formatAttribute->getId(), ExportCoverageSettings::PerBase));
        attrs << exportTypeAttribute;

        attrs << new Attribute(thresholdDesc, BaseTypes::NUM_TYPE(), false, 1);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        const QString filter = FormatUtils::prepareFileFilter(ExportCoverageSettings::BEDGRAPH, QStringList() << ExportCoverageSettings::BEDGRAPH_EXTENSION, true);
        DelegateTags tags;
        tags.set("filter", filter);
        tags.set("extensions", QStringList() << ExportCoverageSettings::BEDGRAPH_EXTENSION << ExportCoverageSettings::BEDGRAPH_EXTENSION + ExportCoverageSettings::COMPRESSED_EXTENSION);
        delegates[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] = new URLDelegate(tags, "", false, false, true, NULL);

        QVariantMap formats;
        formats.insert(ExportCoverageSettings::HISTOGRAM, ExportCoverageSettings::Histogram);
        formats.insert(ExportCoverageSettings::PER_BASE, ExportCoverageSettings::PerBase);
        formats.insert(ExportCoverageSettings::BEDGRAPH, ExportCoverageSettings::Bedgraph);
        delegates[FORMAT_ATTR_ID] = new ComboBoxDelegate(formats);

        QVariantMap exportTypes;
        exportTypes.insert(EXPORT_COVERAGE, true);
        exportTypes.insert(EXPORT_BASES_QUANTITY, false);
        delegates[EXPORT_TYPE_ATTR_ID] = new ComboBoxWithChecksDelegate(exportTypes);

        QVariantMap thresholdMap;
        thresholdMap["minimum"] = 0;
        thresholdMap["maximum"] = 65535;
        delegates[THRESHOLD_ATTR_ID] = new SpinBoxDelegate(thresholdMap);
    }

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attrs);
    proto->setPrompter(new ExtractAssemblyCoverageWorkerPrompter());
    proto->setEditor(new DelegateEditor(delegates));

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ExtractAssemblyCoverageWorkerFactory());
}

/************************************************************************/
/* ExportAssemblyCoverageWorkerPrompter */
/************************************************************************/
ExtractAssemblyCoverageWorkerPrompter::ExtractAssemblyCoverageWorkerPrompter(Actor *actor) :
    PrompterBase<ExtractAssemblyCoverageWorkerPrompter>(actor)
{
}

QString ExtractAssemblyCoverageWorkerPrompter::composeRichDoc() {
    QString exportString;
    switch (getParameter(FORMAT_ATTR_ID).toInt()) {
    case ExportCoverageSettings::Histogram:
        exportString = tr("coverage in \"%1\" format").arg(getHyperlink(FORMAT_ATTR_ID, ExportCoverageSettings::HISTOGRAM));
        break;
    case ExportCoverageSettings::PerBase: {
        QString exportTypeValue = getParameter(EXPORT_TYPE_ATTR_ID).toString();
        if (exportTypeValue.isEmpty()) {
            exportString = getHyperlink(EXPORT_TYPE_ATTR_ID, tr("nothing"));
        } else {
            exportTypeValue.replace(",", ", ");
            exportString =  exportTypeValue + " " + getHyperlink(FORMAT_ATTR_ID, ExportCoverageSettings::PER_BASE.toLower());
        }
        break;
    }
    case ExportCoverageSettings::Bedgraph:
        exportString = tr("coverage in \"%1\" format").arg(getHyperlink(FORMAT_ATTR_ID, ExportCoverageSettings::BEDGRAPH));
        break;
    }

    const QString threshold = getParameter(THRESHOLD_ATTR_ID).toString();
    const QString outputFile = getParameter(BaseAttributes::URL_OUT_ATTRIBUTE().getId()).toString();

    return tr("Exports %1 from the incoming assembly with threshold %2 to %3 in tab delimited plain text format.").
            arg(exportString).
            arg(getHyperlink(THRESHOLD_ATTR_ID, threshold)).
            arg(getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), outputFile));
}

ExtractAssemblyCoverageFileExtensionRelation::ExtractAssemblyCoverageFileExtensionRelation(const QString &relatedAttrId) :
    AttributeRelation(relatedAttrId)
{
}

QVariant ExtractAssemblyCoverageFileExtensionRelation::getAffectResult(const QVariant &influencingValue, const QVariant &dependentValue, DelegateTags * /*infTags*/, DelegateTags *depTags) const {
    const ExportCoverageSettings::Format newFormat = static_cast<ExportCoverageSettings::Format>(influencingValue.toInt());
    updateDelegateTags(influencingValue, depTags);

    QString urlStr = dependentValue.toString();
    if (urlStr.isEmpty()) {
        return "";
    }

    const QString newExtension = ExportCoverageSettings::getFormatExtension(newFormat);
    bool withGz = false;

    if (urlStr.endsWith(ExportCoverageSettings::COMPRESSED_EXTENSION)) {
        withGz = true;
        urlStr.chop(ExportCoverageSettings::COMPRESSED_EXTENSION.size());
    }

    const QString currentExtension = urlStr.mid(urlStr.lastIndexOf('.') + 1);
    if (currentExtension == ExportCoverageSettings::HISTOGRAM_EXTENSION ||
            currentExtension == ExportCoverageSettings::PER_BASE_EXTENSION ||
            currentExtension == ExportCoverageSettings::BEDGRAPH_EXTENSION) {
        urlStr.chop(currentExtension.size());
    }

    urlStr += newExtension + (withGz ? ExportCoverageSettings::COMPRESSED_EXTENSION : "");
    return urlStr;
}

void ExtractAssemblyCoverageFileExtensionRelation::updateDelegateTags(const QVariant &influencingValue, DelegateTags *dependentTags) const {
    const ExportCoverageSettings::Format newFormat = static_cast<ExportCoverageSettings::Format>(influencingValue.toInt());
    if (NULL != dependentTags) {
        dependentTags->set("extensions", QStringList() << ExportCoverageSettings::getFormatExtension(newFormat) << ExportCoverageSettings::getFormatExtension(newFormat) + ExportCoverageSettings::COMPRESSED_EXTENSION);
        const QString filter = FormatUtils::prepareFileFilter(ExportCoverageSettings::getFormat(newFormat) + " coverage files", QStringList() << ExportCoverageSettings::getFormatExtension(newFormat));
        dependentTags->set("filter", filter);
    }
}

RelationType ExtractAssemblyCoverageFileExtensionRelation::getType() const {
    return FILE_EXTENSION;
}

ExtractAssemblyCoverageFileExtensionRelation *ExtractAssemblyCoverageFileExtensionRelation::clone() const {
    return new ExtractAssemblyCoverageFileExtensionRelation(*this);
}

}   // namespace LocalWorkflow
}   // namespace U2
