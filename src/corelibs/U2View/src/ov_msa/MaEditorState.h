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

class MaEditor;

class U2VIEW_EXPORT MaEditorState {
public:
    MaEditorState() = default;

    MaEditorState(const QVariantMap& _stateData)
        : stateData(_stateData) {
    }

    static QVariantMap saveState(MaEditor* v);

    bool isValid() const;

    GObjectReference getMaObjectRef() const;
    void setMaObjectRef(const GObjectReference& ref);

    QFont getFont() const;
    void setFont(const QFont& f);

    int getFirstPos() const;
    void setFirstPos(int y);

    int getFirstSeq() const;
    void setFirstSeq(int seq);

    double getZoomFactor() const;
    void setZoomFactor(double zoomFactor);

    QVariantMap stateData;
};

}  // namespace U2
