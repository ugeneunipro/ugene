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

#include "ConstructMoleculeDialogFiller.h"
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
            case AddAllFragments:
                addAllFragments();
                break;
            case InvertAddedFragment:
                invertAddedFragment(action.second);
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

void ConstructMoleculeDialogFiller::addAllFragments() {
    GTWidget::click(GTWidget::findWidget("takeAllButton", dialog));
    GTGlobals::sleep();
}

void ConstructMoleculeDialogFiller::invertAddedFragment(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a fragment name's part from the action data");
    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    QTreeWidgetItem* item = GTTreeWidget::findItem(GTWidget::findTreeWidget("molConstructWidget", dialog), actionData.toString(), nullptr, 1, options);
    bool isCheck = item->data(3, Qt::CheckStateRole).toInt() == Qt::Unchecked;
    GTTreeWidget::checkItem(item, 3, GTGlobals::UseMouse, isCheck, false);  // Do not validate the result. The item is replaced on toggle.
}

void ConstructMoleculeDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

void ConstructMoleculeDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_CLASS_NAME

}  // namespace U2
