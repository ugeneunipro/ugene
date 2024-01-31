/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
 * https://ugene.net
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
#include <U2Core/U2Region.h>

namespace U2 {

// Settings for UGENE MfoldTask.
struct MfoldSettings final {
    // Mfold specific params. Default values are the same as in mfold script.
    int temperature = 37;
    int percent = 5;
    int window = -1;  // -1=depends on sequence len, see mfold:338
    int maxBp = 0;  // 0=inf i.e. no limit
    int maxFold = 100;
    int labFr = -1;  // -1=depends on sequence len, see mfold:551
    double rotAng = 0;
    double naConc = 1;
    double mgConc = 0;

    // Sequence region for exporting to file and inputting into the Mfold tool.
    U2Region region;
    // Path for HTML report and imgs. Empty if saved temporarily.
    QString outPath;
};
}  // namespace U2
