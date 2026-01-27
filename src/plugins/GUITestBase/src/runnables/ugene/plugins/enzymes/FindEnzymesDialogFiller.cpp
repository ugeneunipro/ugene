/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "FindEnzymesDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QTreeWidget>

#include <U2Gui/MainWindow.h>

#include "runnables/ugene/corelibs/U2Gui/GTComboBoxWithCheckBoxes.h"

namespace U2 {

FindEnzymesDialogFillerSettings::FindEnzymesDialogFillerSettings(const QStringList& _enzymes)
    : enzymes(_enzymes) {
}

#define GT_CLASS_NAME "FindEnzymesDialogFiller"

FindEnzymesDialogFiller::FindEnzymesDialogFiller(
    const FindEnzymesDialogFillerSettings& _settings,
    CustomScenario* scenario)
    : Filler("FindEnzymesDialog", scenario), settings(_settings) {
}

FindEnzymesDialogFiller::FindEnzymesDialogFiller(
    const QStringList& enzymes,
    CustomScenario* scenario)
    : Filler("FindEnzymesDialog", scenario), settings(enzymes) {
}

void FindEnzymesDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget();

    if (settings.clickSelectAllSuppliers) {
        GTWidget::click(GTWidget::findWidget("pbSelectAll", dialog));
    }

    if (!settings.suppliers.isEmpty()) {
        GTComboBoxWithCheckBoxes::selectItemByText("cbSuppliers", dialog, settings.suppliers, GTGlobals::UseMouse);
    }

    if (!settings.clickFindAll) {
        auto enzymesSelectorWidget = GTWidget::findWidget("enzymesSelectorWidget");
        GTWidget::click(GTWidget::findWidget("selectNoneButton", enzymesSelectorWidget));
        auto enzymesTree = GTWidget::findTreeWidget("tree", enzymesSelectorWidget);
        for (const QString& enzyme : qAsConst(settings.enzymes)) {
            QTreeWidgetItem* item = GTTreeWidget::findItem(enzymesTree, enzyme);
            GTTreeWidget::checkItem(item);
        }
    } else {
        GTWidget::click(GTWidget::findWidget("selectAllButton", dialog));
    }

    if (settings.minHits > 0 || settings.maxHits > 0) {
        GTGroupBox::setChecked("filterGroupBox", dialog);
        if (settings.minHits > 0) {
            GTSpinBox::setValue("minHitSB", settings.minHits, dialog);
        }
        if (settings.maxHits > 0) {
            GTSpinBox::setValue("maxHitSB", settings.maxHits, dialog);
        }
    }

    auto regionSelector = GTWidget::findWidget("range_selector");
    if (settings.searchRegionStart != -1 && settings.searchRegionEnd != -1) {
        GTLineEdit::setText("start_edit_line", QString::number(settings.searchRegionStart), regionSelector);
        GTLineEdit::setText("end_edit_line", QString::number(settings.searchRegionEnd), regionSelector);
    } else if (!settings.searchRegions.isEmpty()) {
        QString locationStr;
        GTComboBox::selectItemByText(GTWidget::findComboBox("region_type_combo", regionSelector), "Location");
        for (const U2Region& r : settings.searchRegions) {
            locationStr += QString::number(r.startPos) + ".." + QString::number(r.endPos()) + ",";
        }
        locationStr.chop(1);
        GTLineEdit::setText("location_line_edit", locationStr, regionSelector);
    }

    if ((settings.excludeRegionStart != -1 && settings.excludeRegionEnd != -1) || !settings.excludeRegions.isEmpty()) {
        regionSelector = GTWidget::findWidget("exclude_range_selector");
        auto exclude = GTWidget::findCheckBox("excludeCheckBox");
        GTCheckBox::setChecked(exclude);
        if (settings.excludeRegionStart != -1 && settings.excludeRegionEnd != -1) {
            GTLineEdit::setText("start_edit_line", QString::number(settings.excludeRegionStart), regionSelector);
            GTLineEdit::setText("end_edit_line", QString::number(settings.excludeRegionEnd), regionSelector);
        } else {
            GTComboBox::selectItemByText(GTWidget::findComboBox("region_type_combo", regionSelector), "Location");
            QString locationStr;
            for (const U2Region& r : settings.excludeRegions) {
                locationStr += QString::number(r.startPos) + ".." + QString::number(r.endPos()) + ",";
            }
            locationStr.chop(1);
            GTLineEdit::setText("location_line_edit", locationStr, regionSelector);
        }
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void FindEnzymesDialogFiller::openDialogWithToolbarAction() {
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
}

void FindEnzymesDialogFiller::selectEnzymes(const QStringList& enzymeNames) {
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(enzymeNames));
    openDialogWithToolbarAction();
}

#undef GT_CLASS_NAME

}  // namespace U2
