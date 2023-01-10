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

#ifndef _U2_MA_EDITOR_SPLITTERS_H_
#define _U2_MA_EDITOR_SPLITTERS_H_

#include <QLabel>
#include <QSplitter>
#include <QWidget>

#include <U2Core/global.h>

namespace U2 {

/** Set of stateless QSplitter related utils used by MA Editor. */
class U2VIEW_EXPORT MaSplitterUtils {
public:
    /**
     * Inserts the widget with the given 'scale' into 'index' position.
     * 'Scale' is a share of the splitter size the widget will occupy after insertion.
     * All other widgets are reduced proportionally their current size.
     */
    static void insertWidgetWithScale(QSplitter* splitter, int index, QWidget* newWidget, qreal scale);

    /** Calls insertWidgetWithScale(index, ...) with 'index' equal to 'insertionPointMarker' index plus 'insertionPointMarkerOffset'. */
    static void insertWidgetWithScale(QSplitter* splitter, QWidget* widget, qreal scale, QWidget* insertionPointMarker, int insertionPointMarkerOffset = 0);

    /** Makes handles of fixed height widgets disabled & invisible. */
    static void updateFixedSizeHandleStyle(QSplitter* splitter);
};

}  // namespace U2
#endif  // _U2_MA_EDITOR_SPLITTERS_H_
