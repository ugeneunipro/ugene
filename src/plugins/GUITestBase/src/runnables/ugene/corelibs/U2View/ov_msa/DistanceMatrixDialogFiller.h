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
#ifndef DISTANCEMATRIXDIALOGFILLER_H
#define DISTANCEMATRIXDIALOGFILLER_H

#include "utils/GTUtilsDialog.h"
namespace U2{
using namespace HI;

class DistanceMatrixDialogFiller : public Filler {
public:
    enum SaveFormat {
        HTML,
        CSV,
        NONE
    };

    DistanceMatrixDialogFiller(HI::GUITestOpStatus &os, bool hamming = true, bool counts = true, bool excludeGaps = true);
    DistanceMatrixDialogFiller(HI::GUITestOpStatus &os, SaveFormat format, QString path);
    DistanceMatrixDialogFiller(HI::GUITestOpStatus &os, CustomScenario* c);

    virtual void commonScenario();

public:
    bool hamming;
    bool counts;
    bool excludeGaps;
    bool saveToFile;
    SaveFormat format;
    QString path;
};

}

#endif // DISTANCEMATRIXDIALOGFILLER_H
