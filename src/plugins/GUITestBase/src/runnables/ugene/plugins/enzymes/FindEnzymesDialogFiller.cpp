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

#include "FindEnzymesDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QTreeWidget>

#include <U2Gui/MainWindow.h>

namespace U2 {

FindEnzymesDialogFillerSettings::FindEnzymesDialogFillerSettings() {
}

FindEnzymesDialogFillerSettings::FindEnzymesDialogFillerSettings(const QStringList& _enzymes)
    : enzymes(_enzymes) {
}

#define GT_CLASS_NAME "FindEnzymesDialogFiller"

FindEnzymesDialogFiller::FindEnzymesDialogFiller(HI::GUITestOpStatus& os,
                                                 const FindEnzymesDialogFillerSettings& _settings,
                                                 CustomScenario* scenario)
    : Filler(os, "FindEnzymesDialog", scenario), settings(_settings) {
}

FindEnzymesDialogFiller::FindEnzymesDialogFiller(HI::GUITestOpStatus& os,
                                                 const QStringList& enzymes,
                                                 CustomScenario* scenario)
    : Filler(os, "FindEnzymesDialog", scenario), settings(enzymes) {
}

#define GT_METHOD_NAME "run"
void FindEnzymesDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget(os);

    if (!settings.clickFindAll) {
        auto enzymesSelectorWidget = GTWidget::findWidget(os, "enzymesSelectorWidget");
        GTWidget::click(os, GTWidget::findWidget(os, "selectNoneButton", enzymesSelectorWidget));
        auto enzymesTree = GTWidget::findTreeWidget(os, "tree", enzymesSelectorWidget);
        for (const QString& enzyme : qAsConst(settings.enzymes)) {
            QTreeWidgetItem* item = GTTreeWidget::findItem(os, enzymesTree, enzyme);
            GTTreeWidget::checkItem(os, item);
        }
    } else {
        GTWidget::click(os, GTWidget::findWidget(os, "selectAllButton", dialog));
    }

    if (settings.minHits > 0 || settings.maxHits > 0) {
        GTGroupBox::setChecked(os, "filterGroupBox", dialog);
        if (settings.minHits > 0) {
            GTSpinBox::setValue(os, "minHitSB", settings.minHits, dialog);
        }
        if (settings.maxHits > 0) {
            GTSpinBox::setValue(os, "maxHitSB", settings.maxHits, dialog);
        }
    }

    if (settings.searchRegionStart != -1 && settings.searchRegionEnd != -1) {
        auto regionSelector = GTWidget::findWidget(os, "region_selector_with_excluded");

        GTLineEdit::setText(os, "startLineEdit", QString::number(settings.searchRegionStart), regionSelector);
        GTLineEdit::setText(os, "endLineEdit", QString::number(settings.searchRegionEnd), regionSelector);
    }

    if (settings.excludeRegionStart != -1 && settings.excludeRegionEnd != -1) {
        auto exclude = GTWidget::findCheckBox(os, "excludeCheckBox");
        GTCheckBox::setChecked(os, exclude);

        GTLineEdit::setText(os, "excludeStartLineEdit", QString::number(settings.excludeRegionStart));
        GTLineEdit::setText(os, "excludeEndLinEdit", QString::number(settings.excludeRegionEnd));
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openDialogWithToolbarAction"
void FindEnzymesDialogFiller::openDialogWithToolbarAction(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectEnzymes"
void FindEnzymesDialogFiller::selectEnzymes(HI::GUITestOpStatus& os, const QStringList& enzymeNames) {
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, enzymeNames));
    openDialogWithToolbarAction(os);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
