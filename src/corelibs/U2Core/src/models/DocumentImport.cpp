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

#include <U2Core/DocumentImport.h>
#include <U2Core/Log.h>

#include <U2Gui/ImportWidget.h>

namespace U2 {

DocumentImportersRegistry::~DocumentImportersRegistry() {
    qDeleteAll(importers);
    importers.clear();
}

DocumentImporter* DocumentImportersRegistry::getDocumentImporter(const QString& importerId) const {
    foreach(DocumentImporter* i, importers) {
        if (i->getId() == importerId) {
            return i;
        }
    }
    return NULL;
}

void DocumentImportersRegistry::addDocumentImporter(DocumentImporter* i) {
    importers << i;
    if (i->getImporterDescription().isEmpty()) {
        coreLog.trace("Warn! Importer has no description: " + i->getImporterName());
    }
}

const QString DocumentImporter::LOAD_RESULT_DOCUMENT = "load_result_document";

void DocumentImporter::setWidgetFactory(ImportWidgetFactory* factory) {
    if (widgetFactory) {
        delete widgetFactory;
    }
    widgetFactory = factory;
}

const QSet<GObjectType> &DocumentImporter::getSupportedObjectTypes() const {
    return supportedObjectTypes;
}

void ImportDialog::accept() {
    if (!isValid()) {
        return;
    }
    applySettings();
    QDialog::accept();
}

QString DocumentImporter::getRadioButtonText() const {
    return QString();
}

ImportWidget* DocumentImporter::createImportWidget(const GUrl& url, const QVariantMap& settings) const {
    ImportWidget* res = widgetFactory->getWidget(url, settings);
    return res;
}

} //namespace
