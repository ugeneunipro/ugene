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

#include "AnnotationModification.h"

namespace U2 {

AnnotationModification::AnnotationModification(AnnotationModificationType type, Annotation *a)
    : type(type), annotation(a)
{

}

QualifierModification::QualifierModification(AnnotationModificationType t, Annotation *a, const U2Qualifier &q)
    : AnnotationModification(t, a)
{
    additionalData = QVariant::fromValue<U2Qualifier>(q);
}

U2Qualifier QualifierModification::getQualifier() const {
    return additionalData.value<U2Qualifier>();
}

AnnotationGroupModification::AnnotationGroupModification(AnnotationModificationType t, Annotation *a, AnnotationGroup *g)
    : AnnotationModification(t, a)
{
    additionalData = QVariant::fromValue<AnnotationGroup *>(g);
}

AnnotationGroup *AnnotationGroupModification::getGroup() const {
    return additionalData.value<AnnotationGroup *>();
}

} // namespace U2
