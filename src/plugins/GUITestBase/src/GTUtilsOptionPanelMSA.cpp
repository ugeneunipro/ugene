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
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeWidget>

#include <drivers/GTKeyboardDriver.h>

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <system/GTClipboard.h>

#include <utils/GTThread.h>

#include <U2Core/U2IdTypes.h>

#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"

namespace U2 {
using namespace HI;

QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::initNames() {
    QMap<Tabs,QString> result;
    result.insert(General, "OP_MSA_GENERAL");
    result.insert(Highlighting, "OP_MSA_HIGHLIGHTING");
    result.insert(PairwiseAlignment, "OP_PAIRALIGN");
    result.insert(TreeSettings, "OP_MSA_ADD_TREE_WIDGET");
    result.insert(ExportConsensus, "OP_EXPORT_CONSENSUS");
    result.insert(Statistics, "OP_SEQ_STATISTICS_WIDGET");
    result.insert(Search, "OP_MSA_FIND_PATTERN_WIDGET");
    return result;
}

QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::initInnerWidgetNames() {
    QMap<Tabs, QString> result;
    result.insert(General, "MsaGeneralTab");
    result.insert(Highlighting, "HighlightingOptionsPanelWidget");
    result.insert(PairwiseAlignment, "PairwiseAlignmentOptionsPanelWidget");
    result.insert(TreeSettings, "AddTreeWidget");
    result.insert(ExportConsensus, "ExportConsensusWidget");
    result.insert(Statistics, "SequenceStatisticsOptionsPanelTab");
    result.insert(Search, "FindPatternMsaWidget");
    return result;
}
const QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::tabsNames = initNames();
const QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::innerWidgetNames = initInnerWidgetNames();

#define GT_CLASS_NAME "GTUtilsOptionPanelMSA"

#define GT_METHOD_NAME "toggleTab"
void GTUtilsOptionPanelMsa::toggleTab(HI::GUITestOpStatus &os, GTUtilsOptionPanelMsa::Tabs tab) {
    GTWidget::click(os, GTWidget::findWidget(os, tabsNames[tab]));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsOptionPanelMsa::openTab(HI::GUITestOpStatus &os, Tabs tab) {
    if (!isTabOpened(os, tab)) {
        toggleTab(os, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelMsa::closeTab(HI::GUITestOpStatus &os, Tabs tab) {
    if (isTabOpened(os, tab)) {
        toggleTab(os, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelMsa::isTabOpened(HI::GUITestOpStatus &os, Tabs tab) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *innerTabWidget = GTWidget::findWidget(os, innerWidgetNames[tab], NULL, options);
    return NULL != innerTabWidget && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addReference"
void GTUtilsOptionPanelMsa::addReference(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method){
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    //Option panel should be opned to use this method
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);

    GT_CHECK(nameList.contains(seqName), QString("sequence with name %1 not found").arg(seqName));

    switch (method)
    {
    case Button:
        GTUtilsMSAEditorSequenceArea::selectSequence(os, seqName);
        GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));
        break;
    case Completer:
        QWidget* sequenceLineEdit = GTWidget::findWidget(os, "sequenceLineEdit");
        GTWidget::click(os, sequenceLineEdit);
        GTKeyboardDriver::keyClick(seqName.at(0).toLatin1());
        GTGlobals::sleep(200);
        GTBaseCompleter::click(os, sequenceLineEdit, seqName);
        break;
    }
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeReference"
void GTUtilsOptionPanelMsa::removeReference(HI::GUITestOpStatus &os){
    GTWidget::click(os, GTWidget::findWidget(os, "deleteSeq"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReference"
QString GTUtilsOptionPanelMsa::getReference(HI::GUITestOpStatus &os) {
    openTab(os, General);
    QLineEdit *leReference = GTWidget::findExactWidget<QLineEdit *>(os, "sequenceLineEdit");
    GT_CHECK_RESULT(NULL != leReference, "Reference sequence name lineedit is NULL", QString());
    return leReference->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
int GTUtilsOptionPanelMsa::getLength(HI::GUITestOpStatus &os){
    QLabel* alignmentLengthLabel = qobject_cast<QLabel*>(GTWidget::findWidget(os, "alignmentLength"));
    GT_CHECK_RESULT(alignmentLengthLabel != NULL, "alignmentLengthLabel not found", -1);
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHeight"
int GTUtilsOptionPanelMsa::getHeight(HI::GUITestOpStatus &os){
    QLabel* alignmentHeightLabel = qobject_cast<QLabel*>(GTWidget::findWidget(os, "alignmentHeight"));
    GT_CHECK_RESULT(alignmentHeightLabel != NULL, "alignmentHeightLabel not found", -1);
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok == true, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copySelection"
void GTUtilsOptionPanelMsa::copySelection(HI::GUITestOpStatus &os, const CopyFormat format) {
    openTab(os, General);
    QComboBox *copyType = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "copyType"));
    GT_CHECK_RESULT(copyType != nullptr, "copyType not found", );

    QString stringFirmat;
    switch (format) {
    case CopyFormat::Fasta:
        stringFirmat = "Fasta";
        break;
    case CopyFormat::CLUSTALW:
        stringFirmat = "CLUSTALW";
        break;
    case CopyFormat::Stocholm:
        stringFirmat = "Stocholm";
        break;
    case CopyFormat::MSF:
        stringFirmat = "MSF";
        break;
    case CopyFormat::NEXUS:
        stringFirmat = "NEXUS";
        break;
    case CopyFormat::Mega:
        stringFirmat = "Mega";
        break;
    case CopyFormat::PHYLIP_Interleaved:
        stringFirmat = "PHYLIP Interleaved";
        break;
    case CopyFormat::PHYLIP_Sequential:
        stringFirmat = "PHYLIP Sequential";
        break;
    case CopyFormat::Rich_text:
        stringFirmat = "Rich text (HTML)";
        break;

    default:
        GT_CHECK_RESULT(false, "Unexpected format", );
        break;
    }
    GTComboBox::setIndexWithText(os, copyType, stringFirmat);

    QToolButton *copyButton = qobject_cast<QToolButton *>(GTWidget::findWidget(os, "copyButton"));
    GT_CHECK_RESULT(copyButton != nullptr, "copyType not found", );

    GTWidget::click(os, copyButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setColorScheme"
void GTUtilsOptionPanelMsa::setColorScheme(HI::GUITestOpStatus &os, const QString &colorSchemeName, GTGlobals::UseMethod method) {
    openTab(os, Highlighting);
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "colorScheme"), colorSchemeName, true, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColorScheme"
QString GTUtilsOptionPanelMsa::getColorScheme(HI::GUITestOpStatus &os) {
    openTab(os, Highlighting);
    QComboBox *colorScheme = GTWidget::findExactWidget<QComboBox *>(os, "colorScheme");
    GT_CHECK_RESULT(NULL != colorScheme, "ColorSCheme combobox is NULL", "");
    return colorScheme->currentText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setHighlightingScheme"
void GTUtilsOptionPanelMsa::setHighlightingScheme(GUITestOpStatus &os, const QString &highlightingSchemeName) {
    openTab(os, Highlighting);
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "highlightingScheme"), highlightingSchemeName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addFirstSeqToPA"
void GTUtilsOptionPanelMsa::addFirstSeqToPA(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method){
    addSeqToPA(os, seqName, method, 1);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addSecondSeqToPA"
void GTUtilsOptionPanelMsa::addSecondSeqToPA(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method){
    addSeqToPA(os, seqName, method, 2);
}
#undef GT_METHOD_NAME

QString GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(HI::GUITestOpStatus &os, int num){
    QLineEdit* le = qobject_cast<QLineEdit*>(getWidget(os, "sequenceLineEdit", num));
    return le->text();
}

#define GT_METHOD_NAME "addSeqToPA"
void GTUtilsOptionPanelMsa::addSeqToPA(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method, int number){
    GT_CHECK(number == 1 || number == 2, "number must be 1 or 2");
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    //Option panel should be opned to use this method
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);

    GT_CHECK(nameList.contains(seqName), QString("sequence with name %1 not found").arg(seqName));

    switch (method)
    {
    case Button:
        GTUtilsMSAEditorSequenceArea::selectSequence(os, seqName);
        GTWidget::click(os, getAddButton(os, number));
        break;
    case Completer:
        QWidget* sequenceLineEdit = getSeqLineEdit(os, number);
        GTWidget::click(os, sequenceLineEdit);
        GTKeyboardDriver::keyClick( seqName.at(0).toLatin1());
        GTGlobals::sleep(200);
        GTBaseCompleter::click(os, sequenceLineEdit, seqName);
        break;
    }
}
#undef GT_METHOD_NAME


#define GT_METHOD_NAME "getAddButton"
QToolButton* GTUtilsOptionPanelMsa::getAddButton(HI::GUITestOpStatus &os, int number){
    QToolButton* result = qobject_cast<QToolButton*>(getWidget(os, "addSeq", number));
    GT_CHECK_RESULT(result != NULL, "toolbutton is NULL", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDeleteButton"
QToolButton* GTUtilsOptionPanelMsa::getDeleteButton(HI::GUITestOpStatus &os, int number){
    QToolButton* result = qobject_cast<QToolButton*>(getWidget(os, "deleteSeq", number));
    GT_CHECK_RESULT(result != NULL, "toolbutton is NULL", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAlignButton"
QPushButton *GTUtilsOptionPanelMsa::getAlignButton(HI::GUITestOpStatus &os) {
    openTab(os, PairwiseAlignment);
    return GTWidget::findExactWidget<QPushButton *>(os, "alignButton");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setPairwiseAlignmentAlgorithm"
void GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(HI::GUITestOpStatus &os, const QString &algorithm) {
    openTab(os, PairwiseAlignment);
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox*>(os, "algorithmListComboBox"), algorithm);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThreshold"
void GTUtilsOptionPanelMsa::setThreshold(GUITestOpStatus &os, int threshold) {
    openTab(os, General);
    GTSlider::setValue(os, GTWidget::findExactWidget<QSlider *>(os, "thresholdSlider"), threshold);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThreshold"
int GTUtilsOptionPanelMsa::getThreshold(GUITestOpStatus &os) {
    openTab(os, General);
    QSlider *thresholdSlider = GTWidget::findExactWidget<QSlider *>(os, "thresholdSlider");
    GT_CHECK_RESULT(NULL != thresholdSlider, "thresholdSlider is NULL", -1);
    return thresholdSlider->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThresholdComparison"
void GTUtilsOptionPanelMsa::setThresholdComparison(GUITestOpStatus &os, GTUtilsOptionPanelMsa::ThresholdComparison comparison) {
    openTab(os, Highlighting);
    switch (comparison) {
    case LessOrEqual:
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "thresholdLessRb"));
        break;
    case GreaterOrEqual:
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "thresholdMoreRb"));
        break;
    default:
        GT_CHECK(false, QString("An unknown threshold comparison type: %1").arg(comparison));
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThresholdComparison"
GTUtilsOptionPanelMsa::ThresholdComparison GTUtilsOptionPanelMsa::getThresholdComparison(GUITestOpStatus &os) {
    openTab(os, Highlighting);
    QRadioButton *thresholdLessRb = GTWidget::findExactWidget<QRadioButton *>(os, "thresholdLessRb");
    GT_CHECK_RESULT(NULL != thresholdLessRb, "thresholdLessRb is NULL", LessOrEqual);
    QRadioButton *thresholdMoreRb = GTWidget::findExactWidget<QRadioButton *>(os, "thresholdMoreRb");
    GT_CHECK_RESULT(NULL != thresholdMoreRb, "thresholdMoreRb is NULL", LessOrEqual);
    const bool lessOrEqual = thresholdLessRb->isChecked();
    const bool greaterOrEqual = thresholdMoreRb->isChecked();
    GT_CHECK_RESULT(lessOrEqual ^ greaterOrEqual, "Incorrect state of threshold comparison radiobuttons", LessOrEqual);
    return lessOrEqual ? LessOrEqual : GreaterOrEqual;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUseDotsOption"
void GTUtilsOptionPanelMsa::setUseDotsOption(GUITestOpStatus &os, bool useDots) {
    openTab(os, Highlighting);
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "useDots"), useDots);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUseDotsOptionSet"
bool GTUtilsOptionPanelMsa::isUseDotsOptionSet(GUITestOpStatus &os) {
    openTab(os, Highlighting);
    QCheckBox *useDots = GTWidget::findExactWidget<QCheckBox *>(os, "useDots");
    GT_CHECK_RESULT(NULL != useDots, "useDots checkbox is NULL", false);
    return useDots->isChecked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportConsensusOutputPath"
void GTUtilsOptionPanelMsa::setExportConsensusOutputPath(GUITestOpStatus &os, const QString &filePath) {
    openTab(os, ExportConsensus);
    GTLineEdit::setText(os, "pathLe", filePath, NULL);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportConsensusOutputPath"
QString GTUtilsOptionPanelMsa::getExportConsensusOutputPath(GUITestOpStatus &os) {
    return GTLineEdit::getText(os, "pathLe");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportConsensusOutputFormat"
void GTUtilsOptionPanelMsa::setExportConsensusOutputFormat(GUITestOpStatus &os, const QString &format) {
    openTab(os, ExportConsensus);
    GTComboBox::setIndexWithText(os, "formatCb", NULL, format);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportConsensusOutputFormat"
QString GTUtilsOptionPanelMsa::getExportConsensusOutputFormat(GUITestOpStatus &os) {
    return GTComboBox::getCurrentText(os, "formatCb");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterPattern"
void GTUtilsOptionPanelMsa::enterPattern(HI::GUITestOpStatus &os, QString pattern, bool useCopyPaste /*= false*/) {
    QTextEdit *patternEdit = qobject_cast<QTextEdit *>(GTWidget::findWidget(os, "textPattern"));
    GTWidget::click(os, patternEdit);

    GTTextEdit::clear(os, patternEdit);
    if (useCopyPaste) {
        GTClipboard::setText(os, pattern);
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    } else {
        GTTextEdit::setText(os, patternEdit, pattern);
    }

    GTGlobals::sleep(3000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPattern"
QString GTUtilsOptionPanelMsa::getPattern(GUITestOpStatus &os) {
    QTextEdit *patternEdit = GTWidget::findExactWidget<QTextEdit *>(os, "textPattern");
    GT_CHECK_RESULT(nullptr != patternEdit, "textPattern widget is nullptr", "");
    return patternEdit->toPlainText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAlgorithm"
void GTUtilsOptionPanelMsa::setAlgorithm(HI::GUITestOpStatus &os, QString algorithm) {
    QComboBox *algoBox = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "boxAlgorithm"));
    GT_CHECK(algoBox != NULL, "algoBox is NULL");

    if (!algoBox->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));
    }
    GTComboBox::setIndexWithText(os, algoBox, algorithm);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
void GTUtilsOptionPanelMsa::setMatchPercentage(HI::GUITestOpStatus &os, int percentage) {
    QSpinBox *spinMatchBox = qobject_cast<QSpinBox *>(GTWidget::findWidget(os, "spinBoxMatch"));

    GTSpinBox::setValue(os, spinMatchBox, percentage, GTGlobals::UseKeyBoard);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCheckedRemoveOverlappedResults"
void GTUtilsOptionPanelMsa::setCheckedRemoveOverlappedResults(HI::GUITestOpStatus &os, bool setChecked) {
    QCheckBox *overlapsBox = qobject_cast<QCheckBox *>(GTWidget::findWidget(os, "removeOverlapsBox"));
    GT_CHECK(overlapsBox != NULL, "overlapsBox is NULL");

    if (!overlapsBox->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
    }
    GTCheckBox::setChecked(os, "removeOverlapsBox", setChecked);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkResultsText"
bool GTUtilsOptionPanelMsa::checkResultsText(HI::GUITestOpStatus &os, QString expectedText) {
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QLabel *label = qobject_cast<QLabel *>(GTWidget::findWidget(os, "resultLabel"));
    return label->text() == expectedText;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNext"

void GTUtilsOptionPanelMsa::clickNext(HI::GUITestOpStatus &os) {
    QPushButton *next = qobject_cast<QPushButton *>(GTWidget::findWidget(os, "nextPushButton"));
    GTWidget::click(os, next);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickPrev"

void GTUtilsOptionPanelMsa::clickPrev(HI::GUITestOpStatus &os) {
    QPushButton *prev = qobject_cast<QPushButton *>(GTWidget::findWidget(os, "prevPushButton"));
    GTWidget::click(os, prev);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqLineEdit"
QLineEdit* GTUtilsOptionPanelMsa::getSeqLineEdit(HI::GUITestOpStatus &os, int number){
    QLineEdit* result = qobject_cast<QLineEdit*>(getWidget(os, "sequenceLineEdit", number));
    GT_CHECK_RESULT(result != NULL, "sequenceLineEdit is NULL", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidget"
QWidget* GTUtilsOptionPanelMsa::getWidget(HI::GUITestOpStatus &os, const QString& widgetName, int number){
    QWidget* sequenceContainerWidget = GTWidget::findWidget(os, "sequenceContainerWidget");
    GT_CHECK_RESULT(sequenceContainerWidget != NULL, "sequenceContainerWidget not found", NULL);
    QList<QWidget*> widgetList = sequenceContainerWidget->findChildren<QWidget*>(widgetName);
    GT_CHECK_RESULT(widgetList.count() == 2, QString("unexpected number of widgets: %1").arg(widgetList.count()), NULL);
    QWidget* w1 = widgetList[0];
    QWidget* w2 = widgetList[1];
    int y1 = w1->mapToGlobal(w1->rect().center()).y();
    int y2 = w2->mapToGlobal(w2->rect().center()).y();
    GT_CHECK_RESULT(y1 != y2, "coordinates are unexpectidly equal", NULL);

    if(number == 1){
        if(y1 < y2){
            return w1;
        }else{
            return w2;
        }
    }else if(number == 2){
        if(y1 < y2){
            return w2;
        }else{
            return w1;
        }
    }else{
        GT_CHECK_RESULT(false, "number should be 1 or 2", NULL);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}
