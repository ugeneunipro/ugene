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

#ifndef _U2_GT_RUNNABLES_SMITH_WATERMAN_DIALOG_BASE_FILLER_H_
#define _U2_GT_RUNNABLES_SMITH_WATERMAN_DIALOG_BASE_FILLER_H_

#include "utils/GTUtilsDialog.h"
#include "api/GTRegionSelector.h"
#include <U2Algorithm/SmithWatermanSettings.h>

namespace U2 {
using namespace HI;

class SmithWatermanDialogFiller : public Filler {
public:
    enum Button {Search, Cancel};
    enum SwRealization {CLASSIC, SSE2};
    SmithWatermanDialogFiller(HI::GUITestOpStatus &_os, const QString& _pattern = "",
        const GTRegionSelector::RegionSelectorSettings& _s
        = GTRegionSelector::RegionSelectorSettings(), SwRealization _realization = CLASSIC);

    SmithWatermanDialogFiller(HI::GUITestOpStatus &_os, SwRealization _realization,
        const SmithWatermanSettings::SWResultView _resultView, const QString & _resultFilesPath,
        const QString& _pattern = "", const GTRegionSelector::RegionSelectorSettings& _s
        = GTRegionSelector::RegionSelectorSettings());

    SmithWatermanDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    virtual void commonScenario();
    Button button;

private:
    QString pattern;
    GTRegionSelector::RegionSelectorSettings s;
    SmithWatermanSettings::SWResultView resultView;
    QString resultFilesPath;
    bool autoSetupAlgorithmParams;
    SwRealization realization;
};

} // namespace U2

#endif // _U2_GT_RUNNABLES_SMITH_WATERMAN_DIALOG_BASE_FILLER_H_
