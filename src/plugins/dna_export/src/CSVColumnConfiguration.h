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

#ifndef _U2_CSV_COLUMN_CONFIGURATION_H_
#define _U2_CSV_COLUMN_CONFIGURATION_H_

#include <QString>

namespace U2 {

enum ColumnRole {
    ColumnRole_Ignore,      // ignore this column
    ColumnRole_Qualifier,   // column will be mapped to a qualifier
    ColumnRole_Name,        // name (or key) of the annotation
    ColumnRole_StartPos,    // column will be mapped as start position
    ColumnRole_EndPos,      // column will be mapped as end position
    ColumnRole_Length,      // column will be mapped as length
    ColumnRole_ComplMark,   // column is a complement strand indicator
    ColumnRole_Group        // group of the annotation
};

class ColumnConfig {
public:
    ColumnConfig() : role (ColumnRole_Ignore), startPositionOffset(0), endPositionIsInclusive(false) {}
    ColumnRole  role;
    QString     qualifierName;
    QString     complementMark;
    int         startPositionOffset;
    bool        endPositionIsInclusive;

    void reset() {
        role = ColumnRole_Ignore;
        qualifierName.clear();
        complementMark.clear();
        startPositionOffset = 0;
        endPositionIsInclusive = false;
    }

    //TODO: support groups?
};

}//namespace

#endif
