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
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/BAMUtils.h>

#include "ConvertFileTask.h"
#include "DocumentFormatUtils.h"

namespace U2 {

namespace{
    bool isBamConversion(const QString &srcFormat, const QString &dstFormat) {
        bool isSrcSam = (srcFormat == BaseDocumentFormats::SAM);
        bool isSrcBam = (srcFormat == BaseDocumentFormats::BAM);
        bool isDstSam = (dstFormat == BaseDocumentFormats::SAM);
        bool isDstBam = (dstFormat == BaseDocumentFormats::BAM);

        return (isSrcSam && isDstBam) || (isSrcBam && isDstSam);
    }

}


//////////////////////////////////////////////////////////////////////////
//ConvertFileTask
ConvertFileTask::ConvertFileTask(const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &workingDir)
: Task(DocumentFormatUtils::tr("Conversion file from %1 to %2").arg(detectedFormat).arg(targetFormat), TaskFlags_FOSE_COSC),
    sourceURL(sourceURL), detectedFormat(detectedFormat), targetFormat(targetFormat), workingDir(workingDir)
{
    if (!workingDir.endsWith("/") && !workingDir.endsWith("\\")) {
        this->workingDir += "/";
    }
}

GUrl ConvertFileTask::getSourceURL() const {
    return sourceURL;
}

QString ConvertFileTask::getResult() const {
    return targetUrl;
}

void ConvertFileTask::run(){

}

//////////////////////////////////////////////////////////////////////////
//DefaultConvertFileTask
DefaultConvertFileTask::DefaultConvertFileTask( const GUrl &sourceUrl, const QString &detectedFormat, const QString &targetFormat, const QString &dir)
    : ConvertFileTask(sourceUrl, detectedFormat, targetFormat, dir),
      loadTask(NULL),
      saveTask(NULL)
{

}

DefaultConvertFileTask::DefaultConvertFileTask(const GUrl &sourceUrl, const QString &detectedFormat, const QString &targetUrl, const QString &targetFormat, const QString &dir)
    : ConvertFileTask(sourceUrl, detectedFormat, targetFormat, dir),
      loadTask(NULL),
      saveTask(NULL)
{
    this->targetUrl = targetUrl;
}

void DefaultConvertFileTask::prepare() {
    loadTask = LoadDocumentTask::getDefaultLoadDocTask(sourceURL);
    if (NULL == loadTask) {
        coreLog.info( QString("Cannot load file %1").arg(sourceURL.getURLString()) );
        return;
    }
    addSubTask(loadTask);
}

QList<Task*> DefaultConvertFileTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    CHECK(!subTask->hasError() && !subTask->isCanceled(), result);
    CHECK(!hasError() && !isCanceled(), result);

    if (saveTask == subTask) {
        return result;
    }
    SAFE_POINT_EXT(loadTask == subTask, setError("Unknown subtask"), result);

    bool mainThread = false;
    Document *srcDoc = loadTask->getDocument(mainThread);
    SAFE_POINT_EXT(NULL != srcDoc, setError("NULL document"), result);

    DocumentFormatRegistry *dfr = AppContext::getDocumentFormatRegistry();
    DocumentFormat *df = dfr->getFormatById(targetFormat);
    SAFE_POINT_EXT(NULL != df, setError("NULL document format"), result);

    QSet<GObjectType> selectedFormatObjectsTypes = df->getSupportedObjectTypes();
    QSet<GObjectType> inputFormatObjectTypes;
    QListIterator<GObject*> objectsIterator(srcDoc->getObjects());
    while (objectsIterator.hasNext()) {
        GObject *obj = objectsIterator.next();
        inputFormatObjectTypes << obj->getGObjectType();
    }
    inputFormatObjectTypes.intersect(selectedFormatObjectsTypes);
    if (inputFormatObjectTypes.empty()) {
        setError(tr("The formats are not compatible: %1 and %2").arg(srcDoc->getDocumentFormatId()).arg(targetFormat));
        return result;
    }

    QString ext = targetFormat;
    if (!df->getSupportedDocumentFileExtensions().isEmpty()) {
        ext = df->getSupportedDocumentFileExtensions().first();
    }

    if (targetUrl.isEmpty()) {
        QString fileName = srcDoc->getName() + "." + ext;
        targetUrl = GUrlUtils::rollFileName(workingDir + fileName, QSet<QString>());
    } else {
        if (QFileInfo(targetFormat).suffix() != ext) {
            targetUrl += "." + ext;
        }
        targetUrl = GUrlUtils::rollFileName(targetUrl, QSet<QString>());
    }

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(srcDoc->getURL()));
    Document *dstDoc = srcDoc->getSimpleCopy(df, iof, srcDoc->getURL());

    saveTask = new SaveDocumentTask(dstDoc, iof, targetUrl);
    result << saveTask;
    return result;
}

//////////////////////////////////////////////////////////////////////////
//BamSamConversionTask
BamSamConversionTask::BamSamConversionTask(const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &dir)
:ConvertFileTask(sourceURL, detectedFormat, targetFormat, dir)
,samToBam(true)
{

}
void BamSamConversionTask::prepare(){
    samToBam = (detectedFormat == BaseDocumentFormats::SAM);
    QString extension = (samToBam) ? ".bam" : ".sam";
    QString destURL = workingDir + QFileInfo(sourceURL.getURLString()).fileName() + extension;
    targetUrl = GUrlUtils::rollFileName(destURL, QSet<QString>());
}


void BamSamConversionTask::run() {
    BAMUtils::ConvertOption options(samToBam);
    if (samToBam) {
        BAMUtils::convertToSamOrBam(sourceURL, targetUrl, options, stateInfo);
        CHECK_OP(stateInfo, );

        QString sortedBamBase = targetUrl + ".sorted";
        targetUrl = BAMUtils::sortBam(targetUrl, sortedBamBase, stateInfo).getURLString();
        CHECK_OP(stateInfo, );

        BAMUtils::createBamIndex(targetUrl, stateInfo);
    } else {
        BAMUtils::convertToSamOrBam(targetUrl, sourceURL, options, stateInfo);
    }
}

//////////////////////////////////////////////////////////////////////////
//Factories and registries

bool BAMConvertFactory::isCustomFormatTask( const QString& detectedFormat, const QString& targetFormat ){
    return isBamConversion(detectedFormat, targetFormat);
}

//////////////////////////////////////////////////////////////////////////
//ConvertFactoryRegistry
ConvertFactoryRegistry::ConvertFactoryRegistry( QObject *o )
:QObject(o)
{
    //init factories
    //default factory always goes last
    factories.append(new BAMConvertFactory());
    factories.append(new ConvertFileFactory());
}

ConvertFactoryRegistry::~ConvertFactoryRegistry(){
    foreach(const ConvertFileFactory* f, factories) {
        delete f;
        f = NULL;
    }
    factories.clear();
}

bool ConvertFactoryRegistry::registerConvertFactory( ConvertFileFactory* f ){
    if (!factories.contains(f)){
        factories.prepend(f);
        return true;
    }else{
        return false;
    }
}

ConvertFileFactory * ConvertFactoryRegistry::getFactoryByFormats( const QString& detectedFormat, const QString& targetFormat ){
    foreach(ConvertFileFactory* f, factories) {
        if (f->isCustomFormatTask(detectedFormat, targetFormat)){
            return f;
        }
    }
    return NULL;
}

void ConvertFactoryRegistry::unregisterConvertFactory( ConvertFileFactory* f ){
    if (factories.contains(f)) {
        int id = factories.indexOf(f);
        ConvertFileFactory* fdel = factories.takeAt(id);
        delete fdel;
    }

}

bool ConvertFileFactory::isCustomFormatTask(const QString & /*detectedFormat*/, const QString & /*targetFormat*/) {
    return true;
}


} // U2
