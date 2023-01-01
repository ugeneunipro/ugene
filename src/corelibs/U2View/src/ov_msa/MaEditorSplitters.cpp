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

#include "MaEditorSplitters.h"

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

void MaSplitterUtils::insertWidgetWithScale(QSplitter* splitter, int index, QWidget* newWidget, qreal scale) {
    SAFE_POINT(scale >= 0, "Invalid scale: " + QString::number(scale), );
    int wholeSize = splitter->width();
    QList<int> sizes = splitter->sizes();
    int newWidgetSize = (int)qRound(scale * wholeSize);
    int remainingSize = wholeSize - newWidgetSize;
    for (int i = 0; i < sizes.size(); i++) {
        sizes[i] = (int)qRound(sizes[i] * (qreal)remainingSize / wholeSize);
    }
    splitter->insertWidget(index, newWidget);
    int safeNewWidgetIndex = splitter->indexOf(newWidget);
    sizes.insert(safeNewWidgetIndex, newWidgetSize);
    splitter->setSizes(sizes);
}

void MaSplitterUtils::insertWidgetWithScale(QSplitter* splitter, QWidget* widget, qreal scale, QWidget* insertionPointMarker, int insertionPointMarkerOffset) {
    int index = splitter->indexOf(insertionPointMarker) + insertionPointMarkerOffset;
    insertWidgetWithScale(splitter, index, widget, scale);
}

/** A constant to store the original splitter handle width. */
static const char* CACHED_HANDLE_WIDTH_PROPERTY = "MaSplitterUtils_handle_width";

void MaSplitterUtils::updateFixedSizeHandleStyle(QSplitter* splitter) {
    int resizableWidgetCount = 0;
    for (int i = 0; i < splitter->count(); i++) {
        QWidget* widget = splitter->widget(i);
        if (widget->sizePolicy().verticalPolicy() == QSizePolicy::Fixed) {  // Disable resizing.
            splitter->setStretchFactor(i, 0);
            splitter->handle(i)->setEnabled(false);
        } else {
            resizableWidgetCount++;
        }
    }
    // Hide the handle completely to have 1:1 look with the older versions of UGENE:
    // no handle is visible when there are no resizable widgets (at least 2) in the editor.
    int currentHandleWidth = splitter->handleWidth();
    bool isHandleHidden = resizableWidgetCount < 2;
    if (isHandleHidden) {
        if (currentHandleWidth > 0) {
            splitter->setProperty(CACHED_HANDLE_WIDTH_PROPERTY, currentHandleWidth);
            splitter->setHandleWidth(0);
        }
    } else if (currentHandleWidth == 0) {
        int handleWidth = splitter->property(CACHED_HANDLE_WIDTH_PROPERTY).toInt();
        splitter->setHandleWidth(handleWidth);
    }
}

}  // namespace U2
