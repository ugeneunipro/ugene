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

#ifndef _U2_DOCUMENT_UTILS_H_
#define _U2_DOCUMENT_UTILS_H_

#include <QFile>

#include "DocumentModel.h"

namespace U2 {

class DocumentImporter;

/**
 * Result of the format detection.
 * Contains format detection score and a pointer to a format or to a data importer.
 */
class U2CORE_EXPORT FormatDetectionResult {
public:
    /** Detected document format. */
    DocumentFormat* format = nullptr;

    /** DocumentImporter that can be used to import the data. */
    DocumentImporter* importer = nullptr;

    /** Raw binary data from the URL. Present for all formats. For text formats this may be raw unicode bytes. */
    QByteArray rawBinaryData;

    /** Raw text data. Present only for text formats. */
    QString rawTextData;

    /** Url of the tested resource. */
    GUrl url;

    /** File extension. May be used by the format detection algorithm as an additional hint. */
    QString extension;

    FormatCheckResult rawDataCheckResult;

    QString getFormatDescriptionText() const;

    QString getFormatOrImporterName() const;

    /**
     * Returns textual representation of the raw data.
     * Tries to re-use rawTextData or fall-backs to the pre-formatted rawBinaryData.
     */
    QString getRawDataPreviewText() const;

    /** Returns format detection score. */
    int score() const;
};

class U2CORE_EXPORT FormatDetectionConfig {
public:
    FormatDetectionConfig()
        : bestMatchesOnly(true), useImporters(false), useExtensionBonus(true), excludeHiddenFormats(true) {
    }

    /** if true format detection algorithm returns list of best matches only */
    bool bestMatchesOnly;

    /** if false format detection algorithm do not test format importers and returns only real formats */
    bool useImporters;

    /** if true file extension is checked and bonus is added if extension is matched for a format */
    bool useExtensionBonus;

    /** if true then formats with the 'hidden' flag will be excluded */
    bool excludeHiddenFormats;
};

class U2CORE_EXPORT DocumentUtils : public QObject {
    Q_OBJECT
public:
    /* returns set with document urls */
    static QSet<QString> getURLs(const QList<Document*>& docs);

    /*  The set of urls that should not be used for new documents
        returns list of loaded urls. Gets them from the active project
        */
    static QSet<QString> getNewDocFileNameExcludesHint();

    /* Detects document format. The best match goes first in the returned list */
    static QList<FormatDetectionResult> detectFormat(const GUrl& url, const FormatDetectionConfig& conf = FormatDetectionConfig());

    /*
        Detects document format. The best match goes first in the returned list
        IOAdapter must be opened
        */
    static QList<FormatDetectionResult> detectFormat(IOAdapter* io, const FormatDetectionConfig& conf = FormatDetectionConfig());

    /*
        Detects document format. The best match goes first in the returned list
        ext & url can be used here to add extension bonus to the final score
        */
    static QList<FormatDetectionResult> detectFormat(const QByteArray& rawData, const QString& ext = QString(), const GUrl& url = GUrl(), const FormatDetectionConfig& conf = FormatDetectionConfig());

    /*
        Find the best matching document format and stores it in @resultId.
        */
    enum Detection {
        UNKNOWN,
        FORMAT,
        IMPORTER
    };
    static Detection detectFormat(const GUrl& url, QString& resultId);

    static QList<DocumentFormat*> toFormats(const QList<FormatDetectionResult>& infos);

    static bool canAddGObjectsToDocument(Document* doc, const GObjectType& type);

    static bool canRemoveGObjectFromDocument(GObject* obj);

    static void removeDocumentsContainigGObjectFromProject(GObject* obj);

    static QFile::Permissions getPermissions(Document* doc);

    /** Creates new document that contains data from original one restructured to new form according to document hints
        For example: combines all sequences to alignment, merge sequences, etc
        Return NULL if no restructuring was made
        */
    static Document* createCopyRestructuredWithHints(Document* doc, U2OpStatus& os, bool shallowCopy = false);
};

}  // namespace U2

#endif
