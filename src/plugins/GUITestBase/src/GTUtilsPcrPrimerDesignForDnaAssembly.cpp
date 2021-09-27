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

static void setRange(GUITestOpStatus& os, const QString& minSbName, const QString& maxSbName, U2Range<int> range,
                     const QWidget* parent) {
    if (QSpinBox* min = GTWidget::findSpinBox(os, minSbName, parent)) {
        QSpinBox* max = GTWidget::findSpinBox(os, maxSbName, parent);
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
}

#define GT_CLASS_NAME "GTUtilsPcrPrimerDesignForDnaAssembly"

#define GT_METHOD_NAME "setUserPrimer"
void GTUtilsPcrPrimerDesignForDnaAssembly::setUserPrimer(GUITestOpStatus& os, const QString& primer,
                                                         U2Strand::Direction direction) {
    QWidget* const tab = openTabAndReturnIt(os);
    QLineEdit* const lePrimer = GTWidget::findLineEdit(os,
        direction == U2Strand::Direct ? "leForwardPrimer" : "leReversePrimer",
        GTWidget::findWidget(os, "wgtUserPrimers", tab));
    scrollToWidget(os, lePrimer, tab);
    GTLineEdit::setText(os, lePrimer, primer);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "filterGeneratedSequences"
void GTUtilsPcrPrimerDesignForDnaAssembly::filterGeneratedSequences(GUITestOpStatus& os, const QString& filter) {
    QWidget* const tab = openTabAndReturnIt(os);
    QLineEdit* const leFilter = GTWidget::findLineEdit(os, "leFilter", getGenerateSequenceWgt(os, tab));
    scrollToWidget(os, leFilter, tab);
    GTLineEdit::setText(os, leFilter, filter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectGeneratedSequence"
void GTUtilsPcrPrimerDesignForDnaAssembly::selectGeneratedSequence(GUITestOpStatus& os, int i) {
    QWidget* const tab = openTabAndReturnIt(os);
    if (QTableWidget* const table = getGeneratedSequenceTableWgt(os, tab)) {
        const int tableRowCount = table->rowCount();
        GT_CHECK(0 <= i && i < tableRowCount, QString("Invalid index of the current generated sequence table: "
                                                      "expected 0<=num<%1, current num=%2").arg(tableRowCount).arg(i))
        selectGeneratedSequence(os, i, table, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectGeneratedSequence"
void GTUtilsPcrPrimerDesignForDnaAssembly::selectGeneratedSequence(GUITestOpStatus& os, const QString& sequence,
                                                                   const GTGlobals::FindOptions& options) {
    QWidget* const tab = openTabAndReturnIt(os);
    if (QTableWidget* const table = getGeneratedSequenceTableWgt(os, tab)) {
        QList<QTableWidgetItem*> result = table->findItems(sequence, options.matchPolicy);
        if (options.failIfNotFound) {
            GT_CHECK(!result.isEmpty(), QString("Sequence '%1' not found in generated sequence table").arg(sequence))
        } else if (result.isEmpty()) {
            return;
        }
        const QTableWidgetItem* item = result.first();
        GT_CHECK(item != nullptr, "The first element of the generated sequences table is nullptr")
        selectGeneratedSequence(os, item->row(), table, tab);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findReverseComplement"
void GTUtilsPcrPrimerDesignForDnaAssembly::findReverseComplement(GUITestOpStatus& os) {
    QWidget* const tab = openTabAndReturnIt(os);
    QAbstractButton* const revComplButton = GTWidget::findButtonByText(os, "Find reverse-complement",
                                                                       getGenerateSequenceWgt(os, tab));
    scrollToWidget(os, revComplButton, tab);
    GTWidget::click(os, revComplButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addUserPrimerEnd"
void GTUtilsPcrPrimerDesignForDnaAssembly::addUserPrimerEnd(GUITestOpStatus& os, UserPrimerEndButton buttonType) {
    QString buttonText;
    switch (buttonType) {
        case UserPrimerEndButton::Forward5:
            buttonText = "Forward 5'";
            break;
        case UserPrimerEndButton::Forward3:
            buttonText = "Forward 3'";
            break;
        case UserPrimerEndButton::Reverse5:
            buttonText = "Reverse 5'";
            break;
        default:
            buttonText = "Reverse 3'";
    }
    QWidget* const tab = openTabAndReturnIt(os);
    QAbstractButton* const button = GTWidget::findButtonByText(os, buttonText, getGenerateSequenceWgt(os, tab));
    scrollToWidget(os, button, tab);
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setParamsOfPrimingSeqs"
void GTUtilsPcrPrimerDesignForDnaAssembly::setParamsOfPrimingSeqs(GUITestOpStatus& os,
                                                                  const ParamsOfPrimingSeqs& params) {
    QWidget* const tab = openTabAndReturnIt(os);
    QWidget* const parent = GTWidget::findWidget(os, "wgtParametersOfPrimingSequences", tab);
    scrollToWidget(os, parent, tab);
    setRange(os, "sbMinRequireGibbs",         "sbMaxRequireGibbs",         params.gibbsFreeEnergy, parent);
    setRange(os, "spMinRequireMeltingTeml",   "spMaxRequireMeltingTeml",   params.meltingPoint,    parent);
    setRange(os, "spMinRequireOverlapLength", "spMaxRequireOverlapLength", params.overlapLength,   parent);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setParamsToExcludeInWholePrimers"
void GTUtilsPcrPrimerDesignForDnaAssembly::setParamsToExcludeInWholePrimers(GUITestOpStatus& os,
                                                                            ParamsToExcludeInWholePrimers params) {
    QWidget* const tab = openTabAndReturnIt(os);
    QWidget* const parent = GTWidget::findWidget(os, "wgtParameters2ExcludeInWholePrimers", tab);
    scrollToWidget(os, parent, tab);
    GTSpinBox::setValue(os, "sbExcludeGibbs",            params.gibbsFreeEnergy, parent);
    GTSpinBox::setValue(os, "spExcludeMeltingTeml",      params.meltingPoint,    parent);
    GTSpinBox::setValue(os, "spExcludeComplementLength", params.motifLen,        parent);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setInsertToBackbone"
void GTUtilsPcrPrimerDesignForDnaAssembly::setInsertToBackbone(GUITestOpStatus& os, InsertToBackboneBearings params) {
    const QString rbInsertTo = (params.insertTo5 ? "backbone5" : "backbone3");
    QWidget* const tab = openTabAndReturnIt(os);
    const auto gbInsertToBackbone = GTWidget::findExactWidget<QGroupBox*>(os, "groupBox",
                                                                          getAreasForPrimingSearchWgt(os, tab));
    scrollToWidget(os, gbInsertToBackbone, tab);
    GTRadioButton::click(os, rbInsertTo, gbInsertToBackbone);
    GTSpinBox::setValue(os, "sbBackbone5Length", params.backbone5Len, gbInsertToBackbone);
    GTSpinBox::setValue(os, "sbBackbone3Length", params.backbone3Len, gbInsertToBackbone);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setPrimerSearchArea"
void GTUtilsPcrPrimerDesignForDnaAssembly::setPrimerSearchArea(GUITestOpStatus& os, bool isLeft, U2Range<int> area,
                                                               bool selectManually) {
    const QString startName = (isLeft ? "sbLeftAreaStart" : "sbRightAreaStart"),
                  endName   = (isLeft ? "sbLeftAreaEnd"   : "sbRightAreaEnd");
    QWidget* const tab = openTabAndReturnIt(os);
    const auto gbAreas = GTWidget::findExactWidget<QGroupBox*>(os, "groupBox_2", getAreasForPrimingSearchWgt(os, tab));
    scrollToWidget(os, gbAreas, tab);
    if (selectManually) {
        QToolButton* selectManuallyButton = GTWidget::findToolButton(os,
            isLeft ? "tbLeftAreaSelectManually" : "tbRightAreaSelectManually", gbAreas);
        GTWidget::click(os, selectManuallyButton);
        GTUtilsSequenceView::selectSequenceRegion(os, area.minValue, area.maxValue);
        GTWidget::click(os, selectManuallyButton);
    } else {
        setRange(os, startName, endName, area, gbAreas);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openBackbone"
void GTUtilsPcrPrimerDesignForDnaAssembly::openBackbone(GUITestOpStatus& os, const QString& path, bool useDialog) {
    QWidget* const tab = openTabAndReturnIt(os);
    QWidget* const openBackboneWgt = GTWidget::findWidget(os, "wgtOpenBackboneSequence", tab);
    scrollToWidget(os, openBackboneWgt, tab);
    if (useDialog) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, path));
        GTWidget::click(os, GTWidget::findToolButton(os, "tbLoadBackbone", openBackboneWgt));
    } else {
        GTLineEdit::setText(os, "leBackboneFilePath", path, openBackboneWgt);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openOtherSequences"
void GTUtilsPcrPrimerDesignForDnaAssembly::openOtherSequences(GUITestOpStatus& os, const QString& path,
                                                              bool useDialog) {
    QWidget* const tab = openTabAndReturnIt(os);
    QWidget* const otherSequencesWgt = GTWidget::findWidget(os, "wgtOtherSequencesInPcr", tab);
    scrollToWidget(os, otherSequencesWgt, tab);
    if (useDialog) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, path));
        GTWidget::click(os, GTWidget::findToolButton(os, "tbLoadOtherSequencesInPcr", otherSequencesWgt));
    } else {
        GTLineEdit::setText(os, "leOtherSequencesInPcrFilePath", path, otherSequencesWgt);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "start"
void GTUtilsPcrPrimerDesignForDnaAssembly::start(GUITestOpStatus& os) {
    QWidget* const tab = openTabAndReturnIt(os);
    QAbstractButton* const strtBtn = GTWidget::findButtonByText(os, "Start", openTabAndReturnIt(os));
    scrollToWidget(os, strtBtn, tab);
    GTWidget::click(os, strtBtn);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickResultTable"
void GTUtilsPcrPrimerDesignForDnaAssembly::clickResultTable(GUITestOpStatus& os, int i, bool isDoubleClick) {
    QWidget* const tab = openTabAndReturnIt(os);
    if (QTableWidget* const table = getResultTableWgt(os, tab)) {
        const int tableRowCount = table->rowCount();
        GT_CHECK(0 <= i && i < tableRowCount, QString("Invalid index of the current result table: expected 0<=num<%1, "
                                                      "current num=%2").arg(tableRowCount).arg(i))
        scrollToWidget(os, table, tab);
        if (isDoubleClick) {
            GTMouseDriver::moveTo(GTTableView::getCellPoint(os, table, i, 0));
            GTMouseDriver::doubleClick();
            GTUtilsTaskTreeView::waitTaskFinished(os);
        } else {
            GTMouseDriver::click(GTTableView::getCellPoint(os, table, i, 0));
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkResultTable"
void GTUtilsPcrPrimerDesignForDnaAssembly::checkResultTable(GUITestOpStatus& os, int i, U2Range<int> expected) {
    QWidget* const tab = openTabAndReturnIt(os);
    if (QTableWidget* const table = getResultTableWgt(os, tab)) {
        const int tableRowCount = table->rowCount();
        GT_CHECK(0 <= i && i < tableRowCount, QString("Invalid index of the current result table: expected 0<=num<%1, "
                                                      "current num=%2").arg(tableRowCount).arg(i))
        scrollToWidget(os, table, tab);
        const QString expectedStr = QString::number(expected.minValue) + '-' + QString::number(expected.maxValue);
        const QString current = GTTableView::data(os, table, i, 1);
        GT_CHECK(expectedStr == current, QString("Invalid %1 result: expected '%2', current '%3'").
                                             arg(GTTableView::data(os, table, i, 0), expectedStr, current))
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getUserPrimer"
QLineEdit* GTUtilsPcrPrimerDesignForDnaAssembly::getUserPrimer(GUITestOpStatus& os, U2Strand::Direction direction) {
    QWidget* const tab = openTabAndReturnIt(os);
    return GTWidget::findLineEdit(os,
        direction == U2Strand::Direct ? "leForwardPrimer" : "leReversePrimer",
        GTWidget::findWidget(os, "wgtUserPrimers", tab));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGeneratedSequenceTableWgt"
QTableWidget* GTUtilsPcrPrimerDesignForDnaAssembly::getGeneratedSequenceTableWgt(GUITestOpStatus& os,
    const QWidget* tab) {
    return GTWidget::findExactWidget<QTableWidget*>(os, "twGeneratedSequences", getGenerateSequenceWgt(os,
        tab == nullptr ? openTabAndReturnIt(os) : tab));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAreaSpinboxes"
QPair<QSpinBox*, QSpinBox*> GTUtilsPcrPrimerDesignForDnaAssembly::getAreaSpinboxes(GUITestOpStatus& os, bool isLeft) {
    const QString startName = (isLeft ? "sbLeftAreaStart" : "sbRightAreaStart"),
        endName = (isLeft ? "sbLeftAreaEnd" : "sbRightAreaEnd");
    QWidget* const tab = openTabAndReturnIt(os);
    const auto gbAreas = GTWidget::findExactWidget<QGroupBox*>(os, "groupBox_2", getAreasForPrimingSearchWgt(os, tab));
    return { GTWidget::findSpinBox(os, startName, gbAreas), GTWidget::findSpinBox(os, endName, gbAreas) };
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTabAndReturnIt"
QWidget* GTUtilsPcrPrimerDesignForDnaAssembly::openTabAndReturnIt(GUITestOpStatus& os) {
    const GTUtilsOptionPanelSequenceView::Tabs tab = GTUtilsOptionPanelSequenceView::PcrPrimerDesign;
    GTUtilsOptionPanelSequenceView::openTab(os, tab);
    return GTWidget::findWidget(os, GTUtilsOptionPanelSequenceView::innerWidgetNames[tab],
                                GTUtilsSequenceView::getActiveSequenceViewWindow(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGenerateSequenceWgt"
QWidget* GTUtilsPcrPrimerDesignForDnaAssembly::getGenerateSequenceWgt(GUITestOpStatus& os, const QWidget* tab) {
    return GTWidget::findWidget(os, "wgtGenerateSequence", tab == nullptr ? openTabAndReturnIt(os) : tab);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAreasForPrimingSearchWgt"
QWidget* GTUtilsPcrPrimerDesignForDnaAssembly::getAreasForPrimingSearchWgt(GUITestOpStatus& os, const QWidget* tab) {
    return GTWidget::findWidget(os, "wgtAreasForPriming", tab == nullptr ? openTabAndReturnIt(os) : tab);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getResultTableWgt"
QTableWidget* GTUtilsPcrPrimerDesignForDnaAssembly::getResultTableWgt(GUITestOpStatus& os, const QWidget* tab) {
    return GTWidget::findExactWidget<QTableWidget*>(os, "productsTable", tab == nullptr ? openTabAndReturnIt(os) : tab);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToWidget"
void GTUtilsPcrPrimerDesignForDnaAssembly::scrollToWidget(GUITestOpStatus& os, const QWidget* scrollTo,
                                                          const QWidget* tab) {
    GT_CHECK(scrollTo != nullptr, "Can't scroll to nullptr")
    const auto isWidgetFullyVisible = [] (const QWidget* w) {
        const QRegion visibleRegion = w->visibleRegion();
        return !visibleRegion.isEmpty() && visibleRegion.boundingRect().height() == w->height();
    };
    const auto moveMouseAndScroll = [](QWidget* moveMouseTo, bool scrollUp) {
        GTMouseDriver::moveTo(GTWidget::getWidgetCenter(moveMouseTo));
        GTMouseDriver::scroll(scrollUp ? 20 : -20);
        GTGlobals::sleep(200);
    };

    if (isWidgetFullyVisible(scrollTo)) {
        return;  // Nothing to do.
    }

    tab = (tab == nullptr ? openTabAndReturnIt(os) : tab);
    QLabel* tabTop = GTWidget::findLabel(os, "ArrowHeader_Choose generated sequences as user primer's end", tab);
    if (tabTop == nullptr) {  // Bad.
        return;
    }

    if (tabTop->visibleRegion().isEmpty()) {  // Are we at the bottom of the tab?
        QLabel* tabBottom = GTWidget::findLabel(os, "ArrowHeader_Other sequences in PCR reaction", tab);
        if (tabBottom == nullptr) {  // Bad.
            return;
        }
        moveMouseAndScroll(tabBottom, true);
    } else {
        moveMouseAndScroll(tabTop, false);
    }
    GT_CHECK(isWidgetFullyVisible(scrollTo), QString("Can't scroll to widget '%1'").arg(scrollTo->objectName()))
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectGeneratedSequence"
void GTUtilsPcrPrimerDesignForDnaAssembly::selectGeneratedSequence(GUITestOpStatus& os, int i,
                                                                   QTableWidget* table, const QWidget* tab) {
    scrollToWidget(os, table, tab == nullptr ? openTabAndReturnIt(os) : tab);
    GTMouseDriver::click(GTTableView::getCellPoint(os, table, i, 0));
        // GTWidget::scrollToIndex(os, table, table->model()->index(i, 0)); TODO: NEED???
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
