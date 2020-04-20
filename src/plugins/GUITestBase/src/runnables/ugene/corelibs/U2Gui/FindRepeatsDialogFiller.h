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

#ifndef _U2_GT_RUNNABLES_FIND_REPEATS_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_FIND_REPEATS_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"
#include "api/GTRegionSelector.h"
#include <U2Algorithm/SmithWatermanSettings.h>

namespace U2 {
using namespace HI;
class FindRepeatsDialogFiller : public Filler {
public:
    enum Button {Start, Cancel};

    FindRepeatsDialogFiller(HI::GUITestOpStatus &os, const QString & _resultFilesPath, bool _searchInverted = false,
        int minRepeatLength = -1, int repeatsIdentity = -1, int minDistance = -1);
    FindRepeatsDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    virtual void commonScenario();
    Button button;

private:
    QString resultAnnotationFilesPath;
    bool searchInverted;
    int minRepeatLength;
    int repeatsIdentity;
    int minDistance;
};

} // namespace U2

#endif // _U2_GT_RUNNABLES_FIND_REPEATS_DIALOG_FILLER_H_
