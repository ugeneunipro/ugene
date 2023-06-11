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
#include <QPoint>
#include <QTableView>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTTableView {
public:
    static QPoint getCellPosition(QTableView* table, int column, int row);

    static QPoint getCellPoint(QTableView* table, int row, int column);

    static int rowCount(QTableView* table);

    static QString data(QTableView* table, int row, int column);

    static void scrollTo(QTableView* table, const QModelIndex& index);

    // Scrolls to a cell (`row`, `column`) and clicks on it.
    static void click(QTableView* table, int row, int column);
};

}  // namespace HI
