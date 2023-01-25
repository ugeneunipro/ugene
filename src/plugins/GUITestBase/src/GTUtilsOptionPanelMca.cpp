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

#define GT_METHOD_NAME "toggleTab"
void GTUtilsOptionPanelMca::toggleTab(HI::GUITestOpStatus& os, Tabs tab, QWidget* parent) {
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);
    GTWidget::click(os, GTWidget::findWidget(os, tabsNames[tab], parent));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsOptionPanelMca::openTab(HI::GUITestOpStatus& os, Tabs tab, QWidget* parent) {
    if (!isTabOpened(os, tab, parent)) {
        toggleTab(os, tab, parent);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelMca::closeTab(HI::GUITestOpStatus& os, Tabs tab) {
    if (isTabOpened(os, tab)) {
        toggleTab(os, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelMca::isTabOpened(HI::GUITestOpStatus& os, Tabs tab, QWidget* parent) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    auto innerTabWidget = GTWidget::findWidget(os, innerWidgetNames[tab], parent, options);
    return nullptr != innerTabWidget && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setConsensusType"
void GTUtilsOptionPanelMca::setConsensusType(HI::GUITestOpStatus& os, const QString& consensusTypeName) {
    openTab(os, Consensus);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "consensusType"), consensusTypeName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusType"
QString GTUtilsOptionPanelMca::getConsensusType(HI::GUITestOpStatus& os) {
    openTab(os, Consensus);
    return GTComboBox::getCurrentText(os, GTWidget::findComboBox(os, "consensusType"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusTypes"
QStringList GTUtilsOptionPanelMca::getConsensusTypes(HI::GUITestOpStatus& os) {
    openTab(os, Consensus);
    QStringList types = GTComboBox::getValues(os, GTWidget::findComboBox(os, "consensusType"));
    return types;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHeight"
int GTUtilsOptionPanelMca::getHeight(HI::GUITestOpStatus& os) {
    auto alignmentHeightLabel = GTWidget::findLabel(os, "seqNumLabel");
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
int GTUtilsOptionPanelMca::getLength(HI::GUITestOpStatus& os) {
    auto alignmentLengthLabel = GTWidget::findLabel(os, "lengthLabel");
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThreshold"
void GTUtilsOptionPanelMca::setThreshold(GUITestOpStatus& os, int threshold) {
    openTab(os, Consensus);
    GTSlider::setValue(os, GTWidget::findSlider(os, "thresholdSlider"), threshold);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThreshold"
int GTUtilsOptionPanelMca::getThreshold(GUITestOpStatus& os) {
    openTab(os, Consensus);
    auto thresholdSlider = GTWidget::findSlider(os, "thresholdSlider");
    return thresholdSlider->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportFileName"
void GTUtilsOptionPanelMca::setExportFileName(HI::GUITestOpStatus& os, QString exportFileName) {
    openTab(os, Consensus);
    auto exportToFileLineEdit = GTWidget::findLineEdit(os, "pathLe");
    GTLineEdit::setText(os, exportToFileLineEdit, exportFileName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportFileName"
QString GTUtilsOptionPanelMca::getExportFileName(HI::GUITestOpStatus& os) {
    openTab(os, Consensus);
    return GTLineEdit::getText(os, "pathLe");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFileFormat"
void GTUtilsOptionPanelMca::setFileFormat(HI::GUITestOpStatus& os, FileFormat fileFormat) {
    openTab(os, Consensus);
    auto formatCb = GTWidget::findComboBox(os, "formatCb");
    GTComboBox::selectItemByIndex(os, formatCb, fileFormat);
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pushResetButton"
void GTUtilsOptionPanelMca::pushResetButton(HI::GUITestOpStatus& os) {
    openTab(os, Consensus);
    GTWidget::click(os, GTWidget::findToolButton(os, "thresholdResetButton"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pushExportButton"
void GTUtilsOptionPanelMca::pushExportButton(HI::GUITestOpStatus& os) {
    openTab(os, Consensus);
    GTWidget::click(os, GTWidget::findToolButton(os, "exportBtn"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showAlternativeMutations"
void GTUtilsOptionPanelMca::showAlternativeMutations(HI::GUITestOpStatus& os, bool show, int value, bool withSpinbox, QWidget* parent) {
    GTUtilsOptionPanelMca::openTab(os, Tabs::Reads, parent);
    GTGroupBox::setChecked(os, "mutationsGroupBox", show, parent);
    if (!show) {
        GTThread::waitForMainThread();
        return;
    }

    if (withSpinbox) {
        GTSpinBox::setValue(os, "mutationsThresholdSpinBox", value, parent);
    } else {
        GTSlider::setValue(os, GTWidget::findSlider(os, "mutationsThresholdSlider", parent), value);
    }

    GTWidget::click(os, GTWidget::findPushButton(os, "updateMutationsPushButton", parent));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#undef GT_METHOD_NAME

}  // namespace U2
