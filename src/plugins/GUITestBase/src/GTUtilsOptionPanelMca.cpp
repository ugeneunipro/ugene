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

#include <QComboBox>
#include <QApplication>
#include <QLabel>
#include <QToolButton>

#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSlider.h>
#include <primitives/GTWidget.h>

#include "GTUtilsOptionPanelMca.h"

namespace U2 {
    using namespace HI;

QMap<GTUtilsOptionPanelMca::Tabs, QString> GTUtilsOptionPanelMca::initNames() {
    QMap<Tabs, QString> result;
    result.insert(General, "OP_MCA_GENERAL");
    result.insert(Consensus, "OP_CONSENSUS");
    return result;
}

QMap<GTUtilsOptionPanelMca::Tabs, QString> GTUtilsOptionPanelMca::initInnerWidgetNames() {
    QMap<Tabs, QString> result;
    result.insert(General, "McaGeneralTab");
    result.insert(Consensus, "ExportConsensusWidget");
    return result;
}

const QMap<GTUtilsOptionPanelMca::Tabs, QString> GTUtilsOptionPanelMca::tabsNames = initNames();
const QMap<GTUtilsOptionPanelMca::Tabs, QString> GTUtilsOptionPanelMca::innerWidgetNames = initInnerWidgetNames();

#define GT_CLASS_NAME "GTUtilsOptionPanelMca"

#define GT_METHOD_NAME "toggleTab"
void GTUtilsOptionPanelMca::toggleTab(HI::GUITestOpStatus &os, Tabs tab) {
    GTWidget::click(os, GTWidget::findWidget(os, tabsNames[tab]));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsOptionPanelMca::openTab(HI::GUITestOpStatus &os, Tabs tab) {
    if (!isTabOpened(os, tab)) {
       toggleTab(os, tab);
   }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelMca::closeTab(HI::GUITestOpStatus &os, Tabs tab) {
    if (isTabOpened(os, tab)) {
        toggleTab(os, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelMca::isTabOpened(HI::GUITestOpStatus &os, Tabs tab) {
        GTGlobals::FindOptions options;
        options.failIfNotFound = false;
        QWidget *innerTabWidget = GTWidget::findWidget(os, innerWidgetNames[tab], NULL, options);
        return NULL != innerTabWidget && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setConsensusType"
void GTUtilsOptionPanelMca::setConsensusType(HI::GUITestOpStatus &os, const QString &consensusTypeName) {
    openTab(os, Consensus);
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "consensusType"), consensusTypeName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusType"
QString GTUtilsOptionPanelMca::getConsensusType(HI::GUITestOpStatus &os) {
    openTab(os, Consensus);
    return GTComboBox::getCurrentText(os, GTWidget::findExactWidget<QComboBox *>(os, "consensusType"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusTypes"
QStringList GTUtilsOptionPanelMca::getConsensusTypes(HI::GUITestOpStatus &os) {
    openTab(os, Consensus);
    QStringList types = GTComboBox::getValues(os, GTWidget::findExactWidget<QComboBox *>(os, "consensusType"));
    return types;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHeight"
int GTUtilsOptionPanelMca::getHeight(HI::GUITestOpStatus &os){
    QLabel* alignmentHeightLabel = qobject_cast<QLabel*>(GTWidget::findWidget(os, "seqNumLabel"));
    GT_CHECK_RESULT(alignmentHeightLabel != NULL, "alignmentHeightLabel not found", -1);
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
int GTUtilsOptionPanelMca::getLength(HI::GUITestOpStatus &os){
    QLabel* alignmentLengthLabel = qobject_cast<QLabel*>(GTWidget::findWidget(os, "lengthLabel"));
    GT_CHECK_RESULT(alignmentLengthLabel != NULL, "alignmentLengthLabel not found", -1);
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThreshold"
void GTUtilsOptionPanelMca::setThreshold(GUITestOpStatus &os, int threshold) {
    openTab(os, Consensus);
    GTSlider::setValue(os, GTWidget::findExactWidget<QSlider *>(os, "thresholdSlider"), threshold);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThreshold"
int GTUtilsOptionPanelMca::getThreshold(GUITestOpStatus &os) {
    openTab(os, Consensus);
    QSlider *thresholdSlider = GTWidget::findExactWidget<QSlider *>(os, "thresholdSlider");
    GT_CHECK_RESULT(NULL != thresholdSlider, "thresholdSlider is NULL", -1);
    return thresholdSlider->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportFileName"
void GTUtilsOptionPanelMca::setExportFileName(HI::GUITestOpStatus &os, QString exportFileName) {
    openTab(os, Consensus);
    QLineEdit *exportToFileLineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "pathLe");
    GT_CHECK_RESULT(exportToFileLineEdit != NULL, "exportToFileLineEdit is NULL", );
    GTLineEdit::setText(os, exportToFileLineEdit, exportFileName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportFileName"
QString GTUtilsOptionPanelMca::getExportFileName(HI::GUITestOpStatus &os) {
    openTab(os, Consensus);
    QLineEdit *exportToFileLineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "pathLe");
    GT_CHECK_RESULT(exportToFileLineEdit != NULL, "exportToFileLineEdit is NULL", QString());
    return GTLineEdit::getText(os, exportToFileLineEdit);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFileFormat"
void GTUtilsOptionPanelMca::setFileFormat(HI::GUITestOpStatus &os, FileFormat fileFormat) {
    openTab(os, Consensus);
    QComboBox* formatCb = GTWidget::findExactWidget<QComboBox *>(os, "formatCb");
    GTComboBox::setCurrentIndex(os, formatCb, fileFormat);
    GTGlobals::sleep(1000);

}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pushResetButton"
void GTUtilsOptionPanelMca::pushResetButton(HI::GUITestOpStatus &os) {
    openTab(os, Consensus);
    QToolButton* result = GTWidget::findExactWidget<QToolButton *>(os, "thresholdResetButton");
    result->click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pushExportButton"
void GTUtilsOptionPanelMca::pushExportButton(HI::GUITestOpStatus &os) {
    openTab(os, Consensus);
    QToolButton* result = GTWidget::findExactWidget<QToolButton *>(os, "exportBtn");
    result->click();
}
#undef GT_METHOD_NAME

#undef GT_METHOD_NAME

}//namespace
