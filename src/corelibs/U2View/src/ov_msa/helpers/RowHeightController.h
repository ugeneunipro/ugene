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

#ifndef _U2_ROW_HEIGHT_CONTROLLER_H_
#define _U2_ROW_HEIGHT_CONTROLLER_H_

#include <U2Core/U2Region.h>

namespace U2 {

class MaEditorWgt;

class U2VIEW_EXPORT RowHeightController : public QObject{
    Q_OBJECT
public:
    RowHeightController(MaEditorWgt *ui);

    int getGlobalYPositionByMaRowIndex(int maRowIndex) const;
    int getGlobalYPositionByMaRowIndex(int maRowIndex, const QList<int> &maRowIndexes) const;

    int getMaRowIndexByGlobalYPosition(int y) const;
    int getViewRowIndexByGlobalYPosition(int y) const;

    int getViewRowIndexByScreenYPosition(int y) const;

    int getGlobalYPositionOfTheFirstVisibleRow(bool countClipped) const;
    int getScreenYPositionOfTheFirstVisibleRow(bool countClipped) const;

    virtual int getRowHeightByMaIndex(int maRowIndex) const = 0;
    int getRowHeightByViewRowIndex(int viewRowIndex) const;
    int getSumOfRowHeightsByMaIndexes(const QList<int> &maRowIndexes) const;

    U2Region getGlobalYRegionByMaRowIndex(int maRowIndex) const;
    U2Region getGlobalYRegionByMaRowIndex(int maRowIndex, const QList<int> &maRowIndexes) const;
    U2Region getGlobalYRegionByViewRowIndex(int viewRowIndex) const;

    U2Region getGlobalYRegionByViewRowsRegion(const U2Region& viewRowsRegion) const;
    U2Region getScreenYRegionByViewRowsRegion(const U2Region& viewRowsRegion) const;

    U2Region getScreenYRegionByMaRowIndex(int maRowIndex) const;

    U2Region getScreenYRegionByMaRowIndex(int maRowIndex, int screenYOrigin) const;

    U2Region getScreenYRegionByViewRowIndex(int viewRowIndex) const;

    /* Returns sum of heights of the all view rows. */
    int getTotalAlignmentHeight() const;

    /* Returns height of the single row in the alignment. */
    int getSingleRowHeight() const;

protected:
    U2Region mapGlobalToScreen(const U2Region& globalRegion) const;

    MaEditorWgt *ui;
};

}   // namespace U2

#endif // _U2_ROW_HEIGHT_CONTROLLER_H_
