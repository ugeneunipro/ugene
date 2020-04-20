/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2IdTypes.h>
#include "GTUtilsProjectTreeView.h"
#include "ProjectTreeItemSelectorDialogFiller.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTreeWidget>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "ProjectTreeItemSelectorDialogFiller"

ProjectTreeItemSelectorDialogFiller::ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus &os, const QString& documentName, const QString &objectName,
    const QSet<GObjectType> &acceptableTypes, SelectionMode mode, int expectedDocCount)
    : Filler(os, "ProjectTreeItemSelectorDialogBase"), acceptableTypes(acceptableTypes), mode(mode), expectedDocCount(expectedDocCount)
{
    itemsToSelect.insert(documentName, QStringList() << objectName);
}

ProjectTreeItemSelectorDialogFiller::ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus &os, const QMap<QString, QStringList> &itemsToSelect,
    const QSet<GObjectType> &acceptableTypes, SelectionMode mode, int expectedDocCount)
    : Filler(os, "ProjectTreeItemSelectorDialogBase"), itemsToSelect(itemsToSelect), acceptableTypes(acceptableTypes), mode(mode),
    expectedDocCount(expectedDocCount)
{

}

ProjectTreeItemSelectorDialogFiller::ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "ProjectTreeItemSelectorDialogBase", scenario),
    mode(Single),
    expectedDocCount(0)
{

}

namespace {

bool checkTreeRowCount(QTreeView *tree, int expectedDocCount) {
    int visibleItemCount = 0;
    for (int i = 0; i < tree->model()->rowCount(); ++i) {
        if (Qt::NoItemFlags != tree->model()->flags(tree->model()->index(i, 0))) {
            ++visibleItemCount;
        }
    }
    return visibleItemCount == expectedDocCount;
}

}

#define GT_METHOD_NAME "commonScenario"
void ProjectTreeItemSelectorDialogFiller::commonScenario(){
    GTGlobals::sleep(1000);
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog was not found");

    QTreeView* treeView = dialog->findChild<QTreeView*>();
    GT_CHECK(treeView != NULL, "treeWidget is NULL");

    if (-1 != expectedDocCount) {
        CHECK_SET_ERR(checkTreeRowCount(treeView, expectedDocCount), "Unexpected document count");
    }

    GTGlobals::FindOptions options;
    options.depth = GTGlobals::FindOptions::INFINITE_DEPTH;

    if (Separate == mode) {
        GTKeyboardDriver::keyPress(Qt::Key_Control);
    }

    bool firstIsSelected = false;
    foreach (const QString& documentName, itemsToSelect.keys()) {
        const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, documentName, options);
        GTUtilsProjectTreeView::checkObjectTypes(os, treeView, acceptableTypes, documentIndex);

        const QStringList objects = itemsToSelect.value(documentName);
        if (!objects.isEmpty()) {
            foreach (const QString& objectName, itemsToSelect.value(documentName)) {
                const QModelIndex objectIndex = GTUtilsProjectTreeView::findIndex(os, treeView, objectName, documentIndex, options);
                GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, treeView, objectIndex));
                GTMouseDriver::click();
                if (!firstIsSelected && Continuous == mode) {
                    GTKeyboardDriver::keyPress(Qt::Key_Shift);
                    firstIsSelected = true;
                }
            }
        } else {
            GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, treeView, documentIndex));
            GTMouseDriver::click();
        }
    }

    switch (mode) {
    case Separate:
        GTKeyboardDriver::keyClick( Qt::Key_Control);
        break;
    case Continuous:
        GTKeyboardDriver::keyClick( Qt::Key_Shift);
        break;
    default:
        ; // empty default section to avoid GCC warning
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
