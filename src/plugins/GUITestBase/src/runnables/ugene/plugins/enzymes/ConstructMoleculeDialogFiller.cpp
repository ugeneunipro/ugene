/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "ConstructMoleculeDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QTreeWidget>

#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "ConstructMoleculeDialogFiller"

ConstructMoleculeDialogFiller::ConstructMoleculeDialogFiller(const QList<Action>& actions)
    : Filler("ConstructMoleculeDialog"), actions(actions) {
}

ConstructMoleculeDialogFiller::ConstructMoleculeDialogFiller(CustomScenario* scenario)
    : Filler("ConstructMoleculeDialog", scenario) {
}

void ConstructMoleculeDialogFiller::commonScenario() {
    dialog = GTWidget::getActiveModalWidget();

    foreach (const Action& action, actions) {
        switch (action.first) {
            case AddFragment:
                addFragment(action.second);
                break;
            case AddAllFragments:
                addAllFragments();
                break;
            case SelectAddedFragment:
                selectFragment(action.second);
                break;
            case InvertAddedFragment:
                invertAddedFragment(action.second);
                break;
            case CheckMakeCircular:
                checkMakeCircular(action.second);
                break;
            case ClickAdjustLeft:
                clickAdjustLeft();
                break;
            case ClickAdjustRight:
                clickAdjustRight();
                break;
            case ClickCancel:
                clickCancel();
                break;
            case ClickOk:
                clickOk();
                break;
            default:
                GT_FAIL("An unrecognized action type", );
        }
    }
}

void ConstructMoleculeDialogFiller::addFragment(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a fragment name's part from the action data");

    auto fragmentListWidget = GTWidget::findListWidget("fragmentListWidget", dialog);
    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    GTListWidget::click(fragmentListWidget, actionData.toString(), Qt::LeftButton, 0, options);
    GTWidget::click(GTWidget::findPushButton("takeButton", dialog));
}

void ConstructMoleculeDialogFiller::addAllFragments() {
    GTWidget::click(GTWidget::findWidget("takeAllButton", dialog));
    GTGlobals::sleep();
}

void ConstructMoleculeDialogFiller::selectFragment(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a fragment name's part from the action data");
    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    QTreeWidgetItem* item = GTTreeWidget::findItem(GTWidget::findTreeWidget("molConstructWidget", dialog), actionData.toString(), nullptr, 1, options);
    GTTreeWidget::click(item, 1);
}

void ConstructMoleculeDialogFiller::invertAddedFragment(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a fragment name's part from the action data");

    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    QTreeWidgetItem* item = GTTreeWidget::findItem(GTWidget::findTreeWidget("molConstructWidget", dialog), actionData.toString(), nullptr, 1, options);
    bool isCheck = item->data(3, Qt::CheckStateRole).toInt() == Qt::Unchecked;
    GTTreeWidget::checkItem(item, 3, GTGlobals::UseMouse, isCheck, false);  // Do not validate the result. The item is replaced on toggle.
}

void ConstructMoleculeDialogFiller::checkMakeCircular(const QVariant& actionData) {
    bool check = true;
    if (!actionData.isValid()) {
        GT_CHECK(actionData.canConvert<bool>(), "Can't convert to bool");
        check = actionData.toBool();
    }

    GTCheckBox::setChecked("makeCircularBox", check);
}

void ConstructMoleculeDialogFiller::clickAdjustLeft() {
    GTWidget::click(GTWidget::findToolButton("tbAdjustLeft", dialog));
}

void ConstructMoleculeDialogFiller::clickAdjustRight() {
    GTWidget::click(GTWidget::findToolButton("tbAdjustRight", dialog));
}

void ConstructMoleculeDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

void ConstructMoleculeDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_CLASS_NAME

}  // namespace U2
