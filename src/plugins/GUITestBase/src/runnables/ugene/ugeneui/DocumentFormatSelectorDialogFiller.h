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

#ifndef DOCUMENTFORMATSELECTORDIALOGFILLER_H
#define DOCUMENTFORMATSELECTORDIALOGFILLER_H

#include "utils/GTUtilsDialog.h"

class QRadioButton;

namespace U2{
using namespace HI;

class DocumentFormatSelectorDialogFiller : public Filler
{
public:
    DocumentFormatSelectorDialogFiller(HI::GUITestOpStatus &os,
                                       const QString &_format,
                                       const int _score = -1,
                                       const int _formatLineLable = -1):
        Filler(os, "DocumentFormatSelectorDialog"),
        format(_format),
        score(_score),
        formatLineLable(_formatLineLable){}
    DocumentFormatSelectorDialogFiller(HI::GUITestOpStatus &os, CustomScenario* custom):
        Filler(os, "DocumentFormatSelectorDialog", custom){}
    virtual void commonScenario();
private:
    QString format;
    int score;
    int formatLineLable;
    QRadioButton* getButton(HI::GUITestOpStatus &os);
};

}

#endif // DOCUMENTFORMATSELECTORDIALOGFILLER_H
