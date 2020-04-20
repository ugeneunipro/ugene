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

#include "AssemblyBrowserState.h"
#include "AssemblyBrowser.h"
#include "AssemblyBrowserFactory.h"

namespace U2 {
namespace {
    const QString VIEW_ID   = "view_id";
    const QString OBJ_REF   = "asm_obj_ref";
    const QString REGION    = "asm_region";
    const QString Y_OFFSET  = "asm_y_offset";
}

bool AssemblyBrowserState::isValid() const {
    return stateData.value(VIEW_ID) == AssemblyBrowserFactory::ID;
}

void AssemblyBrowserState::setGObjectRef(const GObjectReference &ref) {
    stateData[OBJ_REF] = QVariant::fromValue(ref);
}

GObjectReference AssemblyBrowserState::getGObjectRef() const {
    return stateData.value(OBJ_REF).value<GObjectReference>();
}

void AssemblyBrowserState::setVisibleBasesRegion(const U2Region &r) {
    stateData[REGION] = QVariant::fromValue(r);
}

U2Region AssemblyBrowserState::getVisibleBasesRegion() const {
    return stateData.value(REGION).value<U2Region>();
}

void AssemblyBrowserState::setYOffset(int y) {
    stateData[Y_OFFSET] = y;
}

int AssemblyBrowserState::getYOffset() const {
    QVariant v = stateData.value(Y_OFFSET);
    if(v.type() == QVariant::Int) {
        return v.toInt();
    }
    return 0;
}

void AssemblyBrowserState::saveState(const AssemblyBrowser *ab) {
    stateData[VIEW_ID] = AssemblyBrowserFactory::ID;
    AssemblyObject * gObj = ab->getAssemblyObject();
    if(gObj != NULL) {
        setGObjectRef(GObjectReference(gObj));
    }

    setVisibleBasesRegion( ab->getVisibleBasesRegion() );
    setYOffset( ab->getYOffsetInAssembly() );
}

void AssemblyBrowserState::restoreState(AssemblyBrowser *ab) const {
    if(NULL != ab->getMainWidget() && ab->getMainWidget()->isCorrectView()) {
        ab->navigateToRegion( getVisibleBasesRegion() );
        ab->setYOffsetInAssembly( getYOffset() );
    }
}

} // namespace
