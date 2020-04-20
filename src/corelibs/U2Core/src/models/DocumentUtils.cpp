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

#include "DocumentUtils.h"

#include <U2Core/AppContext.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

QSet<QString> DocumentUtils::getURLs(const QList<Document*>& docs) {
    QSet<QString> result;
    foreach(Document* d, docs) {
        result.insert(d->getURLString());
    }
    return result;
}

QSet<QString> DocumentUtils::getNewDocFileNameExcludesHint() {
    QSet<QString> excludeFileNames;
    Project* p = AppContext::getProject();
    if (p != NULL) {
        excludeFileNames = DocumentUtils::getURLs(p->getDocuments());
    }
    return excludeFileNames;

}


static void placeOrderedByScore(const FormatDetectionResult& info, QList<FormatDetectionResult>& result, const FormatDetectionConfig& conf) {
    if (info.score() == FormatDetection_NotMatched) {
        return;
    }
    if (result.isEmpty()) {
        result.append(info);
        return;
    }
    if (conf.bestMatchesOnly) {
        int bestScore = result.first().score();
        if (bestScore > info.score()) {
            return;
        } else if (bestScore < info.score()) {
            result.clear();
        }
        result.append(info);
        return;
    }
    for (int i = 0; i < result.length(); i++) {
        int scoreI = result.at(i).score();
        if (scoreI < info.score()) {
            result.insert(i, info);
            return;
        }
    }
    result.append(info);
}

#define FORMAT_DETECTION_EXT_BONUS 3

// returns formats with FormatDetectionResult != Not matched sorted by FormatDetectionResult
// FormatDetectionResult is adjusted by +FORMAT_DETECTION_EXT_BONUS if extension is matched

QList<FormatDetectionResult> DocumentUtils::detectFormat(const QByteArray& rawData, const QString& ext,
    const GUrl& url, const FormatDetectionConfig& conf) {
    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    QList< DocumentFormatId > allFormats = fr->getRegisteredFormats();

    QList<FormatDetectionResult> result;
    foreach(const DocumentFormatId& id, allFormats) {
        DocumentFormat* f = fr->getFormatById(id);
        FormatCheckResult cr = f->checkRawData(rawData, url);
        if (cr.score == FormatDetection_NotMatched) {
            continue;
        }
        if (conf.useExtensionBonus && f->getSupportedDocumentFileExtensions().contains(ext) && cr.score >= FormatDetection_VeryLowSimilarity) {
            cr.score += FORMAT_DETECTION_EXT_BONUS;
        }
        if (conf.excludeHiddenFormats && f->checkFlags(DocumentFormatFlag_Hidden)) {
            continue;
        }
        FormatDetectionResult res;
        res.format = f;
        res.rawDataCheckResult = cr;
        res.rawData = rawData;
        res.url = url;
        res.extension = ext;
        placeOrderedByScore(res, result, conf);
    }
    if (conf.useImporters) {
        DocumentImportersRegistry* importReg = AppContext::getDocumentFormatRegistry()->getImportSupport();
        foreach(DocumentImporter* i, importReg->getImporters()) {
            FormatCheckResult cr = i->checkRawData(rawData, url);
            if (conf.useExtensionBonus && i->getSupportedFileExtensions().contains(ext) && cr.score >= FormatDetection_VeryLowSimilarity) {
                cr.score += FORMAT_DETECTION_EXT_BONUS;
            }
            FormatDetectionResult res;
            res.importer = i;
            res.rawDataCheckResult = cr;
            res.rawData = rawData;
            res.url = url;
            res.extension = ext;
            placeOrderedByScore(res, result, conf);
        }
    }
    return result;
}

QList<FormatDetectionResult> DocumentUtils::detectFormat(const GUrl& url, const FormatDetectionConfig& conf) {
    QList<FormatDetectionResult> result;
    if (url.isEmpty()) {
        return result;
    }
    QByteArray rawData = IOAdapterUtils::readFileHeader(url);
    if (rawData.isEmpty()) {
        return result;
    }
    QString ext = GUrlUtils::getUncompressedExtension(url);
    result = detectFormat(rawData, ext, url, conf);
    return result;
}


QList<FormatDetectionResult> DocumentUtils::detectFormat(IOAdapter *io, const FormatDetectionConfig& conf) {
    QList<FormatDetectionResult> result;
    if (io == NULL || !io->isOpen()) {
        return result;
    }
    QByteArray rawData = IOAdapterUtils::readFileHeader(io);
    QString ext = GUrlUtils::getUncompressedExtension(io->getURL());
    result = detectFormat(rawData, ext, io->getURL(), conf);
    return result;
}

DocumentUtils::Detection DocumentUtils::detectFormat(const GUrl &url, QString &resultId) {
    FormatDetectionConfig cfg;
    cfg.bestMatchesOnly = false;
    cfg.useImporters = true;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, cfg);
    CHECK(formats.size() > 0, UNKNOWN);

    DocumentFormat *format = formats.first().format;
    DocumentImporter *importer = formats.first().importer;
    if (NULL != format) {
        resultId = format->getFormatId();
        return FORMAT;
    } else if (NULL != importer) {
        resultId = importer->getId();
        return IMPORTER;
    } else {
        FAIL("NULL format and importer", UNKNOWN);
    }
}

QList<DocumentFormat*> DocumentUtils::toFormats(const QList<FormatDetectionResult>& infos) {
    QList<DocumentFormat*> result;
    foreach(const FormatDetectionResult& info, infos) {
        if (info.format != NULL) {
            result << info.format;
        }
    }
    return result;
}

bool DocumentUtils::canAddGObjectsToDocument(Document* doc, const GObjectType& type) {
    if (!doc->isLoaded() || doc->isStateLocked()) {
        return false;
    }

    DocumentFormat* df = doc->getDocumentFormat();
    return df->isObjectOpSupported(doc, DocumentFormat::DocObjectOp_Add, type);
}

bool DocumentUtils::canRemoveGObjectFromDocument(GObject *obj) {
    Document* doc = obj->getDocument();

    if (NULL == doc || !doc->isLoaded() || doc->isStateLocked()) {
        return false;
    }

    DocumentFormat* df = doc->getDocumentFormat();
    if (!df->isObjectOpSupported(doc, DocumentFormat::DocObjectOp_Remove, obj->getGObjectType())) {
        return false;
    }

    return true;
}

void DocumentUtils::removeDocumentsContainigGObjectFromProject(GObject *obj) {
    // no results found -> delete empty annotation document
    Project* proj = AppContext::getProject();
    if (proj != NULL) {
        Document* toDelete = NULL;
        QList<Document*> docs = proj->getDocuments();
        foreach(Document* doc, docs) {
            if (doc->getObjects().contains(obj)) {
                toDelete = doc;
                break;
            }
        }
        if (toDelete != NULL) {
            proj->removeDocument(toDelete);
        }
    }
}

QFile::Permissions DocumentUtils::getPermissions(Document *doc) {
    return QFile(doc->getURLString()).permissions();
}

Document* DocumentUtils::createCopyRestructuredWithHints(Document* doc, U2OpStatus& os, bool shallowCopy) {
    Document *resultDoc = NULL;
    QVariantMap hints = doc->getGHintsMap();

    if (hints.value(ProjectLoaderHint_MultipleFilesMode_Flag, false).toBool()) {
        return NULL;
    }

    if (hints.value(DocumentReadingMode_SequenceAsAlignmentHint, false).toBool()) {
        MultipleSequenceAlignmentObject* maObj = MSAUtils::seqObjs2msaObj(doc->getObjects(), hints, os, shallowCopy, true);
        CHECK_OP(os, NULL);
        CHECK(maObj != NULL, resultDoc);
        QList<GObject*> objects;
        objects << maObj;

        DocumentFormatConstraints objTypeConstraints;
        objTypeConstraints.supportedObjectTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
        bool makeReadOnly = !doc->getDocumentFormat()->checkConstraints(objTypeConstraints);

        resultDoc = new Document(doc->getDocumentFormat(), doc->getIOAdapterFactory(), doc->getURL(), doc->getDbiRef(), objects, hints,
            makeReadOnly ? tr("Format does not support writing of alignments") : QString());

        doc->propagateModLocks(resultDoc);
    } else if (hints.contains(DocumentReadingMode_SequenceMergeGapSize)) {
        int mergeGap = hints.value(DocumentReadingMode_SequenceMergeGapSize).toInt();
        if (mergeGap < 0 || doc->findGObjectByType(GObjectTypes::SEQUENCE, UOF_LoadedOnly).count() <= 1) {
            return NULL;
        }

        QList<GObject*> objects = U1SequenceUtils::mergeSequences(doc, doc->getDbiRef(), hints, os);
        resultDoc = new Document(doc->getDocumentFormat(), doc->getIOAdapterFactory(), doc->getURL(),
            doc->getDbiRef(), objects, hints, tr("File content was merged"));
        doc->propagateModLocks(resultDoc);

        if (os.hasError()) {
            delete resultDoc;
            resultDoc = NULL;
        }
    }

    return resultDoc;
}

QString FormatDetectionResult::getFormatDescriptionText() const {
    QString text = format == NULL ? importer->getImporterDescription() : format->getFormatDescription();
    return text;
}

QString FormatDetectionResult::getFormatOrImporterName() const {
    QString name = format == NULL ? importer->getImporterName() : format->getFormatName();
    return name;
}



} //namespace
