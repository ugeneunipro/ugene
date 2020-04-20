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

#ifndef _U2_GT_RUNNABLES_PROJECT_TREE_ITEM_SELECTOR_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_PROJECT_TREE_ITEM_SELECTOR_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"
#include "GTUtilsProjectTreeView.h"

namespace U2 {

class ProjectTreeItemSelectorDialogFiller : public Filler{
public:
    enum SelectionMode {
        Single, Separate, Continuous
    };

    ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus &os, const QString &documentName, const QString &objectName,
        const QSet<GObjectType> &acceptableTypes = QSet<GObjectType>(), SelectionMode mode = Single, int expectedDocCount = -1);
    ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus &os, const QMap<QString, QStringList> &itemsToSelect,
        const QSet<GObjectType> &acceptableTypes = QSet<GObjectType>(), SelectionMode mode = Single, int expectedDocCount = -1);
    ProjectTreeItemSelectorDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    virtual void commonScenario();

private:
    QMap<QString, QStringList> itemsToSelect;
    const QSet<GObjectType> acceptableTypes;
    const SelectionMode mode;
    const int expectedDocCount;
};

}

#endif
