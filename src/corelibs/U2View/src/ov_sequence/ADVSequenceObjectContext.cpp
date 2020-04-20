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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2View/CodonTable.h>

namespace U2 {

ADVSequenceObjectContext::ADVSequenceObjectContext(AnnotatedDNAView* v, U2SequenceObject* obj)
    : SequenceObjectContext(obj, v),
      view(v) {
    if (v != NULL && translations != NULL) {
        const CodonTableView *ct = v->getCodonTableView();
        foreach(QAction* a, translations->actions()) {
            connect(a, SIGNAL(triggered()), ct, SLOT(sl_setAminoTranslation()));
        }
    }
}

AnnotationSelection* ADVSequenceObjectContext::getAnnotationsSelection() const {
    return view->getAnnotationsSelection();
}

void ADVSequenceObjectContext::sl_onAnnotationRelationChange() {
    AnnotationTableObject* obj = qobject_cast<AnnotationTableObject*>(sender());
    SAFE_POINT(obj != NULL, tr("Incorrect signal sender!"), );

    if (!obj->hasObjectRelation(seqObj, ObjectRole_Sequence)) {
        disconnect(obj, SIGNAL(si_relationChanged()), this, SLOT(sl_onAnnotationRelationChange()));
        view->removeObject(obj);
    }
}

} // namespace U2
