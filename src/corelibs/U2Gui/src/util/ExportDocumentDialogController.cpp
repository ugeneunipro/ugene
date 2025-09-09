/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ExportDocumentDialogController.h"

#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/GObject.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/HelpButton.h>

#include "ui_ExportDocumentDialog.h"

namespace U2 {

ExportDocumentDialogController::ExportDocumentDialogController(Document* d, QWidget* p)
    : QDialog(p),
      sourceDoc(d) {
    ui = new Ui_ExportDocumentDialog();
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "65929295");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    saveController = new SaveDocumentController(getSaveConfig(sourceDoc->getURLString()), getAcceptableConstraints(sourceDoc->getObjects()), this);
}

ExportDocumentDialogController::ExportDocumentDialogController(GObject* object, QWidget* parent, const QString& initUrl)
    : QDialog(parent),
      ui(new Ui_ExportDocumentDialog()),
      sourceObject(object) {
    ui->setupUi(this);
    saveController = new SaveDocumentController(getSaveConfig(initUrl), getAcceptableConstraints(QList<GObject*>() << sourceObject), this);
    new HelpButton(this, ui->buttonBox, "65929295");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

ExportDocumentDialogController::ExportDocumentDialogController(const QString& defaultUrl, 
                                                               const DocumentFormatConstraints& dfc, 
                                                               QWidget* parent)
    : QDialog(parent),
      ui(new Ui_ExportDocumentDialog()),
    hiddenAddToProjectAndCompressionOptions(true) {
    ui->setupUi(this);
    setMinimumWidth(size().width());
    ui->compressCheck->setHidden(true);
    ui->addToProjCheck->setHidden(true);
    adjustSize();
    saveController = new SaveDocumentController(getSaveConfig(defaultUrl), dfc, this);
}

SaveDocumentControllerConfig ExportDocumentDialogController::getSaveConfig(const QString& fileUrl) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = fileUrl;
    config.fileDialogButton = ui->browseButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.formatCombo = ui->formatCombo;
    config.compressCheckbox = ui->compressCheck;
    config.parentWidget = this;
    config.rollOutProjectUrls = true;
    config.rollSuffix = "_copy";
    return config;
}

DocumentFormatConstraints ExportDocumentDialogController::getAcceptableConstraints(const QList<GObject*>& objects) {
    DocumentFormatConstraints formatConstraints;

    QMap<GObjectType, int> objPerTypeMap;
    foreach (GObject* obj, objects) {
        GObjectType objectType = obj->getGObjectType();
        formatConstraints.supportedObjectTypes += objectType;
        if (objPerTypeMap.contains(objectType)) {
            objPerTypeMap[objectType] += 1;
        } else {
            objPerTypeMap.insert(objectType, 1);
        }
    }

    int maxObjs = 1;
    foreach (int val, objPerTypeMap) {
        maxObjs = qMax(maxObjs, val);
    }

    if (maxObjs > 1) {
        formatConstraints.addFlagToExclude(DocumentFormatFlag_OnlyOneObject);
    }
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    return formatConstraints;
}

QString ExportDocumentDialogController::getDocumentURL() const {
    QString path = saveController->getSaveFileName();
    if (!hiddenAddToProjectAndCompressionOptions) {
        if (ui->compressCheck != nullptr && ui->compressCheck->isChecked() && ui->compressCheck->isEnabled()) {
            QString suffix = path.split(".").last();
            if (suffix != "gz") {
                return path + ".gz";
            }
        }
    }
    return path;
}

DocumentFormatId ExportDocumentDialogController::getDocumentFormatId() const {
    return saveController->getFormatIdToSave();
}

bool ExportDocumentDialogController::getAddToProjectFlag() const {
    return ui->addToProjCheck->isChecked();
}

void ExportDocumentDialogController::setAddToProjectFlag(bool checked) {
    ui->addToProjCheck->setChecked(checked);
}

Document* ExportDocumentDialogController::getSourceDoc() const {
    return sourceDoc;
}

GObject* ExportDocumentDialogController::getSourceObject() const {
    return sourceObject;
}

}  // namespace U2
