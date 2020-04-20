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

#include <QPushButton>
#include <QMessageBox>

#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>

#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/RegionSelector.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportConsensusDialog.h"

namespace U2 {

ExportConsensusDialog::ExportConsensusDialog(QWidget *p, const ExportConsensusTaskSettings &settings_, const U2Region & visibleRegion)
    : QDialog(p),
      settings(settings_),
      saveController(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748905");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    //hide for this dialog
    variationModeComboBox->hide();
    variationModeLabel->hide();

    initSaveController();

    U2OpStatus2Log os;
    QList<RegionPreset> presets = QList<RegionPreset>() << RegionPreset(tr("Visible"), visibleRegion);
    regionSelector = new RegionSelector(this, settings.model->getModelLength(os), false, NULL, false, presets);

    int insertPos = verticalLayout->count() - 3;
    verticalLayout->insertWidget(insertPos, regionSelector);

    sequenceNameLineEdit->setText(settings.seqObjName);
    addToProjectCheckBox->setChecked(settings.addToProject);
    regionSelector->setCustomRegion(settings.region);
    keepGapsCheckBox->setChecked(settings.keepGaps);

    QList<QString> algos = AppContext::getAssemblyConsensusAlgorithmRegistry()->getAlgorithmIds();
    algorithmComboBox->addItems(algos);
    algorithmComboBox->setCurrentIndex(algos.indexOf(settings.consensusAlgorithm->getId()));

    QPushButton *okPushButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelPushButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(okPushButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelPushButton, SIGNAL(clicked()), SLOT(reject()));
    setMaximumHeight(layout()->minimumSize().height());
}

void ExportConsensusDialog::accept() {
    bool isRegionOk;
    settings.saveToFile = true;
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

void ExportConsensusDialog::initSaveController() {
    SaveDocumentControllerConfig conf;
    conf.fileDialogButton = filepathToolButton;
    conf.fileNameEdit = filepathLineEdit;
    conf.formatCombo = documentFormatComboBox;
    conf.parentWidget = this;
    conf.saveTitle = tr("Export consensus");
    conf.defaultFileName = settings.fileName;
    conf.defaultFormatId = settings.formatId;

    DocumentFormatConstraints dfc;
    dfc.supportedObjectTypes += GObjectTypes::SEQUENCE;
    dfc.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    dfc.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);

    saveController = new SaveDocumentController(conf, dfc, this);
}

} // namespace
