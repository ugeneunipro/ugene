/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "FileFilters.h"

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

/** Separates individual named filters in filters list. */
static const QString FILTER_SEPARATOR = ";;";

QString FileFilters::createAllFilesFilter() {
    return QObject::tr("All files") + " ( * )";
}

QString FileFilters::createSingleFileFilter(const QString &name, const QStringList &extensions, bool addGzipVariant) {
    QString filters;
    for (const QString &extension : qAsConst(extensions)) {
        filters += " *." + extension;
    }
    if (addGzipVariant) {
        for (const QString &extension : qAsConst(extensions)) {
            filters += " *." + extension + ".gz";
        }
    }
    return name + " (" + filters + ")";
}

QString FileFilters::createSingleFileFilter(const DocumentFormat *documentFormat) {
    SAFE_POINT(documentFormat != nullptr, "Document format is null", "");
    const QString &name = documentFormat->getFormatName();
    QStringList extensions = documentFormat->getSupportedDocumentFileExtensions();
    SAFE_POINT(!extensions.isEmpty(), "Document format has empty extensions list", "");
    bool isGzipSupported = !documentFormat->getFlags().testFlag(DocumentFormatFlag_NoPack);
    return createSingleFileFilter(name, extensions, isGzipSupported);
}

QString FileFilters::createSingleFileFilterByDocumentFormatId(const QString &documentFormatId) {
    DocumentFormat *documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(documentFormat != nullptr, "Document format not found: " + documentFormatId, "");
    return createSingleFileFilter(documentFormat);
}

QString FileFilters::createFileFilter(const QString &name, const QStringList &extensions, bool useGzipVariant) {
    return createSingleFileFilter(name, extensions, useGzipVariant) + FILTER_SEPARATOR + createAllFilesFilter();
}

QString FileFilters::createFileFilter(const QMap<QString, QStringList> &formatExtensionsByName, bool addGzipVariant) {
    QStringList filters;
    QList<QString> names = formatExtensionsByName.keys();
    for (const QString &name : qAsConst(names)) {
        filters << createSingleFileFilter(name, formatExtensionsByName[name], addGzipVariant);
    }

    filters << createAllFilesFilter();
    return filters.join(FILTER_SEPARATOR);
}

QString FileFilters::createFileFilter(const QString &name, const QStringList &extensions) {
    return createSingleFileFilter(name, extensions, false) + FILTER_SEPARATOR + createAllFilesFilter();
}

QString FileFilters::createFileFilterByDocumentFormatId(const DocumentFormatId &documentFormatId) {
    DocumentFormat *documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(documentFormat != nullptr, "Document format not found: " + documentFormatId, createAllFilesFilter());
    return createSingleFileFilter(documentFormat) + FILTER_SEPARATOR + createAllFilesFilter();
}

QString FileFilters::createAllSupportedFormatsFileFilter(const QMap<QString, QStringList> &extraFormats) {
    DocumentFormatRegistry *formatRegistry = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> formatIds = formatRegistry->getRegisteredFormats();
    QStringList filters;
    for (const DocumentFormatId &id : qAsConst(formatIds)) {
        if (id == BaseDocumentFormats::DATABASE_CONNECTION) {
            continue;
        }
        DocumentFormat *documentFormat = formatRegistry->getFormatById(id);
        filters << createSingleFileFilter(documentFormat);
    }
    QList<DocumentImporter *> importers = formatRegistry->getImportSupport()->getImporters();
    for (DocumentImporter *importer : qAsConst(importers)) {
        QStringList extensions = importer->getSupportedFileExtensions();
        filters << createSingleFileFilter(importer->getImporterName(), extensions, false);
    }
    QStringList extraFormatFilterNames = extraFormats.keys();
    for (const QString &name : qAsConst(extraFormatFilterNames)) {
        filters << createSingleFileFilter(name, extraFormats.value(name), false);
    }
    filters.sort();
    filters.prepend(createAllFilesFilter());
    return filters.join(FILTER_SEPARATOR);
}

QString FileFilters::createFileFilter(const DocumentFormatConstraints &constraints) {
    DocumentFormatRegistry *formatRegistry = AppContext::getDocumentFormatRegistry();
    QStringList filters;
    QList<DocumentFormatId> formatIds = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();
    for (const DocumentFormatId &formatId : qAsConst(formatIds)) {
        if (formatId == BaseDocumentFormats::DATABASE_CONNECTION) {
            continue;
        }
        DocumentFormat *documentFormat = formatRegistry->getFormatById(formatId);
        if (documentFormat->checkConstraints(constraints)) {
            filters << createSingleFileFilter(documentFormat);
        }
    }
    filters.sort();
    filters.prepend(createAllFilesFilter());
    return filters.join(FILTER_SEPARATOR);
}

QString FileFilters::createFileFilterByObjectTypes(const QList<GObjectType> &objectTypes, bool useWriteOnlyFormats) {
    DocumentFormatConstraints constraints;
    if (useWriteOnlyFormats) {
        constraints.flagsToSupport.setFlag(DocumentFormatFlag_SupportWriting, true);
    }
    constraints.supportedObjectTypes += objectTypes.toSet();
    return createFileFilter(constraints);
}

}  // namespace U2
