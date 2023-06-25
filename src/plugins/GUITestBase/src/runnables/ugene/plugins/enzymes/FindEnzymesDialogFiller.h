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

struct FindEnzymesDialogFillerSettings {
    FindEnzymesDialogFillerSettings() = default;
    FindEnzymesDialogFillerSettings(const QStringList& enzymes);

    QStringList enzymes;
    qint64 searchRegionStart = -1;
    qint64 searchRegionEnd = -1;
    qint64 excludeRegionStart = -1;
    qint64 excludeRegionEnd = -1;
    int minHits = -1;
    int maxHits = -1;
    bool clickFindAll = false;
    // Show suppliers from this list
    QStringList suppliers;
    bool clickSelectAllSuppliers = false;
};

class FindEnzymesDialogFiller : public Filler {
public:
    FindEnzymesDialogFiller(
        const FindEnzymesDialogFillerSettings& settings,
        CustomScenario* scenario = nullptr);

    FindEnzymesDialogFiller(
        const QStringList& enzymes,
        CustomScenario* scenario = nullptr);

    void commonScenario() override;

    /** Opens FindEnzymesDialog in by clicking an action in the main toolbar. */
    static void openDialogWithToolbarAction();

    /** Calls 'openDialogWithToolbarAction' to open the dialog and selects enzymes from 'enzymeNames'. */
    static void selectEnzymes(const QStringList& enzymeNames);

    FindEnzymesDialogFillerSettings settings;
};

}  // namespace U2
