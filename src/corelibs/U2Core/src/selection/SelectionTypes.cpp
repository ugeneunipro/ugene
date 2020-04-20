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

#include "SelectionTypes.h"

// These are included to export symbols
#include "TextSelection.h"
#include "DNASequenceSelection.h"

namespace U2 {

const GSelectionType GSelectionTypes::DOCUMENTS("selection_documents");

const GSelectionType GSelectionTypes::GOBJECTS("selection_gobjects");

const GSelectionType GSelectionTypes::GOBJECT_VIEWS("selection_gobject_views");

const GSelectionType GSelectionTypes::TEXT("selection_text");

const GSelectionType GSelectionTypes::DNASEQUENCE("selection_dnasequence");

const GSelectionType GSelectionTypes::ANNOTATIONS("selection_annotations");

const GSelectionType GSelectionTypes::ANNOTATION_GROUPS("selection_annotation_groups");

const GSelectionType GSelectionTypes::FOLDERS("selection_folders");

}//namespace
