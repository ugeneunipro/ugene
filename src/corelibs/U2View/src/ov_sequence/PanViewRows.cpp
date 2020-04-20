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

#include <QVarLengthArray>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/U2SafePoints.h>

#include "PanViewRows.h"

namespace U2 {

PVRowsManager::~PVRowsManager() {
    qDeleteAll(rows);
}

typedef QVector<U2Region>::const_iterator LRIter;

PVRowData::PVRowData(const QString &key)
    : key(key)
{

}

bool PVRowData::fitToRow(const QVector<U2Region> &location) {
    //assume locations are always in ascending order
    //usually annotations come in sorted by location
    //first check the most frequent way
    if (!ranges.isEmpty()) {
        const U2Region &l = location.first();
        const U2Region &r = ranges.last();
        if (l.startPos > r.endPos()) {
            ranges << location;
            return true;
        } else if (l.startPos >= r.startPos || l.endPos() >= r.startPos) {
            //got intersection
            return false;
        }
    }
    //bad luck, full search required
    QVarLengthArray<int, 16> pos;
    LRIter zero = ranges.constBegin();
    LRIter end = ranges.constEnd();
    foreach (const U2Region &l, location) {
        LRIter it = std::lower_bound(zero, end, l);
        if (it != end && (it->startPos <= l.endPos() || (it != zero && (it - 1)->endPos() >= l.startPos))) {
            //got intersection
            return false;
        }
        pos.append(it - zero);
    }
    //ok this feature can be added to row;
    //keep the ranges in ascending order
    for (int i = location.size() - 1; i >= 0; i--) {
        ranges.insert(pos[i], location.at(i));
    }

    return true;
}

inline bool compare_rows(PVRowData *x, PVRowData *y) {
    return x->key.compare(y->key) > 0;
}

PVRowsManager::PVRowsManager() {

}

void PVRowsManager::addAnnotation(Annotation *a) {
    SAFE_POINT(!rowByAnnotation.contains(a), "Annotation has been already added",);
    const SharedAnnotationData &data = a->getData();
    const QVector<U2Region> location = data->getRegions();

    QString name;
    bool isRestrictionSite = data->type == U2FeatureTypes::RestrictionSite;
    if (isRestrictionSite) {
        name = QObject::tr("Restriction Site");
    } else {
        name = data->name;
    }

    if (rowByName.contains(name)) {
        foreach (PVRowData *row, rowByName[name]) {
            if (row->fitToRow(location)) {
                row->annotations.append(a);
                rowByAnnotation[a] = row;
                if (name != data->name) {
                    rowByName[data->name].append(row);
                }
                return;
            }
        }
    }

    PVRowData *row = new PVRowData(name);

    row->ranges << location;
    row->annotations.append(a);
    rowByAnnotation[a] = row;

    QList<PVRowData *>::iterator i = std::upper_bound(rows.begin(), rows.end(), row, compare_rows);
    rows.insert(i, row);
    rowByName[name].append(row);
    if (name != data->name) {
        rowByName[data->name].append(row);
    }
}

namespace {

void substractRegions(QVector<U2Region> &regionsToProcess, const QVector<U2Region> &regionsToRemove) {
    QVector<U2Region> result;
    foreach (const U2Region &pr, regionsToProcess) {
        if (!regionsToRemove.contains(pr)) {
            result.append(pr);
        }
    }
    regionsToProcess = result;
}

}

void PVRowsManager::removeAnnotation(Annotation *a) {
    PVRowData *row = rowByAnnotation.value(a, NULL);
    CHECK(NULL != row,); // annotation may present in a DB, but has not been added to the panview yet
    rowByAnnotation.remove(a);
    rowByName.remove(a->getName());
    row->annotations.removeOne(a);
    substractRegions(row->ranges, a->getRegions());
    if (row->annotations.isEmpty()) {
        rows.removeOne(row);
        QList<PVRowData *> &rowsWithSameName = rowByName[row->key];
        rowsWithSameName.removeOne(row);
        if (rowsWithSameName.isEmpty()) {
            rowByName.remove(row->key);
        }
        delete row;
    }
}

int PVRowsManager::getAnnotationRowIdx(Annotation *a) const {
    PVRowData *row = rowByAnnotation.value(a, NULL);
    if (NULL == row) {
        return -1;
    } else {
        return rows.indexOf(row);
    }
}

int PVRowsManager::getNumAnnotationsInRow(int rowNum) const {
    SAFE_POINT(rowNum >= 0 && rowNum < rows.size(), "Row number out of range", 0);
    PVRowData *r = rows[rowNum];
    return r->annotations.size();
}

int PVRowsManager::getNumRows() const {
    return rows.size();
}

bool PVRowsManager::contains(const QString &key) const {
    return rowByName.contains(key);
}

PVRowData * PVRowsManager::getAnnotationRow(Annotation *a) const {
    return rowByAnnotation.value(a, NULL);
}

PVRowData * PVRowsManager::getRow(int row) const {
    if (row >= 0 && row < rows.size()) {
        return rows.at(row);
    }
    return NULL;
}

} // namespace
