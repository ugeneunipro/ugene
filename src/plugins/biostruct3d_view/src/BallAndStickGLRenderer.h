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

#pragma once

#include <GraphicUtils.h>

#include "BioStruct3DGLRender.h"

namespace U2 {

class BallAndStickGLRenderer : public BioStruct3DGLRenderer {
protected:
    BallAndStickGLRenderer(const BioStruct3D& struc, const BioStruct3DColorScheme* s, const QList<int>& shownModels, const BioStruct3DRendererSettings* settings);

public:
    virtual ~BallAndStickGLRenderer();

    void drawBioStruct3D() override;

    void create() override;

    void update() override;
    void updateColorScheme() override;
    void updateShownModels() override;
    void updateSettings() override;

private:
    void init();
    void createDisplayList();

private:
    bool inited;
    GLuint dl;
    static QList<GLuint> dlIndexStorage;
    static QMutex mutex;

    RENDERER_FACTORY(BallAndStickGLRenderer)
};

}  // namespace U2
