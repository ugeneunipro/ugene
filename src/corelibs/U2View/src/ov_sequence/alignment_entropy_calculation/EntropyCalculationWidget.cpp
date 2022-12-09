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

#include <QRegularExpression> 

#include "EntropyCalculationWidget.h"

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/FileFilters.h>
#include <U2Core/BaseDocumentFormats.h>

#include <U2View/AnnotatedDNAView.h>

namespace U2 {

const QString EntropyCalculationWidget::MUSCLE("MUSCLE");
const QString EntropyCalculationWidget::MAFFT("MAFFT");

EntropyCalculationWidget::EntropyCalculationWidget(AnnotatedDNAView* _annotatedDnaView)
    : annotatedDnaView(_annotatedDnaView) {
    setupUi(this);
    initLayout();
    initSaveController();
    connectSlots();
}

void EntropyCalculationWidget::initLayout() {
    additionalSettingsLayout->addWidget(new ShowHideSubgroupWidget(
        QObject::tr("Additional settings"), QObject::tr("Additional settings"), additionalSettingsWidget, true));
    algorithmComboBox->addItem(MUSCLE);
    algorithmComboBox->addItem(MAFFT);
}

void EntropyCalculationWidget::initSaveController() {
    SaveDocumentControllerConfig conf;
    conf.fileDialogButton = saveToToolButton;
    conf.fileNameEdit = saveToLineEdit;
    conf.parentWidget = this;
    conf.saveTitle = tr("Save file");
    conf.defaultFormatId = BaseDocumentFormats::PLAIN_PDB;

    //TODO: replace regex with getting the opened file name
    QRegularExpression exprBetweenBrackets("\\[(.*)\\]");
    QRegularExpressionMatch match = exprBetweenBrackets.match(annotatedDnaView->getName());
    conf.defaultFileName = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/" 
        + (match.hasMatch() ? match.captured(1) : annotatedDnaView->getName());

    saveController = new SaveDocumentController(conf, {BaseDocumentFormats::PLAIN_PDB}, this);
}

void EntropyCalculationWidget::connectSlots() {
    connect(alignmentToolButton, SIGNAL(clicked()), SLOT(sl_onFileSelectorClicked()));
    connect(runButton, SIGNAL(clicked()), SLOT(sl_onRunButtonClicked()));
}

void EntropyCalculationWidget::sl_onFileSelectorClicked() {
    LastUsedDirHelper lod("ENTROPY_CALCULATION_LAST_DIR");
    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT}, true, false);
    QString defaultFilter = FileFilters::createSingleFileFilterByDocumentFormatId(BaseDocumentFormats::CLUSTAL_ALN);
    lod.url = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select file to open..."), lod.dir, filter, defaultFilter);
    if (!lod.url.isEmpty())
        alignmentLineEdit->setText(lod.url);
}

void EntropyCalculationWidget::sl_onRunButtonClicked() {
    auto loadTask = new EntropyCalculationTask(annotatedDnaView, alignmentLineEdit->text(), saveToLineEdit->text());
    AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
}

}  // namespace U2
