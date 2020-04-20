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

#ifndef _U2_BASE_WIDTH_CONTROLLER_H_
#define _U2_BASE_WIDTH_CONTROLLER_H_

#include <U2Core/U2Region.h>

namespace U2 {

class MaEditor;
class MaEditorWgt;
class U2Region;

class U2VIEW_EXPORT BaseWidthController : public QObject {
    Q_OBJECT
public:
    BaseWidthController(MaEditorWgt *ui);

    int getFirstVisibleBaseGlobalOffset(bool countClipped) const;
    int getFirstVisibleBaseScreenOffset(bool countClipped) const;

    int getBaseGlobalOffset(int position) const;
    int getBaseScreenOffset(int position) const;
    int getBaseScreenCenter(int position) const;

    int getBaseWidth() const;
    int getBasesWidth(int count) const;
    int getBasesWidth(const U2Region &region) const;

    U2Region getBaseGlobalRange(int position) const;
    U2Region getBasesGlobalRange(int startPosition, int count) const;
    U2Region getBasesGlobalRange(const U2Region &region) const;

    U2Region getBaseScreenRange(int position) const;
    U2Region getBasesScreenRange(const U2Region &region) const;

    U2Region getBaseScreenRange(int position, int screenXOrigin) const;
    U2Region getBasesScreenRange(int startPosition, int count, int screenXOrigin) const;
    U2Region getBasesScreenRange(const U2Region &region, int screenXOrigin) const;

    int getTotalAlignmentWidth() const;

    int globalXPositionToColumn(int x) const;       // can be out of MA boundaries
    int screenXPositionToColumn(int x) const;       // can be out of MA boundaries
    int screenXPositionToBase(int x) const;         // returns -1 if the column is out of alignment boundaries

private:
    MaEditor *maEditor;
    MaEditorWgt *ui;
};

}   // namespace U2

#endif // _U2_BASE_WIDTH_CONTROLLER_H_
