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

#include <QDir>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>

#include "ExportDocumentDialogController.h"
#include "SaveDocumentController.h"
#include "ui_ExportDocumentDialog.h"

namespace U2{

ExportDocumentDialogController::ExportDocumentDialogController(Document* d, QWidget *p)
    : QDialog(p),
      saveController(NULL),
      sourceDoc(d),
      sourceObject(NULL)
{
    ui = new Ui_ExportDocumentDialog();
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748715");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController(sourceDoc->getObjects(), sourceDoc->getURLString());
}

ExportDocumentDialogController::ExportDocumentDialogController(GObject *object, QWidget *parent, const QString &initUrl) :
    QDialog(parent),
    ui(new Ui_ExportDocumentDialog()),
    sourceDoc(NULL),
    sourceObject(object)
{
    ui->setupUi(this);

    QList<GObject *> objectList = QList<GObject *>() << sourceObject;
    initSaveController(objectList, initUrl);

    new HelpButton(this, ui->buttonBox, "24748715");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

void ExportDocumentDialogController::initSaveController(const QList<GObject *> &objects, const QString &fileUrl) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = fileUrl;
    config.fileDialogButton = ui->browseButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.formatCombo = ui->formatCombo;
    config.compressCheckbox = ui->compressCheck;
    config.parentWidget = this;
    config.rollOutProjectUrls = true;
    config.rollSuffix = "_copy";

    const DocumentFormatConstraints formatConstraints = getAcceptableConstraints(objects);
    saveController = new SaveDocumentController(config, formatConstraints, this);
}

DocumentFormatConstraints ExportDocumentDialogController::getAcceptableConstraints(const QList<GObject *> &objects) {
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
    foreach (int val, objPerTypeMap){
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
    if (ui->compressCheck != NULL && ui->compressCheck->isChecked() && ui->compressCheck->isEnabled()) {
        QString suffix = path.split(".").last();
        if(suffix != "gz") {
            return path + ".gz";
        }
    }
    return path;
}

DocumentFormatId ExportDocumentDialogController::getDocumentFormatId() const {
    return saveController->getFormatIdToSave();
}

ExportDocumentDialogController::~ExportDocumentDialogController()
{
    delete ui;
}

bool ExportDocumentDialogController::getAddToProjectFlag() const
{
    return ui->addToProjCheck->isChecked();
}

void ExportDocumentDialogController::setAddToProjectFlag(bool checked)
{
    ui->addToProjCheck->setChecked(checked);
}

Document *ExportDocumentDialogController::getSourceDoc() const {
    return sourceDoc;
}

GObject *ExportDocumentDialogController::getSourceObject() const {
    return sourceObject;
}

}//namespace
