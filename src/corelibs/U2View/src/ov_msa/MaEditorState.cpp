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

#include "MaEditorState.h"
#include "MSAEditor.h"
#include "MaEditorFactory.h"
#include "helpers/ScrollController.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/DNASequenceSelection.h>

namespace U2 {

#define VIEW_ID     QString("view_id")
#define MA_OBJ      QString("ma_obj_ref")
#define FONT        QString("font")
#define FIRST_POS   QString("first_pos")
#define FIRST_SEQ   QString("first_seq")
#define ZOOM_FACTOR QString("zoom_factor")

bool MaEditorState::isValid() const {
    return stateData.value(VIEW_ID) == MsaEditorFactory::ID || stateData.value(VIEW_ID) == McaEditorFactory::ID;
}

GObjectReference MaEditorState::getMaObjectRef() const {
    return stateData.contains(MA_OBJ) ? stateData[MA_OBJ].value<GObjectReference>() : GObjectReference();
}

void MaEditorState::setMaObjectRef(const GObjectReference& ref) {
    stateData[MA_OBJ] = QVariant::fromValue<GObjectReference>(ref);
}

QFont MaEditorState::getFont() const {
    QVariant v = stateData.value(FONT);
    if (v.type() == QVariant::Font) {
        return v.value<QFont>();
    }
    return QFont();
}

void MaEditorState::setFont(const QFont &f) {

    stateData[FONT] = f;
}

int MaEditorState::getFirstPos() const {
    QVariant v = stateData.value(FIRST_POS);
    if (v.type() == QVariant::Int) {
        return v.toInt();
    }
    return -1;
}

void MaEditorState::setFirstPos(int y) {
    stateData[FIRST_POS] = y;
}

int MaEditorState::getFirstSeq() const {
    QVariant v = stateData.value(FIRST_SEQ);
    if (v.type() == QVariant::Int) {
        return v.toInt();
    }
    return -1;
}

void MaEditorState::setFirstSeq(int seq) {
    stateData[FIRST_SEQ] = seq;
}

double MaEditorState::getZoomFactor() const {
    QVariant v = stateData.value(ZOOM_FACTOR);
    if (v.type() == QVariant::Double) {
        return v.toDouble();
    }
    return 1.0;
}

void MaEditorState::setZoomFactor(double zoomFactor) {
    stateData[ZOOM_FACTOR] = zoomFactor;
}

QVariantMap MaEditorState::saveState(MaEditor* v) {
    MaEditorState ss;
    ss.stateData[VIEW_ID]= v->getFactoryId();

    MultipleAlignmentObject* maObj = v->getMaObject();
    if (maObj) {
        ss.setMaObjectRef(GObjectReference(maObj));
    }

    MaEditorWgt* wgt = v->getUI();
    SAFE_POINT(wgt != NULL, "MaEditorWgt is NULL", QVariantMap());
    ScrollController* scrollController = wgt->getScrollController();
    SAFE_POINT(scrollController != NULL, "ScrollController is NULL", QVariantMap());

    int firstBase = scrollController->getFirstVisibleBase();
    int firstSeq = scrollController->getFirstVisibleMaRowIndex();

    ss.setFirstPos(firstBase);
    ss.setFirstSeq(firstSeq);
    ss.setFont(v->getFont());
    ss.setZoomFactor(v->zoomFactor);

    return ss.stateData;
}

} // namespace U2
