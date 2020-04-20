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

#include <U2Core/U2SafePoints.h>

#include "GObjectRelationRoles.h"

namespace U2 {

namespace {
    const QString SEQUENCE("sequence");
    const QString ANNOTATION_TABLE("annotation_table");
    const QString PHYLOGENETIC_TREE("phylogenetic_tree");
    const QString REFERENCE_SEQUENCE("reference_sequence");
}

QString GObjectRelationRoleCompatibility::toString(GObjectRelationRole role) {
    switch (role) {
        case ObjectRole_Sequence:
            return SEQUENCE;
        case ObjectRole_AnnotationTable:
            return ANNOTATION_TABLE;
        case ObjectRole_PhylogeneticTree:
            return PHYLOGENETIC_TREE;
        case ObjectRole_ReferenceSequence:
            return REFERENCE_SEQUENCE;
        default:
            FAIL("Unknown role", "");
    }
}

GObjectRelationRole GObjectRelationRoleCompatibility::fromString(const QString &str) {
    if (SEQUENCE == str) {
        return ObjectRole_Sequence;
    } else if (ANNOTATION_TABLE == str) {
        return ObjectRole_AnnotationTable;
    } else if (PHYLOGENETIC_TREE == str) {
        return ObjectRole_PhylogeneticTree;
    } else if (REFERENCE_SEQUENCE == str) {
        return ObjectRole_ReferenceSequence;
    } else {
        FAIL("Unknown role string", ObjectRole_Sequence);
    }
}

}
