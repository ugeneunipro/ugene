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

#include "GTRegionSelector.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <U2Core/Log.h>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTRegionSelector"

#define GT_METHOD_NAME "setRegion"
void GTRegionSelector::setRegion(HI::GUITestOpStatus& os, RegionSelector *regionSelector, const RegionSelectorSettings& s) {

    GT_CHECK(regionSelector != NULL, "RegionSelector is NULL");

    CHECK_EXT(!s.isUnset(), uiLog.trace("GT_DEBUG_MESSAGE RegionSelectorSettings isUnset, returning"), );

    QLineEdit* startEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "start_edit_line", regionSelector));
    GTLineEdit::setText(os, startEdit, QString::number(s.start));

    QLineEdit* endEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "end_edit_line", regionSelector));
    GTLineEdit::setText(os, endEdit, QString::number(s.end));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
