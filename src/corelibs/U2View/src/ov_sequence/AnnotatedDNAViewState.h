/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QVariant>

#include <U2Core/GObject.h>
#include <U2Core/U2Region.h>

namespace U2 {

class AnnotatedDNAView;

class U2VIEW_EXPORT AnnotatedDNAViewState {
public:
    AnnotatedDNAViewState();
    AnnotatedDNAViewState(const QVariantMap& _stateData)
        : stateData(_stateData) {
    }

    static QVariantMap saveState(AnnotatedDNAView* v);

    bool isValid() const;

    void setSequenceObjects(const QList<GObjectReference>& objs, const QVector<U2Region>& selections);

    QList<GObjectReference> getSequenceObjects() const;

    QVector<U2Region> getSequenceSelections() const;

    QList<GObjectReference> getAnnotationObjects() const;

    void setAnnotationObjects(const QList<GObjectReference>& objs);

    QVariantMap stateData;
};

}  // namespace U2
