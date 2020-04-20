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
#include <QMessageBox>
#include <QPushButton>

#include "ui_SaveSelectedSequenceFromMSADialog.h"

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/U2FileDialog.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

#include "SaveSelectedSequenceFromMSADialogController.h"


namespace U2 {

#if defined(Q_OS_LINUX) | defined (Q_OS_MAC)
const QString SaveDocumentInFolderController::HOME_DIR_IDENTIFIER = "~/";
#else
const QString SaveDocumentInFolderController::HOME_DIR_IDENTIFIER = "%UserProfile%/";
#endif

SaveSelectedSequenceFromMSADialogController::SaveSelectedSequenceFromMSADialogController(QWidget* p, const QString& defaultCustomFilename)
    : QDialog(p),
      saveController(NULL),
      ui(new Ui_SaveSelectedSequenceFromMSADialog())
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748870");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ui->customFileNameEdit->setDisabled(true);
    ui->customFileNameEdit->setText(defaultCustomFilename);

    trimGapsFlag = false;
    addToProjectFlag = true;

    initSaveController();
}
SaveSelectedSequenceFromMSADialogController::~SaveSelectedSequenceFromMSADialogController(){
    delete ui;
}

void SaveSelectedSequenceFromMSADialogController::accept() {
    url = saveController->getSaveDirName();
    formatId = ui->formatCombo->currentData().toString();
    DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
    CHECK(df != NULL, )
    QString extension = df->getSupportedDocumentFileExtensions().first();
    if (ui->customFileNameEdit->isEnabled()) {
        QString filePath = url + QDir::separator() + ui->customFileNameEdit->text() + "." + extension;
        filePath = GUrlUtils::fixFileName(filePath);
        QFile fileToSave(filePath);
        if (fileToSave.exists()) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("File \"%1\" is already exists, choose another filename or select another directory for save!").arg(ui->customFileNameEdit->text()));
            return;
        }

    }
    trimGapsFlag = !ui->keepGapsBox->isChecked();
    addToProjectFlag = ui->addToProjectBox->isChecked();
    customFileName = ui->customFileNameEdit->isEnabled() ? ui->customFileNameEdit->text() : "";

    QDialog::accept();
}



QString SaveSelectedSequenceFromMSADialogController::getUrl() const {
    return url;
}

DocumentFormatId SaveSelectedSequenceFromMSADialogController::getFormat() const {
    return formatId;
}

QString SaveSelectedSequenceFromMSADialogController::getCustomFileName() const {
    return customFileName;
}

bool SaveSelectedSequenceFromMSADialogController::getTrimGapsFlag() const {
    return trimGapsFlag;
}

bool SaveSelectedSequenceFromMSADialogController::getAddToProjectFlag() const {
    return addToProjectFlag;
}

void SaveSelectedSequenceFromMSADialogController::initSaveController() {
    SaveDocumentInFolderControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::FASTA;
    config.fileDialogButton = ui->folderButton;
    config.folderLineEdit = ui->folderNameEdit;
    config.formatCombo = ui->formatCombo;
    config.parentWidget = this;
    config.defaultFileName = LastUsedDirHelper(config.defaultDomain).dir + "/";

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::SEQUENCE;
    formatConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    formatConstraints.addFlagToExclude(DocumentFormatFlag_Hidden);
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentInFolderController(config, formatConstraints, this);
}

SaveDocumentInFolderControllerConfig::SaveDocumentInFolderControllerConfig() :
    SaveDocumentControllerConfig(),
    folderLineEdit(NULL) {
}

SaveDocumentInFolderController::SaveDocumentInFolderController(const SaveDocumentInFolderControllerConfig& config, const DocumentFormatConstraints& formatConstraints, QObject* parent)
    : QObject(parent),
    conf(config) {
    DocumentFormatConstraints fc(formatConstraints);
    fc.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);

    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> selectedFormats = fr->selectFormats(fc);
    foreach(const DocumentFormatId& id, selectedFormats) {
        formatsInfo.addFormat(id,
            fr->getFormatById(id)->getFormatName(),
            fr->getFormatById(id)->getSupportedDocumentFileExtensions());
    }

    init();
}

void SaveDocumentInFolderController::init() {
    QString path = conf.defaultFileName;
    if (conf.defaultFileName.isEmpty()) {
        path = conf.folderLineEdit->text();
    }
    setPath(path);

    initFormatComboBox();
    CHECK(conf.fileDialogButton != NULL, );
    connect(conf.fileDialogButton, SIGNAL(clicked()), SLOT(sl_fileDialogButtonClicked()));
}

void SaveDocumentInFolderController::setPath(const QString &path) {
    conf.folderLineEdit->setText(QDir::toNativeSeparators(path));
    emit si_pathChanged(path);
}

void SaveDocumentInFolderController::initFormatComboBox() {
    QString currentFormat = formatsInfo.getFormatNameById(conf.defaultFormatId);
    CHECK(conf.formatCombo != NULL, );

    QStringList sortedFormatNames = formatsInfo.getNames();
    sortedFormatNames.sort(Qt::CaseInsensitive);
    foreach (QString formatName, sortedFormatNames) {
        QString formatId = formatsInfo.getIdByName(formatName);
        conf.formatCombo->addItem(formatName, formatId);
    }

    if (currentFormat.isEmpty()) {
        currentFormat = conf.formatCombo->itemText(0);
    }
    conf.formatCombo->setCurrentText(currentFormat);
}

void SaveDocumentInFolderController::sl_fileDialogButtonClicked() {
    QString defaultDir = getSaveDirName();
    LastUsedDirHelper lod(conf.defaultDomain, defaultDir);
    if (defaultDir.isEmpty()) {
        defaultDir = lod;
    }

    lod.url = U2FileDialog::getExistingDirectory(conf.parentWidget, conf.saveTitle, defaultDir);
    if (lod.url.isEmpty()) {
        return;
    }
    setPath(lod.url);
}

QString SaveDocumentInFolderController::getSaveDirName() const {
    QString filePath = conf.folderLineEdit->text();
    if (QDir::fromNativeSeparators(filePath).startsWith(HOME_DIR_IDENTIFIER, Qt::CaseInsensitive)) {
        filePath.remove(0, HOME_DIR_IDENTIFIER.length() - 1);
        filePath.prepend(QDir::homePath());
    }
    return filePath;
}

}
