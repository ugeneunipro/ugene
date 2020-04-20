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

#include "TreeViewerState.h"
#include "TreeViewer.h"
#include "TreeViewerFactory.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/DNASequenceSelection.h>

namespace U2 {

#define VIEW_ID     QString("view_id")
#define PHY_OBJ     QString("phy_obj_ref")
#define V_ZOOM      QString("vertical_zoom")
#define H_ZOOM      QString("horizontal_zoom")
#define TRANSFORM   QString("transform")

bool TreeViewerState::isValid() const {
    return stateData.value(VIEW_ID) == TreeViewerFactory::ID;
}

GObjectReference TreeViewerState::getPhyObject() const {
    return stateData.contains(PHY_OBJ) ? stateData[PHY_OBJ].value<GObjectReference>() : GObjectReference();
}

void TreeViewerState::setPhyObject(const GObjectReference& ref) {
    stateData[PHY_OBJ] = QVariant::fromValue<GObjectReference>(ref);
}


qreal TreeViewerState::getVerticalZoom() const {

    QVariant v = stateData.value(V_ZOOM);
    if (v.isValid()) {
        return v.value<qreal>();
    }
    else {
        return 1.0f;
    }
}

qreal TreeViewerState::getHorizontalZoom() const {

    QVariant v = stateData.value(H_ZOOM);
    if (v.isValid()) {
        return v.value<qreal>();
    }
    else {
        return 1.0f;
    }
}

void TreeViewerState::setVerticalZoom(qreal s) {

    stateData[V_ZOOM] = s;
}

void TreeViewerState::setHorizontalZoom(qreal s) {

    stateData[H_ZOOM] = s;
}

QTransform TreeViewerState::getTransform() const {

    QVariant v = stateData.value(TRANSFORM);
    if (v.type() == QVariant::Transform) {
        return v.value<QTransform>();
    }
    QTransform t;
    return t;
}

void TreeViewerState::setTransform(const QTransform& m) {

    stateData[TRANSFORM] = m;
}


QVariantMap TreeViewerState::saveState(TreeViewer* v) {
    TreeViewerState ss;

    ss.stateData[VIEW_ID] = TreeViewerFactory::ID;

    PhyTreeObject* phyObj = v->getPhyObject();
    if (phyObj) {
        ss.setPhyObject(GObjectReference(phyObj));
    }

    ss.setHorizontalZoom(v->getHorizontalZoom());
    ss.setVerticalZoom(v->getVerticalZoom());
    ss.setTransform(v->getTransform());

    ss.stateData.unite(v->getSettingsState());

    return ss.stateData;
}

} // namespace
