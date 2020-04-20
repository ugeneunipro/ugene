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

#include "LRegionsSelection.h"

namespace U2 {

static QVector<U2Region> emptyLRegions;

LRegionsSelection::LRegionsSelection(GSelectionType type, QObject *p) : GSelection(type, p)
{
    connect(this, SIGNAL(si_selectionChanged(LRegionsSelection*,QVector<U2Region>,QVector<U2Region>)), SLOT(sl_selectionChanged()));
}

void LRegionsSelection::clear() {
    QVector<U2Region> tmpRemoved = regions;
    regions.clear();
    if (!tmpRemoved.isEmpty()) {
        emit si_selectionChanged(this, emptyLRegions, tmpRemoved);
    }
}

QVector<U2Region> LRegionsSelection::cropSelection(qint64 sequenceLength, const QVector<U2Region> &regions) {
    QVector<U2Region> result;
    foreach (const U2Region &region, regions) {
        if (region.endPos() < sequenceLength) {
            result << region;
        } else if (region.startPos < sequenceLength) {
            result << U2Region(region.startPos, sequenceLength - region.startPos);
        }
    }
    return result;
}

void LRegionsSelection::addRegion(const U2Region& r) {
    assert(r.length > 0);
    if (regions.contains(r)) {
        return;
    }
    regions.append(r);
    QVector<U2Region> tmpAdded; tmpAdded.append(r);
    emit si_selectionChanged(this, tmpAdded, emptyLRegions);
}

void LRegionsSelection::removeRegion(const U2Region& r) {
    int idx = regions.indexOf(r);
    if (idx == -1) {
        return;
    }
    regions.remove(idx);
    QVector<U2Region> tmpRemoved; tmpRemoved.append(r);
    emit si_selectionChanged(this, emptyLRegions, tmpRemoved);
}

void LRegionsSelection::setRegion(const U2Region& r) {
    if (r.length == 0)
    {
        clear();
        return;
    }
    QVector<U2Region> newRegions;
    newRegions << r;
    setSelectedRegions(newRegions);
}

void LRegionsSelection::setSelectedRegions(const QVector<U2Region>& newSelection) {
    QVector<U2Region> tmpRemoved = regions;
    regions = newSelection;
    emit si_selectionChanged(this, newSelection, tmpRemoved);
}

} //namespace
