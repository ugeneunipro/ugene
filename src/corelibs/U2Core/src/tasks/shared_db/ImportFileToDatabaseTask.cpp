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
#include <U2Core/Counter.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ImportFileToDatabaseTask.h"

namespace U2 {

ImportFileToDatabaseTask::ImportFileToDatabaseTask(const QString& srcUrl, const U2DbiRef& dstDbiRef, const QString& dstFolder, const ImportToDatabaseOptions& options) :
Task(tr("Import file %1 to the database").arg(QFileInfo(srcUrl).fileName()), TaskFlags_FOSE_COSC),
srcUrl(srcUrl),
dstDbiRef(dstDbiRef),
dstFolder(dstFolder),
options(options),
format(NULL) {
    GCOUNTER(cvar, tvar, "ImportFileToDatabaseTask");
    CHECK_EXT(QFileInfo(srcUrl).isFile(), setError(tr("It is not a file: ") + srcUrl), );
    CHECK_EXT(dstDbiRef.isValid(), setError(tr("Invalid database reference")), );
}

void ImportFileToDatabaseTask::prepare() {
    DocumentProviderTask* importTask = detectFormat();
    CHECK_EXT(NULL != format || NULL != importTask, setError(tr("File format is not recognized")), );
    CHECK_OP(stateInfo, );

    CHECK_OP(stateInfo, );

    if (NULL != importTask) {
        addSubTask(importTask);
    }
}

void ImportFileToDatabaseTask::run() {
    CHECK(NULL != format, );

    const QVariantMap hints = prepareHints();

    IOAdapterFactory* ioFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(GUrl(srcUrl)));
    CHECK_EXT(NULL != ioFactory, setError(tr("Unrecognized url: ") + srcUrl), );
    CHECK_OP(stateInfo, );

    Document* loadedDoc = format->loadDocument(ioFactory, srcUrl, hints, stateInfo);
    CHECK_OP(stateInfo, );

    U2OpStatusImpl os;
    Document* restructuredDoc = DocumentUtils::createCopyRestructuredWithHints(loadedDoc, os);
    if (NULL != restructuredDoc) {
        restructuredDoc->setDocumentOwnsDbiResources(false);
        loadedDoc->setDocumentOwnsDbiResources(true);
    } else {
        loadedDoc->setDocumentOwnsDbiResources(false);
    }

    delete restructuredDoc;
    delete loadedDoc;
}

const QString &ImportFileToDatabaseTask::getFilePath() const {
    return srcUrl;
}

DocumentProviderTask* ImportFileToDatabaseTask::detectFormat() {
    FormatDetectionConfig detectionConfig;
    detectionConfig.useImporters = true;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(GUrl(srcUrl), detectionConfig);
    CHECK_EXT(!formats.isEmpty(), setError(tr("File format is not recognized")), NULL);

    const FormatDetectionResult preferredFormat = getPreferredFormat(formats);

    format = preferredFormat.format;
    CHECK(NULL == format, NULL);

    DocumentImporter* importer = preferredFormat.importer;
    CHECK(NULL != importer, NULL);  // do something with unrecognized files here

    QVariantMap hints = prepareHints();
    return importer->createImportTask(preferredFormat, false, hints);
}

QVariantMap ImportFileToDatabaseTask::prepareHints() const {
    QVariantMap hints;

    hints[DocumentReadingMode_DontMakeUniqueNames] = 1;
    hints[DocumentImporter::LOAD_RESULT_DOCUMENT] = false;
    hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(dstDbiRef);
    hints[DocumentFormat::DBI_FOLDER_HINT] = getFolderName();
    hints[DocumentFormat::DEEP_COPY_OBJECT] = true;

    switch (options.multiSequencePolicy) {
    case ImportToDatabaseOptions::SEPARATE:
        // do nothing, it is a standard behavior
        break;
    case ImportToDatabaseOptions::MERGE:
        hints[DocumentReadingMode_SequenceMergeGapSize] = options.mergeMultiSequencePolicySeparatorSize;
        break;
    case ImportToDatabaseOptions::MALIGNMENT:
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
        break;
    };

    return hints;
}

QString ImportFileToDatabaseTask::getFolderName() const {
    QString result = dstFolder;

    if (options.createSubfolderForEachFile) {
        QString fileName = QFileInfo(srcUrl).fileName();
        if (!options.keepFileExtension) {
            if (QFileInfo(fileName).suffix() == "gz") {
                fileName = QFileInfo(fileName).completeBaseName();
            }

            fileName = QFileInfo(fileName).completeBaseName();
        }

        result += U2ObjectDbi::PATH_SEP + fileName;
    }

    return result;
}

FormatDetectionResult ImportFileToDatabaseTask::getPreferredFormat(const QList<FormatDetectionResult> &detectedFormats) const {
    CHECK(!detectedFormats.isEmpty(), FormatDetectionResult());
    CHECK(!options.preferredFormats.isEmpty(), detectedFormats.first());

    QStringList detectedFormatIds;
    foreach(const FormatDetectionResult &detectedFormat, detectedFormats) {
        if (NULL != detectedFormat.format) {
            detectedFormatIds << detectedFormat.format->getFormatId();
        } else if (NULL != detectedFormat.importer) {
            detectedFormatIds << detectedFormat.importer->getId();
        } else {
            detectedFormatIds << "";   // to keep the numeration
        }
    }

    foreach(const QString &formatId, options.preferredFormats) {
        int i = detectedFormatIds.indexOf(formatId);
        if (i >= 0) {
            return detectedFormats[i];
        }
    }

    return detectedFormats.first();
}

}   // namespace U2
