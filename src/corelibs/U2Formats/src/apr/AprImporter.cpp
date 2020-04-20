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
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/RemoveDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Timer.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ImportWidget.h>
#include <U2Gui/ProjectView.h>

#include <U2Formats/AprFormat.h>
#include <U2Formats/ConvertFileTask.h>

#include "AprImporter.h"

#include <QDir>
#include <QFileInfo>

namespace U2 {

///////////////////////////////////
//// AprImporterTask
///////////////////////////////////

AprImporterTask::AprImporterTask(const GUrl& url, const QVariantMap &_settings) :
    DocumentProviderTask(tr("APR file import: %1").arg(url.fileName()), TaskFlags_NR_FOSE_COSC),
    settings(_settings),
    srcUrl(url)
{
    documentDescription = url.fileName();
}

void AprImporterTask::prepare() {
    DocumentFormatId currentFormat = BaseDocumentFormats::VECTOR_NTI_ALIGNX;
    QVariant v = settings.value(ImportHint_DestinationUrl);
    QString dstUrl = v.toString();

    SAFE_POINT_EXT(!dstUrl.isEmpty(), stateInfo.setError(tr("Empty destination url")), );

    QVariant variantFormat = settings.value(ImportHint_FormatId);
    DocumentFormatId formatId = variantFormat.toString();

    IOAdapterRegistry *ioar = AppContext::getIOAdapterRegistry();
    SAFE_POINT_EXT(NULL != ioar, stateInfo.setError(tr("Invalid I/O environment!")), );

    QFileInfo fileInfo(dstUrl);
    QDir qDir = fileInfo.dir();
    QString dir = qDir.path();
    QString name = fileInfo.completeBaseName();
    dstUrl = QFileInfo(qDir, name).filePath();

    DefaultConvertFileTask *convertTask = new DefaultConvertFileTask(srcUrl, currentFormat, dstUrl, formatId, dir);

    addSubTask(convertTask);
}

QList<Task*> AprImporterTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    DefaultConvertFileTask* convTask = qobject_cast<DefaultConvertFileTask*> (subTask);
    if (convTask != NULL) {
        QString dstUrl = convTask->getResult();
        SAFE_POINT_EXT(!dstUrl.isEmpty(), stateInfo.setError(tr("Empty destination url")), res);

        QVariantMap hints;
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
        LoadDocumentTask* loadTask = LoadDocumentTask::getDefaultLoadDocTask(stateInfo, dstUrl, hints);
        CHECK(loadTask != NULL, res);

        res << loadTask;
    }

    LoadDocumentTask* loadTask = qobject_cast<LoadDocumentTask*> (subTask);
    if (loadTask != NULL) {
        resultDocument = loadTask->takeDocument();
    }

    return res;
}

///////////////////////////////////
//// AprImporter
///////////////////////////////////

const QString AprImporter::ID = "Vector_NTI_AlignX-importer";

AprImporter::AprImporter() :
DocumentImporter(ID, tr("Vector NTI/AlignX file importer")) {
    AprFormat aprFormat(NULL);
    extensions << aprFormat.getSupportedDocumentFileExtensions();
    formatIds << aprFormat.getFormatId();
    importerDescription = tr("Vector NTI/AlignX files importer is used to convert conventional APR files to a multiple sequence alignment formats");
    supportedObjectTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

FormatCheckResult AprImporter::checkRawData(const QByteArray& rawData, const GUrl& url) {
    AprFormat aprFormat(NULL);
    return aprFormat.checkRawData(rawData, url);
}

DocumentProviderTask* AprImporter::createImportTask(const FormatDetectionResult& res, bool, const QVariantMap &hints) {
    AprImporterTask* task = new AprImporterTask(res.url, hints);
    return task;
}

QString AprImporter::getRadioButtonText() const{
    QString res = tr("Convert to another format:");
    return res;
}

}  // namespace U2
