/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QComboBox>
#include <QLabel>

#include "GTUtilsMcaEditor.h"
#include "GTUtilsOptionPanelMca.h"

namespace U2 {
using namespace HI;

const QMap<GTUtilsOptionPanelMca::Tabs, QString> GTUtilsOptionPanelMca::tabsNames = {{General, "OP_MCA_GENERAL"},
                                                                                     {Consensus, "OP_CONSENSUS"},
                                                                                     {Reads, "OP_MCA_READS"}};
const QMap<GTUtilsOptionPanelMca::Tabs, QString> GTUtilsOptionPanelMca::innerWidgetNames = {{General, "McaGeneralTab"},
                                                                                            {Consensus, "ExportConsensusWidget"},
                                                                                            {Reads, "McaAlternativeMutationsWidget"}};

#define GT_CLASS_NAME "GTUtilsOptionPanelMca"

void GTUtilsOptionPanelMca::toggleTab(Tabs tab, QWidget* parent) {
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();
    GTWidget::click(GTWidget::findWidget(tabsNames[tab], parent));
    GTGlobals::sleep(500);
}

void GTUtilsOptionPanelMca::openTab(Tabs tab, QWidget* parent) {
    if (!isTabOpened(tab, parent)) {
        toggleTab(tab, parent);
    }
}

void GTUtilsOptionPanelMca::closeTab(Tabs tab) {
    if (isTabOpened(tab)) {
        toggleTab(tab);
    }
}

bool GTUtilsOptionPanelMca::isTabOpened(Tabs tab, QWidget* parent) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    auto innerTabWidget = GTWidget::findWidget(innerWidgetNames[tab], parent, options);
    return innerTabWidget != nullptr && innerTabWidget->isVisible();
}

void GTUtilsOptionPanelMca::setConsensusType(const QString& consensusTypeName) {
    openTab(Consensus);
    GTComboBox::selectItemByText(GTWidget::findComboBox("consensusType"), consensusTypeName);
}

QString GTUtilsOptionPanelMca::getConsensusType() {
    openTab(Consensus);
    return GTComboBox::getCurrentText(GTWidget::findComboBox("consensusType"));
}

QStringList GTUtilsOptionPanelMca::getConsensusTypes() {
    openTab(Consensus);
    QStringList types = GTComboBox::getValues(GTWidget::findComboBox("consensusType"));
    return types;
}

int GTUtilsOptionPanelMca::getHeight() {
    auto alignmentHeightLabel = GTWidget::findLabel("seqNumLabel");
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}

int GTUtilsOptionPanelMca::getLength() {
    auto alignmentLengthLabel = GTWidget::findLabel("lengthLabel");
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}

void GTUtilsOptionPanelMca::setThreshold(int threshold) {
    openTab(Consensus);
    GTSlider::setValue(GTWidget::findSlider("thresholdSlider"), threshold);
}

int GTUtilsOptionPanelMca::getThreshold() {
    openTab(Consensus);
    auto thresholdSlider = GTWidget::findSlider("thresholdSlider");
    return thresholdSlider->value();
}

void GTUtilsOptionPanelMca::setExportFileName(QString exportFileName) {
    openTab(Consensus);
    auto exportToFileLineEdit = GTWidget::findLineEdit("pathLe");
    GTLineEdit::setText(exportToFileLineEdit, exportFileName);
}

QString GTUtilsOptionPanelMca::getExportFileName() {
    openTab(Consensus);
    return GTLineEdit::getText("pathLe");
}

void GTUtilsOptionPanelMca::setFileFormat(FileFormat fileFormat) {
    openTab(Consensus);
    auto formatCb = GTWidget::findComboBox("formatCb");
    GTComboBox::selectItemByIndex(formatCb, fileFormat);
    GTGlobals::sleep(1000);
}

void GTUtilsOptionPanelMca::pushResetButton() {
    openTab(Consensus);
    GTWidget::click(GTWidget::findToolButton("thresholdResetButton"));
}

void GTUtilsOptionPanelMca::pushExportButton() {
    openTab(Consensus);
    GTWidget::click(GTWidget::findToolButton("exportBtn"));
}

void GTUtilsOptionPanelMca::showAlternativeMutations(bool show, int value, bool withSpinbox, QWidget* parent) {
    GTUtilsOptionPanelMca::openTab(Tabs::Reads, parent);
    GTGroupBox::setChecked("mutationsGroupBox", show, parent);
    if (!show) {
        GTThread::waitForMainThread();
        return;
    }

    if (withSpinbox) {
        GTSpinBox::setValue("mutationsThresholdSpinBox", value, parent);
    } else {
        GTSlider::setValue(GTWidget::findSlider("mutationsThresholdSlider", parent), value);
    }

    GTWidget::click(GTWidget::findPushButton("updateMutationsPushButton", parent));
    GTThread::waitForMainThread();
}


}  // namespace U2
