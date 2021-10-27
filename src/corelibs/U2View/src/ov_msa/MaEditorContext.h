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

#ifndef _U2_MAEDITOR_CONTEXT_H_
#define _U2_MAEDITOR_CONTEXT_H_

#include <U2Core/global.h>

namespace U2 {

class MaEditor;
class MultipleAlignmentObject;
class MaEditorWgt;
class MaEditorSelectionController;
class MaEditorSelection;
class MaCollapseModel;

/**
 * Holds immutable pointers to the core models and controllers in MAEditor: the editor itself, ma-object, UI, controllers, etc.
 * Helps to avoid a boilerplate code with a lot of getters and nullptr checks.
 *
 * This class when instantiated guarantees that all fields are valid and non-null.
 */
class U2VIEW_EXPORT MaEditorContext {
public:
    MaEditorContext(MaEditor *editor, MaEditorWgt *wgt);

    MaEditor* const editor;
    MultipleAlignmentObject* const maObject;
    MaEditorWgt* const ui;
    MaEditorSelectionController* const selectionController;
    MaCollapseModel* const collapseModel;

    const MaEditorSelection& getSelection() const;
};

}  // namespace U2

#endif  // _U2_MAEDITOR_CONTEXT_H_
