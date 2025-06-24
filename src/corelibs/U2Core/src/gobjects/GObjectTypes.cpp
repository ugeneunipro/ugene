/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "GObjectTypes.h"

#include <U2Core/GObject.h>

/* TRANSLATOR U2::GObject */

namespace U2 {

GObjectTypeInfo::GObjectTypeInfo(const GObjectType& _type,
                                 const QString& _name,
                                 const QString& _pluralName,
                                 const QString& _treeSign,
                                 const IconRef& _iconRef,
                                 const IconRef& _lockedIconRef)
    : type(_type),
      name(_name),
      pluralName(_pluralName),
      treeSign(_treeSign),
      iconRef(_iconRef),
      lockedIconRef(_lockedIconRef) {}

#define REGISTER_TYPE_EX(VAR, ID, NAME, P_NAME, SIGN, ICON_URI, LOCKED_ICON_URI) \
    const GObjectType GObjectTypes::VAR = registerTypeInfo(GObjectTypeInfo(ID, NAME, P_NAME, SIGN, ICON_URI, LOCKED_ICON_URI))

#define REGISTER_TYPE(VAR, ID, NAME, P_NAME, SIGN) \
    REGISTER_TYPE_EX(VAR, ID, NAME, P_NAME, SIGN, IconRef(), IconRef())

///            variable             id                  visual name                     plural name                    sign     icon                            locked icon
REGISTER_TYPE(UNKNOWN, "OT_UNKNOWN", GObject::tr("Unknown"), GObject::tr("Unknown"), "?");
REGISTER_TYPE(UNLOADED, "OT_UNLOADED", GObject::tr("Unloaded"), GObject::tr("Unloaded"), "u");
REGISTER_TYPE_EX(TEXT, "OT_TEXT", GObject::tr("Text"), GObject::tr("Text"), "t", IconRef("core", "texto.png"), IconRef("core", "ro_texto.png"));
REGISTER_TYPE_EX(SEQUENCE, "OT_SEQUENCE", GObject::tr("Sequence"), GObject::tr("Sequences"), "s", IconRef("core", "dna.png"), IconRef("core", "ro_dna.png"));
REGISTER_TYPE(ANNOTATION_TABLE, "OT_ANNOTATIONS", GObject::tr("Annotation"), GObject::tr("Annotations"), "a");
REGISTER_TYPE(VARIANT_TRACK, "OT_VARIATIONS", GObject::tr("Variation"), GObject::tr("Variations"), "v");
REGISTER_TYPE(CHROMATOGRAM, "OT_CHROMATOGRAM", GObject::tr("Chromatogram"), GObject::tr("Chromatograms"), "c");
// SANGER_TODO: icons!!
REGISTER_TYPE_EX(MULTIPLE_CHROMATOGRAM_ALIGNMENT, "OT_MCA", GObject::tr("Alignment"), GObject::tr("Alignments"), "mc", IconRef("core", "msa.png"), IconRef("core", "ro_msa.png"));
REGISTER_TYPE_EX(MULTIPLE_SEQUENCE_ALIGNMENT, "OT_MSA", GObject::tr("Alignment"), GObject::tr("Alignments"), "m", IconRef("core", "msa.png"), IconRef("core", "ro_msa.png"));
REGISTER_TYPE_EX(PHYLOGENETIC_TREE, "OT_PTREE", GObject::tr("Tree"), GObject::tr("Trees"), "tr", IconRef("core", "tree.png"), IconRef("core", "ro_tree.png"));
REGISTER_TYPE_EX(BIOSTRUCTURE_3D, "OT_BIOSTRUCT3D", GObject::tr("3D model"), GObject::tr("3D models"), "3d", IconRef("core", "biostruct3d.png"), IconRef("core", "ro_biostruct3d.png"));
REGISTER_TYPE(ASSEMBLY, "OT_ASSEMBLY", GObject::tr("Assembly"), GObject::tr("Assemblies"), "as");

static QHash<GObjectType, GObjectTypeInfo>& getTypeMap() {
    static QHash<GObjectType, GObjectTypeInfo> map;
    return map;
}

const GObjectTypeInfo& GObjectTypes::getTypeInfo(const QString& t) {
    QHash<GObjectType, GObjectTypeInfo>& m = getTypeMap();
    if (m.contains(t)) {
        GObjectTypeInfo& r = m[t];
        return r;
    }
    GObjectTypeInfo& r = m[GObjectTypes::UNKNOWN];
    return r;
}

GObjectType GObjectTypes::registerTypeInfo(const GObjectTypeInfo& ti) {
    QHash<GObjectType, GObjectTypeInfo>& map = getTypeMap();
    assert(!map.contains(ti.type));
    map[ti.type] = ti;
    return ti.type;
}

void GObjectTypes::initTypeTranslations() {
    QHash<GObjectType, GObjectTypeInfo>& map = getTypeMap();
    foreach (GObjectType t, map.keys()) {
        GObjectTypeInfo& info = map[t];
        info.name = GObject::tr(info.name.toLocal8Bit().data());
        info.pluralName = GObject::tr(info.pluralName.toLocal8Bit().data());
    }
}

void GObjectTypes::initTypeIcons() {
    QHash<GObjectType, GObjectTypeInfo>& map = getTypeMap();
    const auto& types = map.keys();
    for (const auto& type : qAsConst(types)) {
        GObjectTypeInfo& info = map[type];
        if (info.iconRef.isEmpty()) {
            info.iconRef = IconRef("core", "gobject.png");
            info.lockedIconRef = IconRef("core", "ro_gobject.png");
        }
    }
}

}  // namespace U2
