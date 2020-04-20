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

#include <U2Core/SelectionTypes.h>

#include "FolderSelection.h"

namespace U2 {

FolderSelection::FolderSelection(QObject *p)
: GSelection(GSelectionTypes::FOLDERS, p)
{

}

bool FolderSelection::isEmpty() const {
    return selection.isEmpty();
}

void FolderSelection::clear() {
    selection.clear();
    sl_selectionChanged();
}

const QList<Folder> & FolderSelection::getSelection() const {
    return selection;
}

void FolderSelection::setSelection(const QList<Folder> &folders) {
     selection = folders;
     sl_selectionChanged();
}

} // U2
