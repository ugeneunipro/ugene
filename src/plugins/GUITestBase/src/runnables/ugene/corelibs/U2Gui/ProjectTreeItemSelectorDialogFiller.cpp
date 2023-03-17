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

#include "ProjectTreeItemSelectorDialogFiller.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTTreeView.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QTreeWidget>

#include <U2Core/U2IdTypes.h>

#include "GTUtilsProjectTreeView.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "ProjectTreeItemSelectorDialogFiller"

ProjectTreeItemSelectorDialogFiller::ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus& os,
                                                                         const QString& documentName,
                                                                         const QString& objectName,
                                                                         const QSet<GObjectType>& acceptableTypes,
                                                                         SelectionMode mode,
                                                                         int expectedDocCount)
    : Filler(os, "ProjectTreeItemSelectorDialogBase"), acceptableTypes(acceptableTypes), mode(mode), expectedDocCount(expectedDocCount) {
    itemsToSelect.insert(documentName, {objectName});
}

ProjectTreeItemSelectorDialogFiller::ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus& os,
                                                                         const QMap<QString, QStringList>& itemsToSelect,
                                                                         const QSet<GObjectType>& acceptableTypes,
                                                                         SelectionMode mode,
                                                                         int expectedDocCount)
    : Filler(os, "ProjectTreeItemSelectorDialogBase"), itemsToSelect(itemsToSelect), acceptableTypes(acceptableTypes), mode(mode),
      expectedDocCount(expectedDocCount) {
}

ProjectTreeItemSelectorDialogFiller::ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "ProjectTreeItemSelectorDialogBase", scenario),
      mode(Single),
      expectedDocCount(0) {
}

static bool checkTreeRowCount(QTreeView* tree, int expectedDocCount) {
    int visibleItemCount = 0;
    for (int i = 0; i < tree->model()->rowCount(); ++i) {
        Qt::ItemFlags itemFlags = tree->model()->flags(tree->model()->index(i, 0));
        if (itemFlags != Qt::NoItemFlags) {
            visibleItemCount++;
        }
    }
    return visibleItemCount == expectedDocCount;
}

#define GT_METHOD_NAME "commonScenario"
void ProjectTreeItemSelectorDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget(os);
    auto treeView = GTWidget::findTreeView(os, "treeView", dialog);
    CHECK_SET_ERR(expectedDocCount == -1 || checkTreeRowCount(treeView, expectedDocCount), "Unexpected document count");

    GTGlobals::FindOptions options;
    options.depth = GTGlobals::FindOptions::INFINITE_DEPTH;

    bool isFirstClick = true;
    QList<QString> allItemKeys = itemsToSelect.keys();
    auto getCurrentClickModifier = [this, &isFirstClick] { return isFirstClick ? Qt::Key_unknown : (mode == Continuous ? Qt::Key_Shift : Qt::Key_Control); };
    for (const QString& itemKey : qAsConst(allItemKeys)) {
        QModelIndex parentItemIndex = GTUtilsProjectTreeView::findIndex(os, treeView, itemKey, options);
        if (!acceptableTypes.isEmpty()) {
            GTUtilsProjectTreeView::checkObjectTypes(os, treeView, acceptableTypes, parentItemIndex);
        }
        QStringList objectNames = itemsToSelect.value(itemKey);
        if (objectNames.isEmpty()) {  // Select the document itself.
            GTTreeView::click(os, treeView, parentItemIndex, getCurrentClickModifier());
            isFirstClick = false;
            continue;
        }
        for (const QString& objectName : qAsConst(objectNames)) {
            QModelIndex objectIndex = GTUtilsProjectTreeView::findIndex(os, treeView, objectName, parentItemIndex, options);
            GTTreeView::click(os, treeView, objectIndex, getCurrentClickModifier());
            isFirstClick = false;
        }
    }
    // Close the dialog.
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
