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

#include <QCoreApplication>
#include <QDir>
#include <QStringList>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GHints.h>
#include <U2Core/GObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/SAMFormat.h>

#include <U2Gui/ImportWidget.h>

#include <U2Test/GTest.h>

#include "DocumentModelTests.h"

namespace U2 {

/* TRANSLATOR U2::GTest */

#define VALUE_ATTR "value"
#define DOC_ATTR   "doc"
#define NAME_ATTR   "name"
#define TYPE_ATTR   "type"

#define READ_BUFF_SIZE 4096

/*******************************
 * GTest_LoadDocument
 *******************************/
static QString getTempDir(const GTestEnvironment* env) {
    QString result = env->getVar("TEMP_DATA_DIR");
    if (result.isEmpty()) {
        result = QCoreApplication::applicationDirPath();
    }
    return result;
}

void GTest_LoadDocument::init(XMLTestFormat*, const QDomElement& el) {
    loadTask = NULL;
    contextAdded = false;
    docContextName = el.attribute("index");
    needVerifyLog = false;
    QVariantMap hints;

    if (NULL != el.attribute("message")) {
        expectedLogMessage = el.attribute("message");
    }

    if (NULL != el.attribute("message2")) {
        expectedLogMessage2 = el.attribute("message2");
    }

    if (NULL != el.attribute("no-message")) {
        unexpectedLogMessage = el.attribute("no-message");
    }

    if (NULL != el.attribute("sequence-mode")) {
        QString seqMode = el.attribute("sequence-mode");
        if ("msa" == seqMode) {
            hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
        } else if ("merge" == seqMode) {
            hints[DocumentReadingMode_SequenceMergeGapSize] = 10; // just default value
        } else if ("split" == seqMode) {
            hints[DocumentReadingMode_SequenceAsSeparateHint] = true;
        }
    }

    QString dir = el.attribute("dir");
    if(dir == "temp"){
        tempFile = true;
        url = getTempDir(env) + "/" + el.attribute("url");
    } else{
        tempFile = false;
        QString commonDataDir = env->getVar("COMMON_DATA_DIR");
        url =  commonDataDir + "/" + el.attribute("url");
    }
    IOAdapterId         io = el.attribute("io");
    IOAdapterFactory*   iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(io);
    DocumentFormatId    format = el.attribute("format");
    if (iof == NULL) {
        stateInfo.setError( QString("io_adapter_not_found_%1").arg(io));
    } else if (format.isEmpty()) {
        stateInfo.setError( QString("doc_format_is_not_specified"));
    } else {
        if (format == BaseDocumentFormats::SAM) {
            //SAM format is temporarily removed from base formats list -> create it manually
            SAMFormat* samFormat = new SAMFormat();
            loadTask = new LoadDocumentTask(samFormat, url, iof, hints);
            samFormat->setParent(loadTask);
        } else {
            loadTask = new LoadDocumentTask(format, url, iof, hints);
        }
        addSubTask(loadTask);
    }
}

void GTest_LoadDocument::cleanup() {
    if (contextAdded) {
        removeContext(docContextName);
    }
    if(!XMLTestUtils::parentTasksHaveError(this) && tempFile){
        taskLog.trace(QString("Temporary file removed: %1").arg(url));
        QFile::remove(url);
    }

    XmlTest::cleanup();
}

void GTest_LoadDocument::prepare() {
    QStringList expectedMessages;
    QStringList unexpectedMessages;

    if (!expectedLogMessage.isEmpty()) {
        expectedMessages << expectedLogMessage;
    }

    if (!expectedLogMessage2.isEmpty()) {
        expectedMessages << expectedLogMessage2;
    }

    if (!unexpectedLogMessage.isEmpty()){
        unexpectedMessages << unexpectedLogMessage;
    }
    if(expectedLogMessage.length()!=0 || unexpectedMessages.length()!=0){
        needVerifyLog=true;
        logHelper.initMessages(expectedMessages, unexpectedMessages);
    }
}

Task::ReportResult GTest_LoadDocument::report() {
    if (loadTask!=NULL && loadTask->hasError()) {
        stateInfo.setError( loadTask->getError());
    } else if (!docContextName.isEmpty()) {
        addContext(docContextName, loadTask->getDocument());
        contextAdded = true;

        if (needVerifyLog && GTest_LogHelper_Invalid == logHelper.verifyStatus()) {
            stateInfo.setError("Log is incorrect!");
        }
    }
    return ReportResult_Finished;
}


/*******************************
* GTest_SaveDocument
*******************************/
void GTest_SaveDocument::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    saveTask = NULL;
    QString dir = getTempDir(env);
    if (!QDir(dir).exists()) {
        bool ok = QDir::root().mkpath(dir);
        if (!ok) {
            setError(QString("Can't create TEMP_DATA_DIR dir: %1").arg(dir));
            return;
        }
    }

    url = dir + "/" + el.attribute("url");

    IOAdapterId io = el.attribute("io");
    iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(io);

    if (iof == NULL) {
        stateInfo.setError(QString("io_adapter_not_found_%1").arg(io));
        return ;
    }

    formatId = el.attribute("format");
}

void GTest_SaveDocument::prepare(){
    //////////////////////
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return;
    }

    SaveDocFlags saveTaskFlags = SaveDoc_Overwrite;
    if (!formatId.isEmpty() && formatId != doc->getDocumentFormatId()) {
        DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
        CHECK_EXT(NULL != format, stateInfo.setError(QString("Document format not found: %1").arg(formatId)), );
        doc = doc->getSimpleCopy(format, iof, url);
        saveTaskFlags |= SaveDoc_DestroyButDontUnload;
    }

    saveTask = new SaveDocumentTask(doc, iof, url, saveTaskFlags);
    addSubTask(saveTask);
    /////////////////////////
}

/*******************************
* GTest_LoadBrokenDocument
*******************************/
void GTest_LoadBrokenDocument::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QString             urlAttr = el.attribute("url");
    QString             dir = el.attribute("dir");
    IOAdapterId         io = el.attribute("io");
    IOAdapterFactory*   iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(io);
    DocumentFormatId    format = el.attribute("format");

    tempFile = (dir == "temp");

    if (dir == "temp") {
        url = getTempDir(env) + "/" + urlAttr;
    } else {
        url = env->getVar("COMMON_DATA_DIR") + "/" + urlAttr;
    }

    message = el.attribute("message");

    QVariantMap hints;
    if (NULL != el.attribute("sequence-mode")) {
        QString seqMode = el.attribute("sequence-mode");
        if ("msa" == seqMode) {
            hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
        } else if ("merge" == seqMode) {
            hints[DocumentReadingMode_SequenceMergeGapSize] = 10; // just default value
        }
    }

    loadTask = new LoadDocumentTask(format, url, iof, hints);
    addSubTask(loadTask);
}

Task::ReportResult GTest_LoadBrokenDocument::report() {
    Document* doc = loadTask->getDocument();
    if (doc == NULL && loadTask->hasError()) {
        CHECK(!message.isEmpty(), ReportResult_Finished);
        if (loadTask->getError().contains(message)) {
            return ReportResult_Finished;
        }
        stateInfo.setError(QString("expected message is not found"));
        return ReportResult_Finished;
    }
    stateInfo.setError(QString("file read without errors"));
    return ReportResult_Finished;
}

void GTest_LoadBrokenDocument::cleanup() {
    if (!XMLTestUtils::parentTasksHaveError(this) && tempFile) {
        QFile::remove(url);
    }

    XmlTest::cleanup();
}

/*******************************
 * GTest_ImportDocument
 *******************************/
void GTest_ImportDocument::init(XMLTestFormat*, const QDomElement& el) {
    importTask = NULL;
    contextAdded = false;
    DocumentFormatId outFormatId = el.attribute("outFormat");
    QString outUrlAttr = el.attribute("outUrl");
    docContextName = el.attribute("index");
    needVerifyLog = false;

    if (NULL != el.attribute("message")) {
        expectedLogMessage = el.attribute("message");
    }

    if (NULL != el.attribute("message2")) {
        expectedLogMessage2 = el.attribute("message2");
    }

    if (NULL != el.attribute("no-message")) {
        unexpectedLogMessage = el.attribute("no-message");
    }

    QString dir = el.attribute("dir");
    if(dir == "temp"){
        tempFile = true;
        url = getTempDir(env) + "/" + el.attribute("url");
    } else{
        tempFile = false;
        QString commonDataDir = env->getVar("COMMON_DATA_DIR");
        url =  commonDataDir + "/" + el.attribute("url");
    }

    DocumentFormatId formatId = el.attribute("format");
    if (formatId.isEmpty()) {
        stateInfo.setError( QString("doc_format_is_not_specified"));
        return;
    }

    destUrl = getTempDir(env) + "/" + url.mid(url.lastIndexOf("/")) + ".ugenedb";

    FormatDetectionConfig conf;
    conf.useImporters = true;
    QList<FormatDetectionResult> detectionRes = DocumentUtils::detectFormat(url, conf);
    CHECK_EXT(!detectionRes.isEmpty(), setError("Format is not recognized"), );

    FormatDetectionResult* bestRes = NULL;
    for (int i = 0; i < detectionRes.size(); ++i) {
        if (NULL != detectionRes[i].importer && detectionRes[i].importer->getFormatIds().contains(formatId)) {
            bestRes = &detectionRes[i];
            break;
        }
    }
    CHECK_EXT(NULL != bestRes && NULL != bestRes->importer, setError(QString("Can't find an importer for format: %1").arg(formatId)), );

    QVariantMap hints;
    U2DbiRef destDb(SQLITE_DBI_ID, destUrl);
    QVariant variant;
    variant.setValue<U2DbiRef>(destDb);
    hints.insert(ImportHint_FormatId, outFormatId);
    hints.insert(ImportHint_DestinationUrl, outUrlAttr);
    hints.insert(DocumentFormat::DBI_REF_HINT, variant);

    importTask = bestRes->importer->createImportTask(*bestRes, false, hints);

    addSubTask(importTask);
}

void GTest_ImportDocument::cleanup() {
    if (contextAdded) {
        removeContext(docContextName);
    }

    if (!XMLTestUtils::parentTasksHaveError(this)) {
        if (tempFile) {
            QFile::remove(url);
        }

        QFile::remove(destUrl);
    }

    XmlTest::cleanup();
}

void GTest_ImportDocument::prepare() {
    QStringList expectedMessages;
    QStringList unexpectedMessages;

    if (!expectedLogMessage.isEmpty()) {
        expectedMessages << expectedLogMessage;
    }

    if (!expectedLogMessage2.isEmpty()) {
        expectedMessages << expectedLogMessage2;
    }

    if (!unexpectedLogMessage.isEmpty()){
        unexpectedMessages << unexpectedLogMessage;
    }
    if (expectedLogMessage.length() != 0 || unexpectedMessages.length() != 0) {
        needVerifyLog = true;
        logHelper.initMessages(expectedMessages, unexpectedMessages);
    }
}

Task::ReportResult GTest_ImportDocument::report() {
    if (NULL != importTask && importTask->hasError()) {
        stateInfo.setError(importTask->getError());
    } else if (!docContextName.isEmpty()) {
        addContext(docContextName, importTask->getDocument());
        contextAdded = true;

        if (needVerifyLog && GTest_LogHelper_Invalid == logHelper.verifyStatus()) {
            stateInfo.setError("Log is incorrect!");
        }
    }
    return ReportResult_Finished;
}

/*******************************
* GTest_ImportBrokenDocument
*******************************/
void GTest_ImportBrokenDocument::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QString             urlAttr = el.attribute("url");
    QString             outUrlAttr = getTempDir(env) + "/" + el.attribute("outUrl");
    QString             dir = el.attribute("dir");
    DocumentFormatId    formatId = el.attribute("format");

    tempFile = (dir == "temp");

    if (dir == "temp") {
        url = getTempDir(env) + "/" + urlAttr;
    } else {
        url = env->getVar("COMMON_DATA_DIR") + "/" + urlAttr;
    }

    destUrl = getTempDir(env) + "/" + url.mid(url.lastIndexOf("/")) + ".ugenedb";

    message = el.attribute("message");

    FormatDetectionConfig conf;
    conf.useImporters = true;
    QList<FormatDetectionResult> detectionRes = DocumentUtils::detectFormat(url, conf);
    CHECK_EXT(!detectionRes.isEmpty(), setError("Format is not recognized"), );

    FormatDetectionResult* bestRes = NULL;
    for (int i = 0; i < detectionRes.size(); ++i) {
        if (NULL != detectionRes[i].importer && detectionRes[i].importer->getFormatIds().contains(formatId)) {
            bestRes = &detectionRes[i];
            break;
        }
    }
    CHECK_EXT(NULL != bestRes && NULL != bestRes->importer, setError(QString("Can't find an importer for format: %1").arg(formatId)), );

    QVariantMap hints;
    U2DbiRef destDb(SQLITE_DBI_ID, destUrl);
    QVariant variant;
    variant.setValue<U2DbiRef>(destDb);
    hints.insert(DocumentFormat::DBI_REF_HINT, variant);
    hints.insert(ImportHint_DestinationUrl, outUrlAttr);
    importTask = bestRes->importer->createImportTask(*bestRes, false, hints);

    addSubTask(importTask);
}

Task::ReportResult GTest_ImportBrokenDocument::report() {
    Document* doc = importTask->getDocument();
    if (doc == NULL && importTask->hasError()) {
        CHECK(!message.isEmpty(), ReportResult_Finished);
        if (importTask->getError().contains(message)) {
            return ReportResult_Finished;
        }
        stateInfo.setError(QString("expected message is not found"));
        return ReportResult_Finished;
    }
    stateInfo.setError(QString("file read without errors"));
    return ReportResult_Finished;
}

void GTest_ImportBrokenDocument::cleanup() {
    if (!XMLTestUtils::parentTasksHaveError(this)) {
        if (tempFile) {
            QFile::remove(url);
        }
        QFile::remove(destUrl);
    }

    XmlTest::cleanup();
}

/*******************************
*GTest_DocumentFormat
*******************************/

void GTest_DocumentFormat::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docUrl = el.attribute("url");
    if (docUrl.isEmpty()) {
        failMissingValue("url");
        return;
    }
    docUrl = env->getVar("COMMON_DATA_DIR") + "/" + docUrl;

    docFormat = el.attribute(VALUE_ATTR);
    if (docFormat.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
}

Task::ReportResult GTest_DocumentFormat::report() {

    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(GUrl(docUrl));
    if(formats.isEmpty()) {
        stateInfo.setError(QString("Can't detect format for file %1").arg(docUrl));
        return ReportResult_Finished;
    }

    QString format = formats.first().format->getFormatId();
    if (format != docFormat) {
        stateInfo.setError(QString("Format not matched: %1, expected %2").arg(format).arg(docFormat));
    }
    return ReportResult_Finished;
}

/*******************************
* GTest_DocumentNumObjects
*******************************/
void GTest_DocumentNumObjects::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    bool ok = false;
    numObjs = v.toInt(&ok);
    if (!ok) {
        failMissingValue(VALUE_ATTR);
    }
}

Task::ReportResult GTest_DocumentNumObjects::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    int num = doc->getObjects().size();
    if (num != numObjs) {
        stateInfo.setError(QString("number of objects in document not matched: %1, expected %2").arg(num).arg(numObjs));
    }
    return ReportResult_Finished;
}

/*******************************
* GTest_DocumentObjectNames
*******************************/
void GTest_DocumentObjectNames::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    names =  v.split(",");
}


Task::ReportResult GTest_DocumentObjectNames::report() {
    Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    const QList<GObject*>& objs = doc->getObjects();
    int namesSize = names.size();
    int objsSize = objs.size();
    if (namesSize != objsSize) {
        stateInfo.setError(QString("number of objects in document not matched: %1, expected %2").arg(objs.size()).arg(names.size()));
        return ReportResult_Finished;
    }
    QStringList objNames;
    foreach(GObject *ob, objs) {
        objNames.append(ob->getGObjectName());
    }
    qSort(objNames);
    qSort(names);
    for(int i=0; i<names.size(); i++) {
        QString name = names[i];
        QString objName = objNames[i];
        if (name != objName) {
            stateInfo.setError(QString("name of the objects not matched: %1, expected %2").arg(objName).arg(name));
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

/*******************************
* GTest_DocumentObjectTypes
*******************************/
void GTest_DocumentObjectTypes::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    types =  v.split(",");
}

Task::ReportResult GTest_DocumentObjectTypes::report() {
    const Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    const QList<GObject*>& objs = doc->getObjects();
    int typesSize = types.size();
    int objsSize = objs.size();
    if (typesSize != objsSize) {
        stateInfo.setError(QString("number of objects in document not matched: %1, expected %2").arg(objs.size()).arg(types.size()));
        return ReportResult_Finished;
    }
    QStringList objTypes;
    foreach(GObject *ob, objs) {
        objTypes.append(ob->getGObjectType());
    }
    qSort(objTypes);
    qSort(types);
    for(int i=0; i<types.size(); i++) {
        GObjectType type = types[i];
        GObjectType objType = objTypes[i];
        if (type != objType) {
            stateInfo.setError(QString("types of the objects not matched: %1, expected %2").arg(objType).arg(type));
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

/*******************************
* GTest_FindGObjectByName
*******************************/
void GTest_FindGObjectByName::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    docContextName = el.attribute(DOC_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    objName = el.attribute(NAME_ATTR);
    if (objName.isEmpty()) {
        failMissingValue(NAME_ATTR);
        return;
    }

    type = el.attribute(TYPE_ATTR);
    if (type.isEmpty()) {
        failMissingValue(TYPE_ATTR);
        return;
    }

    objContextName = el.attribute("index");

    result = NULL;

}

Task::ReportResult GTest_FindGObjectByName::report() {
    const Document* doc = getContext<Document>(this, docContextName);
    if (doc == NULL) {
        stateInfo.setError(QString("document not found %1").arg(docContextName));
        return ReportResult_Finished;
    }
    const QList<GObject*>& objs = doc->getObjects();

    foreach(GObject* obj, objs) {
        QString objectType = obj->getGObjectType();
        QString objectName = obj->getGObjectName();
        if ((objectType == type) && (objectName == objName)) {
            result = obj;
            break;
        }
    }
    if (result == NULL) {
        stateInfo.setError(QString("object not found: name '%1',type '%2' ").arg(objName).arg(type));
    } else if (!objContextName.isEmpty()) {
        addContext(objContextName, result);
    }
    return ReportResult_Finished;
}


void GTest_FindGObjectByName::cleanup() {
    if (result!=NULL && !objContextName.isEmpty()) {
        removeContext(objContextName);
    }

    XmlTest::cleanup();
}


/*******************************
 * GTest_CompareFiles
 *******************************/

static const QString DOC1_ATTR_ID = "doc1";
static const QString DOC2_ATTR_ID = "doc2";
static const QString TMP_ATTR_ID  = "temp";
static const QString TMP_ATTR_SPLITTER = ",";
static const QString BY_LINES_ATTR_ID = "by_lines";
static const QString COMMENTS_START_WITH = "comments_start_with";
static const QString COMPARE_LINE_NUMBER_ONLY = "line_num_only";
static const QString COMPARE_FIRST_N_LINES = "first_n_lines";
static const QString COMPARE_MIXED_LINES = "mixed-lines";
static const QString COMPARE_FORCE_BUFFER_SIZE = "buffer-size";

void GTest_CompareFiles::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    // Get the attributes values
    QString tmpAttr = el.attribute(TMP_ATTR_ID);

    doc1Path = el.attribute(DOC1_ATTR_ID);
    if(doc1Path.isEmpty()) {
        failMissingValue(DOC1_ATTR_ID);
        return;
    }

    doc2Path = el.attribute(DOC2_ATTR_ID);
    if(doc2Path.isEmpty()) {
        failMissingValue(DOC2_ATTR_ID);
        return;
    }

    if (!el.attribute(COMPARE_FIRST_N_LINES).isEmpty()) {
        first_n_lines = el.attribute(COMPARE_FIRST_N_LINES).toInt();
    } else {
        first_n_lines = -1;
    }

    if (!el.attribute(COMPARE_LINE_NUMBER_ONLY).isEmpty()) {
        line_num_only = true;
    } else {
        line_num_only = false;
    }

    if (!el.attribute(COMPARE_MIXED_LINES).isEmpty()) {
        mixed_lines = true;
    } else {
        mixed_lines = false;
    }

    if (!el.attribute(COMPARE_FORCE_BUFFER_SIZE).isEmpty()) {
        forceBufferSize = el.attribute(COMPARE_FORCE_BUFFER_SIZE).toInt();
    } else {
        forceBufferSize = 0;
    }

    // Get the full documents paths
    if (!tmpAttr.isEmpty()) {
        // Attribute "tmp" is used to determine paths prefixes,
        // specified paths are relative (used in all old tests, obsolete)
        QStringList tmpDocNums = tmpAttr.split(TMP_ATTR_SPLITTER, QString::SkipEmptyParts);
        doc1Path = (tmpDocNums.contains("1") ? env->getVar("TEMP_DATA_DIR") : env->getVar("COMMON_DATA_DIR")) + "/" + doc1Path;
        doc2Path = (tmpDocNums.contains("2") ? env->getVar("TEMP_DATA_DIR") : env->getVar("COMMON_DATA_DIR")) + "/" + doc2Path;
        byLines = !el.attribute(BY_LINES_ATTR_ID).isEmpty();
        if(el.attribute(COMMENTS_START_WITH).isEmpty()){
            commentsStartWith=QStringList();
        }else{
            QString commentsStartWithString=el.attribute(COMMENTS_START_WITH);
            commentsStartWith=commentsStartWithString.split(",");
        }
    }
    else {
        // Only "doc1" and "doc2" attributes are specified,
        // paths contain prefixes, e.g. "!common_data_dir!"
        XMLTestUtils::replacePrefix(env, doc1Path);
        XMLTestUtils::replacePrefix(env, doc2Path);
    }
}

static const qint64 READ_LINE_MAX_SZ = 2048;

Task::ReportResult GTest_CompareFiles::report() {
    if(mixed_lines){
        compareMixed();
        return ReportResult_Finished;
    }

    QFile f1(doc1Path);
    if(!f1.open(QIODevice::ReadOnly)) {
        setError(QString("Cannot open file '%1'!").arg(doc1Path));
        return ReportResult_Finished;
    }

    QFile f2(doc2Path);
    if(!f2.open(QIODevice::ReadOnly)) {
        setError(QString("Cannot open file '%1'!").arg(doc2Path));
        return ReportResult_Finished;
    }

    int lineNum = 0;
    while(1) {
        if(first_n_lines != -1 && lineNum >= first_n_lines){
            break;
        }
        QByteArray bytes1 = f1.readLine(READ_LINE_MAX_SZ);
        QByteArray bytes2 = f2.readLine(READ_LINE_MAX_SZ);

        if(bytes1.isEmpty() || bytes2.isEmpty()) {
            if( bytes1 != bytes2 ) {
                setError(QString("The files %1 and %2 are of different sizes!").arg(f1.fileName()).arg(f2.fileName()));
                return ReportResult_Finished;
            }
            break;
        }

        if(byLines) {
            bytes1 = bytes1.trimmed();
            bytes2 = bytes2.trimmed();
        }

        if(line_num_only){
            //do not compare lines values
            continue;
        }

        if(commentsStartWith.isEmpty()){
            if( bytes1 != bytes2 ) {
                setError(QString("The files \'%1\' and \'%2\' are not equal at line %3."
                    "The first file contains '%4' and the second contains '%5'!").arg(f1.fileName()).arg(f2.fileName())
                    .arg(lineNum).arg(QString(bytes1)).arg(QString(bytes2)));
                return ReportResult_Finished;
            }
        }else{
            foreach (QString commentStartWith, commentsStartWith) {
                if(!bytes1.startsWith(commentStartWith.toLatin1()) && !bytes2.startsWith(commentStartWith.toLatin1())){
                    if( bytes1 != bytes2 ) {
                        setError(QString("The files %1 and %2 are not equal at line %3."
                            "The first file contains '%4' and the second contains '%5'!")
                            .arg(f1.fileName()).arg(f2.fileName()).arg(lineNum).arg(QString(bytes1)).arg(QString(bytes2)));
                        return ReportResult_Finished;
                    }
                }else
                if(!(bytes1.startsWith(commentStartWith.toLatin1())&&bytes2.startsWith(commentStartWith.toLatin1()))){
                    setError(QString("The files have comments and are not equal at line %1."
                        "The first file contains '%2' and the second contains '%3'!")
                        .arg(lineNum).arg(QString(bytes1)).arg(QString(bytes2)));
                    return ReportResult_Finished;
                }

            }
        }

        if(bytes1.endsWith("\n") || byLines) {
            lineNum++;
        }
    }

    return ReportResult_Finished;
}

IOAdapter* GTest_CompareFiles::createIoAdapter(const QString& filePath) {
    IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filePath));
    CHECK_EXT(NULL != factory, setError("IOAdapterFactory is NULL"), NULL);
    IOAdapter* ioAdapter = factory->createIOAdapter();

    if (!ioAdapter->open(filePath, IOAdapterMode_Read)) {
        delete ioAdapter;
        setError(QString("Can't open file '%1'").arg(filePath));
        return NULL;
    }

    return ioAdapter;
}


QByteArray GTest_CompareFiles::getLine(IOAdapter* io) {
    QByteArray line;

    const qint64 bufferSize = forceBufferSize > 0 ? forceBufferSize : READ_BUFF_SIZE;
    QByteArray readBuff(bufferSize + 1, 0);
    char* buff = readBuff.data();
    bool commentString = false;

    do {
        bool lineOk = true;
        qint64 len = io->readUntil(buff, bufferSize, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        CHECK(len != 0, "");
        CHECK_EXT(lineOk, setError("Line is too long"), "");

        line = (QByteArray(buff, len)).trimmed();
        commentString = false;
        foreach(const QString& comment, commentsStartWith){
            if (line.startsWith(comment.toLatin1())){
                commentString = true;
                break;
            }
        }
    } while (commentString);

    return line;
}


void GTest_CompareFiles::compareMixed(){
    QScopedPointer<IOAdapter> doc1Adapter(createIoAdapter(doc1Path));
    CHECK_OP(stateInfo, );

    int lineNum = 0;
    while(!doc1Adapter->isEof()) {
        QByteArray bytes1 = getLine(doc1Adapter.data());
        lineNum++;

        bool found = false;

        QScopedPointer<IOAdapter> doc2Adapter(createIoAdapter(doc2Path));
        CHECK_OP(stateInfo, );
        while(!doc2Adapter->isEof()){
            QByteArray bytes2 = getLine(doc2Adapter.data());
            if (bytes1 == bytes2) {
                found = true;
                break;
            }
        }
        doc2Adapter->close();

        if(!found){
            setError(QString("Cannot find line %1. \n%2\n in \n%3\n").arg(lineNum).arg(QString(bytes1)).arg(QString(doc2Path)));
            return;
        }
    }

}

/*******************************
 * GTest_Compare_VCF_Files
 *******************************/
const QByteArray GTest_Compare_VCF_Files::COMMENT_MARKER = "#";

void GTest_Compare_VCF_Files::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QStringList tmpDocNums = el.attribute(TMP_ATTR_ID).split(TMP_ATTR_SPLITTER, QString::SkipEmptyParts);

    doc1Path = el.attribute(DOC1_ATTR_ID);
    if(doc1Path.isEmpty()) {
        failMissingValue(DOC1_ATTR_ID);
        return;
    }
    doc1Path = (tmpDocNums.contains("1") ? env->getVar( "TEMP_DATA_DIR" ) : env->getVar("COMMON_DATA_DIR")) + "/" + doc1Path;

    doc2Path = el.attribute(DOC2_ATTR_ID);
    if(doc2Path.isEmpty()) {
        failMissingValue(DOC2_ATTR_ID);
        return;
    }
    doc2Path = (tmpDocNums.contains("2") ? env->getVar( "TEMP_DATA_DIR" ) : env->getVar("COMMON_DATA_DIR")) + "/" + doc2Path;
}

Task::ReportResult GTest_Compare_VCF_Files::report() {
    QScopedPointer<IOAdapter> doc1Adapter(createIoAdapter(doc1Path));
    CHECK_OP(stateInfo, ReportResult_Finished);

    QScopedPointer<IOAdapter> doc2Adapter(createIoAdapter(doc2Path));
    CHECK_OP(stateInfo, ReportResult_Finished);

    QStringList vcfList1;
    while(!doc1Adapter->isEof()) {
        QString line = getLine(doc1Adapter.data());
        vcfList1.append(line);
    }
    QStringList vcfList2;
    while(!doc2Adapter->isEof()) {
        QString line = getLine(doc2Adapter.data());
        vcfList2.append(line);
    }
    if (vcfList1.size() != vcfList2.size()) {
        setError("files are of different size");
        return ReportResult_Finished;
    }
    vcfList1.sort();
    vcfList2.sort();
    for(int i = 0; i < vcfList1.size(); i++){
        if(vcfList1.at(i) != vcfList2.at(i)){
            setError(QString("The files %1 and %2 are not equal."
                            "The first file contains '%3'' and the second contains '%4'!")
                            .arg(doc1Path).arg(doc2Path).arg(vcfList1.at(i)).arg(vcfList2.at(i)));
                        return ReportResult_Finished;
        }
    }

    return ReportResult_Finished;
}

IOAdapter* GTest_Compare_VCF_Files::createIoAdapter(const QString& filePath) {
    IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filePath));
    CHECK_EXT(NULL != factory, setError("IOAdapterFactory is NULL"), NULL);
    IOAdapter* ioAdapter = factory->createIOAdapter();

    if (!ioAdapter->open(filePath, IOAdapterMode_Read)) {
        delete ioAdapter;
        setError(QString("Can't open file '%1'").arg(filePath));
        return NULL;
    }

    return ioAdapter;
}

QString GTest_Compare_VCF_Files::getLine(IOAdapter* io) {
    QByteArray line;

    QByteArray readBuff(READ_BUFF_SIZE + 1, 0);
    char* buff = readBuff.data();

    do {
        bool lineOk = true;
        qint64 len = io->readUntil(buff, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        CHECK(len != 0, "");
        CHECK_EXT(lineOk, setError("Line is too long"), "");

        line = (QByteArray(buff, len)).trimmed();
    } while (line.startsWith(COMMENT_MARKER));

    return QString(line);
}

/*******************************
 * GTest_PDF_CompareFiles
 *******************************/
const int NUMDER_OF_LINES = 10;

void GTest_Compare_PDF_Files::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QStringList tmpDocNums = el.attribute(TMP_ATTR_ID).split(TMP_ATTR_SPLITTER, QString::SkipEmptyParts);

    doc1Path = el.attribute(DOC1_ATTR_ID);
    if(doc1Path.isEmpty()) {
        failMissingValue(DOC1_ATTR_ID);
        return;
    }
    doc1Path = (tmpDocNums.contains("1") ? env->getVar( "TEMP_DATA_DIR" ) : env->getVar("COMMON_DATA_DIR")) + "/" + doc1Path;

    doc2Path = el.attribute(DOC2_ATTR_ID);
    if(doc2Path.isEmpty()) {
        failMissingValue(DOC2_ATTR_ID);
        return;
    }
    doc2Path = (tmpDocNums.contains("2") ? env->getVar( "TEMP_DATA_DIR" ) : env->getVar("COMMON_DATA_DIR")) + "/" + doc2Path;

    byLines = !el.attribute(BY_LINES_ATTR_ID).isEmpty();
}

Task::ReportResult GTest_Compare_PDF_Files::report() {
    QFile f1(doc1Path);
    if(!f1.open(QIODevice::ReadOnly)) {
        setError(QString("Cannot open %1 file").arg(doc1Path));
        return ReportResult_Finished;
    }

    QFile f2(doc2Path);
    if(!f2.open(QIODevice::ReadOnly)) {
        setError(QString("Cannot open %1 file").arg(doc2Path));
        return ReportResult_Finished;
    }

    int lineNum = 0;
    int i=0;
    while(1) {
        QByteArray bytes1 = f1.readLine(READ_LINE_MAX_SZ);
        QByteArray bytes2 = f2.readLine(READ_LINE_MAX_SZ);

        if(i<NUMDER_OF_LINES){
            i++;
            continue;
        }//skip first lines containing file info

        if(bytes1.isEmpty() || bytes2.isEmpty()) {
            if( bytes1 != bytes2 ) {
                setError(QString("files are of different size"));
                return ReportResult_Finished;
            }
            break;
        }

        if(byLines) {
            bytes1 = bytes1.trimmed();
            bytes2 = bytes2.trimmed();
        }

        if( bytes1 != bytes2 ) {
            setError(QString("files are note equal at line %1. %2 and %3").arg(lineNum).arg(QString(bytes1)).arg(QString(bytes2)));
            return ReportResult_Finished;
        }

        if(bytes1.endsWith("\n") || byLines) {
            lineNum++;
        }
    }

    return ReportResult_Finished;
}



/*******************************
* DocumentModelTests
*******************************/
QList<XMLTestFactory*> DocumentModelTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_LoadDocument::createFactory());
    res.append(GTest_LoadBrokenDocument::createFactory());
    res.append(GTest_ImportDocument::createFactory());
    res.append(GTest_ImportBrokenDocument::createFactory());
    res.append(GTest_DocumentFormat::createFactory());
    res.append(GTest_DocumentNumObjects::createFactory());
    res.append(GTest_DocumentObjectNames::createFactory());
    res.append(GTest_DocumentObjectTypes::createFactory());
    res.append(GTest_FindGObjectByName::createFactory());
    res.append(GTest_SaveDocument::createFactory());
    res.append(GTest_CompareFiles::createFactory());
    res.append(GTest_Compare_VCF_Files::createFactory());
    res.append(GTest_Compare_PDF_Files::createFactory());
    return res;
}


} // namespace
