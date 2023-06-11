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

class BuildDotPlotFiller : public Filler {
public:
    BuildDotPlotFiller(
        const QString& _firstFileName,
        const QString& _secondFileName = "",
        bool _mergeFirstBoxChecked = false,
        bool _oneSequenceBoxChecked = false,
        bool _mergeSecondBoxChecked = false,
        int _gapFirstValue = 5,
        int _gapSecondValue = 5,
        bool cancel = false);

    void commonScenario() override;

private:
    bool mergeFirstBoxChecked;
    bool oneSequenceBoxChecked;
    bool mergeSecondBoxChecked;
    QString firstFileName;
    QString secondFileName;
    int firstGapSize;
    int secondGapSize;
    bool cancel;
};
}  // namespace U2
