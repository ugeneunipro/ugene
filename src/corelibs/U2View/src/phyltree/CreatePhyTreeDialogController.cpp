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

#include "CreatePhyTreeDialogController.h"

#include <QMessageBox>

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PluginModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include <U2View/LicenseDialog.h>

#include "CreatePhyTreeWidget.h"
#include "ui_CreatePhyTreeDialog.h"

namespace U2 {

CreatePhyTreeDialogController::CreatePhyTreeDialogController(QWidget* parent, const MultipleSequenceAlignmentObject* msaObject, CreatePhyTreeSettings& settings)
    : QDialog(parent),
      msa(msaObject->getMsaCopy()),
      settings(settings),
      settingsWidget(nullptr),
      ui(new Ui_CreatePhyTree),
      saveController(nullptr) {
    ui->setupUi(this);

    QMap<QString, QString> helpPagesMap;
    helpPagesMap.insert("PHYLIP Neighbor Joining", "65929715");
    helpPagesMap.insert("MrBayes", "65929718");
    helpPagesMap.insert("PhyML Maximum Likelihood", "65929720");
    helpPagesMap.insert("IQ-TREE", "74809348");
    new ComboboxDependentHelpButton(this, ui->buttonBox, ui->algorithmBox, helpPagesMap);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Build"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    ui->algorithmBox->addItems(registry->getNameList());

    initSaveController(msaObject);

    connect(ui->algorithmBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_comboIndexChanged(int)));
    connect(ui->storeSettings, SIGNAL(clicked()), SLOT(sl_onStoreSettings()));
    connect(ui->restoreSettings, SIGNAL(clicked()), SLOT(sl_onRestoreDefault()));

    const QString defaultAlgorithm = "PHYLIP Neighbor Joining";
    int defaultIndex = ui->algorithmBox->findText(defaultAlgorithm);
    if (defaultIndex == -1) {
        defaultIndex = 0;
    }
    ui->algorithmBox->setCurrentIndex(defaultIndex);
    sl_comboIndexChanged(defaultIndex);
}

void CreatePhyTreeDialogController::accept() {
    settings.algorithm = ui->algorithmBox->currentText();

    CHECK(checkAndPrepareOutputFilePath(), );
    SAFE_POINT(settingsWidget != nullptr, "Settings widget is NULL", );
    settingsWidget->fillSettings(settings);
    CHECK(checkSettings(), );
    CHECK(checkMemory(), );

    QDialog::accept();
}

void CreatePhyTreeDialogController::sl_comboIndexChanged(int) {
    delete settingsWidget;
    settingsWidget = nullptr;
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    PhyTreeGenerator* generator = registry->getGenerator(ui->algorithmBox->currentText());
    SAFE_POINT(generator != nullptr, "PhyTree Generator is NULL", );
    settingsWidget = generator->createPhyTreeSettingsWidget(msa, this);
    SAFE_POINT(settingsWidget != nullptr, "Settings widget is NULL", );
    ui->settingsContainerLayout->addWidget(settingsWidget);
}

CreatePhyTreeDialogController::~CreatePhyTreeDialogController() {
    delete ui;
}

void CreatePhyTreeDialogController::sl_onStoreSettings() {
    settingsWidget->storeSettings();
}
void CreatePhyTreeDialogController::sl_onRestoreDefault() {
    settingsWidget->restoreDefault();
}

bool CreatePhyTreeDialogController::checkAndPrepareOutputFilePath() {
    U2OpStatusImpl os;
    QString outputFilePath = saveController->getValidatedSaveFilePath(os);
    if (os.hasError()) {
        QMessageBox::critical(this, L10N::errorTitle(), os.getError());
        ui->fileNameEdit->setFocus();
        return false;
    }
    // Prepare the result dir.
    QFileInfo outputFileInfo(outputFilePath);
    QDir outputFileDir(outputFileInfo.absolutePath());
    if (!outputFileDir.exists()) {
        bool result = QDir().mkpath(outputFileInfo.absolutePath());
        // This should never happen: saveController->getValidatedSaveFilePath checked that the dir is writable.
        SAFE_POINT(result, L10N::internalError(L10N::errorWritingFile(outputFilePath)), false);
    }

    settings.fileUrl = outputFilePath;
    return true;
}

bool CreatePhyTreeDialogController::checkSettings() {
    QString msg;
    if (!settingsWidget->checkSettings(msg, settings)) {
        if (!msg.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), msg);
        }
        return false;
    }
    return true;
}

bool CreatePhyTreeDialogController::checkMemory() {
    SAFE_POINT(settingsWidget != nullptr, "Settings widget is NULL", false);

    QString msg;
    const bool memCheckOk = settingsWidget->checkMemoryEstimation(msg, msa, settings);

    if (!memCheckOk) {
        QObjectScopedPointer<QMessageBox> mb = new QMessageBox(QMessageBox::Warning, tr("Warning"), msg, QMessageBox::Ok | QMessageBox::Cancel, this);
        mb->exec();
        CHECK(!mb.isNull(), false);

        return (mb->result() == QMessageBox::Ok);
    }

    return true;
}

void CreatePhyTreeDialogController::initSaveController(const MultipleSequenceAlignmentObject* msaObject) {
    SaveDocumentControllerConfig config;
    GUrl msaDocumentUrl = msaObject->getDocument()->getURL();
    QString saveDirPath = QFileInfo(msaDocumentUrl.getURLString()).absolutePath();
    if (!FileAndDirectoryUtils::canWriteToPath(saveDirPath)) {
        saveDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath();
    }
    config.defaultFileName = GUrlUtils::getNewLocalUrlByExtension(saveDirPath + "/" + msaDocumentUrl.fileName(), msaObject->getGObjectName(), ".nwk", "");
    config.defaultFormatId = BaseDocumentFormats::NEWICK;
    config.fileDialogButton = ui->browseButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Choose file name");

    saveController = new SaveDocumentController(config, {BaseDocumentFormats::NEWICK}, this);
}

}  // namespace U2
