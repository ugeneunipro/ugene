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
#include <primitives/GTSpinBox.h>
#include <primitives/GTTableView.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTWidget.h>
#include <system/GTClipboard.h>
#include <utils/GTThread.h>

#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>

#include <U2Core/U2SafePoints.h>

#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {
using namespace HI;

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

void GTUtilsOptionPanelSequenceView::enterPattern(HI::GUITestOpStatus& os, QString pattern, bool useCopyPaste) {
    auto patternEdit = GTWidget::findTextEdit(os, "textPattern");
    GTWidget::click(os, patternEdit);

    if (!patternEdit->toPlainText().isEmpty()) {
        GTTextEdit::clear(os, patternEdit);
    }
    if (useCopyPaste) {
        GTClipboard::setText(os, pattern);
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    } else {
        GTTextEdit::setText(os, patternEdit, pattern);
    }
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleTab"
void GTUtilsOptionPanelSequenceView::toggleTab(HI::GUITestOpStatus& os, GTUtilsOptionPanelSequenceView::Tabs tab) {
    GTWidget::click(os, GTWidget::findWidget(os, tabsNames[tab], GTUtilsSequenceView::getActiveSequenceViewWindow(os)));
    GTGlobals::sleep(200);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsOptionPanelSequenceView::openTab(HI::GUITestOpStatus& os, Tabs tab) {
    if (!isTabOpened(os, tab)) {
        toggleTab(os, tab);
        GTThread::waitForMainThread();
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeTab"
void GTUtilsOptionPanelSequenceView::closeTab(HI::GUITestOpStatus& os, GTUtilsOptionPanelSequenceView::Tabs tab) {
    if (isTabOpened(os, tab)) {
        toggleTab(os, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabOpened"
bool GTUtilsOptionPanelSequenceView::isTabOpened(HI::GUITestOpStatus& os, GTUtilsOptionPanelSequenceView::Tabs tab) {
    auto innerTabWidget = GTWidget::findWidget(os, innerWidgetNames[tab], GTUtilsSequenceView::getActiveSequenceViewWindow(os), {false});
    return innerTabWidget != nullptr && innerTabWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTabIsOpened"
void GTUtilsOptionPanelSequenceView::checkTabIsOpened(HI::GUITestOpStatus& os, GTUtilsOptionPanelSequenceView::Tabs tab) {
    QString name = innerWidgetNames[tab];
    GTWidget::findWidget(os, name, GTUtilsSequenceView::getActiveSequenceViewWindow(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkResultsText"

bool GTUtilsOptionPanelSequenceView::checkResultsText(HI::GUITestOpStatus& os, QString expectedText) {
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto label = GTWidget::findLabel(os, "resultLabel");
    return label->text() == expectedText;
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchWithAmbiguousBases"
void GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases(HI::GUITestOpStatus& os, bool searchWithAmbiguousBases) {
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "useAmbiguousBasesBox"), searchWithAmbiguousBases);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNext"

void GTUtilsOptionPanelSequenceView::clickNext(HI::GUITestOpStatus& os) {
    auto next = GTWidget::findPushButton(os, "nextPushButton");
    GTWidget::click(os, next);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickPrev"

void GTUtilsOptionPanelSequenceView::clickPrev(HI::GUITestOpStatus& os) {
    auto prev = GTWidget::findPushButton(os, "prevPushButton");
    GTWidget::click(os, prev);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickGetAnnotation"

void GTUtilsOptionPanelSequenceView::clickGetAnnotation(HI::GUITestOpStatus& os) {
    auto getAnnotations = GTWidget::findPushButton(os, "getAnnotationsPushButton");
    GTWidget::click(os, getAnnotations);
    GTThread::waitForMainThread();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isPrevNextEnabled"

bool GTUtilsOptionPanelSequenceView::isPrevNextEnabled(HI::GUITestOpStatus& os) {
    auto next = GTWidget::findPushButton(os, "nextPushButton");
    auto prev = GTWidget::findPushButton(os, "prevPushButton");
    return prev->isEnabled() && next->isEnabled();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isGetAnnotationsEnabled"
bool GTUtilsOptionPanelSequenceView::isGetAnnotationsEnabled(HI::GUITestOpStatus& os) {
    auto getAnnotations = GTWidget::findPushButton(os, "getAnnotationsPushButton");
    return getAnnotations->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCircularView"
void GTUtilsOptionPanelSequenceView::toggleCircularView(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTWidget::findButtonByText(os, "Open Circular View(s)", GTUtilsSequenceView::getActiveSequenceViewWindow(os)));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setTitleFontSize"
void GTUtilsOptionPanelSequenceView::setTitleFontSize(HI::GUITestOpStatus& os, int fontSize) {
    GTSpinBox::setValue(os, GTWidget::findSpinBox(os, "fontSizeSpinBox"), fontSize);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTitleFontSize"
int GTUtilsOptionPanelSequenceView::getTitleFontSize(HI::GUITestOpStatus& os) {
    return GTSpinBox::getValue(os, GTWidget::findSpinBox(os, "fontSizeSpinBox", GTUtilsSequenceView::getActiveSequenceViewWindow(os)));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setForwardPrimer"
void GTUtilsOptionPanelSequenceView::setForwardPrimer(HI::GUITestOpStatus& os, const QString& primer) {
    openTab(os, InSilicoPcr);
    auto primerContainer = GTWidget::findWidget(os, "forwardPrimerBox");
    GTLineEdit::setText(os, "primerEdit", primer, primerContainer);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setForwardPrimerMismatches"
void GTUtilsOptionPanelSequenceView::setForwardPrimerMismatches(HI::GUITestOpStatus& os, const int mismatches) {
    openTab(os, InSilicoPcr);
    auto primerContainer = GTWidget::findWidget(os, "forwardPrimerBox");

    auto mismatchesSpinBox = GTWidget::findSpinBox(os, "mismatchesSpinBox", primerContainer);

    GTSpinBox::setValue(os, mismatchesSpinBox, mismatches, GTGlobals::UseKey);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReversePrimer"
void GTUtilsOptionPanelSequenceView::setReversePrimer(HI::GUITestOpStatus& os, const QString& primer) {
    openTab(os, InSilicoPcr);
    auto primerContainer = GTWidget::findWidget(os, "reversePrimerBox");
    GTLineEdit::setText(os, "primerEdit", primer, primerContainer);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReversePrimerMismatches"
void GTUtilsOptionPanelSequenceView::setReversePrimerMismatches(HI::GUITestOpStatus& os, const int mismatches) {
    openTab(os, InSilicoPcr);
    auto primerContainer = GTWidget::findWidget(os, "reversePrimerBox");

    auto mismatchesSpinBox = GTWidget::findSpinBox(os, "mismatchesSpinBox", primerContainer);

    GTSpinBox::setValue(os, mismatchesSpinBox, mismatches, GTGlobals::UseKey);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "productsCount"
int GTUtilsOptionPanelSequenceView::productsCount(HI::GUITestOpStatus& os) {
    openTab(os, InSilicoPcr);
    auto tableWidget = GTWidget::findTableWidget(os, "productsTable");

    return tableWidget->rowCount();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showPrimersDetails"
void GTUtilsOptionPanelSequenceView::showPrimersDetails(HI::GUITestOpStatus& os) {
    openTab(os, InSilicoPcr);
    auto label = GTWidget::findWidget(os, "detailsLinkLabel");
    GTWidget::click(os, GTWidget::findWidget(os, "detailsLinkLabel"), Qt::LeftButton, QPoint(20, label->geometry().height() / 2));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pressFindProducts"
void GTUtilsOptionPanelSequenceView::pressFindProducts(HI::GUITestOpStatus& os) {
    openTab(os, InSilicoPcr);
    auto findProducts = GTWidget::findPushButton(os, "findProductButton");

    GTWidget::click(os, findProducts);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pressExtractProduct"
void GTUtilsOptionPanelSequenceView::pressExtractProduct(HI::GUITestOpStatus& os) {
    openTab(os, InSilicoPcr);
    auto extractProduct = GTWidget::findPushButton(os, "extractProductButton");
    GT_CHECK(extractProduct->isEnabled(), "Extract Product buttons is unexpectably disabled");

    GTWidget::click(os, extractProduct);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSearchAlgorithmShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isSearchAlgorithmShowHideWidgetOpened(HI::GUITestOpStatus& os) {
    auto algorithmInnerWidget = GTWidget::findWidget(os, "widgetAlgorithm");
    return algorithmInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSearchInShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isSearchInShowHideWidgetOpened(HI::GUITestOpStatus& os) {
    auto searchInInnerWidget = GTWidget::findWidget(os, "widgetSearchIn");
    return searchInInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isOtherSettingsShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isOtherSettingsShowHideWidgetOpened(HI::GUITestOpStatus& os) {
    auto otherSettingsInnerWidget = GTWidget::findWidget(os, "widgetOther");
    return otherSettingsInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSaveAnnotationToShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isSaveAnnotationToShowHideWidgetOpened(HI::GUITestOpStatus& os) {
    auto annotationsWidget = GTWidget::findWidget(os, "annotationsWidget");
    auto saveAnnotationsToInnerWidget = GTWidget::findWidget(os, "saveAnnotationsInnerWidget", annotationsWidget);
    return saveAnnotationsToInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isAnnotationParametersShowHideWidgetOpened"
bool GTUtilsOptionPanelSequenceView::isAnnotationParametersShowHideWidgetOpened(HI::GUITestOpStatus& os) {
    auto annotationsWidget = GTWidget::findWidget(os, "annotationsWidget");
    auto annotationsParametersInnerWidget = GTWidget::findWidget(os, "annotationParametersInnerWidget", annotationsWidget);
    return annotationsParametersInnerWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSearchAlgorithmShowHideWidget"
void GTUtilsOptionPanelSequenceView::openSearchAlgorithmShowHideWidget(HI::GUITestOpStatus& os, bool open) {
    CHECK(open != isSearchAlgorithmShowHideWidgetOpened(os), );
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSearchInShowHideWidget"
void GTUtilsOptionPanelSequenceView::openSearchInShowHideWidget(HI::GUITestOpStatus& os, bool open) {
    CHECK(open != isSearchInShowHideWidgetOpened(os), );
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openOtherSettingsShowHideWidget"
void GTUtilsOptionPanelSequenceView::openOtherSettingsShowHideWidget(HI::GUITestOpStatus& os, bool open) {
    CHECK(open != isOtherSettingsShowHideWidgetOpened(os), );
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openSaveAnnotationToShowHideWidget"
void GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget(HI::GUITestOpStatus& os, bool open) {
    CHECK(open != isSaveAnnotationToShowHideWidgetOpened(os), );
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Save annotation(s) to"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openAnnotationParametersShowHideWidget"
void GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(HI::GUITestOpStatus& os, bool open) {
    CHECK(open != isAnnotationParametersShowHideWidgetOpened(os), );
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Annotation parameters"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleInputFromFilePattern"

void GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern(HI::GUITestOpStatus& os) {
    auto loadFromFile = GTWidget::findRadioButton(os, "usePatternFromFileRadioButton");
    GTWidget::click(os, loadFromFile);
    // kinda hack for QGroupBox should be rewriten
    GTKeyboardDriver::keyClick(Qt::Key_Space);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleSaveAnnotationsTo"

void GTUtilsOptionPanelSequenceView::toggleSaveAnnotationsTo(HI::GUITestOpStatus& os) {
    auto obj = GTWidget::findLabel(os, "ArrowHeader_Save annotation(s) to");
    GTWidget::click(os, obj);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterPatternFromFile"

void GTUtilsOptionPanelSequenceView::enterPatternFromFile(HI::GUITestOpStatus& os, QString filePathStr, QString fileName) {
    GTFileDialogUtils* ob = new GTFileDialogUtils(os, filePathStr, fileName, GTFileDialogUtils::Open);
    GTUtilsDialog::waitForDialog(os, ob);

    auto browse = GTWidget::findToolButton(os, "loadFromFileToolButton");
    GTWidget::click(os, browse);
    GTGlobals::sleep(2500);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setStrand"

void GTUtilsOptionPanelSequenceView::setStrand(HI::GUITestOpStatus& os, QString strandStr) {
    auto strand = GTWidget::findComboBox(os, "boxStrand", nullptr, {false});

    if (!strand->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    }
    GTComboBox::selectItemByText(os, strand, strandStr);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegionType"
void GTUtilsOptionPanelSequenceView::setRegionType(HI::GUITestOpStatus& os, const QString& regionType) {
    openSearchInShowHideWidget(os);
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "boxRegion"), regionType);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setRegion"
void GTUtilsOptionPanelSequenceView::setRegion(HI::GUITestOpStatus& os, int from, int to) {
    openSearchInShowHideWidget(os);
    GTLineEdit::setText(os, "editStart", QString::number(from));
    GTLineEdit::setText(os, "editEnd", QString::number(to));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterFilepathForSavingAnnotations"
void GTUtilsOptionPanelSequenceView::enterFilepathForSavingAnnotations(HI::GUITestOpStatus& os, QString filepath) {
    QDir().mkpath(QFileInfo(filepath).dir().absolutePath());
    GTLineEdit::setText(os, "leNewTablePath", filepath);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAlgorithm"
void GTUtilsOptionPanelSequenceView::setAlgorithm(HI::GUITestOpStatus& os, QString algorithm) {
    auto algoBox = GTWidget::findComboBox(os, "boxAlgorithm");

    if (!algoBox->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));
    }
    GTComboBox::selectItemByText(os, algoBox, algorithm);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMatchPercentage"
int GTUtilsOptionPanelSequenceView::getMatchPercentage(HI::GUITestOpStatus& os) {
    auto spinMatchBox = GTWidget::findSpinBox(os, "spinBoxMatch");
    return spinMatchBox->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRegionType"
QString GTUtilsOptionPanelSequenceView::getRegionType(HI::GUITestOpStatus& os) {
    openSearchInShowHideWidget(os);
    auto cbRegionType = GTWidget::findComboBox(os, "boxRegion");
    return cbRegionType->currentText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
QPair<int, int> GTUtilsOptionPanelSequenceView::getRegion(HI::GUITestOpStatus& os) {
    openSearchInShowHideWidget(os);

    QPair<int, int> result;
    auto leRegionStartText = GTLineEdit::getText(os, "editStart");
    auto leRegionEndText = GTLineEdit::getText(os, "editEnd");

    bool ok = false;
    const int regionStart = leRegionStartText.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Can't convert the string to int: %1").arg(leRegionStartText), result);
    const int regionEnd = leRegionEndText.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Can't convert the string to int: %1").arg(leRegionEndText), result);

    return qMakePair(regionStart, regionEnd);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHintText"
const QString GTUtilsOptionPanelSequenceView::getHintText(HI::GUITestOpStatus& os) {
    auto lblHint = GTWidget::findLabel(os, "lblErrorMessage");
    return lblHint->isVisible() ? lblHint->text() : "";
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMatchPercentage"
void GTUtilsOptionPanelSequenceView::setMatchPercentage(HI::GUITestOpStatus& os, int percentage) {
    auto spinMatchBox = GTWidget::findSpinBox(os, "spinBoxMatch");

    GTSpinBox::setValue(os, spinMatchBox, percentage, GTGlobals::UseKeyBoard);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUsePatternName"
void GTUtilsOptionPanelSequenceView::setUsePatternName(HI::GUITestOpStatus& os, bool setChecked) {
    openAnnotationParametersShowHideWidget(os);
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbUsePatternNames"), setChecked);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchInTranslation"
void GTUtilsOptionPanelSequenceView::setSearchInTranslation(HI::GUITestOpStatus& os, bool inTranslation /* = true*/) {
    auto searchIn = GTWidget::findComboBox(os, "boxSeqTransl", nullptr, {false});

    if (!searchIn->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    }
    if (inTranslation) {
        GTComboBox::selectItemByText(os, searchIn, "Translation");
    } else {
        GTComboBox::selectItemByText(os, searchIn, "Sequence");
    }
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSearchInLocation"
void GTUtilsOptionPanelSequenceView::setSearchInLocation(HI::GUITestOpStatus& os, QString locationStr) {
    auto region = GTWidget::findComboBox(os, "boxRegion", nullptr, {false});

    if (!region->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    }
    GTComboBox::selectItemByText(os, region, locationStr);
    GTGlobals::sleep(2500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setSetMaxResults"
void GTUtilsOptionPanelSequenceView::setSetMaxResults(HI::GUITestOpStatus& os, int maxResults) {
    auto limit = GTWidget::findSpinBox(os, "boxMaxResult", nullptr, {false});

    if (!limit->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
    }
    GTSpinBox::setValue(os, limit, maxResults);
    GTGlobals::sleep(2500);
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
