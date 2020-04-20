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

#include "SnpEffDatabaseListModel.h"

#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include <QFile>

namespace U2 {

SnpEffDatabaseInfo::SnpEffDatabaseInfo(QString line) {
    QStringList info = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    CHECK(info.size() > 2, );
    genome = info.at(0);
    organism = info.at(1);
}

SnpEffDatabaseListModel::SnpEffDatabaseListModel(QObject* parent)
    : QAbstractTableModel(parent) {
    databaseCount = 0;
}

void SnpEffDatabaseListModel::getData(const QString &databaseListFilePath) {
    GTIMER(cvar, tvar, "GetSnpEffDatabaseList");
    GCOUNTER(cv, ct, "GetSnpEffDatabaseList");

    QFile file(databaseListFilePath);
    file.open(QFile::ReadOnly);

    // Skip the first two lines:
    // Genome Organism Status Bundle Database download link
    // ------ -------- ------ ------ ----------------------
    file.readLine();
    file.readLine();

    int counter = 0;
    while (!file.atEnd()) {
        SnpEffDatabaseInfo info(file.readLine());
        databaseList.insert(counter, info);
        counter++;
    }
    file.close();
    databaseCount = counter;
}

QString SnpEffDatabaseListModel::getGenome(int index) const {
    SAFE_POINT(databaseList.contains(index), "Invalid index", QString());
    return databaseList.value(index).getGenome();
}

int SnpEffDatabaseListModel::rowCount(const QModelIndex &) const {
    return databaseCount;
}

int SnpEffDatabaseListModel::columnCount(const QModelIndex &) const {
    return 2;
}

QVariant SnpEffDatabaseListModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        const SnpEffDatabaseInfo& info = databaseList.value(index.row());
        switch (index.column()) {
        case 0:
            return QVariant(info.getGenome());
        case 1:
            return QVariant(info.getOrganism());
        default:
            SAFE_POINT(true, "Invalid state", QVariant());
        }
    }
    return QVariant();
}

QVariant SnpEffDatabaseListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Genome");
        } else {
            return tr("Organism");
        }
    }
    return QVariant();
}

} // namespace U2
