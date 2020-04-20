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

#ifndef _U2_BIOSTRUCT3D_COLORSCHEME_H
#define _U2_BIOSTRUCT3D_COLORSCHEME_H

#include <U2Core/BioStruct3D.h>
#include <QColor>
#include <QMap>
#include <QHash>
#include <QPair>
#include <QLinkedList>

#include "GraphicUtils.h"

namespace U2 {

class BioStruct3DObject;
class BioStruct3DColorScheme;
class BioStruct3DColorSchemeFactory;
class LRegionsSelection;
class U2Region;

//! Singleton regisrtry for color scheme fabrics.
class BioStruct3DColorSchemeRegistry {
public:
    //! @return Default colo scheme factory name.
    static const QString defaultFactoryName();

    //! @return List of all factories names.
    static const QList<QString> factoriesNames();

    //! @return Concreete factory by name.
    static const BioStruct3DColorSchemeFactory* getFactory(const QString &name);

    //! @return Constructed color scheme by factory name.
    static BioStruct3DColorScheme* createColorScheme(const QString &name, const BioStruct3DObject *biostruct);

private:
    //! Hidden constructor. Called by getInstance()
    BioStruct3DColorSchemeRegistry();

    //! Returns singleton instance of registry.
    static BioStruct3DColorSchemeRegistry* getInstance();

    //! Registers all render factories.
    void registerFactories();

private:
    QMap<QString, BioStruct3DColorSchemeFactory*> factories;
};  // class BioStruct3DColorSchemeRegistry


//!  Abstract factory for BioStruct3DColorScheme
class BioStruct3DColorSchemeFactory {
public:
    virtual BioStruct3DColorScheme* createInstance(const BioStruct3DObject *biostruct) const = 0;
    //! Method creates factories
};

#define COLOR_SCHEME_FACTORY(c) \
public: \
    static const QString schemeName; \
    class Factory : public BioStruct3DColorSchemeFactory { \
    public: \
        BioStruct3DColorScheme* createInstance(const BioStruct3DObject *biostructObj) const { return new c(biostructObj); } \
    };

//! Abstract BioStruct3DColorScheme
/*!
*    Color scheme determines 3d structure coloring style. For example, we could use colors to mark chemical
*    elements or secondary structure.
*/
class BioStruct3DColorScheme {
protected:
    BioStruct3DColorScheme(const BioStruct3DObject *biostruct);

public:
    virtual ~BioStruct3DColorScheme() {};

    Color4f getAtomColor(const SharedAtom& atom) const;

    void updateSelectionRegion(int chainID, const QVector<U2Region>& added, const QVector<U2Region>& removed);
    bool isInSelection(const SharedAtom& atom) const;

    void setSelectionColor(QColor color);
    void setUnselectedShadingLevel(float shading);

protected:
    virtual Color4f getSchemeAtomColor(const SharedAtom& atom) const;

protected:
    Color4f defaultAtomColor;
    Color4f selectionColor;

    BioStruct3DChainSelection selection;

private:
    float unselectedShading;
};  // class BioStruct3DColorScheme


class ChemicalElemColorScheme : public BioStruct3DColorScheme {
    QHash<int, Color4f> elementColorMap;
    ChemicalElemColorScheme(const BioStruct3DObject *biostruct);
    virtual Color4f getSchemeAtomColor(const SharedAtom& atom) const;

public:
    COLOR_SCHEME_FACTORY(ChemicalElemColorScheme)
};  // class ChemicalElemColorScheme


class ChainsColorScheme : public BioStruct3DColorScheme {
    QMap<int, Color4f> chainColorMap;
    ChainsColorScheme(const BioStruct3DObject *biostruct);
    virtual Color4f getSchemeAtomColor(const SharedAtom& atom) const;

private:
    //! @return Molecular chain colors for biostruct object.
    static const QMap<int, QColor> getChainColors(const BioStruct3DObject *biostruct);

public:
    COLOR_SCHEME_FACTORY(ChainsColorScheme)
};  // class ChainsColorScheme


class SecStructColorScheme : public BioStruct3DColorScheme {
    struct MolStructs {
        QHash<int, QByteArray> strucResidueTable;
    };
    QMap<QByteArray, Color4f> secStrucColorMap;
    QMap<int, MolStructs> molMap;
    SecStructColorScheme(const BioStruct3DObject *biostruct);
    virtual Color4f getSchemeAtomColor(const SharedAtom& atom) const;

private:
    //! @return Secondary structure annotation colors for biostruct object.
    static const QMap<QString, QColor> getSecStructAnnotationColors(const BioStruct3DObject *biostruct);

public:
    COLOR_SCHEME_FACTORY(SecStructColorScheme)
};  // class SecStructColorScheme

/** One color scheme for debug purposes */
class SimpleColorScheme : public BioStruct3DColorScheme {
    SimpleColorScheme(const BioStruct3DObject *biostruct);

private:
    static QVector<Color4f> colors;
    static void createColors();

public:
    COLOR_SCHEME_FACTORY(SimpleColorScheme)
};  // class SimpleColorScheme

}   // namespace U2

#endif //_U2_BIOSTRUCT3D_COLORSCHEME_H_
