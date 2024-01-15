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
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

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

void GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::Tabs tab) {
    GTWidget::click(GTWidget::findWidget(tabsNames[tab]));
    GTGlobals::sleep(500);
}

QWidget* GTUtilsOptionPanelMsa::openTab(Tabs tab) {
    if (!isTabOpened(tab)) {
        toggleTab(tab);
    }
    GTThread::waitForMainThread();
    QString widgetName = innerWidgetNames[tab];
    return GTWidget::findWidget(widgetName);
}

void GTUtilsOptionPanelMsa::closeTab(Tabs tab) {
    checkTabIsOpened(tab);
    toggleTab(tab);
}

bool GTUtilsOptionPanelMsa::isTabOpened(Tabs tab) {
    auto innerTabWidget = GTWidget::findWidget(innerWidgetNames[tab], nullptr, {false});
    return innerTabWidget != nullptr && innerTabWidget->isVisible();
}

QWidget* GTUtilsOptionPanelMsa::checkTabIsOpened(Tabs tab) {
    QString name = innerWidgetNames[tab];
    auto innerTabWidget = GTWidget::findWidget(name);
    GT_CHECK_RESULT(innerTabWidget->isVisible(), "MSA Editor options panel is not opened: " + name, nullptr);
    return innerTabWidget;
}

void GTUtilsOptionPanelMsa::addReference(const QString& seqName, AddRefMethod method) {
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    // Option panel should be opened to use this method.
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

void GTUtilsOptionPanelMsa::removeReference() {
    GTWidget::click(GTWidget::findWidget("deleteSeq"));
}

QString GTUtilsOptionPanelMsa::getReference() {
    openTab(General);
    return GTLineEdit::getText("sequenceLineEdit");
}

int GTUtilsOptionPanelMsa::getLength() {
    auto alignmentLengthLabel = GTWidget::findLabel("alignmentLength");
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok, "label text is not int", -1);
    return result;
}

int GTUtilsOptionPanelMsa::getHeight() {
    auto alignmentHeightLabel = GTWidget::findLabel("alignmentHeight");
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok, "label text is not int", -1);
    return result;
}

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

void GTUtilsOptionPanelMsa::setColorScheme(const QString& colorSchemeName, GTGlobals::UseMethod method) {
    openTab(Highlighting);
    GTComboBox::selectItemByText(GTWidget::findComboBox("colorScheme"), colorSchemeName, method);
}

QString GTUtilsOptionPanelMsa::getColorScheme() {
    openTab(Highlighting);
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    return colorScheme->currentText();
}

void GTUtilsOptionPanelMsa::setHighlightingScheme(const QString& highlightingSchemeName) {
    openTab(Highlighting);
    GTComboBox::selectItemByText(GTWidget::findComboBox("highlightingScheme"), highlightingSchemeName);
}

void GTUtilsOptionPanelMsa::addFirstSeqToPA(const QString& seqName, AddRefMethod method) {
    addSeqToPA(seqName, method, 1);
}

void GTUtilsOptionPanelMsa::addSecondSeqToPA(const QString& seqName, AddRefMethod method) {
    addSeqToPA(seqName, method, 2);
}

QString GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(int num) {
    auto le = qobject_cast<QLineEdit*>(getWidget("sequenceLineEdit", num));
    return le->text();
}

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

QToolButton* GTUtilsOptionPanelMsa::getAddButton(int number) {
    auto result = qobject_cast<QToolButton*>(getWidget("addSeq", number));
    GT_CHECK_RESULT(result != nullptr, "toolbutton is NULL", nullptr);
    return result;
}

QToolButton* GTUtilsOptionPanelMsa::getDeleteButton(int number) {
    auto result = qobject_cast<QToolButton*>(getWidget("deleteSeq", number));
    GT_CHECK_RESULT(result != nullptr, "toolbutton is NULL", nullptr);
    return result;
}

QPushButton* GTUtilsOptionPanelMsa::getAlignButton() {
    openTab(PairwiseAlignment);
    return GTWidget::findPushButton("alignButton");
}

void GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(const QString& algorithm) {
    openTab(PairwiseAlignment);
    GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmListComboBox"), algorithm);
}

void GTUtilsOptionPanelMsa::setThreshold(int threshold) {
    openTab(General);
    GTSlider::setValue(GTWidget::findSlider("thresholdSlider"), threshold);
}

int GTUtilsOptionPanelMsa::getThreshold() {
    openTab(General);
    auto thresholdSlider = GTWidget::findSlider("thresholdSlider");
    return thresholdSlider->value();
}

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

GTUtilsOptionPanelMsa::ThresholdComparison GTUtilsOptionPanelMsa::getThresholdComparison() {
    openTab(Highlighting);
    auto thresholdLessRb = GTWidget::findRadioButton("thresholdLessRb");
    auto thresholdMoreRb = GTWidget::findRadioButton("thresholdMoreRb");
    const bool lessOrEqual = thresholdLessRb->isChecked();
    const bool greaterOrEqual = thresholdMoreRb->isChecked();
    GT_CHECK_RESULT(lessOrEqual ^ greaterOrEqual, "Incorrect state of threshold comparison radiobuttons", LessOrEqual);
    return lessOrEqual ? LessOrEqual : GreaterOrEqual;
}

void GTUtilsOptionPanelMsa::setUseDotsOption(bool useDots) {
    openTab(Highlighting);
    GTCheckBox::setChecked(GTWidget::findCheckBox("useDots"), useDots);
}

bool GTUtilsOptionPanelMsa::isUseDotsOptionSet() {
    openTab(Highlighting);
    auto useDots = GTWidget::findCheckBox("useDots");
    return useDots->isChecked();
}

void GTUtilsOptionPanelMsa::setExportConsensusOutputPath(const QString& filePath) {
    openTab(ExportConsensus);
    GTLineEdit::setText("pathLe", filePath, nullptr);
}

QString GTUtilsOptionPanelMsa::getExportConsensusOutputPath() {
    return GTLineEdit::getText("pathLe");
}

QString GTUtilsOptionPanelMsa::getExportConsensusOutputFormat() {
    return GTComboBox::getCurrentText("formatCb");
}

void GTUtilsOptionPanelMsa::enterPattern(const QString& pattern, bool useCopyPaste /*= false*/) {
    auto patternEdit = GTWidget::findPlainTextEdit("textPattern");
    GTWidget::click(patternEdit);

    if (!patternEdit->toPlainText().isEmpty()) {
        GTPlainTextEdit::clear(patternEdit);
    }
    if (useCopyPaste) {
        GTClipboard::setText(pattern);
        GTKeyboardUtils::paste();
    } else {
        GTPlainTextEdit::setText(patternEdit, pattern);
    }
}

QString GTUtilsOptionPanelMsa::getPattern() {
    auto patternEdit = GTWidget::findPlainTextEdit("textPattern");
    return patternEdit->toPlainText();
}

void GTUtilsOptionPanelMsa::setAlgorithm(const QString& algorithm) {
    auto algoBox = GTWidget::findComboBox("boxAlgorithm");

    if (!algoBox->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));
    }
    GTComboBox::selectItemByText(algoBox, algorithm);
    GTGlobals::sleep(2500);
}

void GTUtilsOptionPanelMsa::setMatchPercentage(int percentage) {
    auto spinMatchBox = GTWidget::findSpinBox("spinBoxMatch");

    GTSpinBox::setValue(spinMatchBox, percentage, GTGlobals::UseKeyBoard);
    GTGlobals::sleep(2500);
}

void GTUtilsOptionPanelMsa::setCheckedRemoveOverlappedResults(bool setChecked) {
    auto overlapsBox = GTWidget::findCheckBox("removeOverlapsBox");

    if (!overlapsBox->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Other settings"));
    }
    GTCheckBox::setChecked("removeOverlapsBox", setChecked);
}

void GTUtilsOptionPanelMsa::checkResultsText(const QString& expectedText) {
    GTUtilsTaskTreeView::waitTaskFinished();
    auto label = GTWidget::findLabel("resultLabel");
    QString actualText = label->text();
    CHECK_SET_ERR(actualText == expectedText, QString("Wrong result. Expected: %1, got: %2").arg(expectedText).arg(actualText));
}

void GTUtilsOptionPanelMsa::clickNext() {
    auto next = GTWidget::findPushButton("nextPushButton");
    GTWidget::click(next);
}

void GTUtilsOptionPanelMsa::clickPrev() {
    auto prev = GTWidget::findPushButton("prevPushButton");
    GTWidget::click(prev);
}

QLineEdit* GTUtilsOptionPanelMsa::getSeqLineEdit(int number) {
    auto result = qobject_cast<QLineEdit*>(getWidget("sequenceLineEdit", number));
    GT_CHECK_RESULT(result != nullptr, "sequenceLineEdit is NULL", nullptr);
    return result;
}

bool GTUtilsOptionPanelMsa::isSearchInShowHideWidgetOpened() {
    auto searchInInnerWidget = GTWidget::findWidget("widgetSearchIn");
    return searchInInnerWidget->isVisible();
}

void GTUtilsOptionPanelMsa::openSearchInShowHideWidget(bool open) {
    CHECK(open != isSearchInShowHideWidgetOpened(), );
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
}

void GTUtilsOptionPanelMsa::setRegionType(const QString& regionType) {
    openSearchInShowHideWidget();
    GTComboBox::selectItemByText(GTWidget::findComboBox("boxRegion"), regionType);
}

void GTUtilsOptionPanelMsa::setRegion(int from, int to) {
    openSearchInShowHideWidget();
    GTLineEdit::setText("editStart", QString::number(from));
    GTLineEdit::setText("editEnd", QString::number(to));
}

void GTUtilsOptionPanelMsa::setSearchContext(const QString& context) {
    auto searchContextBox = GTWidget::findComboBox("searchContextComboBox");
    GTComboBox::selectItemByText(searchContextBox, context);
}

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

QString GTUtilsOptionPanelMsa::getAlphabetLabelText() {
    checkTabIsOpened(General);
    auto label = GTWidget::findLabel("alignmentAlphabet");
    return label->text();
}

void GTUtilsOptionPanelMsa::setOutputFile(const QString& outputFilePath) {
    auto tabWidget = checkTabIsOpened(PairwiseAlignment);
    auto outputLineEdit = GTWidget::findLineEdit("outputFileLineEdit");
    if (!outputLineEdit->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Output settings"));
    }

    GTLineEdit::setText(outputLineEdit, outputFilePath, tabWidget);
}

#undef GT_CLASS_NAME
}  // namespace U2
