/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MA_EDITOR_SELECTION_
#define _U2_MA_EDITOR_SELECTION_

#include <QRect>

#include <U2Core/U2Region.h>

namespace U2 {

/** Data model for selection in MSA editor: list of visual non-overlapping rectangles. */
class U2VIEW_EXPORT MaEditorSelection {
public:
    /** Creates a new empty MSA editor selection first and calls addRect() for every rect in the list. */
    explicit MaEditorSelection(const QList<QRect> &rectList = QList<QRect>());

    /**
     * Adds rect to the selection. Does not change the selection if the new rect area is already in the selection.
     * Returns 'true' if selection was changed.
     *
     * Only rect with width and height > 0 can be added to the selection.
     */
    bool addRect(const QRect &rect);

    /* Returns true if the selection contains no rects. */
    bool isEmpty() const;

    /**
     * Returns selection state as a rect.
     * The returned rect is a bounding rect for all rects in the 'rectList'.
     * If 'rectList' is empty the method returns an empty (0, 0, 0, 0) rect.
     */
    QRect toRect() const;

    /** Returns list of selected rects. */
    const QList<QRect> &getRectList() const;

    /** Compares 2 selection. Two selections are equal if they have equal list of rects with the same order. */
    bool operator==(const MaEditorSelection &other) const;

private:
    /**
     * Unsorted list of visual non-overlapping on-screen rectangles.
     * The list may contain only valid rectangles with x>=0, y>=0, height>0 and width>=0.
     * Width equal to 0 is allowed by historical reasons (see MCA editor) and is processed as a 'whole row' selection.
     */
    QList<QRect> rectList;
};

}    // namespace U2

#endif    // _U2_MA_EDITOR_SELECTION_
