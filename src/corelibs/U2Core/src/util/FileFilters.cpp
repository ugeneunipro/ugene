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
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

/** Separates individual named filters in filters list. */
static const QString FILTER_SEPARATOR = ";;";

QString FileFilters::createAllFilesFilter() {
    return QObject::tr("All files") + " ( * )";
}

QString FileFilters::createSingleFileFilter(const QString& name, const QStringList& extensions, bool addGzipVariant) {
    QString filters;
    for (const QString& extension : qAsConst(extensions)) {
        filters += " *." + extension;
    }
    if (addGzipVariant) {
        for (const QString& extension : qAsConst(extensions)) {
            filters += " *." + extension + ".gz";
        }
    }
    return name + " (" + filters + ")";
}

QString FileFilters::createSingleFileFilter(const DocumentFormat* documentFormat) {
    SAFE_POINT(documentFormat != nullptr, "Document format is null", "");
    const QString& name = documentFormat->getFormatName();
    QStringList extensions = documentFormat->getSupportedDocumentFileExtensions();
    SAFE_POINT(!extensions.isEmpty(), "Document format has empty extensions list", "");
    bool isGzipSupported = !documentFormat->getFlags().testFlag(DocumentFormatFlag_NoPack);
    return createSingleFileFilter(name, extensions, isGzipSupported);
}

QString FileFilters::createSingleFileFilterByDocumentFormatId(const QString& documentFormatId) {
    DocumentFormat* documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(documentFormat != nullptr, "Document format not found: " + documentFormatId, "");
    return createSingleFileFilter(documentFormat);
}

QString FileFilters::withAllFilesFilter(const QStringList& filters) {
    QStringList resultFilters = filters;
    resultFilters.sort();
    resultFilters.prepend(createAllFilesFilter());
    return resultFilters.join(FILTER_SEPARATOR);
}

QString FileFilters::createFileFilter(const QString& name, const QStringList& extensions, bool useGzipVariant) {
    return withAllFilesFilter({createSingleFileFilter(name, extensions, useGzipVariant)});
}

QString FileFilters::createFileFilter(const QMap<QString, QStringList>& formatExtensionsByName, bool addGzipVariant) {
    QStringList filters;
    QList<QString> names = formatExtensionsByName.keys();
    for (const QString& name : qAsConst(names)) {
        filters << createSingleFileFilter(name, formatExtensionsByName[name], addGzipVariant);
    }

    return withAllFilesFilter(filters);
}

QString FileFilters::createFileFilter(const QString& name, const QStringList& extensions) {
    return withAllFilesFilter({createSingleFileFilter(name, extensions, false)});
}

QString FileFilters::createFileFilterByDocumentFormatId(const DocumentFormatId& documentFormatId) {
    DocumentFormat* documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(documentFormatId);
    SAFE_POINT(documentFormat != nullptr, "Document format not found: " + documentFormatId, createAllFilesFilter());
    return withAllFilesFilter({createSingleFileFilter(documentFormat)});
}

QString FileFilters::createAllSupportedFormatsFileFilter(const QMap<QString, QStringList>& extraFormats) {
    DocumentFormatRegistry* formatRegistry = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> formatIds = formatRegistry->getRegisteredFormats();
    QStringList filters;
    for (const DocumentFormatId& id : qAsConst(formatIds)) {
        DocumentFormat* documentFormat = formatRegistry->getFormatById(id);
        filters << createSingleFileFilter(documentFormat);
    }
    QList<DocumentImporter*> importers = formatRegistry->getImportSupport()->getImporters();
    for (DocumentImporter* importer : qAsConst(importers)) {
        QStringList extensions = importer->getSupportedFileExtensions();
        filters << createSingleFileFilter(importer->getImporterName(), extensions, false);
    }
    QStringList extraFormatFilterNames = extraFormats.keys();
    for (const QString& name : qAsConst(extraFormatFilterNames)) {
        filters << createSingleFileFilter(name, extraFormats.value(name), false);
    }
    return withAllFilesFilter(filters);
}

QString FileFilters::createFileFilterByObjectTypes(const QList<GObjectType>& objectTypes, bool useWriteOnlyFormats) {
    SAFE_POINT(!objectTypes.isEmpty(), "createFileFilterByObjectTypes with no object types", withAllFilesFilter({}));

    QStringList filters;
    DocumentFormatRegistry* formatRegistry = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> formatIds = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();
    for (const DocumentFormatId& formatId : qAsConst(formatIds)) {
        DocumentFormat* documentFormat = formatRegistry->getFormatById(formatId);
        for (const auto& objectType : qAsConst(objectTypes)) {
            DocumentFormatConstraints constraints;
            constraints.flagsToSupport.setFlag(DocumentFormatFlag_SupportWriting, useWriteOnlyFormats);
            constraints.supportedObjectTypes.insert(objectType);
            if (documentFormat->checkConstraints(constraints)) {
                filters << createSingleFileFilter(documentFormat);
                break;
            }
        }
    }
    return withAllFilesFilter(filters);
}

}  // namespace U2
