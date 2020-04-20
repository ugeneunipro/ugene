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

#ifndef GENERATEALIGNMENTPROFILEDIALOGFILLER_H
#define GENERATEALIGNMENTPROFILEDIALOGFILLER_H

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class GenerateAlignmentProfileDialogFiller : public Filler
{
public:
    enum saveFormat{NONE,HTML, CSV};
    GenerateAlignmentProfileDialogFiller(HI::GUITestOpStatus &os, bool _counts=true, saveFormat _format=NONE,
                                         QString _filePath = "") : Filler(os, "DNAStatMSAProfileDialog"),
        counts(_counts),
        gapScore(false),
        symdolScore(false),
        skipGaps(false),
        format(_format),
        filePath(_filePath)
    {
        checkBoxItems[NONE] = "NONE";
        checkBoxItems[HTML] = "htmlRB";
        checkBoxItems[CSV] = "csvRB";
    }

    GenerateAlignmentProfileDialogFiller(HI::GUITestOpStatus &os, bool _gapsScore, bool _symbolScore, bool _skipGaps) : Filler(os, "DNAStatMSAProfileDialog"),
        counts(true),
        gapScore(_gapsScore),
        symdolScore(_symbolScore),
        skipGaps(_skipGaps),
        format(NONE)
        {}

    void commonScenario();
private:
    bool counts, gapScore,symdolScore, skipGaps;
    saveFormat format;
    QMap<saveFormat,QString> checkBoxItems;
    QString filePath;
};

}

#endif // GENERATEALIGNMENTPROFILEDIALOGFILLER_H
