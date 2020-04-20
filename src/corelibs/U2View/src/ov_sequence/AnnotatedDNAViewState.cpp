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

#include "ADVSequenceObjectContext.h"
#include "AnnotatedDNAView.h"
#include "AnnotatedDNAViewFactory.h"
#include "AnnotatedDNAViewState.h"

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/AnnotationTableObject.h>

namespace U2 {

#define VIEW_ID                 QString("view_id")
#define SEQUENCE_OBJECTS        QString("dna_obj_ref")
#define ANNOTATION_OBJECTS      QString("ann_obj_ref")
#define SEQUENCE_SELECTION      QString("dna_obj_sel")

AnnotatedDNAViewState::AnnotatedDNAViewState() {

}

bool AnnotatedDNAViewState::isValid() const {
    bool idOK = stateData.value(VIEW_ID) == AnnotatedDNAViewFactory::ID;
    return idOK && !getSequenceObjects().isEmpty();
}

QList<GObjectReference> AnnotatedDNAViewState::getSequenceObjects() const {
    QList<GObjectReference> res = stateData.value(SEQUENCE_OBJECTS).value<QList<GObjectReference> >();
    return res;
}

void AnnotatedDNAViewState::setSequenceObjects(const QList<GObjectReference>& objs, const QVector<U2Region>& selections) {
    assert(objs.size() == selections.size());
    stateData[SEQUENCE_OBJECTS] = QVariant::fromValue<QList<GObjectReference> >(objs);
    stateData[SEQUENCE_SELECTION]= QVariant::fromValue<QVector<U2Region> >(selections);
}


QVector<U2Region> AnnotatedDNAViewState::getSequenceSelections() const {
    QVector<U2Region> res = stateData.value(SEQUENCE_SELECTION).value<QVector<U2Region> >();
    return res;
}

QList<GObjectReference> AnnotatedDNAViewState::getAnnotationObjects() const {
    QList<GObjectReference> res = stateData.value(ANNOTATION_OBJECTS).value<QList<GObjectReference> >();
    return res;
}

void AnnotatedDNAViewState::setAnnotationObjects(const QList<GObjectReference>& objs) {
    stateData[ANNOTATION_OBJECTS] = QVariant::fromValue<QList<GObjectReference> >(objs);
}

QVariantMap AnnotatedDNAViewState::saveState(AnnotatedDNAView* v) {
    AnnotatedDNAViewState s;
    s.stateData[VIEW_ID] = AnnotatedDNAViewFactory::ID;
    QList<GObjectReference> seqRefs;
    QVector<U2Region> seqSels;
    foreach(const ADVSequenceObjectContext * ctx, v->getSequenceContexts()) {
        seqRefs.append(ctx->getSequenceObject());
        DNASequenceSelection* sel = ctx->getSequenceSelection();
        seqSels.append(sel->isEmpty()? U2Region() : sel->getSelectedRegions().first());
    }

    QList<GObjectReference> anRefs;
    foreach(GObject* ao, v->getAnnotationObjects()) {
        anRefs.append(ao);
    }
    s.setSequenceObjects(seqRefs, seqSels);
    s.setAnnotationObjects(anRefs);

    return s.stateData;
}


} // namespace

