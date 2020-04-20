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

#ifndef _U2_SNPEFF_DATABASE_LIST_MODEL_H_
#define _U2_SNPEFF_DATABASE_LIST_MODEL_H_

#include <QAbstractTableModel>

namespace U2 {

class SnpEffDatabaseInfo {
public:
    SnpEffDatabaseInfo() {}
    SnpEffDatabaseInfo(QString line);
    QString getGenome() const { return genome; }
    QString getOrganism() const { return organism; }

private:
    QString genome;
    QString organism;
};

class SnpEffDatabaseListModel : public QAbstractTableModel {
    Q_OBJECT
public:
    SnpEffDatabaseListModel(QObject* parent = 0);
    void getData(const QString& databaseListFilePath);

    QString getGenome(int index) const;
    bool isEmpty() { return databaseCount == 0; }

private:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    int databaseCount;
    QMap<int, SnpEffDatabaseInfo> databaseList;
};

} // namespace U2

#endif // _U2_SNPEFF_DATABASE_LIST_MODEL_H_
