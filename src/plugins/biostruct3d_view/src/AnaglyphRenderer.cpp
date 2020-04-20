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

#include "AnaglyphRenderer.h"

#include "BioStruct3DGLWidget.h"
#include "GLFrameManager.h"
#include "GraphicUtils.h"

namespace U2 {

static const QString EYES_SHIFT_NAME("EyesShift");
static const QString LEFT_EYE_COLOR_NAME("LeftEyeColor");
static const QString RIGHT_EYE_COLOR_NAME("RightEyeColor");

QVariantMap AnaglyphSettings::toMap(QVariantMap &map) const {
    map[EYES_SHIFT_NAME] = qVariantFromValue(eyesShift);
    map[LEFT_EYE_COLOR_NAME] = qVariantFromValue(leftEyeColor);
    map[RIGHT_EYE_COLOR_NAME] = qVariantFromValue(rightEyeColor);

    return map;
}

AnaglyphSettings AnaglyphSettings::fromMap(const QVariantMap &map) {
    float eyesShift = map[EYES_SHIFT_NAME].value<float>();
    QColor leftEyeColor = map[LEFT_EYE_COLOR_NAME].value<QColor>();
    QColor rightEyeColor= map[RIGHT_EYE_COLOR_NAME].value<QColor>();

    return AnaglyphSettings(eyesShift, leftEyeColor, rightEyeColor);
}

void AnaglyphRenderer::init() {
    createEmptyTextures();
}

void AnaglyphRenderer::resize(int _width, int _height) {
    CHECK_GL_ERROR

    width = _width, height = _height;

    if ( !(width > 0 && height > 0) ) {
        return;
    }

    createEmptyTextures();

    CHECK_GL_ERROR
}

void AnaglyphRenderer::draw() {
    CHECK_GL_ERROR

    GLFrame *glFrame = renderer->getGLFrame();
    float eyesShift = 5.0 * settings.eyesShift * glFrame->getCameraPosition().z / 200.0;

    glMatrixMode(GL_MODELVIEW);

    // Prepare anaglyph textures
    glPushMatrix();
        //glTranslatef(eyesShift, 0, 0);
        glLoadIdentity();
        gluLookAt(eyesShift, 0.0, glFrame->getCameraPosition().z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        // at this moment buffer must be clean glClear omitted as a slow operation
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // isolate errors from main scene renderer
        CHECK_GL_ERROR
        renderer->draw();
        glGetError();

        glBindTexture(GL_TEXTURE_2D, anaglyphRenderTextureRight);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
    glPopMatrix();

    glPushMatrix();
        //glTranslatef(-eyesShift, 0, 0);
        glLoadIdentity();
        gluLookAt(-eyesShift, 0.0, glFrame->getCameraPosition().z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // isolate errors from main scene renderer
        CHECK_GL_ERROR
        renderer->draw();
        glGetError();

        glBindTexture(GL_TEXTURE_2D, anaglyphRenderTextureLeft);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
    glPopMatrix();

    // Draw anaglyph textures in ortho projection
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

        setOrthoProjection();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTexturesAnaglyph();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    CHECK_GL_ERROR
}

void AnaglyphRenderer::setOrthoProjection() {
    CHECK_GL_ERROR

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    CHECK_GL_ERROR
}

void AnaglyphRenderer::createEmptyTextures() {
    CHECK_GL_ERROR

    if(anaglyphRenderTextureLeft != 0) {
        glDeleteTextures(1, &anaglyphRenderTextureLeft);
    }
    if(anaglyphRenderTextureRight != 0) {
        glDeleteTextures(1, &anaglyphRenderTextureRight);
    }
    if(tempAnaglyphRenderTexture != 0) {
        glDeleteTextures(1, &tempAnaglyphRenderTexture);
    }

    // TODO : check for NPOT sizes
    GLuint texwidth = width, texheight = height;

    GLuint txtnumbers[3] = {0};
    glGenTextures(3, txtnumbers);

    char *txtdata = new char[texwidth * texheight * 4];

    for (GLuint *txtnumber = txtnumbers; txtnumber < txtnumbers + 3; ++txtnumber) {
        glBindTexture(GL_TEXTURE_2D, *txtnumber);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) txtdata);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }

    delete[] txtdata;

    anaglyphRenderTextureLeft   = txtnumbers[0];
    anaglyphRenderTextureRight  = txtnumbers[1];
    tempAnaglyphRenderTexture   = txtnumbers[2];

    CHECK_GL_ERROR
}

void AnaglyphRenderer::drawTexturesAnaglyph() {
    CHECK_GL_ERROR

    drawTexture(anaglyphRenderTextureLeft, settings.rightEyeColor.red(), settings.rightEyeColor.green(), settings.rightEyeColor.blue(), 0.5f, false); // colored left image

    glBindTexture(GL_TEXTURE_2D, tempAnaglyphRenderTexture);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);    // Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)

    drawTexture(anaglyphRenderTextureRight, settings.leftEyeColor.red(), settings.leftEyeColor.green(), settings.leftEyeColor.blue(), 0.5f, false); // colored right image
    drawTexture(tempAnaglyphRenderTexture, 255, 255, 255, 1.0f, true);

    CHECK_GL_ERROR
}

void AnaglyphRenderer::drawTexture(GLuint anaglyphRenderTexture, int red, int green, int blue, float alpha, bool alphaOnly)
{
    CHECK_GL_ERROR

    glEnable(GL_TEXTURE_2D);

    if (alphaOnly){
        // the second image is this, it has needed alpha color
        glBlendFunc(GL_DST_ALPHA, GL_DST_ALPHA);
    }
    else {
        glBlendFunc(GL_ONE, GL_ONE);
    }

    glBindTexture(GL_TEXTURE_2D, anaglyphRenderTexture);

    glColor4ub(red, green, blue, (GLubyte) (alpha * 255.0));
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex2f(0,0);

        glTexCoord2f(0, 0);
        glVertex2f(0, 1);

        glTexCoord2f(1, 0);
        glVertex2f(1, 1);

        glTexCoord2f(1, 1);
        glVertex2f(1, 0);
    glEnd();

    // Unbind the blur texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    CHECK_GL_ERROR
}

AnaglyphRenderer::AnaglyphRenderer(BioStruct3DGLWidget *_renderer, const AnaglyphSettings &_settings) :
    renderer(_renderer),
    settings(_settings),
    width(1),
    height(1),
    anaglyphRenderTextureLeft(0),
    anaglyphRenderTextureRight(0),
    tempAnaglyphRenderTexture(0),
    hasErrors(false)
{

}

AnaglyphRenderer::~AnaglyphRenderer() {

}

bool AnaglyphRenderer::isAvailable() {
    draw();
    return !hasErrors;
}

}   // namespace U2

