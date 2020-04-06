/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#ifndef _U2_LREGION_SELECTION_H_
#define _U2_LREGION_SELECTION_H_

#include <U2Core/U2Region.h>
#include <U2Core/SelectionModel.h>

namespace U2 {

class U2CORE_EXPORT LRegionsSelection : public GSelection {
    Q_OBJECT
public:
    LRegionsSelection(GSelectionType type, QObject* p = NULL);

    const QVector<U2Region>& getSelectedRegions() const {return regions;}

    void setSelectedRegions(const QVector<U2Region>& newSelection);

    void addRegion(const U2Region& r);

    void removeRegion(const U2Region& r);

    void setRegion(const U2Region& r);

    virtual bool isEmpty() const {return regions.isEmpty();}

    virtual void clear();

    static QVector<U2Region> cropSelection(qint64 sequenceLength, const QVector<U2Region> &regions);

signals:
    void si_selectionChanged(LRegionsSelection* thiz, const QVector<U2Region>& added, const QVector<U2Region>& removed);

public:
    QVector<U2Region> regions;
};

}//namespace

#endif
