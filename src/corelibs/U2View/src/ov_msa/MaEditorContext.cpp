/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "MaEditorContext.h"

#include <U2Core/U2SafePoints.h>

#include "MaEditor.h"
#include "MaEditorSelection.h"
#include "MaEditorWgt.h"

namespace U2 {

MaEditorContext::MaEditorContext(MaEditor *maEditor, MaEditorWgt *wgt)
    : editor(maEditor),
      maObject(maEditor->getMaObject()),
      ui(wgt),
      selectionController(maEditor->getSelectionController()),
      collapseModel(editor->getCollapseModel()) {
    SAFE_POINT(maObject != nullptr, "maObject is null", );
    SAFE_POINT(ui != nullptr, "ui is null", );
    SAFE_POINT(selectionController != nullptr, "selectionController is null", );
    SAFE_POINT(collapseModel != nullptr, "collapseModel is null", );
}

const MaEditorSelection& MaEditorContext::getSelection() const {
    return selectionController->getSelection();
}

}  // namespace U2
