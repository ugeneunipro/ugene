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

#ifndef _U2_GT_UTILS_MCA_EDITOR_STATUS_WIDGET_H_
#define _U2_GT_UTILS_MCA_EDITOR_STATUS_WIDGET_H_

class QWidget;

namespace HI {
class GUITestOpStatus;
}

namespace U2 {

class GTUtilsMcaEditorStatusWidget {
public:
    static QWidget *getStatusWidget(HI::GUITestOpStatus &os);

    static QString getRowNumberString(HI::GUITestOpStatus &os);
    static int getRowNumber(HI::GUITestOpStatus &os);
    static QString getRowsCountString(HI::GUITestOpStatus &os);
    static int getRowsCount(HI::GUITestOpStatus &os);

    static QString getReferenceUngappedPositionString(HI::GUITestOpStatus &os);
    static int getReferenceUngappedPosition(HI::GUITestOpStatus &os);
    static QString getReferenceUngappedLengthString(HI::GUITestOpStatus &os);
    static int getReferenceUngappedLength(HI::GUITestOpStatus &os);
    static bool isGapInReference(HI::GUITestOpStatus &os);

    static QString getReadUngappedPositionString(HI::GUITestOpStatus &os);
    static int getReadUngappedPosition(HI::GUITestOpStatus &os);
    static QString getReadUngappedLengthString(HI::GUITestOpStatus &os);
    static int getReadUngappedLength(HI::GUITestOpStatus &os);
    static bool isGapInRead(HI::GUITestOpStatus &os);
};

}   // namespace U2

#endif // _U2_GT_UTILS_MCA_EDITOR_STATUS_WIDGET_H_
