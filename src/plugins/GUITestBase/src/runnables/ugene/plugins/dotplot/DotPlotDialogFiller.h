/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

class DotPlotFiller : public Filler {
public:
    DotPlotFiller(int _minLen = 100, int _identity = 0, bool _invertedRepeats = false, bool _but1kpressed = false)
        : Filler("DotPlotDialog"), minLen(_minLen), identity(_identity), invertedRepeats(_invertedRepeats), but1kpressed(_but1kpressed) {
    }
    DotPlotFiller(CustomScenario* customScenario);
    void commonScenario() override;

private:
    int minLen, identity;
    bool invertedRepeats, but1kpressed;
};
}  // namespace U2
