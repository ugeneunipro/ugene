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

#ifndef _U2_CONSTRUCT_MOLECULE_DIALOG_FILLER_H_
#define _U2_CONSTRUCT_MOLECULE_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class ConstructMoleculeDialogFiller : public Filler {
public:
    enum ActionType {           // an appropriate action data
        AddAllFragments,        // ignored
        InvertAddedFragment,    // QString with a part of the fragment name, if several fragments match this part, the first one will be inverted
        ClickCancel,            // ignored
        ClickOk
    };
    typedef QPair<ActionType, QVariant> Action;

    ConstructMoleculeDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions);
    ConstructMoleculeDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    void commonScenario();

private:
    void addAllFragments();
    void invertAddedFragment(const QVariant &actionData);
    void clickCancel();
    void clickOk();

    QWidget *dialog;
    const QList<Action> actions;
};

}   // namespace U2

#endif // _U2_CONSTRUCT_MOLECULE_DIALOG_FILLER_H_
