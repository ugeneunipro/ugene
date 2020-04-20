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

#ifndef _U2_CAP3_SUPPORT_DIALOG_FILLER_H_
#define _U2_CAP3_SUPPORT_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class CAP3SupportDialogFiller : public Filler
{
public:
    CAP3SupportDialogFiller(HI::GUITestOpStatus &os, QStringList _input, const QString& _output):
    Filler(os, "CAP3SupportDialog"), input(_input), output(_output){}
    CAP3SupportDialogFiller(HI::GUITestOpStatus &os, CustomScenario* c): Filler(os, "CAP3SupportDialog", c){}
    void commonScenario();
private:
    QStringList input;
    QString output;
};
}
#endif // CAP3SUPPORTDIALOGFILLER_H
