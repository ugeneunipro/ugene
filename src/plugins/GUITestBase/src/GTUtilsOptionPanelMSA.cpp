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

#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <system/GTClipboard.h>
#include <utils/GTThread.h>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>

#include <U2Core/U2IdTypes.h>

#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"

namespace U2 {
using namespace HI;

QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::initNames() {
    QMap<Tabs, QString> result;
    result.insert(General, "OP_MSA_GENERAL");
    result.insert(Highlighting, "OP_MSA_HIGHLIGHTING");
    result.insert(PairwiseAlignment, "OP_PAIRALIGN");
    result.insert(AddTree, "OP_MSA_ADD_TREE_WIDGET");
    result.insert(TreeOptions, "OP_MSA_TREES_WIDGET");
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
    result.insert(AddTree, "AddTreeWidget");
    result.insert(TreeOptions, "TreeOptionsWidget");
    result.insert(ExportConsensus, "ExportConsensusWidget");
    result.insert(Statistics, "SequenceStatisticsOptionsPanelTab");
    result.insert(Search, "FindPatternMsaWidget");
    return result;
}
const QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::tabsNames = initNames();
const QMap<GTUtilsOptionPanelMsa::Tabs, QString> GTUtilsOptionPanelMsa::innerWidgetNames = initInnerWidgetNames();

#define GT_CLASS_NAME "GTUtilsOptionPanelMSA"

#define GT_METHOD_NAME "toggleTab"
void GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::Tabs tab) {
    GTWidget::click(GTWidget::findWidget(tabsNames[tab]));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
QWidget* GTUtilsOptionPanelMsa::openTab(Tabs tab) {
    if (!isTabOpened(tab)) {
        toggleTab(tab);
    }
    GTThread::waitForMainThread();
    QString widgetName = innerWidgetNames[tab];
    return GTWidget::findWidget(widgetName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelMsa::closeTab(Tabs tab) {
    checkTabIsOpened(tab);
    toggleTab(tab);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelMsa::isTabOpened(Tabs tab) {
    auto innerTabWidget = GTWidget::findWidget(innerWidgetNames[tab], nullptr, {false});
    return innerTabWidget != nullptr && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTabIsOpened"
QWidget* GTUtilsOptionPanelMsa::checkTabIsOpened(Tabs tab) {
    QString name = innerWidgetNames[tab];
    auto innerTabWidget = GTWidget::findWidget(name);
    GT_CHECK_RESULT(innerTabWidget->isVisible(), "MSA Editor options panel is not opened: " + name, nullptr);
    return innerTabWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addReference"
void GTUtilsOptionPanelMsa::addReference(const QString& seqName, AddRefMethod method) {
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    // Option panel should be opned to use this method
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();

    GT_CHECK(nameList.contains(seqName), QString("sequence with name %1 not found").arg(seqName));

    switch (method) {
        case Button:
            GTUtilsMSAEditorSequenceArea::selectSequence(seqName);
            GTWidget::click(GTWidget::findWidget("addSeq"));
            break;
        case Completer:
            auto sequenceLineEdit = GTWidget::findWidget("sequenceLineEdit");
            GTWidget::click(sequenceLineEdit);
            GTKeyboardDriver::keyClick(seqName.at(0).toLatin1());
            GTGlobals::sleep(200);
            GTBaseCompleter::click(sequenceLineEdit, seqName);
            break;
    }
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeReference"
void GTUtilsOptionPanelMsa::removeReference() {
    GTWidget::click(GTWidget::findWidget("deleteSeq"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReference"
QString GTUtilsOptionPanelMsa::getReference() {
    openTab(General);
    return GTLineEdit::getText("sequenceLineEdit");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
int GTUtilsOptionPanelMsa::getLength() {
    auto alignmentLengthLabel = GTWidget::findLabel("alignmentLength");
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHeight"
int GTUtilsOptionPanelMsa::getHeight() {
    auto alignmentHeightLabel = GTWidget::findLabel("alignmentHeight");
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copySelection"
void GTUtilsOptionPanelMsa::copySelection(const CopyFormat& format) {
    openTab(General);
    auto copyType = GTWidget::findComboBox("copyType");

    QString stringFormat;
    switch (format) {
        case CopyFormat::Fasta:
            stringFormat = "Fasta";
            break;
        case CopyFormat::CLUSTALW:
            stringFormat = "CLUSTALW";
            break;
        case CopyFormat::Stocholm:
            stringFormat = "Stocholm";
            break;
        case CopyFormat::MSF:
            stringFormat = "MSF";
            break;
        case CopyFormat::NEXUS:
            stringFormat = "NEXUS";
            break;
        case CopyFormat::Mega:
            stringFormat = "Mega";
            break;
        case CopyFormat::PHYLIP_Interleaved:
            stringFormat = "PHYLIP Interleaved";
            break;
        case CopyFormat::PHYLIP_Sequential:
            stringFormat = "PHYLIP Sequential";
            break;
        case CopyFormat::Rich_text:
            stringFormat = "Rich text (HTML)";
            break;
        default:
            GT_FAIL("Unexpected format", );
    }
    GTComboBox::selectItemByText(copyType, stringFormat);

    auto copyButton = GTWidget::findToolButton("copyButton");

    GTWidget::click(copyButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setColorScheme"
void GTUtilsOptionPanelMsa::setColorScheme(const QString& colorSchemeName, GTGlobals::UseMethod method) {
    openTab(Highlighting);
    GTComboBox::selectItemByText(GTWidget::findComboBox("colorScheme"), colorSchemeName, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColorScheme"
QString GTUtilsOptionPanelMsa::getColorScheme() {
    openTab(Highlighting);
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    return colorScheme->currentText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setHighlightingScheme"
void GTUtilsOptionPanelMsa::setHighlightingScheme(const QString& highlightingSchemeName) {
    openTab(Highlighting);
    GTComboBox::selectItemByText(GTWidget::findComboBox("highlightingScheme"), highlightingSchemeName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addFirstSeqToPA"
void GTUtilsOptionPanelMsa::addFirstSeqToPA(const QString& seqName, AddRefMethod method) {
    addSeqToPA(seqName, method, 1);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addSecondSeqToPA"
void GTUtilsOptionPanelMsa::addSecondSeqToPA(const QString& seqName, AddRefMethod method) {
    addSeqToPA(seqName, method, 2);
}
#undef GT_METHOD_NAME

QString GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(int num) {
    auto le = qobject_cast<QLineEdit*>(getWidget("sequenceLineEdit", num));
    return le->text();
}

#define GT_METHOD_NAME "addSeqToPA"
void GTUtilsOptionPanelMsa::addSeqToPA(const QString& seqName, AddRefMethod method, int number) {
    GT_CHECK(number == 1 || number == 2, "number must be 1 or 2");
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    // Option panel should be opned to use this method
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();

    GT_CHECK(nameList.contains(seqName), QString("sequence with name %1 not found").arg(seqName));

    switch (method) {
        case Button:
            GTUtilsMSAEditorSequenceArea::selectSequence(seqName);
            GTWidget::click(getAddButton(number));
            break;
        case Completer:
            QWidget* sequenceLineEdit = getSeqLineEdit(number);
            GTWidget::click(sequenceLineEdit);
            GTKeyboardDriver::keyClick(seqName.at(0).toLatin1());
            GTGlobals::sleep(200);
            GTBaseCompleter::click(sequenceLineEdit, seqName);
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAddButton"
QToolButton* GTUtilsOptionPanelMsa::getAddButton(int number) {
    auto result = qobject_cast<QToolButton*>(getWidget("addSeq", number));
    GT_CHECK_RESULT(result != nullptr, "toolbutton is NULL", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDeleteButton"
QToolButton* GTUtilsOptionPanelMsa::getDeleteButton(int number) {
    auto result = qobject_cast<QToolButton*>(getWidget("deleteSeq", number));
    GT_CHECK_RESULT(result != nullptr, "toolbutton is NULL", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAlignButton"
QPushButton* GTUtilsOptionPanelMsa::getAlignButton() {
    openTab(PairwiseAlignment);
    return GTWidget::findPushButton("alignButton");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setPairwiseAlignmentAlgorithm"
void GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(const QString& algorithm) {
    openTab(PairwiseAlignment);
    GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmListComboBox"), algorithm);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThreshold"
void GTUtilsOptionPanelMsa::setThreshold(int threshold) {
    openTab(General);
    GTSlider::setValue(GTWidget::findSlider("thresholdSlider"), threshold);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThreshold"
int GTUtilsOptionPanelMsa::getThreshold() {
    openTab(General);
    auto thresholdSlider = GTWidget::findSlider("thresholdSlider");
    return thresholdSlider->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThresholdComparison"
void GTUtilsOptionPanelMsa::setThresholdComparison(GTUtilsOptionPanelMsa::ThresholdComparison comparison) {
    openTab(Highlighting);
    switch (comparison) {
        case LessOrEqual:
            GTRadioButton::click(GTWidget::findRadioButton("thresholdLessRb"));
            break;
        case GreaterOrEqual:
            GTRadioButton::click(GTWidget::findRadioButton("thresholdMoreRb"));
            break;
        default:
            GT_FAIL(QString("An unknown threshold comparison type: %1").arg(comparison), );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThresholdComparison"
GTUtilsOptionPanelMsa::ThresholdComparison GTUtilsOptionPanelMsa::getThresholdComparison() {
    openTab(Highlighting);
    auto thresholdLessRb = GTWidget::findRadioButton("thresholdLessRb");
    auto thresholdMoreRb = GTWidget::findRadioButton("thresholdMoreRb");
    const bool lessOrEqual = thresholdLessRb->isChecked();
    const bool greaterOrEqual = thresholdMoreRb->isChecked();
    GT_CHECK_RESULT(lessOrEqual ^ greaterOrEqual, "Incorrect state of threshold comparison radiobuttons", LessOrEqual);
    return lessOrEqual ? LessOrEqual : GreaterOrEqual;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUseDotsOption"
void GTUtilsOptionPanelMsa::setUseDotsOption(bool useDots) {
    openTab(Highlighting);
    GTCheckBox::setChecked(GTWidget::findCheckBox("useDots"), useDots);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUseDotsOptionSet"
bool GTUtilsOptionPanelMsa::isUseDotsOptionSet() {
    openTab(Highlighting);
    auto useDots = GTWidget::findCheckBox("useDots");
    return useDots->isChecked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportConsensusOutputPath"
void GTUtilsOptionPanelMsa::setExportConsensusOutputPath(const QString& filePath) {
    openTab(ExportConsensus);
    GTLineEdit::setText("pathLe", filePath, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportConsensusOutputPath"
QString GTUtilsOptionPanelMsa::getExportConsensusOutputPath() {
    return GTLineEdit::getText("pathLe");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportConsensusOutputFormat"
QString GTUtilsOptionPanelMsa::getExportConsensusOutputFormat() {
    return GTComboBox::getCurrentText("formatCb");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterPattern"
void GTUtilsOptionPanelMsa::enterPattern(const QString& pattern, bool useCopyPaste /*= false*/) {
    auto patternEdit = GTWidget::findPlainTextEdit("textPattern");
    GTWidget::click(patternEdit);

    if (!patternEdit->toPlainText().isEmpty()) {
        GTPlainTextEdit::clear(patternEdit);
    }
    if (useCopyPaste) {
        GTClipboard::setText(pattern);
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    } else {
        GTPlainTextEdit::setText(patternEdit, pattern);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPattern"
QString GTUtilsOptionPanelMsa::getPattern() {
    auto patternEdit = GTWidget::findPlainTextEdit("textPattern");
    return patternEdit->toPlainText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAlgorithm"
void GTUtilsOptionPanelMsa::setAlgorithm(const QString& algorithm) {
    auto algoBox = GTWidget::findComboBox("boxAlgorithm");

    if (!algoBox->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));
    }
    GTComboBox::selectItemByText(algoBox, algorithm);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
void GTUtilsOptionPanelMsa::setMatchPercentage(int percentage) {
    auto spinMatchBox = GTWidget::findSpinBox("spinBoxMatch");

    GTSpinBox::setValue(spinMatchBox, percentage, GTGlobals::UseKeyBoard);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCheckedRemoveOverlappedResults"
void GTUtilsOptionPanelMsa::setCheckedRemoveOverlappedResults(bool setChecked) {
    auto overlapsBox = GTWidget::findCheckBox("removeOverlapsBox");

    if (!overlapsBox->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Other settings"));
    }
    GTCheckBox::setChecked("removeOverlapsBox", setChecked);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkResultsText"
void GTUtilsOptionPanelMsa::checkResultsText(const QString& expectedText) {
    GTUtilsTaskTreeView::waitTaskFinished();
    auto label = GTWidget::findLabel("resultLabel");
    QString actualText = label->text();
    CHECK_SET_ERR(actualText == expectedText, QString("Wrong result. Expected: %1, got: %2").arg(expectedText).arg(actualText));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNext"

void GTUtilsOptionPanelMsa::clickNext() {
    auto next = GTWidget::findPushButton("nextPushButton");
    GTWidget::click(next);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickPrev"
void GTUtilsOptionPanelMsa::clickPrev() {
    auto prev = GTWidget::findPushButton("prevPushButton");
    GTWidget::click(prev);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqLineEdit"
QLineEdit* GTUtilsOptionPanelMsa::getSeqLineEdit(int number) {
    auto result = qobject_cast<QLineEdit*>(getWidget("sequenceLineEdit", number));
    GT_CHECK_RESULT(result != nullptr, "sequenceLineEdit is NULL", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSearchInShowHideWidgetOpened"
bool GTUtilsOptionPanelMsa::isSearchInShowHideWidgetOpened() {
    auto searchInInnerWidget = GTWidget::findWidget("widgetSearchIn");
    return searchInInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSearchInShowHideWidget"
void GTUtilsOptionPanelMsa::openSearchInShowHideWidget(bool open) {
    CHECK(open != isSearchInShowHideWidgetOpened(), );
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegionType"
void GTUtilsOptionPanelMsa::setRegionType(const QString& regionType) {
    openSearchInShowHideWidget();
    GTComboBox::selectItemByText(GTWidget::findComboBox("boxRegion"), regionType);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegion"
void GTUtilsOptionPanelMsa::setRegion(int from, int to) {
    openSearchInShowHideWidget();
    GTLineEdit::setText("editStart", QString::number(from));
    GTLineEdit::setText("editEnd", QString::number(to));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchContext"
void GTUtilsOptionPanelMsa::setSearchContext(const QString& context) {
    auto searchContextBox = GTWidget::findComboBox("searchContextComboBox");
    GTComboBox::selectItemByText(searchContextBox, context);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidget"
QWidget* GTUtilsOptionPanelMsa::getWidget(const QString& widgetName, int number) {
    auto sequenceContainerWidget = GTWidget::findWidget("sequenceContainerWidget");
    QList<QWidget*> widgetList = sequenceContainerWidget->findChildren<QWidget*>(widgetName);
    GT_CHECK_RESULT(widgetList.count() == 2, QString("unexpected number of widgets: %1").arg(widgetList.count()), nullptr);
    QWidget* w1 = widgetList[0];
    QWidget* w2 = widgetList[1];
    int y1 = w1->mapToGlobal(w1->rect().center()).y();
    int y2 = w2->mapToGlobal(w2->rect().center()).y();
    GT_CHECK_RESULT(y1 != y2, "coordinates are unexpectidly equal", nullptr);

    if (number == 1) {
        return y1 < y2 ? w1 : w2;
    } else if (number == 2) {
        return y1 < y2 ? w2 : w1;
    }
    GT_FAIL("Number should be 1 or 2", nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAlphabetLabelText"
QString GTUtilsOptionPanelMsa::getAlphabetLabelText() {
    checkTabIsOpened(General);
    auto label = GTWidget::findLabel("alignmentAlphabet");
    return label->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setOutputFile"
void GTUtilsOptionPanelMsa::setOutputFile(const QString& outputFilePath) {
    auto tabWidget = checkTabIsOpened(PairwiseAlignment);
    auto outputLineEdit = GTWidget::findLineEdit("outputFileLineEdit");
    if (!outputLineEdit->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Output settings"));
    }

    GTLineEdit::setText(outputLineEdit, outputFilePath, tabWidget);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
