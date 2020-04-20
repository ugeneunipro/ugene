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

#ifndef _U2_DRAW_HELPER_H_
#define _U2_DRAW_HELPER_H_

#include <U2Core/U2Region.h>

namespace U2 {

class MaEditorWgt;
class MaEditorSelection;
class MaCollapseModel;
class ScrollController;

class U2VIEW_EXPORT DrawHelper {
public:
    DrawHelper(MaEditorWgt *maEditorWgt);

    U2Region getVisibleBases(int widgetWidth, bool countFirstClippedBase = true, bool countLastClippedBase = true) const;
    U2Region getVisibleViewRowsRegion(int widgetHeight, bool countFirstClippedRow = true, bool countLastClippedRow = true) const;
    QList<int> getVisibleMaRowIndexes(int widgetHeight, bool countFirstClippedRow = true, bool countLastClippedRow = true) const;

    int getVisibleBasesCount(int widgetWidth, bool countFirstClippedBase = true, bool countLastClippedBase = true) const;

    QRect getSelectionScreenRect(const MaEditorSelection &selection) const;

private:
    MaEditorWgt *ui;
    ScrollController *scrollController;
    MaCollapseModel *collapsibleModel;
};

}   // namespace U2

#endif // _U2_DRAW_HELPER_H_
