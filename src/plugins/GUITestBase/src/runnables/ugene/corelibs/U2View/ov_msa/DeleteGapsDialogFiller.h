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

#ifndef _U2_GT_RUNNABLES_DELETE_GAPS_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_DELETE_GAPS_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class DeleteGapsDialogFiller : public Filler {
public:
    DeleteGapsDialogFiller(HI::GUITestOpStatus &os, int _radioButNum=0) : Filler(os, "DeleteGapsDialog"),
        radioButNum(_radioButNum){}
    void commonScenario();
private:
    int radioButNum;
};

class RemoveGapColsDialogFiller : public Filler {
public:
    enum Radio {Number, Percent, Column};
    RemoveGapColsDialogFiller(HI::GUITestOpStatus &os, Radio _button, int _spinValue=1) : Filler(os, "DeleteGapsDialog"),
        button(_button), spinValue(_spinValue){
        radioMap[Number] = "absoluteRadioButton";
        radioMap[Percent] = "relativeRadioButton";
        radioMap[Column] = "allRadioButton";
    }
    void commonScenario();
private:
    Radio button;
    int spinValue;
    QMap<Radio,QString> radioMap;
};
}

#endif
