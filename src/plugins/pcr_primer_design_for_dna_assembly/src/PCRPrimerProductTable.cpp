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

#include <U2Core/U2SafePoints.h>

#include "PCRPrimerProductTable.h"

namespace U2 {

const QStringList PCRPrimerProductTable::INDEX_TO_NAME = {
    QObject::tr("A Forward"),
    QObject::tr("A Reverse"),
    QObject::tr("B1 Forward"),
    QObject::tr("B1 Reverse"),
    QObject::tr("B2 Forward"),
    QObject::tr("B2 Reverse"),
    QObject::tr("B3 Forward"),
    QObject::tr("B3 Reverse")
};

 PCRPrimerProductTable::PCRPrimerProductTable(QWidget *parent)
    : QTableWidget(parent) {
     setColumnCount(2);
     setRowCount(8);
     setHorizontalHeaderLabels(QStringList() << tr("Fragment") << tr("Region"));
 }

void PCRPrimerProductTable::setCurrentProducts(const QList<U2Region> &currentProducts) {
    SAFE_POINT(currentProducts.size() == 8, "Should be 8 results", );
    int index = 0;
    int row = 0;
    foreach (const U2Region &region, currentProducts) {
        if (region != U2Region()) {
            setItem(row, 0, new QTableWidgetItem(INDEX_TO_NAME.at(index)));
            setItem(row, 1, new QTableWidgetItem(tr("%1-%2").arg(QString::number(region.startPos)).arg(QString::number(region.endPos()))));
            row++;
        }
        index++;
    }
}

}