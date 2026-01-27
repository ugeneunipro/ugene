/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <QString>

#include <U2Core/GUrl.h>
#include <U2Core/U2Location.h>

namespace U2 {

// mfold specific params for script args. Default values are the same as in mfold script.
struct MfoldAlgoSettings final {
    int temperature = 37;
    double naConc = 1;
    double mgConc = 0;
    int percent = 5;
    int window = -1;  // -1=depends on sequence len, see mfold:338
    int maxBp = 0;  // 0=inf i.e. no limit
    int maxFold = 50;
    int labFr = -1;  // -1=depends on sequence len, see mfold:551
    double rotAng = 0;
};
// Out settings.
struct MfoldOutSettings final {
    // Path for HTML report and imgs.
    QString outPath;
    int dpi = 96;  // default DPI set in ui
};
// Sequence info.
struct MfoldSequenceInfo final {
    QString seqName;
    // Use the same output file name as input file.
    GUrl seqPath;
    bool isCircular = false;
    bool isDna = false;
};

// All settings specified in the dialog, together.
struct MfoldSettings final {
    MfoldAlgoSettings algoSettings;
    MfoldOutSettings outSettings;
    // Contains one region if it's simple region (start < end)
    // or one circular region with sequence endpoint inside (start of region > end region)
    // or two regions if it's single circular region with sequence endpoint inside.
    // The last two views are equivalent, but are used in different parts of code in different views.
    U2Location region;
};

namespace Mfold {
// Splits region with sequence endpoint inside (start of region > end region) into two regions.
// Does nothing with normal (start < end) region. Expects location with one region or two regions.
// Does nothing if there are two regions.
void toGenbankLocation(U2Location& location, qint64 seqLen);
}  // namespace Mfold
}  // namespace U2
