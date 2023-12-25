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

#include <U2Core/U2Region.h>

namespace U2 {

class Annotation;

class PVRowData {
public:
    PVRowData(const QString& key);

    static const QString RESTRICTION_SITE_NAME;

    bool fitToRow(const QVector<U2Region>& locations);

    QString key;
    // invariant: keep the ranges in ascending order
    QVector<U2Region> ranges;
    QList<Annotation*> annotations;
};

class PVRowsManager {
public:
    PVRowsManager();
    ~PVRowsManager();

    void addAnnotation(Annotation* a);
    void removeAnnotation(Annotation* f);

    QList<PVRowData*> getRowsByName(const QString& name) const;
    int getRowCount() const;
    PVRowData* getRow(int row) const;

    /**
     * returns -1 if @a is not found
     */
    int getAnnotationRowIdx(Annotation* a) const;
    PVRowData* getAnnotationRow(Annotation* a) const;

    int getNumAnnotationsInRow(int rowNum) const;

private:
    QList<PVRowData*> rows;
    QHash<Annotation*, PVRowData*> rowByAnnotation;
};

}  // namespace U2
