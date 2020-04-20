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

#ifndef _U2_MOLECULAR_SURFACE_RENDERER_H_
#define _U2_MOLECULAR_SURFACE_RENDERER_H_

#include <QString>
#include <QMap>

namespace U2 {

class MolecularSurface;
class MolecularSurfaceRenderer;
class MolecularSurfaceRendererFactory;

//! Singleton regisrtry for molecular surface(ms) renderers fabrics.
class MolecularSurfaceRendererRegistry {
public:
    //! @return Default ms renderer factory name.
    static const QString defaultFactoryName();

    //! @return List of all factories names.
    static const QList<QString> factoriesNames();

    //! @return Concreete factory by name.
    static const MolecularSurfaceRendererFactory* getFactory(const QString &name);

    //! @return Constructed ms renderer by factory name.
    static MolecularSurfaceRenderer* createMSRenderer(const QString &name);

private:
    //! Hidden constructor. Called by getInstance()
    MolecularSurfaceRendererRegistry();

    //! Returns singleton instance of registry.
    static MolecularSurfaceRendererRegistry* getInstance();

    //! Registers all ms render factories.
    void registerFactories();

private:
    QMap<QString, MolecularSurfaceRendererFactory*> factories;
};  // class MolecularSurfaceRendererRegistry


//! MolecularSurfaceRenderer abstract factory
class MolecularSurfaceRendererFactory {
public:
    virtual MolecularSurfaceRenderer* createInstance() const = 0;
};  // class MolecularSurfaceRendererFactory

#define SURF_RENDERER_FACTORY(c) \
public: \
    static const QString ID; \
class Factory : public MolecularSurfaceRendererFactory { \
public: \
    MolecularSurfaceRenderer* createInstance() const { return new c; } \
};

class MolecularSurfaceRenderer {
protected:
    MolecularSurfaceRenderer() {};

public:
    virtual ~MolecularSurfaceRenderer() {};

    virtual void drawSurface(MolecularSurface& surface) = 0;
};

class DotsRenderer : public MolecularSurfaceRenderer {
private:
    DotsRenderer() : MolecularSurfaceRenderer() {}

public:
    virtual void drawSurface(MolecularSurface& surface);
    SURF_RENDERER_FACTORY(DotsRenderer)
};

class ConvexMapRenderer : public MolecularSurfaceRenderer {
public:
    ConvexMapRenderer() {}
    virtual void drawSurface(MolecularSurface& surface);
    SURF_RENDERER_FACTORY(ConvexMapRenderer)
};



} //namespace

#endif //_U2_MOLECULAR_SURFACE_RENDERER_H_

