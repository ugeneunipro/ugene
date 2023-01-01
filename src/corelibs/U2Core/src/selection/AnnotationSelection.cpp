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

#include "AnnotationSelection.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/SelectionTypes.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

static QList<Annotation*> emptyAnnotations;

AnnotationSelection::AnnotationSelection(QObject* p)
    : GSelection(GSelectionTypes::ANNOTATIONS, p) {
    connect(this, SIGNAL(si_selectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&)), SLOT(sl_selectionChanged()));
}

const QList<Annotation*>& AnnotationSelection::getAnnotations() const {
    return selection;
}

bool AnnotationSelection::isEmpty() const {
    return selection.isEmpty();
}

void AnnotationSelection::clear() {
    if (selection.isEmpty()) {
        return;
    }
    QList<Annotation*> tmpRemoved = selection;
    selection.clear();
    emit si_selectionChanged(this, emptyAnnotations, tmpRemoved);
}

void AnnotationSelection::removeObjectAnnotations(const AnnotationTableObject* obj) {
    QList<Annotation*> removed;
    foreach (Annotation* a, obj->getAnnotations()) {
        for (int i = 0; i < selection.size(); i++) {
            if (selection[i] == a) {
                removed.append(a);
                selection.removeAt(i);
                --i;
            }
        }
    }
    if (!removed.isEmpty()) {
        emit si_selectionChanged(this, emptyAnnotations, removed);
    }
}

void AnnotationSelection::add(Annotation* a) {
    if (selection.contains(a)) {
        return;  // nothing changed
    }
    selection.append(a);
    emit si_selectionChanged(this, QList<Annotation*>() << a, emptyAnnotations);
}

void AnnotationSelection::setAnnotations(QList<Annotation*> annotationList) {
    QList<Annotation*> oldSelection = selection;
    selection = annotationList;
    emit si_selectionChanged(this, annotationList, oldSelection);
}

void AnnotationSelection::remove(Annotation* a) {
    bool removed = false;
    for (int i = 0; i < selection.size(); i++) {
        if (selection[i] == a) {
            selection.removeAt(i);
            removed = true;
            break;
        }
    }
    if (removed) {
        emit si_selectionChanged(this, emptyAnnotations, QList<Annotation*>() << a);
    }
}

QByteArray AnnotationSelection::getSequenceUnderAnnotation(const U2EntityRef& sequenceObjectRef,
                                                           const Annotation* annotation,
                                                           const DNATranslation* complTT,
                                                           const DNATranslation* aminoTT,
                                                           U2OpStatus& os) {
    bool isJoin = annotation->isJoin() || annotation->isBond();
    QList<QByteArray> parts = U2SequenceUtils::extractRegions(sequenceObjectRef, annotation->getRegions(), complTT, aminoTT, isJoin, os);
    CHECK_OP(os, {});
    CHECK(!parts.isEmpty(), {});
    SAFE_POINT(!isJoin || parts.size() == 1, L10N::internalError("Joined annotation should result into a single sequence."), {});
    CHECK(parts.size() != 1, parts[0]);

    quint64 resultBufferLength = parts.size() - 1;
    for (const QByteArray& part : qAsConst(parts)) {
        resultBufferLength += part.length();
    }
    CHECK_EXT(resultBufferLength < INT_MAX, os.setError(tr("Sequence is too long.")), {});

    QByteArray result;
    result.reserve((int)resultBufferLength);
    CHECK_EXT(result.capacity() == (int)resultBufferLength, os.setError(tr("Can't reserve enough space for the result sequence.")), {});
    result.append(parts[0]);
    for (int i = 1; i < parts.size(); i++) {
        result.append('-');
        result.append(parts[i]);
    }
    return result;
}

bool AnnotationSelection::contains(Annotation* a) const {
    return selection.contains(a);
}

//////////////////////////////////////////////////////////////////////////
// Annotation group selection
static QList<AnnotationGroup*> emptyGroups;

AnnotationGroupSelection::AnnotationGroupSelection(QObject* p)
    : GSelection(GSelectionTypes::ANNOTATION_GROUPS, p) {
    connect(this, SIGNAL(si_selectionChanged(AnnotationGroupSelection*, const QList<AnnotationGroup*>&, const QList<AnnotationGroup*>&)), SLOT(sl_selectionChanged()));
}

const QList<AnnotationGroup*>& AnnotationGroupSelection::getSelection() const {
    return selection;
}

bool AnnotationGroupSelection::isEmpty() const {
    return selection.isEmpty();
}

bool AnnotationGroupSelection::contains(AnnotationGroup* g) const {
    return selection.contains(g);
}

void AnnotationGroupSelection::clear() {
    if (selection.isEmpty()) {
        return;
    }
    QList<AnnotationGroup*> tmpRemoved = selection;
    selection.clear();
    emit si_selectionChanged(this, emptyGroups, tmpRemoved);
}

void AnnotationGroupSelection::addToSelection(AnnotationGroup* g) {
    if (selection.contains(g)) {
        return;
    }
    selection.append(g);
    QList<AnnotationGroup*> tmpAdded;
    tmpAdded.append(g);
    emit si_selectionChanged(this, tmpAdded, emptyGroups);
}

void AnnotationGroupSelection::removeFromSelection(AnnotationGroup* g) {
    const bool found = selection.removeOne(g);
    if (found) {
        QList<AnnotationGroup*> tmpRemoved;
        tmpRemoved.append(g);
        emit si_selectionChanged(this, emptyGroups, tmpRemoved);
    }
}

}  // namespace U2
