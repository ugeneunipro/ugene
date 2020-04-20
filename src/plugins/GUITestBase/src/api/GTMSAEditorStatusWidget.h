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

#ifndef _U2_GT_MSAEDITOR_STATUSWIDGET_H_
#define _U2_GT_MSAEDITOR_STATUSWIDGET_H_

#include "GTGlobals.h"
#include <U2View/MaEditorStatusBar.h>

namespace U2 {
using namespace HI;

class GTMSAEditorStatusWidget {
public:
    static QWidget *getStatusWidget(HI::GUITestOpStatus &os);

    // fails if the widget is NULL or can't get length
    static int length(HI::GUITestOpStatus& os, QWidget* w);
    static int getSequencesCount(HI::GUITestOpStatus &os, QWidget *w);

    static QString getRowNumberString(HI::GUITestOpStatus &os);
    static QString getRowsCountString(HI::GUITestOpStatus &os);

    static QString getColumnNumberString(HI::GUITestOpStatus &os);
    static QString getColumnsCountString(HI::GUITestOpStatus &os);

    static QString getSequenceUngappedPositionString(HI::GUITestOpStatus &os);
    static QString getSequenceUngappedLengthString(HI::GUITestOpStatus &os);
};

}
#endif // _U2_GT_MSAEDITOR_STATUSWIDGET_H_
