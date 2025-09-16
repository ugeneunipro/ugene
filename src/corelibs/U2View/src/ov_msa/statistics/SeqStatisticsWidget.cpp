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

#include "SeqStatisticsWidget.h"

#include <U2Algorithm/MsaDistanceAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LabelClickTransmitter.h>
#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/Theme.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MsaEditor.h>

#include "../MsaEditorSimilarityColumn.h"

namespace U2 {

static inline QVBoxLayout* initLayout(QWidget* w) {
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    w->setLayout(layout);
    return layout;
}

SeqStatisticsWidget::SeqStatisticsWidget(MsaEditor* m)
    : msa(m), savableTab(this, GObjectViewUtils::findViewByName(m->getName())) {
    setObjectName("SequenceStatisticsOptionsPanelTab");
    SAFE_POINT(m != nullptr, QString("Invalid parameter were passed into constructor SeqStatisticsWidget"), );

    copySettings();

    QVBoxLayout* mainLayout = initLayout(this);
    mainLayout->setSpacing(0);

    distancesStatisticsGroup = new QWidget(this);
    ui.setupUi(distancesStatisticsGroup);
    new LabelClickTransmitter(ui.showDistancesColumnCheckLabel, ui.showDistancesColumnCheck);

    QWidget* similarityGroup = new ShowHideSubgroupWidget("REFERENCE", tr("Distances column"), distancesStatisticsGroup, true);
    updateWidgetsSettings();
    mainLayout->addWidget(similarityGroup);

    U2WidgetStateStorage::restoreWidgetState(savableTab);
}

void SeqStatisticsWidget::copySettings() {
    auto msaEditorUi = qobject_cast<MsaEditorWgt*>(msa->getLineWidget(0));
    const MsaEditorAlignmentDependentWidget* similarityWidget = msaEditorUi->getSimilarityWidget();
    statisticsIsShown = false;
    if (similarityWidget != nullptr) {
        auto s = similarityWidget->getSettings();
        settings = s != nullptr ? new SimilarityStatisticsSettings(*s) : new SimilarityStatisticsSettings();
        statisticsIsShown = !similarityWidget->isHidden();
    } else {
        settings = new SimilarityStatisticsSettings();
        settings->excludeGaps = false;
        settings->autoUpdate = true;
        settings->usePercents = true;
    }
}

void SeqStatisticsWidget::updateWidgetsSettings() {
    QList<MsaDistanceAlgorithmFactory*> algos = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactories();
    foreach (MsaDistanceAlgorithmFactory* a, algos) {
        ui.algoComboBox->addItem(a->getName(), a->getId());
    }
    ui.algoComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

    ui.refSeqWarning->setText(tr("Hint: select a reference above"));
    ui.refSeqWarning->setStyleSheet(QString("color: %1; font: bold;").arg(Theme::infoColorLabelHtmlStr()));
    ui.refSeqWarning->setWordWrap(true);

    ui.dataState->setText(tr("Press button to update"));

    restoreSettings();

    connectSlots();
}

void SeqStatisticsWidget::connectSlots() {
    connect(ui.algoComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onAlgoChanged()));
    connect(ui.excludeGapsCheckBox, SIGNAL(stateChanged(int)), SLOT(sl_onGapsChanged(int)));
    connect(ui.countsButton, SIGNAL(clicked(bool)), SLOT(sl_onUnitsChanged(bool)));
    connect(ui.percentsButton, SIGNAL(clicked(bool)), SLOT(sl_onUnitsChanged(bool)));
    connect(ui.updateButton, SIGNAL(pressed()), SLOT(sl_onUpdateClicked()));
    connect(ui.showDistancesColumnCheck, SIGNAL(stateChanged(int)), SLOT(sl_onShowStatisticsChanged(int)));
    connect(ui.autoUpdateCheck, SIGNAL(stateChanged(int)), SLOT(sl_onAutoUpdateChanged(int)));
    connect(msa, SIGNAL(si_referenceSeqChanged(qint64)), SLOT(sl_onRefSeqChanged(qint64)));
    connect(AppContext::getMainWindow(), &MainWindow::si_colorThemeSwitched, this, &SeqStatisticsWidget::sl_colorThemeSwitched);
}

void SeqStatisticsWidget::restoreSettings() {
    ui.showDistancesColumnCheck->setCheckState(statisticsIsShown ? Qt::Checked : Qt::Unchecked);
    ui.percentsButton->setChecked(settings->usePercents);
    ui.countsButton->setChecked(!settings->usePercents);
    ui.excludeGapsCheckBox->setCheckState(settings->excludeGaps ? Qt::Checked : Qt::Unchecked);
    ui.autoUpdateCheck->setCheckState(settings->autoUpdate ? Qt::Checked : Qt::Unchecked);
    ui.updateButton->setEnabled(!settings->autoUpdate);
    ui.dataState->setEnabled(!settings->autoUpdate);

    int index = ui.algoComboBox->findData(settings->algoId);
    if (0 <= index) {
        ui.algoComboBox->setCurrentIndex(index);
    } else {
        settings->algoId = ui.algoComboBox->currentData().toString();
    }
    if (!statisticsIsShown) {
        hideSimilaritySettings();
    } else {
        sl_onRefSeqChanged(msa->getReferenceRowId());
    }
}

void SeqStatisticsWidget::sl_onAlgoChanged() {
    settings->algoId = ui.algoComboBox->currentData().toString();
    msa->getMainWidget()->setSimilaritySettings(settings);
}

void SeqStatisticsWidget::sl_onGapsChanged(int state) {
    settings->excludeGaps = (Qt::Checked == state);
    msa->getMainWidget()->setSimilaritySettings(settings);
}

void SeqStatisticsWidget::sl_onUnitsChanged(bool) {
    settings->usePercents = ui.percentsButton->isChecked();
    msa->getMainWidget()->setSimilaritySettings(settings);
}

void SeqStatisticsWidget::sl_onAutoUpdateChanged(int state) {
    settings->autoUpdate = state == Qt::Checked;
    ui.updateButton->setEnabled(!settings->autoUpdate);
    ui.dataState->setEnabled(!settings->autoUpdate);
    msa->getMainWidget()->setSimilaritySettings(settings);
}

void SeqStatisticsWidget::sl_onRefSeqChanged(qint64 referenceRowId) {
    if (U2MsaRow::INVALID_ROW_ID == referenceRowId && statisticsIsShown) {
        ui.refSeqWarning->show();
    } else {
        ui.refSeqWarning->hide();
    }
}

void SeqStatisticsWidget::sl_colorThemeSwitched() {
    ui.refSeqWarning->setStyleSheet(QString("color: %1; font: bold;").arg(Theme::infoColorLabelHtmlStr()));
}

void SeqStatisticsWidget::sl_onShowStatisticsChanged(int state) {
    if (Qt::Checked == state) {
        showSimilaritySettings();
    } else {
        hideSimilaritySettings();
    }
}

void SeqStatisticsWidget::sl_onUpdateClicked() {
    msa->getMainWidget()->refreshSimilarityColumn();
}

void SeqStatisticsWidget::hideSimilaritySettings() {
    statisticsIsShown = false;
    ui.optionsWidget->setEnabled(false);
    ui.refSeqWarning->hide();
    msa->getMainWidget()->hideSimilarity();
}

void SeqStatisticsWidget::showSimilaritySettings() {
    statisticsIsShown = true;
    ui.optionsWidget->setEnabled(true);
    ui.refSeqWarning->show();
    MsaEditorMultilineWgt* mui = msa->getMainWidget();
    mui->showSimilarity();
    mui->setSimilaritySettings(settings);
    sl_onRefSeqChanged(msa->getReferenceRowId());
}

}  // namespace U2
