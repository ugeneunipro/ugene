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

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/ConvertFileTask.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ConvertFilesFormatWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ConvertFilesFormatWorkerFactory::ACTOR_ID("files-conversion");
static const QString SHORT_NAME( "cff" );
static const QString INPUT_PORT( "in-file" );
static const QString OUTPUT_PORT( "out-file" );
static const QString OUTPUT_SUBDIR( "Converted_files/" );
static const QString EXCLUDED_FORMATS_ID( "excluded-formats" );
static const QString OUT_MODE_ID( "out-mode" );
static const QString CUSTOM_DIR_ID( "custom-dir" );

/************************************************************************/
/* ConvertFilesFormatPrompter */
/************************************************************************/
QString ConvertFilesFormatPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Convert file %1 to selected format if it is not excluded.").arg(producerName);
    return doc;
}

/************************************************************************/
/* ConvertFilesFormatWorkerFactory */
/************************************************************************/
namespace {
    enum OutDirectory{
        FILE_DIRECTORY = 0,
        WORKFLOW_INTERNAL,
        CUSTOM
    };
    enum MapType {IDS,
        BOOLEANS
    };
    QVariantMap getFormatsMap(MapType mapType) {
        const QList<DocumentFormatId> allFormats = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();

        QVariantMap result;
        foreach (const DocumentFormatId &fid, allFormats) {
            const DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(fid);
            if (NULL == format || format->checkFlags(DocumentFormatFlag_CannotBeCreated)) {
                continue;
            }
            if (format->checkFlags(DocumentFormatFlag_SupportWriting) || (BOOLEANS == mapType)) {
                if (BOOLEANS == mapType) {
                    result[fid] = false;
                } else {
                    result[fid] = fid;
                }
            }
        }
        return result;
    }
}

void ConvertFilesFormatWorkerFactory::init() {
    Descriptor desc( ACTOR_ID, ConvertFilesFormatWorker::tr("File Format Conversion"),
        ConvertFilesFormatWorker::tr("Converts the file to selected format if it is not excluded.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, ConvertFilesFormatWorker::tr("File"),
            ConvertFilesFormatWorker::tr("A file to perform format conversion"));
        Descriptor outD(OUTPUT_PORT, ConvertFilesFormatWorker::tr("File"),
            ConvertFilesFormatWorker::tr("File of selected format"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType(SHORT_NAME + ".input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType(SHORT_NAME + ".output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor excludedFormats(EXCLUDED_FORMATS_ID, ConvertFilesFormatWorker::tr("Excluded formats"),
            ConvertFilesFormatWorker::tr("Input file won't be converted to any of selected formats."));

        Descriptor outDir(OUT_MODE_ID, ConvertFilesFormatWorker::tr("Output folder"),
            ConvertFilesFormatWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(CUSTOM_DIR_ID, ConvertFilesFormatWorker::tr("Custom folder"),
            ConvertFilesFormatWorker::tr("Select the custom output folder."));

        a << new Attribute( BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true );
        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(OUT_MODE_ID, CUSTOM));
        a << customDirAttr;
        //a << new Attribute( customDir, BaseTypes::STRING_TYPE(), false, QString(""));
        a << new Attribute( excludedFormats, BaseTypes::STRING_TYPE(), false );
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap formatsIds = getFormatsMap(IDS);
        delegates[BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId()] = new ComboBoxDelegate(formatsIds);

        QVariantMap formatsBooleans = getFormatsMap(BOOLEANS);
        delegates[EXCLUDED_FORMATS_ID] = new ComboBoxWithChecksDelegate(formatsBooleans);

        QVariantMap directoryMap;
        QString fileDir = ConvertFilesFormatWorker::tr("Input file");
        QString workflowDir = ConvertFilesFormatWorker::tr("Workflow");
        QString customD = ConvertFilesFormatWorker::tr("Custom");
        directoryMap[fileDir] = FILE_DIRECTORY;
        directoryMap[workflowDir] = WORKFLOW_INTERNAL;
        directoryMap[customD] = CUSTOM;
        delegates[OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ConvertFilesFormatPrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CONVERTERS(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ConvertFilesFormatWorkerFactory());
}

/************************************************************************/
/* ConvertFilesFormatWorker */
/************************************************************************/
ConvertFilesFormatWorker::ConvertFilesFormatWorker(Actor *a)
: BaseWorker(a), inputUrlPort(NULL), outputUrlPort(NULL)
{

}

void ConvertFilesFormatWorker::init() {
    inputUrlPort = ports.value(INPUT_PORT);
    outputUrlPort = ports.value(OUTPUT_PORT);
    targetFormat = getValue<QString>(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId());
    excludedFormats = getValue<QString>(EXCLUDED_FORMATS_ID).split(",", QString::SkipEmptyParts);
}

bool ConvertFilesFormatWorker::ensureFileExists(const QString &url) {
    if (!QFile::exists(url)) {
        reportError(tr("The file does not exist: %1").arg(url));
        return false;
    }
    return true;
}

Task * ConvertFilesFormatWorker::tick() {
    if (inputUrlPort->hasMessage()) {
        const QString url = takeUrl();
        CHECK(!url.isEmpty(), NULL);
        CHECK(ensureFileExists(url), NULL);

        const QString detectedFormat = detectFormat(url);
        CHECK(!detectedFormat.isEmpty(), NULL);

        // without conversion
        if ((targetFormat == detectedFormat) || (excludedFormats.contains(detectedFormat))) {
            sendResult(url);
            return NULL;
        }

        Task *t = getConvertTask(detectedFormat, url);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return t;
    } else if (inputUrlPort->isEnded()) {
        setDone();
        outputUrlPort->setEnded();
    }
    return NULL;
}

void ConvertFilesFormatWorker::cleanup() {

}

namespace {
    QString getTargetUrl(Task *task) {
        ConvertFileTask *convertFileTask = dynamic_cast<ConvertFileTask*>(task);

        if (NULL != convertFileTask) {
            return convertFileTask->getResult();
        }
        return "";
    }
}

void ConvertFilesFormatWorker::sl_taskFinished(Task *task) {
    CHECK(!task->hasError(), );
    CHECK(!task->isCanceled(), );

    QString url = getTargetUrl(task);
    CHECK(!url.isEmpty(), );

    sendResult(url);
    monitor()->addOutputFile(url, getActorId());
}

QString ConvertFilesFormatWorker::createWorkingDir( const QString& fileUrl ){
    QString result;

    bool useInternal = false;

    int dirMode = getValue<int>(OUT_MODE_ID);

    if(dirMode == FILE_DIRECTORY){
        result = GUrl(fileUrl).dirPath() + "/";
    }else if (dirMode == CUSTOM){
        QString customDir = getValue<QString>(CUSTOM_DIR_ID);
        if (!customDir.isEmpty()){
            result = customDir;
            if (!result.endsWith("/")) {
                result += "/";
            }
        }else{
            algoLog.error(tr("Convert Format: result folder is empty, default workflow folder is used"));
            useInternal = true;
        }
    }else{
        useInternal = true;
    }

    if (useInternal){
        result = context->workingDir();
        if (!result.endsWith("/")) {
            result += "/";
        }
        result += OUTPUT_SUBDIR;
    }

    QDir dir(result);
    if (!dir.exists(result)) {
        dir.mkdir(result);
    }
    return result;
}

namespace {
    QString getFormatId(const FormatDetectionResult &r) {
        if (NULL != r.format) {
            return r.format->getFormatId();
        }
        if (NULL != r.importer) {
            return r.importer->getId();
        }
        return "";
    }
}

QString ConvertFilesFormatWorker::takeUrl() {
    const Message inputMessage = getMessageAndSetupScriptValues(inputUrlPort);
    if (inputMessage.isEmpty()) {
        outputUrlPort->transit();
        return "";
    }

    const QVariantMap data = inputMessage.getData().toMap();
    return data[BaseSlots::URL_SLOT().getId()].toString();
}

QString ConvertFilesFormatWorker::detectFormat(const QString &url) {
    FormatDetectionConfig cfg;
    cfg.bestMatchesOnly = false;
    cfg.useImporters = true;
    cfg.excludeHiddenFormats = false;

    const QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, cfg);
    if (formats.empty()) {
        reportError(tr("Unknown file format: ") + url);
        return "";
    }

    return getFormatId(formats.first());
}

void ConvertFilesFormatWorker::sendResult(const QString &url) {
    const Message message(BaseTypes::STRING_TYPE(), url);
    outputUrlPort->put(message);
}

Task * ConvertFilesFormatWorker::getConvertTask(const QString &detectedFormat, const QString &url) {
    QString workingDir = createWorkingDir(url);

    ConvertFactoryRegistry* r =  AppContext::getConvertFactoryRegistry();
    SAFE_POINT(r != NULL, "ConvertFilesFormatWorker::getConvertTask ConvertFactoryRegistry is NULL", NULL);
    ConvertFileFactory* f = r->getFactoryByFormats(detectedFormat, targetFormat);
    SAFE_POINT(f != NULL, "ConvertFilesFormatWorker::getConvertTask ConvertFileFactory is NULL", NULL);

    return f->getTask(url, detectedFormat, targetFormat, workingDir);
}


} //LocalWorkflow
} //U2
