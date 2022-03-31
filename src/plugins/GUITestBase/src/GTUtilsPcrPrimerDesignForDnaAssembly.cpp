/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GTUtilsPcrPrimerDesignForDnaAssembly.h"

#include <QGroupBox>
#include <QTableWidget>

#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "base_dialogs/GTFileDialog.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTRadioButton.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTTableView.h"
#include "primitives/GTWidget.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsPcrPrimerDesignStaticFuncs"

static const QString badBackboneDialogName = "UnwantedStructuresInBackboneDialog";

// Parent of most widgets used in this class. Doesn't open the tab.
static QWidget *getTabWgt(GUITestOpStatus &os) {
    QWidget *inner = GTWidget::findWidget(os,
        GTUtilsOptionPanelSequenceView::innerWidgetNames[GTUtilsOptionPanelSequenceView::PcrPrimerDesign],
        GTUtilsSequenceView::getActiveSequenceViewWindow(os));
    return GTWidget::findWidget(os, "runPcrPrimerDesignWidget", inner);
}

// Return the sequence generator widget. Doesn't open the tab.
static QWidget *getGenSeqWgt(GUITestOpStatus &os, QWidget *tab) {
    return GTWidget::findWidget(os, "wgtGenerateSequence", tab == nullptr ? getTabWgt(os) : tab);
}

// Return the generated sequence table. Doesn't open the tab.
static QTableWidget *getGenSeqTable(GUITestOpStatus &os, QWidget *tab) {
    return GTWidget::findExactWidget<QTableWidget *>(os, "twGeneratedSequences", getGenSeqWgt(os, tab));
}

// Return the table of result sequences. Doesn't open the tab.
static QTableWidget *getResultTable(GUITestOpStatus &os, QWidget *tab) {
    return GTWidget::findExactWidget<QTableWidget *>(os, "productsTable", tab);
}

/**
 * Moves the mouse pointer over a label, then scrolls in the required direction so that the specified
 * widget ('scrollTo') is displayed in full. If the widget isn't displayed after scrolling, sets an operation error.
 * scrollTo should not be nullptr.
 *
 * At the moment the scroll doesn't work if the tasks/log are open.
 */
#define GT_METHOD_NAME "scrollToWidget"
static void scrollToWidget(GUITestOpStatus &os, const QWidget *scrollTo, QWidget *tab) {
    GT_CHECK(scrollTo != nullptr, "Can't scroll to nullptr")
    const auto isWidgetFullyVisible = [](const QWidget *w) {
        QRegion visibleRegion = w->visibleRegion();
        return !visibleRegion.isEmpty() && visibleRegion.boundingRect().height() == w->height();
    };
    const auto moveMouseAndScroll = [](QWidget *moveMouseTo, bool scrollUp) {
        GTMouseDriver::moveTo(GTWidget::getWidgetCenter(moveMouseTo));
        int scrollCoef = 20;
        GTMouseDriver::scroll(scrollUp ? scrollCoef : -scrollCoef);
        GTGlobals::sleep(200);
    };

    if (isWidgetFullyVisible(scrollTo)) {
        return;  // Nothing to do.
    }

    tab = tab == nullptr ? getTabWgt(os) : tab;
    QLabel *tabTop = GTWidget::findLabel(os, "ArrowHeader_Choose generated sequences as user primer's end", tab);

    if (tabTop->visibleRegion().isEmpty()) {  // Are we at the bottom of the tab?
        QLabel *tabBottom = GTWidget::findLabel(os, "ArrowHeader_Other sequences in PCR reaction", tab);
        moveMouseAndScroll(tabBottom, true);
    } else {
        moveMouseAndScroll(tabTop, false);
    }
    GT_CHECK(isWidgetFullyVisible(scrollTo), QString("Can't scroll to widget '%1'").arg(scrollTo->objectName()))
}
#undef GT_METHOD_NAME

/**
 * Checks if row index is correct for table. Indices are numbered starting from 0. Runs without errors if the table
 * isn't empty, the index is non-negative and less than the number of table rows. The table should not be nullptr.
 */
#define GT_METHOD_NAME "checkTableIndex"
static void checkTableIndex(GUITestOpStatus &os, int ind, const QTableWidget *table) {
    GT_CHECK(table != nullptr, "Table is nullptr")
    int tableRowCount = table->rowCount();
    GT_CHECK(ind >= 0 && ind < tableRowCount, QString("Invalid index of '%1': expected 0<=i<%2, current i=%3")
        .arg(table->objectName()).arg(tableRowCount).arg(ind))
}
#undef GT_METHOD_NAME

/**
 * Clicks generated sequence at given index. Checks validity of the index before clicking. Indices are numbered starting
 * from 0. The table should not be nullptr.
 */
static void selectGenSeq(GUITestOpStatus &os, int ind, QTableWidget *table, QWidget *tab) {
    checkTableIndex(os, ind, table);
    scrollToWidget(os, table, tab == nullptr ? getTabWgt(os) : tab);
    GTMouseDriver::click(GTTableView::getCellPoint(os, table, ind, 0));
}

// Clicks on button named btnName which is child of the generated sequences widget. Doesn't open the tab.
static void clickButtonInGenSeqWgt(GUITestOpStatus &os, const QString &btnName) {
    QWidget *tab = getTabWgt(os);
    QAbstractButton *btn = GTWidget::findButtonByText(os, btnName, getGenSeqWgt(os, tab));
    scrollToWidget(os, btn, tab);
    GTWidget::click(os, btn);
}

/**
 * Searches for 2 paired spinboxes and sets their values according to range. Parent is needed to refine the search for
 * spinboxes. Doesn't open the tab.
 */
static void setRange(GUITestOpStatus &os, const QString &minSbName, const QString &maxSbName, const U2Range<int> &range,
                     QWidget *parent) {
    QSpinBox *min = GTWidget::findSpinBox(os, minSbName, parent);
    QSpinBox *max = GTWidget::findSpinBox(os, maxSbName, parent);
    if (range.minValue <= min->maximum()) {
        GTSpinBox::setValue(os, min, range.minValue);
        GTWidget::click(os, max);
        GTSpinBox::setValue(os, max, range.maxValue);
    } else {
        GTSpinBox::setValue(os, max, range.maxValue);
        GTWidget::click(os, min);
        GTSpinBox::setValue(os, min, range.minValue);
    }
}

#undef GT_CLASS_NAME

///////////////////////////////////////////////////////SearchArea///////////////////////////////////////////////////////
GTUtilsPcrPrimerDesign::SearchArea::SearchArea(const U2Range<int> &r, bool selectManually)
    : region(r),
      selectManually(selectManually) {
}

////////////////////////////////////////////////////BadBackboneFiller///////////////////////////////////////////////////
GTUtilsPcrPrimerDesign::BadBackboneFiller::BadBackboneFiller(GUITestOpStatus &os, const Button &btn)
    : DefaultDialogFiller(os, badBackboneDialogName, btn == Button::No ? QDialogButtonBox::No : QDialogButtonBox::Yes) {
}

GTUtilsPcrPrimerDesign::BadBackboneFiller::BadBackboneFiller(GUITestOpStatus &os, CustomScenario *scenario)
    : DefaultDialogFiller(os, badBackboneDialogName, QDialogButtonBox::Yes, scenario) {
}

/////////////////////////////////////////////////GTUtilsPcrPrimerDesign/////////////////////////////////////////////////

#define GT_CLASS_NAME "GTUtilsPcrPrimerDesign"

void GTUtilsPcrPrimerDesign::openTab(GUITestOpStatus &os) {
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::PcrPrimerDesign);
}

void GTUtilsPcrPrimerDesign::setUserPrimer(GUITestOpStatus &os, const QString &primer,
                                           const U2Strand::Direction &direction) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtUserPrimers", tab);
    scrollToWidget(os, parent, tab);
    GTLineEdit::setText(os, direction == U2Strand::Direct ? "leForwardPrimer" : "leReversePrimer", primer, parent);
}

void GTUtilsPcrPrimerDesign::filterGeneratedSequences(GUITestOpStatus &os, const QString &filter) {
    QWidget *tab = getTabWgt(os);
    QLineEdit *filterLe = GTWidget::findLineEdit(os, "leFilter", getGenSeqWgt(os, tab));
    scrollToWidget(os, filterLe, tab);
    GTLineEdit::setText(os, filterLe, filter);
}

void GTUtilsPcrPrimerDesign::selectGeneratedSequence(GUITestOpStatus &os, int num) {
    QWidget *tab = getTabWgt(os);
    selectGenSeq(os, num, getGenSeqTable(os, tab), tab);
}

#define GT_METHOD_NAME "selectGeneratedSequenceBySequence"
void GTUtilsPcrPrimerDesign::selectGeneratedSequence(GUITestOpStatus &os, const QString &sequence,
                                                     const GTGlobals::FindOptions &options) {
    QWidget *tab = getTabWgt(os);
    QTableWidget *table = getGenSeqTable(os, tab);
    QList<QTableWidgetItem *> result = table->findItems(sequence, options.matchPolicy);
    if (options.failIfNotFound) {
        GT_CHECK(!result.isEmpty(), QString("Sequence '%1' not found in generated sequence table").arg(sequence))
    } else if (result.isEmpty()) {
        return;
    }
    QTableWidgetItem *item = result.first();
    GT_CHECK(item != nullptr, "The first element among the sequences found is nullptr")
    selectGenSeq(os, item->row(), table, tab);
}
#undef GT_METHOD_NAME

void GTUtilsPcrPrimerDesign::findReverseComplement(GUITestOpStatus &os) {
    clickButtonInGenSeqWgt(os, "Find reverse-complement");
}

void GTUtilsPcrPrimerDesign::addToUserPrimer(GUITestOpStatus &os, const UserPrimer &userPrimer) {
    QString buttonText;
    switch (userPrimer) {
        case UserPrimer::Forward5:
            buttonText = "Forward 5'";
            break;
        case UserPrimer::Forward3:
            buttonText = "Forward 3'";
            break;
        case UserPrimer::Reverse5:
            buttonText = "Reverse 5'";
            break;
        default:
            buttonText = "Reverse 3'";
    }
    clickButtonInGenSeqWgt(os, buttonText);
}

void GTUtilsPcrPrimerDesign::setParametersOfPrimingSequences(GUITestOpStatus &os,
                                                             const ParametersOfPrimingSequences &params) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtParametersOfPrimingSequences", tab);
    scrollToWidget(os, parent, tab);
    setRange(os, "sbMinRequireGibbs", "sbMaxRequireGibbs", params.gibbsFreeEnergy, parent);
    setRange(os, "spMinRequireMeltingTeml", "spMaxRequireMeltingTeml", params.meltingPoint, parent);
    setRange(os, "spMinRequireOverlapLength", "spMaxRequireOverlapLength", params.overlapLength, parent);
}

void GTUtilsPcrPrimerDesign::setParametersToExcludeInWholePrimers(GUITestOpStatus &os,
                                                                  const ParametersToExcludeInWholePrimers &params) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtParameters2ExcludeInWholePrimers", tab);
    scrollToWidget(os, parent, tab);
    GTSpinBox::setValue(os, "sbExcludeGibbs", params.gibbsFreeEnergy, parent);
    GTSpinBox::setValue(os, "spExcludeMeltingTeml", params.meltingPoint, parent);
    GTSpinBox::setValue(os, "spExcludeComplementLength", params.motifLen, parent);
}

void GTUtilsPcrPrimerDesign::configureInsertToBackboneBearings(GUITestOpStatus &os,
                                                               const InsertToBackboneBearings &params) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtOpenBackboneSequence", tab);
    scrollToWidget(os, parent, tab);

    GTRadioButton::click(os, params.insertTo == InsertToBackboneBearings::InsertTo::Backbone5 ? "backbone5" :
                                                                                                "backbone3", parent);
    GTSpinBox::setValue(os, "sbBackbone5Length", params.backbone5Len, parent);
    GTSpinBox::setValue(os, "sbBackbone3Length", params.backbone3Len, parent);
}

void GTUtilsPcrPrimerDesign::setSearchArea(GUITestOpStatus &os, const SearchArea &params, const AreaType &areaType) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtAreasForPriming", tab);
    scrollToWidget(os, parent, tab);

    auto currentGb = GTWidget::findExactWidget<QGroupBox *>(os, "groupBox_2", parent);
    if (params.selectManually) {
        QToolButton *selectManuallyButton = GTWidget::findToolButton(os, areaType == AreaType::Left ?
            "tbLeftAreaSelectManually" : "tbRightAreaSelectManually", currentGb);
        GTWidget::click(os, selectManuallyButton);
        GTUtilsSequenceView::selectSequenceRegion(os, params.region.minValue, params.region.maxValue);
        GTWidget::click(os, selectManuallyButton);
    } else {
        setRange(os, areaType == AreaType::Left ? "sbLeftAreaStart" : "sbRightAreaStart",
                 areaType == AreaType::Left ? "sbLeftAreaEnd" : "sbRightAreaEnd", params.region, currentGb);
    }
}

void GTUtilsPcrPrimerDesign::setBackbone(GUITestOpStatus &os, const QString &path, bool useButton) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtOpenBackboneSequence", tab);
    scrollToWidget(os, parent, tab);
    if (useButton) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, path));
        GTWidget::click(os, GTWidget::findButtonByText(os, "...", parent));
    } else {
        GTLineEdit::setText(os, "leBackboneFilePath", path, parent);
    }
}

void GTUtilsPcrPrimerDesign::setOtherSequences(GUITestOpStatus &os, const QString &path, bool useButton) {
    QWidget *tab = getTabWgt(os);
    QWidget *parent = GTWidget::findWidget(os, "wgtOtherSequencesInPcr", tab);
    scrollToWidget(os, parent, tab);
    if (useButton) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, path));
        GTWidget::click(os, GTWidget::findButtonByText(os, "...", parent));
    } else {
        GTLineEdit::setText(os, "leOtherSequencesInPcrFilePath", path, parent);
    }
}

void GTUtilsPcrPrimerDesign::clickInResultsTable(GUITestOpStatus &os, int num, const ClickType &clickType) {
    QWidget *tab = getTabWgt(os);
    QTableWidget *table = getResultTable(os, tab);
    checkTableIndex(os, num, table);
    scrollToWidget(os, table, tab);
    QPoint cell = GTTableView::getCellPoint(os, table, num, 0);
    if (clickType == ClickType::Single) {
        GTMouseDriver::click(cell);
    } else {
        GTMouseDriver::moveTo(cell);
        GTMouseDriver::doubleClick();
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
}

#define GT_METHOD_NAME "checkEntryInResultsTable"
void GTUtilsPcrPrimerDesign::checkEntryInResultsTable(GUITestOpStatus &os, int num, const QString &expectedFragment,
                                                      const U2Range<int> &expectedRegion) {
    QWidget *tab = getTabWgt(os);
    QTableWidget *table = getResultTable(os, tab);
    checkTableIndex(os, num, table);
    scrollToWidget(os, table, tab);
    QString current = GTTableView::data(os, table, num, 0);
    GT_CHECK(expectedFragment == current, QString("Incorrect name of the %1 row: expected '%2', current '%3'").arg(num)
        .arg(expectedFragment, current))

    QString expected = QString::number(expectedRegion.minValue) + '-' + QString::number(expectedRegion.maxValue);
    current = GTTableView::data(os, table, num, 1);
    GT_CHECK(expected == current, QString("Incorrect result of the %1 row: expected '%2', current '%3'").arg(num)
        .arg(expected, current))
}
#undef GT_METHOD_NAME

void GTUtilsPcrPrimerDesign::clickStart(GUITestOpStatus &os) {
    QWidget *tab = getTabWgt(os);
    QAbstractButton *strtBtn = GTWidget::findButtonByText(os, "Start", tab);
    scrollToWidget(os, strtBtn, tab);
    GTWidget::click(os, strtBtn);
}

#undef GT_CLASS_NAME
}  // namespace U2
