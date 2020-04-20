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
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/BAMUtils.h>

#include "PrepareToImportTask.h"
#include "LoadBamInfoTask.h"

namespace U2 {
namespace BAM {

PrepareToImportTask::PrepareToImportTask( const GUrl& url, bool sam, const QString& refUrl, const QString &workingDir ) : Task("Prepare assembly file to import", TaskFlag_None),
                                                                        sourceURL( url ), refUrl(refUrl), workingDir(workingDir), samFormat(sam), newURL(false)
{
    tpm = Progress_Manual;
}

QString PrepareToImportTask::getBamUrl() const {
    if (samFormat) {
        QString samUrl = sourceURL.getURLString();
        return workingDir + "/" + QFileInfo(samUrl).fileName() + ".bam";
    } else {
        return sourceURL.getURLString();
    }
}

QString PrepareToImportTask::getSortedBamUrl(const QString &bamUrl) const {
    return workingDir + "/" + QFileInfo(bamUrl).fileName() + "_sorted";
}

QString PrepareToImportTask::getIndexedBamUrl(const QString &sortedBamUrl) const {
    return workingDir + "/" + QFileInfo(sortedBamUrl).fileName();
}

QString PrepareToImportTask::getFastaUrl() const {
    return workingDir + "/" + QFileInfo(refUrl).fileName();
}

QString PrepareToImportTask::getCopyError(const QString &url1, const QString &url2) const {
    return LoadInfoTask::tr("Can not copy the '%1' file to '%2'").arg(url1).arg(url2);
}

namespace {
    bool equalUrls(const QString &url1, const QString &url2) {
        return QFileInfo(url1).absoluteFilePath() == QFileInfo(url2).absoluteFilePath();
    }
}

bool PrepareToImportTask::needToCopyBam(const QString &sortedBamUrl) const {
    const QString indexedBamUrl = getIndexedBamUrl(sortedBamUrl);
    return !equalUrls(indexedBamUrl, sortedBamUrl);
}

bool PrepareToImportTask::needToCopyFasta() const {
    return !equalUrls(getFastaUrl(), refUrl);
}

void PrepareToImportTask::run() {
    // SAM to BAM if needed
    QString bamUrl = getBamUrl();
    if (samFormat) {
        newURL = true;
        stateInfo.setDescription(LoadInfoTask::tr("Converting SAM to BAM"));

        checkReferenceFile();
        CHECK_OP(stateInfo, );

        BAMUtils::ConvertOption options(true /*SAM to BAM*/, refUrl);
        BAMUtils::convertToSamOrBam(sourceURL, bamUrl, options, stateInfo);
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

        sortedBamUrl = BAMUtils::sortBam(bamUrl, getSortedBamUrl(bamUrl), stateInfo).getURLString();
        CHECK_OP(stateInfo, );
    }
    stateInfo.setProgress( 66 );

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

namespace {
static bool isUnknownFormat(const QList<FormatDetectionResult> &formats) {
    if (formats.isEmpty()) {
        return true;
    }
    FormatDetectionResult f = formats.first();
    if (NULL == f.format && NULL == f.importer) {
        return true;
    }
    return false;
}

static QString detectedFormatId(const FormatDetectionResult &f) {
    if (NULL == f.format && f.importer == NULL) {
        return "";
    } else if (NULL == f.format) {
        return f.importer->getImporterName();
    }
    return f.format->getFormatId();
}
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
    if (BaseDocumentFormats::FASTA != formatId) {
        setError(LoadInfoTask::tr("The detected reference sequence format is '%1'. Only FASTA is supported").arg(formatId));
        return;
    }

    if (!BAMUtils::hasValidFastaIndex(refUrl)) {
        if (needToCopyFasta()) {
            bool copied = QFile::copy(refUrl, getFastaUrl());
            CHECK_EXT(copied, setError(getCopyError(refUrl, getFastaUrl())), );

            refUrl = getFastaUrl();
        }
    }
}

const GUrl& PrepareToImportTask::getSourceUrl() const {
    return sourceURL;
}

bool PrepareToImportTask::isNewURL() {
    return newURL;
}

} // namespace BAM
} // namespace U2
