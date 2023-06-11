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

class QWidget;

namespace HI {
class GUITestOpStatus;
}

namespace U2 {

class GTUtilsMcaEditorStatusWidget {
public:
    static QWidget* getStatusWidget();

    static QString getRowNumberString();
    static int getRowNumber();
    static QString getRowsCountString();
    static int getRowsCount();

    static QString getReferenceUngappedPositionString();
    static int getReferenceUngappedPosition();
    static QString getReferenceUngappedLengthString();
    static int getReferenceUngappedLength();
    static bool isGapInReference();

    static QString getReadUngappedPositionString();
    static int getReadUngappedPosition();
    static QString getReadUngappedLengthString();
    static int getReadUngappedLength();
    static bool isGapInRead();
};

}  // namespace U2
