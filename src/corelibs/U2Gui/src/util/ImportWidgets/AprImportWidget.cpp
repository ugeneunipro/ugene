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
#include <U2Core/GUrlUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>

#include <U2Gui/SaveDocumentController.h>

#include "AprImportWidget.h"

namespace U2 {

AprImportWidget::AprImportWidget(const GUrl& url, const QVariantMap& settings) :
    ImportWidget()
{
    setupUi(this);

    DocumentFormatId formatId = getFormatId(settings);

    GUrl resultUrl = GUrlUtils::changeFileExt(url.getURLString(), formatId);
    QString resultUrlString = resultUrl.getURLString();
    if (resultUrlString.endsWith(QString(".gz"))) {
        resultUrlString.chop(3);
    }

    initSaveController(resultUrlString, formatId);
}

QVariantMap AprImportWidget::getSettings() const {
    QVariantMap settings;
    settings[ImportHint_DestinationUrl] = saveController->getSaveFileName();
    settings[ImportHint_FormatId] = saveController->getFormatIdToSave();

    return settings;
}

void AprImportWidget::initSaveController(const QString& url, const DocumentFormatId defaultFormatId){
    SaveDocumentControllerConfig config;

    config.defaultFormatId = defaultFormatId;
    config.fileDialogButton = browseButton;
    config.defaultFileName = url;
    config.fileNameEdit = fileNameEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;
    config.rollOutProjectUrls = true;

    DocumentFormatConstraints c;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    c.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    c.addFlagToExclude(DocumentFormatFlag_Hidden);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(c);

    saveController = new SaveDocumentController(config, formats, this);
}

DocumentFormatId AprImportWidget::getFormatId(const QVariantMap& settings) {
    DocumentFormatId res = settings.value(ImportHint_FormatId).toString();
    if (res.isEmpty()) {
        res = BaseDocumentFormats::FASTA;
    }
    return res;
}

}   // namespace U2
