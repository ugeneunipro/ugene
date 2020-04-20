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

#ifndef _U2_BIOSTRUCT3D_RENDERER_H_
#define _U2_BIOSTRUCT3D_RENDERER_H_

#include <QString>
#include <QMap>

namespace U2 {

class BioStruct3D;
class BioStruct3DGLRenderer;
class BioStruct3DGLRendererFactory;
class BioStruct3DColorScheme;
class BioStruct3DRendererSettings;

//! Singleton regisrtry for renderers fabrics.
class BioStruct3DGLRendererRegistry {
public:
    //! @return Default renderer factory name.
    static const QString defaultFactoryName();

    //! @return List of all factories names.
    static const QList<QString> factoriesNames();

    //! @return Concreete factory by name.
    static const BioStruct3DGLRendererFactory* getFactory(const QString &name);

    //! @return Constructed renderer by factory name.
    static BioStruct3DGLRenderer* createRenderer(const QString &name,   const BioStruct3D&,
                                                                        const BioStruct3DColorScheme*,
                                                                        const QList<int>&,
                                                                        const BioStruct3DRendererSettings*);

    /** Check if biostruct can be drawn with renderer */
    static bool isAvailableFor(const QString &name, const BioStruct3D &biostruct);

    /** @returns List of renderer names available for biostruct */
    static QList<QString> getRenderersAvailableFor(const BioStruct3D &biostruct);

private:
    //! Hidden constructor. Called by getInstance()
    BioStruct3DGLRendererRegistry();

    //! Returns singleton instance of registry.
    static BioStruct3DGLRendererRegistry* getInstance();

    //! Registers all render factories.
    void registerFactories();

private:
    QMap<QString, BioStruct3DGLRendererFactory*> factories;
};  // class BioStruct3DGLRendererRegistry


/** BioStruct3DGLRenderer abstract factory */
class BioStruct3DGLRendererFactory {
public:
    BioStruct3DGLRendererFactory(const QString &_name) : name(_name) {}

    /** @returns Factory name */
    const QString getName() const { return name; }

    /** Check if biostruct can be drawn with renderer */
    virtual bool isAvailableFor(const BioStruct3D &biostruct) const = 0;

    /** @returns Contructed renderer */
    virtual BioStruct3DGLRenderer* createInstance(const BioStruct3D&, const BioStruct3DColorScheme*, const
                                                  QList<int> &shownModels, const BioStruct3DRendererSettings*) const = 0;

private:
    QString name;

};

/** Macro helper for factories defining */
#define RENDERER_FACTORY(c) \
public: \
    static const QString ID; \
    \
class Factory : public BioStruct3DGLRendererFactory { \
public: \
    Factory(const QString &name) : BioStruct3DGLRendererFactory(name) {} \
    \
    virtual BioStruct3DGLRenderer* createInstance(const BioStruct3D &biostruct, const BioStruct3DColorScheme *cscheme, \
                                          const QList<int> &shownModels, const BioStruct3DRendererSettings *settings) const { \
        return new c(biostruct, cscheme, shownModels, settings); \
    } \
    \
    virtual bool isAvailableFor(const BioStruct3D &biostruct) const { \
        return c::isAvailableFor(biostruct); \
    } \
};

/** Settings common for all renderers */
class BioStruct3DRendererSettings {
public:
    BioStruct3DRendererSettings(float _detailLevel)
            : detailLevel(_detailLevel) {}

    float detailLevel;
};

//! Abstract biological 3D structure OpenGL renderer
class BioStruct3DGLRenderer {
protected:
    BioStruct3DGLRenderer(const BioStruct3D &biostruct, const BioStruct3DColorScheme *scheme, const QList<int> &shownModels, const BioStruct3DRendererSettings *settings);

public:
    virtual ~BioStruct3DGLRenderer() {}

    /** Create and initialize all stuff.
        Can be called for full reinialization */
    virtual void create() = 0;

    //! Visualizes macromolecule.
    virtual void drawBioStruct3D() = 0;

    //! Used to completely update model, for ex to recreate display lists.
    virtual void update() {}

    //! Used to update current color scheme, whenever it is changed.
    virtual void updateColorScheme() = 0;

    //! Used to update shown models list, whenever it is changed.
    virtual void updateShownModels() = 0;

    /** Used for settings updating */
    virtual void updateSettings() = 0;

    //! Sets new color scheme.
    void setColorScheme(const BioStruct3DColorScheme* s);

    //! @returns current color scheme.
    const BioStruct3DColorScheme *getColorScheme() const { return colorScheme; }

    /** @returns shown models indexes list reference.
      * indexes are just index numbers of models, NOT modelIds
      */
    const QList<int>& getShownModelsIndexes() const { return shownModels; }

    /** Sets shown models models list. */
    void setShownModelsIndexes(const QList<int> &_shownModels) { shownModels = _shownModels; }

public:
    /** Check if biostruct can be visualized by renderer */
    static bool isAvailableFor(const BioStruct3D &) { return true; }

protected:
    const BioStruct3D& bioStruct;
    const BioStruct3DColorScheme* colorScheme;

protected:
    QList<int> shownModels;

protected:
    const BioStruct3DRendererSettings* settings;
};


} //namespace

#endif // _U2_BIOSTRUCT3D_RENDERER_H_
