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

#include "TreeViewerState.h"

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/PhyTreeObject.h>

#include "TreeViewer.h"
#include "TreeViewerFactory.h"

namespace U2 {

#define VIEW_ID QString("view_id")
#define PHY_OBJ QString("phy_obj_ref")
#define ZOOM_LEVEL QString("zoom_level")

bool TreeViewerState::isValid() const {
    return stateData.value(VIEW_ID) == TreeViewerFactory::ID;
}

GObjectReference TreeViewerState::getPhyObject() const {
    return stateData.contains(PHY_OBJ) ? stateData[PHY_OBJ].value<GObjectReference>() : GObjectReference();
}

double TreeViewerState::getZoomLevel() const {
    QVariant v = stateData.value(ZOOM_LEVEL);
    return v.isValid() ? v.value<double>() : 1.0;
}

QVariantMap TreeViewerState::saveState(TreeViewer* v) {
    TreeViewerState ss;

    ss.stateData[VIEW_ID] = TreeViewerFactory::ID;
    PhyTreeObject* phyObj = v->getPhyObject();
    if (phyObj) {
        ss.stateData[PHY_OBJ] = QVariant::fromValue<GObjectReference>(GObjectReference(phyObj));
    }
    ss.stateData[ZOOM_LEVEL] = v->getZoomLevel();
    ss.stateData.unite(v->getSettingsState());

    return ss.stateData;
}

}  // namespace U2
