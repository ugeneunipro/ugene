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
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTWidget.h>
#include <system/GTClipboard.h>
#include <utils/GTThread.h>

#include <QDir>
#include <QGroupBox>
#include <QTableWidget>

#include <U2Core/U2SafePoints.h>

#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {
using namespace HI;

const QString GTUtilsOptionPanelSequenceView::meltingTmReportString = "<tr><td>Melting temperature: </td><td style=\"vertical-align:top;\">%1 °C&nbsp;&nbsp;<a href=\"Melting temperature\"><img src=\":core/images/gear.svg\" width=16 height=16;\"></a></td></tr>";

QMap<GTUtilsOptionPanelSequenceView::Tabs, QString> GTUtilsOptionPanelSequenceView::initNames() {
    QMap<Tabs, QString> result;
    result.insert(Search, "OP_FIND_PATTERN");
    result.insert(AnnotationsHighlighting, "OP_ANNOT_HIGHLIGHT");
    result.insert(Statistics, "OP_SEQ_INFO");
    result.insert(InSilicoPcr, "OP_IN_SILICO_PCR");
    result.insert(CircularView, "OP_CV_SETTINGS");
    result.insert(GeneCut, "OP_GENECUT");
    return result;
}

QMap<GTUtilsOptionPanelSequenceView::Tabs, QString> GTUtilsOptionPanelSequenceView::initInnerWidgetNames() {
    QMap<Tabs, QString> result;
    result.insert(Search, "FindPatternWidget");
    result.insert(AnnotationsHighlighting, "AnnotHighlightWidget");
    result.insert(Statistics, "SequenceInfo");
    result.insert(InSilicoPcr, "InSilicoPcrOptionPanelWidget");
    result.insert(CircularView, "CircularViewSettingsWidget");
    result.insert(GeneCut, "GenecutOPWidget");
    return result;
}

const QMap<GTUtilsOptionPanelSequenceView::Tabs, QString> GTUtilsOptionPanelSequenceView::tabsNames = initNames();
const QMap<GTUtilsOptionPanelSequenceView::Tabs, QString> GTUtilsOptionPanelSequenceView::innerWidgetNames = initInnerWidgetNames();

#define GT_CLASS_NAME "GTUtilsOptionPanelSequenceView"

#define GT_METHOD_NAME "enterPattern"

void GTUtilsOptionPanelSequenceView::enterPattern(const QString& pattern, bool useCopyPaste) {
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

#define GT_METHOD_NAME "toggleTab"
void GTUtilsOptionPanelSequenceView::toggleTab(GTUtilsOptionPanelSequenceView::Tabs tab) {
    GTWidget::click(GTWidget::findWidget(tabsNames[tab], GTUtilsSequenceView::getActiveSequenceViewWindow()));
    GTGlobals::sleep(200);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsOptionPanelSequenceView::openTab(Tabs tab) {
    if (!isTabOpened(tab)) {
        toggleTab(tab);
        GTThread::waitForMainThread();
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelSequenceView::closeTab(GTUtilsOptionPanelSequenceView::Tabs tab) {
    if (isTabOpened(tab)) {
        toggleTab(tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelSequenceView::isTabOpened(GTUtilsOptionPanelSequenceView::Tabs tab) {
    auto innerTabWidget = GTWidget::findWidget(innerWidgetNames[tab], GTUtilsSequenceView::getActiveSequenceViewWindow(), {false});
    return innerTabWidget != nullptr && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTabIsOpened"
void GTUtilsOptionPanelSequenceView::checkTabIsOpened(GTUtilsOptionPanelSequenceView::Tabs tab) {
    QString name = innerWidgetNames[tab];
    GTWidget::findWidget(name, GTUtilsSequenceView::getActiveSequenceViewWindow());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkResultsText"

bool GTUtilsOptionPanelSequenceView::checkResultsText(const QString& expectedText) {
    GTUtilsTaskTreeView::waitTaskFinished();

    auto label = GTWidget::findLabel("resultLabel");
    return label->text() == expectedText;
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchWithAmbiguousBases"
void GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases(bool searchWithAmbiguousBases) {
    GTCheckBox::setChecked(GTWidget::findCheckBox("useAmbiguousBasesBox"), searchWithAmbiguousBases);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNext"

void GTUtilsOptionPanelSequenceView::clickNext() {
    auto next = GTWidget::findPushButton("nextPushButton");
    GTWidget::click(next);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickPrev"

void GTUtilsOptionPanelSequenceView::clickPrev() {
    auto prev = GTWidget::findPushButton("prevPushButton");
    GTWidget::click(prev);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickGetAnnotation"

void GTUtilsOptionPanelSequenceView::clickGetAnnotation() {
    auto getAnnotations = GTWidget::findPushButton("getAnnotationsPushButton");
    GTWidget::click(getAnnotations);
    GTThread::waitForMainThread();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isPrevNextEnabled"

bool GTUtilsOptionPanelSequenceView::isPrevNextEnabled() {
    auto next = GTWidget::findPushButton("nextPushButton");
    auto prev = GTWidget::findPushButton("prevPushButton");
    return prev->isEnabled() && next->isEnabled();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isGetAnnotationsEnabled"
bool GTUtilsOptionPanelSequenceView::isGetAnnotationsEnabled() {
    auto getAnnotations = GTWidget::findPushButton("getAnnotationsPushButton");
    return getAnnotations->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCircularView"
void GTUtilsOptionPanelSequenceView::toggleCircularView() {
    GTWidget::click(GTWidget::findButtonByText("Open Circular View(s)", GTUtilsSequenceView::getActiveSequenceViewWindow()));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setTitleFontSize"
void GTUtilsOptionPanelSequenceView::setTitleFontSize(int fontSize) {
    GTSpinBox::setValue(GTWidget::findSpinBox("fontSizeSpinBox"), fontSize);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTitleFontSize"
int GTUtilsOptionPanelSequenceView::getTitleFontSize() {
    return GTSpinBox::getValue(GTWidget::findSpinBox("fontSizeSpinBox", GTUtilsSequenceView::getActiveSequenceViewWindow()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setForwardPrimer"
void GTUtilsOptionPanelSequenceView::setForwardPrimer(const QString& primer) {
    openTab(InSilicoPcr);
    auto primerContainer = GTWidget::findWidget("forwardPrimerBox");
    GTLineEdit::setText("primerEdit", primer, primerContainer);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setForwardPrimerMismatches"
void GTUtilsOptionPanelSequenceView::setForwardPrimerMismatches(const int mismatches) {
    openTab(InSilicoPcr);
    auto primerContainer = GTWidget::findWidget("forwardPrimerBox");

    auto mismatchesSpinBox = GTWidget::findSpinBox("mismatchesSpinBox", primerContainer);

    GTSpinBox::setValue(mismatchesSpinBox, mismatches, GTGlobals::UseKey);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReversePrimer"
void GTUtilsOptionPanelSequenceView::setReversePrimer(const QString& primer) {
    openTab(InSilicoPcr);
    auto primerContainer = GTWidget::findWidget("reversePrimerBox");
    GTLineEdit::setText("primerEdit", primer, primerContainer);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReversePrimerMismatches"
void GTUtilsOptionPanelSequenceView::setReversePrimerMismatches(const int mismatches) {
    openTab(InSilicoPcr);
    auto primerContainer = GTWidget::findWidget("reversePrimerBox");

    auto mismatchesSpinBox = GTWidget::findSpinBox("mismatchesSpinBox", primerContainer);

    GTSpinBox::setValue(mismatchesSpinBox, mismatches, GTGlobals::UseKey);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "productsCount"
int GTUtilsOptionPanelSequenceView::productsCount() {
    openTab(InSilicoPcr);
    auto tableWidget = GTWidget::findTableWidget("productsTable");

    return tableWidget->rowCount();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showPrimersDetails"
void GTUtilsOptionPanelSequenceView::showPrimersDetails() {
    openTab(InSilicoPcr);
    auto label = GTWidget::findWidget("detailsLinkLabel");
    GTWidget::click(GTWidget::findWidget("detailsLinkLabel"), Qt::LeftButton, QPoint(20, label->geometry().height() / 2));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pressFindProducts"
void GTUtilsOptionPanelSequenceView::pressFindProducts() {
    openTab(InSilicoPcr);
    auto findProducts = GTWidget::findPushButton("findProductButton");

    GTWidget::click(findProducts);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pressExtractProduct"
void GTUtilsOptionPanelSequenceView::pressExtractProduct() {
    openTab(InSilicoPcr);
    auto extractProduct = GTWidget::findPushButton("extractProductButton");
    GT_CHECK(extractProduct->isEnabled(), "Extract Product buttons is unexpectedly disabled");

    GTWidget::click(extractProduct);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSearchAlgorithmShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isSearchAlgorithmShowHideWidgetOpened() {
    auto algorithmInnerWidget = GTWidget::findWidget("widgetAlgorithm");
    return algorithmInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSearchInShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isSearchInShowHideWidgetOpened() {
    auto searchInInnerWidget = GTWidget::findWidget("widgetSearchIn");
    return searchInInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isOtherSettingsShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isOtherSettingsShowHideWidgetOpened() {
    auto otherSettingsInnerWidget = GTWidget::findWidget("widgetOther");
    return otherSettingsInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSaveAnnotationToShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isSaveAnnotationToShowHideWidgetOpened() {
    auto annotationsWidget = GTWidget::findWidget("annotationsWidget");
    auto saveAnnotationsToInnerWidget = GTWidget::findWidget("saveAnnotationsInnerWidget", annotationsWidget);
    return saveAnnotationsToInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isAnnotationParametersShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isAnnotationParametersShowHideWidgetOpened() {
    auto annotationsWidget = GTWidget::findWidget("annotationsWidget");
    auto annotationsParametersInnerWidget = GTWidget::findWidget("annotationParametersInnerWidget", annotationsWidget);
    return annotationsParametersInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSearchInShowHideWidget"
void GTUtilsOptionPanelSequenceView::openSearchInShowHideWidget(bool open) {
    CHECK(open != isSearchInShowHideWidgetOpened(), );
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSaveAnnotationToShowHideWidget"
void GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(bool open) {
    CHECK(open != isSaveAnnotationToShowHideWidgetOpened(), );
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Save annotation(s) to"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openAnnotationParametersShowHideWidget"
void GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(bool open) {
    CHECK(open != isAnnotationParametersShowHideWidgetOpened(), );
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Annotation parameters"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openInSilicoPcrMeltingTemperatureShowHideWidget"
void GTUtilsOptionPanelSequenceView::openInSilicoPcrMeltingTemperatureShowHideWidget() {
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Melting temperature"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showMeltingTemperatureDialog"
void GTUtilsOptionPanelSequenceView::showMeltingTemperatureDialog() {
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);

    auto statisticsLabel = GTWidget::findLabel("Common Statistics");
    GTMouseDriver::moveTo(statisticsLabel->mapToGlobal({30, 30}));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Tab);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleInputFromFilePattern"

void GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern() {
    auto loadFromFile = GTWidget::findRadioButton("usePatternFromFileRadioButton");
    GTWidget::click(loadFromFile);
    // kinda hack for QGroupBox should be rewritten
    GTKeyboardDriver::keyClick(Qt::Key_Space);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleSaveAnnotationsTo"

void GTUtilsOptionPanelSequenceView::toggleSaveAnnotationsTo() {
    auto obj = GTWidget::findLabel("ArrowHeader_Save annotation(s) to");
    GTWidget::click(obj);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterPatternFromFile"

void GTUtilsOptionPanelSequenceView::enterPatternFromFile(const QString& filePathStr, const QString& fileName) {
    GTFileDialogUtils* ob = new GTFileDialogUtils(filePathStr, fileName, GTFileDialogUtils::Open);
    GTUtilsDialog::waitForDialog(ob);

    auto browse = GTWidget::findToolButton("loadFromFileToolButton");
    GTWidget::click(browse);
    GTGlobals::sleep(2500);
    GTUtilsTaskTreeView::waitTaskFinished();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setStrand"

void GTUtilsOptionPanelSequenceView::setStrand(const QString& strandStr) {
    auto strand = GTWidget::findComboBox("boxStrand", nullptr, {false});

    if (!strand->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    }
    GTComboBox::selectItemByText(strand, strandStr);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegionType"
void GTUtilsOptionPanelSequenceView::setRegionType(const QString& regionType) {
    openSearchInShowHideWidget();
    GTComboBox::selectItemByText(GTWidget::findComboBox("boxRegion"), regionType);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegion"
void GTUtilsOptionPanelSequenceView::setRegion(int from, int to) {
    openSearchInShowHideWidget();
    GTLineEdit::setText("editStart", QString::number(from));
    GTLineEdit::setText("editEnd", QString::number(to));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterFilepathForSavingAnnotations"
void GTUtilsOptionPanelSequenceView::enterFilepathForSavingAnnotations(const QString& filepath) {
    QDir().mkpath(QFileInfo(filepath).dir().absolutePath());
    GTLineEdit::setText("leNewTablePath", filepath);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAlgorithm"
void GTUtilsOptionPanelSequenceView::setAlgorithm(const QString& algorithm) {
    auto algoBox = GTWidget::findComboBox("boxAlgorithm");

    if (!algoBox->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));
    }
    GTComboBox::selectItemByText(algoBox, algorithm);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMatchPercentage"
int GTUtilsOptionPanelSequenceView::getMatchPercentage() {
    auto spinMatchBox = GTWidget::findSpinBox("spinBoxMatch");
    return spinMatchBox->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRegionType"
QString GTUtilsOptionPanelSequenceView::getRegionType() {
    openSearchInShowHideWidget();
    auto cbRegionType = GTWidget::findComboBox("boxRegion");
    return cbRegionType->currentText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
QPair<int, int> GTUtilsOptionPanelSequenceView::getRegion() {
    openSearchInShowHideWidget();

    QPair<int, int> result;
    auto leRegionStartText = GTLineEdit::getText("editStart");
    auto leRegionEndText = GTLineEdit::getText("editEnd");

    bool ok = false;
    const int regionStart = leRegionStartText.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Can't convert the string to int: %1").arg(leRegionStartText), result);
    const int regionEnd = leRegionEndText.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Can't convert the string to int: %1").arg(leRegionEndText), result);

    return qMakePair(regionStart, regionEnd);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHintText"
QString GTUtilsOptionPanelSequenceView::getHintText() {
    auto lblHint = GTWidget::findLabel("lblErrorMessage");
    return lblHint->isVisible() ? lblHint->text() : "";
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
void GTUtilsOptionPanelSequenceView::setMatchPercentage(int percentage) {
    auto spinMatchBox = GTWidget::findSpinBox("spinBoxMatch");

    GTSpinBox::setValue(spinMatchBox, percentage, GTGlobals::UseKeyBoard);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUsePatternName"
void GTUtilsOptionPanelSequenceView::setUsePatternName(bool setChecked) {
    openAnnotationParametersShowHideWidget();
    GTCheckBox::setChecked(GTWidget::findCheckBox("chbUsePatternNames"), setChecked);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchInTranslation"
void GTUtilsOptionPanelSequenceView::setSearchInTranslation(bool inTranslation /* = true*/) {
    auto searchIn = GTWidget::findComboBox("boxSeqTransl", nullptr, {false});

    if (!searchIn->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    }
    if (inTranslation) {
        GTComboBox::selectItemByText(searchIn, "Translation");
    } else {
        GTComboBox::selectItemByText(searchIn, "Sequence");
    }
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchInLocation"
void GTUtilsOptionPanelSequenceView::setSearchInLocation(const QString& locationStr) {
    auto region = GTWidget::findComboBox("boxRegion", nullptr, {false});

    if (!region->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    }
    GTComboBox::selectItemByText(region, locationStr);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSetMaxResults"
void GTUtilsOptionPanelSequenceView::setSetMaxResults(int maxResults) {
    auto limit = GTWidget::findSpinBox("boxMaxResult", nullptr, {false});

    if (!limit->isVisible()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Other settings"));
    }
    GTSpinBox::setValue(limit, maxResults);
    GTGlobals::sleep(2500);
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
