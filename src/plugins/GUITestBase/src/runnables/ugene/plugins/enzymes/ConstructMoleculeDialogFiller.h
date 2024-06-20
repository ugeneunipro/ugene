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

#pragma once

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class ConstructMoleculeDialogFiller : public Filler {
public:
    enum ActionType {  // an appropriate action data
        AddFragment,  // QString with a part of the fragment name, if several fragments match this part, the first one will be selected
        AddAllFragments,  // ignored
        SelectAddedFragment,  // QString with a part of the fragment name, if several fragments match this part, the first one will be selected
        InvertAddedFragment,  // QString with a part of the fragment name, if several fragments match this part, the first one will be inverted
        CheckMakeCircular,  // Check state, true if ignored
        ClickAdjustLeft,  // ignored
        CheckAdjustLeftEnabled,  // Button enabled state, true if enabled, false if not
        ClickAdjustRight,  // ignored
        CheckAdjustRightEnabled,  // Button enabled state, true if enabled, false if not
        ClickRemove,  // ignored
        ClickCancel,  // ignored
        ClickOk
    };
    typedef QPair<ActionType, QVariant> Action;

    ConstructMoleculeDialogFiller(const QList<Action>& actions);
    ConstructMoleculeDialogFiller(CustomScenario* scenario);

    void commonScenario() override;

private:
    void addFragment(const QVariant& actionData);
    void addAllFragments();
    void selectFragment(const QVariant& actionData);
    void invertAddedFragment(const QVariant& actionData);
    void checkMakeCircular(const QVariant& actionData);
    void clickAdjustLeft();
    void checkAdjustLeftEnabled(const QVariant& actionData);
    void clickAdjustRight();
    void checkAdjustRightEnabled(const QVariant& actionData);
    void clickRemove();
    void clickCancel();
    void clickOk();

    QWidget* dialog = nullptr;
    const QList<Action> actions;
};

}  // namespace U2
