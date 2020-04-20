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

#include "SelectionModel.h"


namespace U2 {

void MultiGSelection::addSelection(const GSelection* s) {
    assert(!selections.contains(s));
    selections.push_back(s);
}

void MultiGSelection::removeSelection(const GSelection* s) {
    assert(selections.contains(s));
    selections.removeAll(s);
}


const GSelection* MultiGSelection::findSelectionByType(GSelectionType t) const {
    foreach(const GSelection* s, selections) {
        if (s->getSelectionType() == t) {
            return s;
        }
    }
    return NULL;
}

void GSelection::sl_selectionChanged() {
    emit si_onSelectionChanged(this);
}



}//namespace
