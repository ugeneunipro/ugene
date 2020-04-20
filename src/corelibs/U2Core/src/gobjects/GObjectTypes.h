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

#ifndef _U2_GOBJECT_TYPES_H_
#define _U2_GOBJECT_TYPES_H_

#include <U2Core/global.h>

#include <QIcon>

namespace U2 {

class U2CORE_EXPORT GObjectTypeInfo {
public:
    GObjectTypeInfo(){}
    GObjectTypeInfo(const GObjectType& _type, const QString& _name, const QString& _pluralName, const QString& treeSign, const QString iconURL, const QString& lockedIconUrl);

    GObjectType type;
    QString     name;
    QString     pluralName;
    QString     treeSign;
    QString     iconURL;
    QString     lockedIconUrl;
    QIcon       icon;
    QIcon       lockedIcon;
};

class U2CORE_EXPORT GObjectTypes {
public:
    static const GObjectType UNKNOWN;
    static const GObjectType UNLOADED;
    static const GObjectType TEXT;
    static const GObjectType SEQUENCE;
    static const GObjectType ANNOTATION_TABLE;
    static const GObjectType VARIANT_TRACK;
    static const GObjectType CHROMATOGRAM;
    static const GObjectType MULTIPLE_CHROMATOGRAM_ALIGNMENT;
    static const GObjectType MULTIPLE_SEQUENCE_ALIGNMENT;
    static const GObjectType PHYLOGENETIC_TREE;
    static const GObjectType BIOSTRUCTURE_3D;
    static const GObjectType UINDEX;
    static const GObjectType ASSEMBLY;

    static GObjectType registerTypeInfo(const GObjectTypeInfo& ti);

    //Returns reference to the type by type.
    //WARN: Caching return value by reference is not safe -> it can be relocated if new type is registered
    static const GObjectTypeInfo& getTypeInfo(const GObjectType& type);

    static void initTypeTranslations(); //must be called only once after all translations are loaded
    static void initTypeIcons();

};

}//namespace


#endif
