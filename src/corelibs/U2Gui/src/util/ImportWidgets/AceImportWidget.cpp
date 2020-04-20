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
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiRegistry.h>

#include <U2Gui/SaveDocumentController.h>

#include "AceImportWidget.h"

namespace U2 {

const QString AceImportWidget::EXTENSION = ".ugenedb";

AceImportWidget::AceImportWidget(const GUrl& url) : ImportWidget() {
    setupUi(this);

    initSaveController(url);
}

QVariantMap AceImportWidget::getSettings() const {
    QVariantMap settings;
    U2DbiRef ref(SQLITE_DBI_ID, saveController->getSaveFileName());
    settings.insert(DocumentFormat::DBI_REF_HINT, qVariantFromValue(ref));

    return settings;
}

void AceImportWidget::initSaveController(const GUrl& url) {
    SaveDocumentControllerConfig config;

    config.defaultFileName = url.getURLString() + EXTENSION;
    config.defaultFormatId = BaseDocumentFormats::UGENEDB;
    config.fileDialogButton = browseButton;
    config.fileNameEdit = fileNameEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Destination UGENEDB file");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::UGENEDB;

    saveController = new SaveDocumentController(config, formats, this);
}

}   // namespace U2
