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

#include <QApplication>
#include <QTreeWidget>

#include "ConstructMoleculeDialogFiller.h"
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "ConstructMoleculeDialogFiller"

ConstructMoleculeDialogFiller::ConstructMoleculeDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions) :
    Filler(os, "ConstructMoleculeDialog"),
    dialog(NULL),
    actions(actions)
{
}

ConstructMoleculeDialogFiller::ConstructMoleculeDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario)
: Filler(os, "ConstructMoleculeDialog", scenario), dialog(NULL)
{
}

#define GT_METHOD_NAME "commonScenario"
void ConstructMoleculeDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(NULL != dialog, "activeModalWidget is NULL");

    foreach (const Action &action, actions) {
        CHECK_OP(os, );

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
            GT_CHECK(false, "An unrecognized action type");
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addAllFragments"
void ConstructMoleculeDialogFiller::addAllFragments() {
    GTWidget::click(os, GTWidget::findWidget(os, "takeAllButton", dialog));
    GTGlobals::sleep();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "invertAddedFragment"
void ConstructMoleculeDialogFiller::invertAddedFragment(const QVariant &actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a fragment name's part from the action data");
    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    GTTreeWidget::checkItem(os, GTTreeWidget::findItem(os, GTWidget::findExactWidget<QTreeWidget *>(os, "molConstructWidget", dialog), actionData.toString(), NULL, 1, options), 3);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCancel"
void ConstructMoleculeDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOk"
void ConstructMoleculeDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
