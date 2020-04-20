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

#ifndef _U2_ANAGLYPH_RENDERER_H_
#define _U2_ANAGLYPH_RENDERER_H_

#include <U2Core/Log.h>

#include <QColor>
// include this for proper cross-platform including of glu.h
#include "GraphicUtils.h"

namespace U2 {

class BioStruct3DGLWidget;

enum AnaglyphStatus {NOT_AVAILABLE = -1, DISABLED = 0, ENABLED = 1};

class AnaglyphSettings {
public:
    AnaglyphSettings(float _eyesShift, const QColor &_leftEyeColor, const QColor &_rightEyeColor)
            : eyesShift(_eyesShift), leftEyeColor(_leftEyeColor), rightEyeColor(_rightEyeColor)
    {}

    float eyesShift;
    QColor leftEyeColor, rightEyeColor;

public:
    static AnaglyphSettings defaultSettings() {
        return AnaglyphSettings(1.6f, QColor(0, 255, 255), QColor(255, 0, 0));
    }

public:
    QVariantMap toMap(QVariantMap &map) const;
    static AnaglyphSettings fromMap(const QVariantMap &map);
};


/** Anaglyph effect renderer */
class AnaglyphRenderer {
public:
    /** Construct anaglyph renderer over scene renderer */
    AnaglyphRenderer(BioStruct3DGLWidget *_renderer, const AnaglyphSettings &_settings);
    virtual ~AnaglyphRenderer();

    /** Checks if possible to enable anaglyph. Performs test rendering */
    virtual bool isAvailable();

    virtual void init();
    virtual void resize(int w, int h);
    virtual void draw();

    const AnaglyphSettings& getSettings() const { return settings; }
    void setSettings(const AnaglyphSettings &_settings) { settings = _settings; }

private:
    /** Set up an ortho projection. For textures rendering */
    void setOrthoProjection();

    /** Creates empty textures sized width*height */
    void createEmptyTextures();

    void drawTexturesAnaglyph();
    void drawTexture(GLuint anaglyphRenderTexture, int red, int green, int blue, float alpha, bool alphaOnly);

private:
    // this will redefine CHECK_GL_ERROR macro
    inline void checkGlError(const char *file, int line);

private:
    BioStruct3DGLWidget *renderer;
    AnaglyphSettings settings;
    int width, height;

private:
    GLuint anaglyphRenderTextureLeft, anaglyphRenderTextureRight, tempAnaglyphRenderTexture;
    bool hasErrors;
};

inline void AnaglyphRenderer::checkGlError(const char *file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        QString where = QString("%1:%2: ").arg(file).arg(line);
        QString msg = QString("OpenGL error (%1): %2").arg(error).arg((char*)gluErrorString(error));
        uiLog.trace(where + msg);

        hasErrors = true;
    }
}

}   // namespace U2

#endif  // #ifndef _U2_ANAGLYPH_RENDERER_H_
