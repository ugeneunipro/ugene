/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <primitives/GTLineEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QTreeWidget>

#include <U2Gui/MainWindow.h>

namespace U2 {

#define GT_CLASS_NAME "FindEnzymesDialogFiller"

FindEnzymesDialogFiller::FindEnzymesDialogFiller(HI::GUITestOpStatus& os, const QStringList& enzymesToFind, CustomScenario* scenario)
    : Filler(os, "FindEnzymesDialog", scenario),
      enzymesToFind(enzymesToFind) {
}

FindEnzymesDialogFiller::FindEnzymesDialogFiller(GUITestOpStatus& os, const QStringList& enzymesToFind, qint64 searchRegionStart, qint64 searchRegionEnd, qint64 excludedRegionStart, qint64 excludedRegionEnd, CustomScenario* scenario)
    : Filler(os, "FindEnzymesDialog", scenario),
      enzymesToFind(enzymesToFind),
      searchStart(searchRegionStart),
      searchEnd(searchRegionEnd),
      excludeStart(excludedRegionStart),
      excludeEnd(excludedRegionEnd) {
}

#define GT_METHOD_NAME "run"
void FindEnzymesDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget(os);

    auto enzymesSelectorWidget = GTWidget::findWidget(os, "enzymesSelectorWidget");
    GTWidget::click(os, GTWidget::findWidget(os, "selectNoneButton", enzymesSelectorWidget));

    auto enzymesTree = GTWidget::findTreeWidget(os, "tree", enzymesSelectorWidget);
    for (const QString& enzyme : qAsConst(enzymesToFind)) {
        QTreeWidgetItem* item = GTTreeWidget::findItem(os, enzymesTree, enzyme);
        GTTreeWidget::checkItem(os, item);
    }

    if (searchStart != -1 && searchEnd != -1) {
        auto regionSelector = GTWidget::findWidget(os, "region_selector_with_excluded");

        GTLineEdit::setText(os, "startLineEdit", QString::number(searchStart), regionSelector);
        GTLineEdit::setText(os, "endLineEdit", QString::number(searchEnd), regionSelector);
    }

    if (excludeStart != -1 && excludeEnd != -1) {
        auto exclude = GTWidget::findCheckBox(os, "excludeCheckBox");
        GTCheckBox::setChecked(os, exclude);

        GTLineEdit::setText(os, "excludeStartLineEdit", QString::number(excludeStart));
        GTLineEdit::setText(os, "excludeEndLinEdit", QString::number(excludeEnd));
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
