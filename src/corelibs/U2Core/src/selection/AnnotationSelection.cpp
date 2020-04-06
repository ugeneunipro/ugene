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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/SelectionTypes.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SequenceUtils.h>

#include "AnnotationSelection.h"

namespace U2 {

static QList<Annotation *> emptyAnnotations;

AnnotationSelectionData::AnnotationSelectionData(Annotation *a, int lIdx)
    : annotation(a) {
    if (lIdx != -1) {
        locationIdxList.append(lIdx);
    } else {
        for (int i = 0; i < a->getRegions().size(); i++) {
            locationIdxList.append(i);
        }
    }
}

AnnotationSelectionData::AnnotationSelectionData(Annotation *a, const QList<int> &listIdx)
    : annotation(a),
    locationIdxList(listIdx) {
    qSort(locationIdxList);
}

bool AnnotationSelectionData::operator ==(const AnnotationSelectionData &d) const {
    return d.annotation == annotation && d.locationIdxList == locationIdxList;
}

QVector<U2Region> AnnotationSelectionData::getSelectedRegions() const {
    QVector<U2Region> regions = annotation->getRegions();
    if (locationIdxList.size() == regions.size()) {
        return regions;
    }

    QVector<U2Region> res;
    foreach(int idx, locationIdxList) {
        SAFE_POINT(0 <= idx && idx < regions.size(), "Invalid location index", QVector<U2Region>());
        res << regions[idx];
    }
    return res;
}

bool AnnotationSelectionData::contains(int locIndx) const {
    int regSize = annotation->getRegions().size();
    SAFE_POINT(-1 <= locIndx && locIndx < regSize, "Invalid location index", false);
    if (locationIdxList.size() == regSize) {
        return locIndx < regSize;
    }
    return locationIdxList.contains(locIndx);
}

bool AnnotationSelectionData::contains(const AnnotationSelectionData &other) const {
    if (annotation != other.annotation) {
        return false;
    }
    bool contains = true;
    foreach(int loc, other.locationIdxList) {
        if (!locationIdxList.contains(loc)) {
            return false;
        }
    }
    return contains;
}

bool AnnotationSelectionData::deselectLocation(int locIndx) {
    SAFE_POINT(-1 <= locIndx && locIndx < annotation->getRegions().size(), "Invalid location index", false);
    if (locIndx == -1) {
        locationIdxList.clear();
        return true;
    }
    bool ok = locationIdxList.removeOne(locIndx);
    CHECK(ok, false);
    return locationIdxList.isEmpty();
}

void AnnotationSelectionData::addLocation(int locIdx) {
    SAFE_POINT(-1 <= locIdx && locIdx < annotation->getRegions().size(), "Invalid location index", );
    if (locationIdxList.contains(locIdx)) {
        return;
    }
    if (locIdx == -1) {
        locationIdxList.clear();
        for (int i = 0; i < annotation->getRegions().size(); i++) {
            locationIdxList.append(i);
        }
    } else {
        locationIdxList.append(locIdx);
        qSort(locationIdxList);
    }
}

int AnnotationSelectionData::getSelectedRegionsLen() const {
    int len = 0;
    QVector<U2Region> regions = annotation->getRegions();
    foreach(int idx, locationIdxList) {
        SAFE_POINT(0 <= idx && idx < regions.size(), "Invalid location index", 0);
        len += regions[idx].length;
    }

    return len;
}

AnnotationSelection::AnnotationSelection(QObject *p) : GSelection(GSelectionTypes::ANNOTATIONS, p) {
    connect(this, SIGNAL(si_selectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)), SLOT(sl_selectionChanged()));
}

const QList<AnnotationSelectionData> & AnnotationSelection::getSelection() const {
    return selection;
}

bool AnnotationSelection::isEmpty() const {
    return selection.isEmpty();
}

void AnnotationSelection::clear() {
    if (selection.isEmpty()) {
        return;
    }
    QList<Annotation *> tmpRemoved;
    foreach(const AnnotationSelectionData &asd, selection) {
        tmpRemoved.push_back(asd.annotation);
    }
    selection.clear();
    emit si_selectionChanged(this, emptyAnnotations, tmpRemoved);
}

bool AnnotationSelection::contains(Annotation *a) const {
    return NULL != getAnnotationData(a);
}

void AnnotationSelection::removeObjectAnnotations(AnnotationTableObject *obj) {
    QList<Annotation *> removed;
    foreach(Annotation *a, obj->getAnnotations()) {
        for (int i = 0; i < selection.size(); i++) {
            if (selection[i].annotation == a) {
                removed.append(a);
                selection.removeAt(i);
                --i;
            }
        }
    }
    emit si_selectionChanged(this, emptyAnnotations, removed);
}

void AnnotationSelection::addToSelection(Annotation *a, int locationIdx) {
    const int nRegionsTotal = a->getRegions().size();
    SAFE_POINT(locationIdx >= -1 && locationIdx < nRegionsTotal, "Invalid location index!", );

    for (int i = 0; i < selection.size(); i++) {
        AnnotationSelectionData &asd = selection[i];
        if (asd.annotation == a) {
            if (asd.contains(locationIdx) || (locationIdx == -1 && asd.locationIdxList.size() == nRegionsTotal)) {
                return; //nothing changed
            }

            asd.addLocation(locationIdx);
            QList<Annotation *> tmp;
            tmp.append(a);
            emit si_selectionChanged(this, tmp, emptyAnnotations);
            return;
        }
    }

    selection.append(AnnotationSelectionData(a, locationIdx));
    QList<Annotation *> tmp;
    tmp.append(a);
    emit si_selectionChanged(this, tmp, emptyAnnotations);
}

void AnnotationSelection::removeFromSelection(Annotation *a, int locationIdx) {
    for (int i = 0; i < selection.size(); i++) {
        AnnotationSelectionData& asd = selection[i];
        if (asd.annotation == a) {
            if (!asd.contains(locationIdx)) {
                return; // nothing changed
            }
            bool empty = asd.deselectLocation(locationIdx);
            if (empty) {
                selection.removeOne(asd);
                break;
            }
        }
    }
    QList<Annotation *> tmp;
    tmp.append(a);
    emit si_selectionChanged(this, emptyAnnotations, tmp);
}

const AnnotationSelectionData * AnnotationSelection::getAnnotationData(Annotation *a) const {
    foreach(const AnnotationSelectionData &asd, selection) {
        if (asd.annotation == a) {
            return &asd;
        }
    }
    return NULL;
}

void AnnotationSelection::getAnnotationSequence(QByteArray &res, const AnnotationSelectionData &ad, char gapSym, const U2EntityRef &seqRef,
    const DNATranslation *complTT, const DNATranslation *aminoTT, U2OpStatus &os) {
    QVector<U2Region> regions = ad.getSelectedRegions();
    QList<QByteArray> parts = U2SequenceUtils::extractRegions(seqRef, regions, complTT, aminoTT, false, os);
    CHECK_OP(os, );
    qint64 resLen = 0;
    foreach(const QByteArray &p, parts) {
        resLen += p.length();
    }
    res.reserve(resLen);
    foreach(const QByteArray &p, parts) {
        if (!res.isEmpty()) {
            res.append(gapSym);
        }
        res.append(p);
    }
}

bool AnnotationSelection::contains(Annotation *a, int locationIdx) const {
    foreach(const AnnotationSelectionData &asd, selection) {
        if (asd.annotation == a && asd.contains(locationIdx)) {
            return true;
        }
    }
    return false;
}

bool AnnotationSelection::contains(const AnnotationSelectionData &selData) const {
    foreach(const AnnotationSelectionData& asd, selection) {
        if (asd.contains(selData)) {
            return true;
        }
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////
// Annotation group selection
static QList<AnnotationGroup *> emptyGroups;

AnnotationGroupSelection::AnnotationGroupSelection(QObject *p)
    : GSelection(GSelectionTypes::ANNOTATION_GROUPS, p) {
    connect(this, SIGNAL(si_selectionChanged(AnnotationGroupSelection *, const QList<AnnotationGroup *> &, const QList<AnnotationGroup *> &)),
        SLOT(sl_selectionChanged()));
}

const QList<AnnotationGroup *> & AnnotationGroupSelection::getSelection() const {
    return selection;
}

bool AnnotationGroupSelection::isEmpty() const {
    return selection.isEmpty();
}

bool AnnotationGroupSelection::contains(AnnotationGroup *g) const {
    return selection.contains(g);
}

void AnnotationGroupSelection::clear() {
    if (selection.isEmpty()) {
        return;
    }
    QList<AnnotationGroup *> tmpRemoved = selection;
    selection.clear();
    emit si_selectionChanged(this, emptyGroups, tmpRemoved);
}

void AnnotationGroupSelection::removeObjectGroups(AnnotationTableObject *obj) {
    QList<AnnotationGroup *> tmpRemoved;
    foreach(AnnotationGroup *g, selection) {
        if (g->getGObject() == obj) {
            tmpRemoved.append(g);
        }
    }
    foreach(AnnotationGroup *g, tmpRemoved) {
        selection.removeOne(g);
    }
    emit si_selectionChanged(this, emptyGroups, tmpRemoved);
}

void AnnotationGroupSelection::addToSelection(AnnotationGroup *g) {
    if (selection.contains(g)) {
        return;
    }
    selection.append(g);
    QList<AnnotationGroup *> tmpAdded;
    tmpAdded.append(g);
    emit si_selectionChanged(this, tmpAdded, emptyGroups);
}


void AnnotationGroupSelection::removeFromSelection(AnnotationGroup *g) {
    const bool found = selection.removeOne(g);
    if (found) {
        QList<AnnotationGroup *> tmpRemoved;
        tmpRemoved.append(g);
        emit si_selectionChanged(this, emptyGroups, tmpRemoved);
    }
}

// Returns list of locations of all selected annotations
QVector<U2Region> AnnotationSelection::getSelectedLocations(const QSet<AnnotationTableObject *> &objects) const {
    QVector<U2Region> result;
    if (objects.isEmpty()) {
        return result;
    }
    foreach(const AnnotationSelectionData &d, selection) {
        if (objects.contains(d.annotation->getGObject())) {
            result << d.getSelectedRegions();
        }
    }
    return result;
}
}//namespace
