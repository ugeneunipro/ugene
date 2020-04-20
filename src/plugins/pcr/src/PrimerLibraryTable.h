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

#ifndef _U2_PRIMER_LIBRARY_TABLE_H_
#define _U2_PRIMER_LIBRARY_TABLE_H_

#include <QAbstractItemModel>
#include <QTableView>

#include <U2Core/U2OpStatus.h>

#include "Primer.h"

namespace U2 {

class PrimerLibraryModel : public QAbstractItemModel {
    Q_OBJECT
public:
    PrimerLibraryModel(QObject *parent);

    /* QAbstractItemModel */
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;

    Primer getPrimer(const QModelIndex &index, U2OpStatus &os) const;
    void addPrimer(const Primer &primer);
    void updatePrimer(const Primer &primer);
    void removePrimer(const QModelIndex &index, U2OpStatus &os);
    void removePrimer(const U2DataId &primerId, U2OpStatus &os);

private:
    QVariant displayData(const QModelIndex &index) const;
    int getRow(const U2DataId &primerId) const;
    void onPrimerChanged(const Primer &primer);

private:
    QList<Primer> primers;
};

class PrimerLibraryTable : public QTableView {
    Q_OBJECT
public:
    PrimerLibraryTable(QWidget *parent);

    QList<Primer> getSelection() const;
    void addPrimer(const Primer &primer);
    void updatePrimer(const Primer &primer);
    void removePrimer(const U2DataId &primerId, U2OpStatus &os);

    bool eventFilter(QObject *watched, QEvent *event);

    enum Mode {Browser, Selector};
    void setMode(Mode value);

private:
    Mode mode;
    PrimerLibraryModel *model;
};

} // U2

#endif // _U2_PRIMER_LIBRARY_TABLE_H_
