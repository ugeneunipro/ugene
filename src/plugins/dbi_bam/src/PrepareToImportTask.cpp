/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PrepareToImportTask.h"

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/BAMUtils.h>

#include "LoadBamInfoTask.h"

namespace U2 {
namespace BAM {

PrepareToImportTask::PrepareToImportTask(const GUrl& _assemblyUrl, bool _sam, const QString& _refUrl, const QString& _workDir)
    : Task(tr("Prepare assembly file to import"), TaskFlag_None),
      sourceURL(_assemblyUrl), refUrl(_refUrl), workDir(_workDir), samFormat(_sam), newURL(false) {
    tpm = Progress_Manual;
}

QString PrepareToImportTask::getIndexedBamUrl(const QString& sortedBamUrl) const {
    return workDir + "/" + QFileInfo(sortedBamUrl).fileName();
}

QString PrepareToImportTask::getCopyError(const QString& url1, const QString& url2) const {
    return LoadInfoTask::tr("Can not copy the '%1' file to '%2'").arg(url1).arg(url2);
}

namespace {
bool equalUrls(const QString& url1, const QString& url2) {
    return QFileInfo(url1).absoluteFilePath() == QFileInfo(url2).absoluteFilePath();
}
}  // namespace

bool PrepareToImportTask::needToCopyBam(const QString& sortedBamUrl) const {
    QString indexedBamUrl = getIndexedBamUrl(sortedBamUrl);
    return !equalUrls(indexedBamUrl, sortedBamUrl);
}

void PrepareToImportTask::run() {
    // SAM to BAM if needed
    QString bamUrl = samFormat
                         ? workDir + "/" + QFileInfo(sourceURL.getURLString()).fileName() + ".bam"
                         : sourceURL.getURLString();
    if (samFormat) {
        newURL = true;
        stateInfo.setDescription(LoadInfoTask::tr("Converting SAM to BAM"));

        checkReferenceFile();
        CHECK_OP(stateInfo, );

        BAMUtils::convertSamToBam(stateInfo, sourceURL.getURLString(), bamUrl, refUrl);
        CHECK_OP(stateInfo, );
    }
    stateInfo.setProgress(33);

    bool sorted = BAMUtils::isSortedBam(bamUrl, stateInfo);
    CHECK_OP(stateInfo, );

    // Sort BAM if needed
    QString sortedBamUrl;
    if (sorted) {
        sortedBamUrl = bamUrl;
    } else {
        newURL = true;
        stateInfo.setDescription(LoadInfoTask::tr("Sorting BAM"));

        QString sortedBamFilePathPrefix = workDir + "/" + QFileInfo(bamUrl).fileName() + "_sorted";
        sortedBamUrl = BAMUtils::sortBam(bamUrl, sortedBamFilePathPrefix, stateInfo).getURLString();
        CHECK_OP(stateInfo, );
    }
    stateInfo.setProgress(66);

    bool indexed = BAMUtils::hasValidBamIndex(sortedBamUrl);

    // Index BAM if needed
    QString indexedBamUrl;
    if (indexed) {
        indexedBamUrl = sortedBamUrl;
    } else {
        indexedBamUrl = getIndexedBamUrl(sortedBamUrl);
        if (needToCopyBam(sortedBamUrl)) {
            newURL = true;
            stateInfo.setDescription(LoadInfoTask::tr("Coping sorted BAM"));

            bool copied = QFile::copy(sortedBamUrl, indexedBamUrl);
            CHECK_EXT(copied, setError(getCopyError(sortedBamUrl, indexedBamUrl)), );
        }
        stateInfo.setDescription(LoadInfoTask::tr("Creating BAM index"));

        BAMUtils::createBamIndex(indexedBamUrl, stateInfo);
        CHECK_OP(stateInfo, );
    }
    stateInfo.setProgress(100);
    sourceURL = indexedBamUrl;
}

static bool isUnknownFormat(const QList<FormatDetectionResult>& formats) {
    if (formats.isEmpty()) {
        return true;
    }
    FormatDetectionResult f = formats.first();
    if (f.format == nullptr && nullptr == f.importer) {
        return true;
    }
    return false;
}

static QString detectedFormatId(const FormatDetectionResult& f) {
    if (f.format == nullptr && f.importer == nullptr) {
        return "";
    } else if (f.format == nullptr) {
        return f.importer->getImporterName();
    }
    return f.format->getFormatId();
}

void PrepareToImportTask::checkReferenceFile() {
    CHECK(!refUrl.isEmpty(), );

    FormatDetectionConfig cfg;
    cfg.useImporters = true;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(refUrl, cfg);
    if (isUnknownFormat(formats)) {
        setError(LoadInfoTask::tr("Unknown reference sequence format. Only FASTA is supported"));
        return;
    }
    QString formatId = detectedFormatId(formats.first());
    if (formatId != BaseDocumentFormats::FASTA) {
        setError(LoadInfoTask::tr("The detected reference sequence format is '%1'. Only FASTA is supported").arg(formatId));
        return;
    }

    // Check if FASTA index will be created and ensure the reference dir is writable.
    // Copy reference to the work dir if the reference dir is not writable.
    bool isFaiBuildNeeded = !BAMUtils::hasValidFastaIndex(refUrl);
    QString refUrlDir = QFileInfo(refUrl).absolutePath();
    if (isFaiBuildNeeded && !FileAndDirectoryUtils::isDirectoryWritable(refUrlDir)) {
        QString fastaUrl = workDir + "/" + QFileInfo(refUrl).fileName();
        if (!equalUrls(fastaUrl, refUrl)) {
            CHECK_EXT(QFileInfo::exists(fastaUrl), setError(tr("Can't copy reference file to the work dir. File already exists: %1").arg(fastaUrl)), );
            bool copied = QFile::copy(refUrl, fastaUrl);
            CHECK_EXT(copied, setError(getCopyError(refUrl, fastaUrl)), );
            refUrl = fastaUrl;
        }
    }
}

const GUrl& PrepareToImportTask::getSourceUrl() const {
    return sourceURL;
}

bool PrepareToImportTask::isNewURL() const {
    return newURL;
}

}  // namespace BAM
}  // namespace U2
