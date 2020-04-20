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

#include <QMessageBox>
#include <QPushButton>

#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>

#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/RegionSelector.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportConsensusVariationsDialog.h"

namespace U2 {

ExportConsensusVariationsDialog::ExportConsensusVariationsDialog(QWidget *p, const ExportConsensusVariationsTaskSettings &settings_, const U2Region & visibleRegion)
    : QDialog(p), settings(settings_)
{
    setupUi(this);
    setWindowTitle(tr("Export Consensus Variations"));
    new HelpButton(this, buttonBox, "24748907");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    //hide for this dialog
    sequenceNameLabel->hide();
    sequenceNameLineEdit->hide();

    initSaveController();

    U2OpStatus2Log os;
    QList<RegionPreset> presets = QList<RegionPreset>() << RegionPreset(tr("Visible"), visibleRegion);
    regionSelector = new RegionSelector(this, settings.model->getModelLength(os), false, NULL, false, presets);

    int insertPos = verticalLayout->count() - 3;
    verticalLayout->insertWidget(insertPos, regionSelector);

    filepathLineEdit->setText(settings.fileName);
    sequenceNameLineEdit->setText(settings.seqObjName);
    addToProjectCheckBox->setChecked(settings.addToProject);
    regionSelector->setCustomRegion(settings.region);
    keepGapsCheckBox->setChecked(settings.keepGaps);

    QList<QString> algos = AppContext::getAssemblyConsensusAlgorithmRegistry()->getAlgorithmIds();
    algorithmComboBox->addItems(algos);
    algorithmComboBox->setCurrentIndex(algos.indexOf(settings.consensusAlgorithm->getId()));

    variationModeComboBox->addItem(tr("Variations"), Mode_Variations);
    variationModeComboBox->addItem(tr("Similar"), Mode_Similar);
    variationModeComboBox->addItem(tr("All"), Mode_All);

    QPushButton *okPushButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelPushButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(okPushButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelPushButton, SIGNAL(clicked()), SLOT(reject()));
    setMaximumHeight(layout()->minimumSize().height());
}

void ExportConsensusVariationsDialog::accept() {
    bool isRegionOk;
    settings.fileName = saveController->getSaveFileName();
    settings.formatId = saveController->getFormatIdToSave();
    settings.seqObjName = sequenceNameLineEdit->text();
    settings.addToProject = addToProjectCheckBox->isChecked();
    settings.region = regionSelector->getRegion(&isRegionOk);
    settings.keepGaps = keepGapsCheckBox->isChecked();

    QString algoId = algorithmComboBox->currentText();
    if(algoId != settings.consensusAlgorithm->getId()) {
        AssemblyConsensusAlgorithmFactory * f = AppContext::getAssemblyConsensusAlgorithmRegistry()->getAlgorithmFactory(algoId);
        SAFE_POINT(f != NULL, QString("ExportConsensusDialog: consensus algorithm factory %1 not found").arg(algoId),);
        settings.consensusAlgorithm = QSharedPointer<AssemblyConsensusAlgorithm>(f->createAlgorithm());
    }

    int modeIdx = variationModeComboBox->currentIndex();
    if (modeIdx == -1){
        QMessageBox::critical(this, tr("Error!"), tr("Select consensus variation mode"));
        variationModeComboBox->setFocus(Qt::OtherFocusReason);
        return;
    }
    settings.mode = static_cast<CallVariationsMode>(variationModeComboBox->itemData(modeIdx).toInt());

    if(!isRegionOk){
        regionSelector->showErrorMessage();
        regionSelector->setFocus(Qt::OtherFocusReason);
        return;
    }
    // TODO: check if exists
    if(settings.fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Error!"), tr("Select destination file"));
        filepathLineEdit->setFocus(Qt::OtherFocusReason);
        return;
    }
    if(settings.seqObjName.isEmpty()) {
        QMessageBox::critical(this, tr("Error!"), tr("Sequence name cannot be empty"));
        sequenceNameLineEdit->setFocus(Qt::OtherFocusReason);
        return;
    }

    QDialog::accept();
}

const ExportConsensusVariationsTaskSettings &ExportConsensusVariationsDialog::getSettings() const {
    return settings;
}

void ExportConsensusVariationsDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFileName = settings.fileName;
    config.defaultFormatId = settings.formatId;
    config.fileDialogButton = filepathToolButton;
    config.fileNameEdit = filepathLineEdit;
    config.formatCombo = documentFormatComboBox;
    config.parentWidget = this;
    config.saveTitle = tr("Export Consensus Variations");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::VARIANT_TRACK;
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    formatConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

} // namespace
