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

#include <QPainter>

#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "PrimerGroupBox.h"
#include "PrimerLibrary.h"

#include "PrimerLibraryTable.h"
#include "PrimerStatistics.h"

namespace U2 {

/************************************************************************/
/* PrimerLibraryModel */
/************************************************************************/
PrimerLibraryModel::PrimerLibraryModel(QObject *parent)
: QAbstractItemModel(parent)
{
    U2OpStatus2Log os;
    PrimerLibrary *primerLibrary = PrimerLibrary::getInstance(os);
    SAFE_POINT_OP(os, );
    primers = primerLibrary->getPrimers(os);
}

int PrimerLibraryModel::columnCount(const QModelIndex & /*parent*/) const {
    return 5;
}

QVariant PrimerLibraryModel::data(const QModelIndex &index, int role) const {
    CHECK(index.isValid(), QVariant());
    CHECK(index.row() < rowCount(index.parent()), QVariant());

    if (Qt::DisplayRole == role) {
        return displayData(index);
    }

    return QVariant();
}

QVariant PrimerLibraryModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
    CHECK(Qt::DisplayRole == role, QVariant());

    switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("GC-content (%)");
        case 2:
            return PrimersPairStatistics::TmString;
        case 3:
            return tr("Length (bp)");
        case 4:
            return tr("Sequence");
        default:
            return QVariant();
    }
}

QModelIndex PrimerLibraryModel::index(int row, int column, const QModelIndex & /*parent*/) const {
    CHECK(row < primers.size(), QModelIndex());
    return createIndex(row, column);
}

QModelIndex PrimerLibraryModel::parent(const QModelIndex & /*index*/) const {
    return QModelIndex();
}

int PrimerLibraryModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return primers.size();
}

Primer PrimerLibraryModel::getPrimer(const QModelIndex &index, U2OpStatus &os) const {
    CHECK_EXT(index.row() >= 0 && index.row() < primers.size(), os.setError(L10N::internalError("Incorrect primer number")), Primer());
    return primers.at(index.row());
}

void PrimerLibraryModel::addPrimer(const Primer &primer) {
    beginInsertRows(QModelIndex(), primers.size(), primers.size());
    primers << primer;
    endInsertRows();
}

void PrimerLibraryModel::updatePrimer(const Primer &primer) {
    onPrimerChanged(primer);
}

void PrimerLibraryModel::removePrimer(const QModelIndex &index, U2OpStatus &os) {
    SAFE_POINT_EXT(index.row() >= 0 && index.row() < primers.size(), os.setError(tr("Incorrect primer number")), );
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    primers.removeAt(index.row());
    endRemoveRows();
}

void PrimerLibraryModel::removePrimer(const U2DataId &primerId, U2OpStatus &os) {
    int row = getRow(primerId);
    SAFE_POINT_EXT(row >= 0 && row < primers.size(), os.setError(tr("Incorrect primer number")), );
    beginRemoveRows(QModelIndex(), row, row);
    primers.removeAt(row);
    endRemoveRows();
}

QVariant PrimerLibraryModel::displayData(const QModelIndex &index) const {
    Primer primer = primers[index.row()];
    switch (index.column()) {
        case 0:
            return primer.name;
        case 1:
            return primer.gc != Primer::INVALID_GC ? PrimerStatistics::getDoubleStringValue(primer.gc) : tr("N/A");
        case 2:
            return primer.tm != Primer::INVALID_TM ? PrimerStatistics::getDoubleStringValue(primer.tm) : tr("N/A");
        case 3:
            return primer.sequence.length();
        case 4:
            return primer.sequence;
        default:
            return QVariant();
    }
}

int PrimerLibraryModel::getRow(const U2DataId &primerId) const {
    int row = 0;
    foreach (const Primer &primer, primers) {
        if (primer.id == primerId) {
            return row;
        }
        row++;
    }
    return -1;
}

void PrimerLibraryModel::onPrimerChanged(const Primer &newPrimer) {
    int row = getRow(newPrimer.id);
    CHECK(row >= 0, );

    primers.replace(row, newPrimer);
    QModelIndex leftIndex = index(row, 0, QModelIndex());
    QModelIndex rightIndex = index(row, columnCount(QModelIndex()) - 1, QModelIndex());
    emit dataChanged(leftIndex, rightIndex);
}

/************************************************************************/
/* PrimerLibraryTable */
/************************************************************************/
PrimerLibraryTable::PrimerLibraryTable(QWidget *parent)
: QTableView(parent), mode(Browser)
{
    model = new PrimerLibraryModel(this);
    setModel(model);

    viewport()->installEventFilter(this);
}

QList<Primer> PrimerLibraryTable::getSelection() const {
    QList<Primer> result;

    QModelIndexList selection = selectionModel()->selectedIndexes();
    foreach (const QModelIndex &index, selection) {
        if (index.column() > 0) {
            continue;
        }
        U2OpStatusImpl os;
        Primer primer = model->getPrimer(index, os);
        SAFE_POINT_OP(os, result);
        result << primer;
    }
    return result;
}

void PrimerLibraryTable::addPrimer(const Primer &primer) {
    model->addPrimer(primer);
}

void PrimerLibraryTable::updatePrimer(const Primer &primer) {
    model->updatePrimer(primer);
}

void PrimerLibraryTable::removePrimer(const U2DataId &primerId, U2OpStatus &os) {
    model->removePrimer(primerId, os);
}

bool PrimerLibraryTable::eventFilter(QObject *watched, QEvent *event) {
    CHECK(Selector == mode, false);
    CHECK(event->type() == QEvent::Paint, false);
    CHECK(viewport() == watched, false);
    CHECK(0 == model->rowCount(QModelIndex()), false);

    QPainter p(viewport());
    Qt::Alignment centerAlignment = QStyle::visualAlignment(Qt::LeftToRight, Qt::AlignHCenter) | Qt::AlignTop;
    p.drawText(viewport()->geometry(), centerAlignment, tr("Your primer library is empty.\nUse \"Tools -> Primer -> Primer Library\" for managing the library."));

    return false;
}

void PrimerLibraryTable::setMode(Mode value) {
    mode = value;
}

} // U2
