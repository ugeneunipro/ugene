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
void GTUtilsOptionPanelMsa::toggleTab(HI::GUITestOpStatus& os, GTUtilsOptionPanelMsa::Tabs tab) {
    GTWidget::click(os, GTWidget::findWidget(os, tabsNames[tab]));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
QWidget* GTUtilsOptionPanelMsa::openTab(HI::GUITestOpStatus& os, Tabs tab) {
    if (!isTabOpened(os, tab)) {
        toggleTab(os, tab);
    }
    GTThread::waitForMainThread();
    QString widgetName = innerWidgetNames[tab];
    return GTWidget::findWidget(os, widgetName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelMsa::closeTab(HI::GUITestOpStatus& os, Tabs tab) {
    checkTabIsOpened(os, tab);
    toggleTab(os, tab);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelMsa::isTabOpened(HI::GUITestOpStatus& os, Tabs tab) {
    auto innerTabWidget = GTWidget::findWidget(os, innerWidgetNames[tab], nullptr, {false});
    return innerTabWidget != nullptr && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTabIsOpened"
QWidget* GTUtilsOptionPanelMsa::checkTabIsOpened(HI::GUITestOpStatus& os, Tabs tab) {
    QString name = innerWidgetNames[tab];
    auto innerTabWidget = GTWidget::findWidget(os, name);
    GT_CHECK_RESULT(innerTabWidget->isVisible(), "MSA Editor options panel is not opened: " + name, nullptr);
    return innerTabWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addReference"
void GTUtilsOptionPanelMsa::addReference(HI::GUITestOpStatus& os, const QString& seqName, AddRefMethod method) {
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    // Option panel should be opned to use this method
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);

    GT_CHECK(nameList.contains(seqName), QString("sequence with name %1 not found").arg(seqName));

    switch (method) {
        case Button:
            GTUtilsMSAEditorSequenceArea::selectSequence(os, seqName);
            GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));
            break;
        case Completer:
            auto sequenceLineEdit = GTWidget::findWidget(os, "sequenceLineEdit");
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
void GTUtilsOptionPanelMsa::removeReference(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTWidget::findWidget(os, "deleteSeq"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReference"
QString GTUtilsOptionPanelMsa::getReference(HI::GUITestOpStatus& os) {
    openTab(os, General);
    return GTLineEdit::getText(os, "sequenceLineEdit");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
int GTUtilsOptionPanelMsa::getLength(HI::GUITestOpStatus& os) {
    auto alignmentLengthLabel = GTWidget::findLabel(os, "alignmentLength");
    bool ok;
    int result = alignmentLengthLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHeight"
int GTUtilsOptionPanelMsa::getHeight(HI::GUITestOpStatus& os) {
    auto alignmentHeightLabel = GTWidget::findLabel(os, "alignmentHeight");
    bool ok;
    int result = alignmentHeightLabel->text().toInt(&ok);
    GT_CHECK_RESULT(ok, "label text is not int", -1);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copySelection"
void GTUtilsOptionPanelMsa::copySelection(HI::GUITestOpStatus& os, const CopyFormat& format) {
    openTab(os, General);
    auto copyType = GTWidget::findComboBox(os, "copyType");

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
    GTComboBox::selectItemByText(os, copyType, stringFormat);

    auto copyButton = GTWidget::findToolButton(os, "copyButton");

    GTWidget::click(os, copyButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setColorScheme"
void GTUtilsOptionPanelMsa::setColorScheme(HI::GUITestOpStatus& os, const QString& colorSchemeName, GTGlobals::UseMethod method) {
    openTab(os, Highlighting);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "colorScheme"), colorSchemeName, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColorScheme"
QString GTUtilsOptionPanelMsa::getColorScheme(HI::GUITestOpStatus& os) {
    openTab(os, Highlighting);
    auto colorScheme = GTWidget::findComboBox(os, "colorScheme");
    return colorScheme->currentText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setHighlightingScheme"
void GTUtilsOptionPanelMsa::setHighlightingScheme(GUITestOpStatus& os, const QString& highlightingSchemeName) {
    openTab(os, Highlighting);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "highlightingScheme"), highlightingSchemeName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addFirstSeqToPA"
void GTUtilsOptionPanelMsa::addFirstSeqToPA(HI::GUITestOpStatus& os, const QString& seqName, AddRefMethod method) {
    addSeqToPA(os, seqName, method, 1);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addSecondSeqToPA"
void GTUtilsOptionPanelMsa::addSecondSeqToPA(HI::GUITestOpStatus& os, const QString& seqName, AddRefMethod method) {
    addSeqToPA(os, seqName, method, 2);
}
#undef GT_METHOD_NAME

QString GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(HI::GUITestOpStatus& os, int num) {
    auto le = qobject_cast<QLineEdit*>(getWidget(os, "sequenceLineEdit", num));
    return le->text();
}

#define GT_METHOD_NAME "addSeqToPA"
void GTUtilsOptionPanelMsa::addSeqToPA(HI::GUITestOpStatus& os, const QString& seqName, AddRefMethod method, int number) {
    GT_CHECK(number == 1 || number == 2, "number must be 1 or 2");
    GT_CHECK(!seqName.isEmpty(), "sequence name is empty");
    // Option panel should be opned to use this method
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);

    GT_CHECK(nameList.contains(seqName), QString("sequence with name %1 not found").arg(seqName));

    switch (method) {
        case Button:
            GTUtilsMSAEditorSequenceArea::selectSequence(os, seqName);
            GTWidget::click(os, getAddButton(os, number));
            break;
        case Completer:
            QWidget* sequenceLineEdit = getSeqLineEdit(os, number);
            GTWidget::click(os, sequenceLineEdit);
            GTKeyboardDriver::keyClick(seqName.at(0).toLatin1());
            GTGlobals::sleep(200);
            GTBaseCompleter::click(os, sequenceLineEdit, seqName);
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAddButton"
QToolButton* GTUtilsOptionPanelMsa::getAddButton(HI::GUITestOpStatus& os, int number) {
    auto result = qobject_cast<QToolButton*>(getWidget(os, "addSeq", number));
    GT_CHECK_RESULT(result != nullptr, "toolbutton is NULL", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDeleteButton"
QToolButton* GTUtilsOptionPanelMsa::getDeleteButton(HI::GUITestOpStatus& os, int number) {
    auto result = qobject_cast<QToolButton*>(getWidget(os, "deleteSeq", number));
    GT_CHECK_RESULT(result != nullptr, "toolbutton is NULL", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAlignButton"
QPushButton* GTUtilsOptionPanelMsa::getAlignButton(HI::GUITestOpStatus& os) {
    openTab(os, PairwiseAlignment);
    return GTWidget::findPushButton(os, "alignButton");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setPairwiseAlignmentAlgorithm"
void GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(HI::GUITestOpStatus& os, const QString& algorithm) {
    openTab(os, PairwiseAlignment);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "algorithmListComboBox"), algorithm);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThreshold"
void GTUtilsOptionPanelMsa::setThreshold(GUITestOpStatus& os, int threshold) {
    openTab(os, General);
    GTSlider::setValue(os, GTWidget::findSlider(os, "thresholdSlider"), threshold);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThreshold"
int GTUtilsOptionPanelMsa::getThreshold(GUITestOpStatus& os) {
    openTab(os, General);
    auto thresholdSlider = GTWidget::findSlider(os, "thresholdSlider");
    return thresholdSlider->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThresholdComparison"
void GTUtilsOptionPanelMsa::setThresholdComparison(GUITestOpStatus& os, GTUtilsOptionPanelMsa::ThresholdComparison comparison) {
    openTab(os, Highlighting);
    switch (comparison) {
        case LessOrEqual:
            GTRadioButton::click(os, GTWidget::findRadioButton(os, "thresholdLessRb"));
            break;
        case GreaterOrEqual:
            GTRadioButton::click(os, GTWidget::findRadioButton(os, "thresholdMoreRb"));
            break;
        default:
            GT_FAIL(QString("An unknown threshold comparison type: %1").arg(comparison), );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getThresholdComparison"
GTUtilsOptionPanelMsa::ThresholdComparison GTUtilsOptionPanelMsa::getThresholdComparison(GUITestOpStatus& os) {
    openTab(os, Highlighting);
    auto thresholdLessRb = GTWidget::findRadioButton(os, "thresholdLessRb");
    auto thresholdMoreRb = GTWidget::findRadioButton(os, "thresholdMoreRb");
    const bool lessOrEqual = thresholdLessRb->isChecked();
    const bool greaterOrEqual = thresholdMoreRb->isChecked();
    GT_CHECK_RESULT(lessOrEqual ^ greaterOrEqual, "Incorrect state of threshold comparison radiobuttons", LessOrEqual);
    return lessOrEqual ? LessOrEqual : GreaterOrEqual;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUseDotsOption"
void GTUtilsOptionPanelMsa::setUseDotsOption(GUITestOpStatus& os, bool useDots) {
    openTab(os, Highlighting);
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "useDots"), useDots);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUseDotsOptionSet"
bool GTUtilsOptionPanelMsa::isUseDotsOptionSet(GUITestOpStatus& os) {
    openTab(os, Highlighting);
    auto useDots = GTWidget::findCheckBox(os, "useDots");
    return useDots->isChecked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportConsensusOutputPath"
void GTUtilsOptionPanelMsa::setExportConsensusOutputPath(GUITestOpStatus& os, const QString& filePath) {
    openTab(os, ExportConsensus);
    GTLineEdit::setText(os, "pathLe", filePath, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportConsensusOutputPath"
QString GTUtilsOptionPanelMsa::getExportConsensusOutputPath(GUITestOpStatus& os) {
    return GTLineEdit::getText(os, "pathLe");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExportConsensusOutputFormat"
QString GTUtilsOptionPanelMsa::getExportConsensusOutputFormat(GUITestOpStatus& os) {
    return GTComboBox::getCurrentText(os, "formatCb");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterPattern"
void GTUtilsOptionPanelMsa::enterPattern(HI::GUITestOpStatus& os, const QString& pattern, bool useCopyPaste /*= false*/) {
    auto patternEdit = GTWidget::findPlainTextEdit(os, "textPattern");
    GTWidget::click(os, patternEdit);

    if (!patternEdit->toPlainText().isEmpty()) {
        GTPlainTextEdit::clear(os, patternEdit);
    }
    if (useCopyPaste) {
        GTClipboard::setText(os, pattern);
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    } else {
        GTPlainTextEdit::setText(os, patternEdit, pattern);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPattern"
QString GTUtilsOptionPanelMsa::getPattern(GUITestOpStatus& os) {
    auto patternEdit = GTWidget::findPlainTextEdit(os, "textPattern");
    return patternEdit->toPlainText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAlgorithm"
void GTUtilsOptionPanelMsa::setAlgorithm(HI::GUITestOpStatus& os, const QString& algorithm) {
    auto algoBox = GTWidget::findComboBox(os, "boxAlgorithm");

    if (!algoBox->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));
    }
    GTComboBox::selectItemByText(os, algoBox, algorithm);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
void GTUtilsOptionPanelMsa::setMatchPercentage(HI::GUITestOpStatus& os, int percentage) {
    auto spinMatchBox = GTWidget::findSpinBox(os, "spinBoxMatch");

    GTSpinBox::setValue(os, spinMatchBox, percentage, GTGlobals::UseKeyBoard);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCheckedRemoveOverlappedResults"
void GTUtilsOptionPanelMsa::setCheckedRemoveOverlappedResults(HI::GUITestOpStatus& os, bool setChecked) {
    auto overlapsBox = GTWidget::findCheckBox(os, "removeOverlapsBox");

    if (!overlapsBox->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
    }
    GTCheckBox::setChecked(os, "removeOverlapsBox", setChecked);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkResultsText"
void GTUtilsOptionPanelMsa::checkResultsText(HI::GUITestOpStatus& os, const QString& expectedText) {
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto label = GTWidget::findLabel(os, "resultLabel");
    QString actualText = label->text();
    CHECK_SET_ERR(actualText == expectedText, QString("Wrong result. Expected: %1, got: %2").arg(expectedText).arg(actualText));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNext"

void GTUtilsOptionPanelMsa::clickNext(HI::GUITestOpStatus& os) {
    auto next = GTWidget::findPushButton(os, "nextPushButton");
    GTWidget::click(os, next);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickPrev"
void GTUtilsOptionPanelMsa::clickPrev(HI::GUITestOpStatus& os) {
    auto prev = GTWidget::findPushButton(os, "prevPushButton");
    GTWidget::click(os, prev);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSeqLineEdit"
QLineEdit* GTUtilsOptionPanelMsa::getSeqLineEdit(HI::GUITestOpStatus& os, int number) {
    auto result = qobject_cast<QLineEdit*>(getWidget(os, "sequenceLineEdit", number));
    GT_CHECK_RESULT(result != nullptr, "sequenceLineEdit is NULL", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSearchInShowHideWidgetOpened"
bool GTUtilsOptionPanelMsa::isSearchInShowHideWidgetOpened(HI::GUITestOpStatus& os) {
    auto searchInInnerWidget = GTWidget::findWidget(os, "widgetSearchIn");
    return searchInInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSearchInShowHideWidget"
void GTUtilsOptionPanelMsa::openSearchInShowHideWidget(HI::GUITestOpStatus& os, bool open) {
    CHECK(open != isSearchInShowHideWidgetOpened(os), );
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegionType"
void GTUtilsOptionPanelMsa::setRegionType(HI::GUITestOpStatus& os, const QString& regionType) {
    openSearchInShowHideWidget(os);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "boxRegion"), regionType);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegion"
void GTUtilsOptionPanelMsa::setRegion(HI::GUITestOpStatus& os, int from, int to) {
    openSearchInShowHideWidget(os);
    GTLineEdit::setText(os, "editStart", QString::number(from));
    GTLineEdit::setText(os, "editEnd", QString::number(to));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchContext"
void GTUtilsOptionPanelMsa::setSearchContext(HI::GUITestOpStatus& os, const QString& context) {
    auto searchContextBox = GTWidget::findComboBox(os, "searchContextComboBox");
    GTComboBox::selectItemByText(os, searchContextBox, context);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidget"
QWidget* GTUtilsOptionPanelMsa::getWidget(HI::GUITestOpStatus& os, const QString& widgetName, int number) {
    auto sequenceContainerWidget = GTWidget::findWidget(os, "sequenceContainerWidget");
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
QString GTUtilsOptionPanelMsa::getAlphabetLabelText(HI::GUITestOpStatus& os) {
    checkTabIsOpened(os, General);
    auto label = GTWidget::findLabel(os, "alignmentAlphabet");
    return label->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setOutputFile"
void GTUtilsOptionPanelMsa::setOutputFile(HI::GUITestOpStatus& os, const QString& outputFilePath) {
    auto tabWidget = checkTabIsOpened(os, PairwiseAlignment);
    auto outputLineEdit = GTWidget::findLineEdit(os, "outputFileLineEdit");
    if (!outputLineEdit->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));
    }

    GTLineEdit::setText(os, outputLineEdit, outputFilePath, tabWidget);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
