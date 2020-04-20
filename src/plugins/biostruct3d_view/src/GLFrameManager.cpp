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

#include <QOpenGLWidget>

#include <U2Core/Vector3D.h>

#include "GLFrameManager.h"
#include <U2Core/Log.h>
// include this for proper cross-platform including of glu.h
#include "GraphicUtils.h"

namespace U2 {

const GLfloat GLFrame::DEFAULT_ZOOM = 45.0f;
//const Vector3D GLFrame::DEFAULT_CAMERA = Vector3D(0, 0, 0);

///////////////////////////////////////////////////////////////////////////////////////////
/// GLFrame

GLFrame::GLFrame(QOpenGLWidget* widget)
        : glWidget(widget), rotMatrix(),
          cameraClipNear(0), cameraClipFar(0),
          zoomFactor(DEFAULT_ZOOM), cameraPosition(0,0,0)
{
    rotMatrix.loadIdentity();
}

void GLFrame::performZoom( float delta )
{
    static const float maxZoom = 150.0;
    static const float minZoom = 2.0;
    zoomFactor += delta;
    if (zoomFactor < minZoom) {
        zoomFactor = minZoom;
        return;
    }

    if (zoomFactor > maxZoom) {
        zoomFactor = maxZoom;
        return;
    }
}

void GLFrame::performShift(float deltaX, float deltaY)
{
    static float defaultWidth=1092;
    Vector3D shiftVector;
    float frameFactor = glWidget->width()/defaultWidth;

    shiftVector.set(deltaX*zoomFactor*frameFactor, deltaY*zoomFactor*frameFactor, 0);
    cameraPosition+=shiftVector;
}

const Vector3D GLFrame::getCameraPosition()const
{
    return cameraPosition;
}

void GLFrame::setCameraPosition(float x, float y, float z)
{
    cameraPosition.set(x, y, z);
}

void GLFrame::setCameraPosition(const Vector3D &v) {
    cameraPosition = v;
}

void GLFrame::setCameraClip(float clipNear, float clipFar) {
    cameraClipNear = clipNear;
    cameraClipFar = clipFar;
}

void GLFrame::rotateCamera(const Vector3D& rotAxis, float rotAngle )
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z );
    glMultMatrixf(rotMatrix.data());
    glGetFloatv( GL_MODELVIEW_MATRIX, rotMatrix.data());
}

#define ZOOM_FACTOR_ID "ZOOM_FACTOR"
#define CAMERA_STATE_POSITION_X_ID "CAMERA_STATE_POSITION_X"
#define CAMERA_STATE_POSITION_Y_ID "CAMERA_STATE_POSITION_Y"
#define ROTATION_MATRIX_ID "ROTATION_MATRIX"

void GLFrame::setState( const QVariantMap& state )
{
    cameraPosition.x = state.value(CAMERA_STATE_POSITION_X_ID, 0).value<float>();
    cameraPosition.y = state.value(CAMERA_STATE_POSITION_Y_ID, 0).value<float>();

    zoomFactor = state.value(ZOOM_FACTOR_ID, DEFAULT_ZOOM).value<float>();
    QVariantList rotML = state.value(ROTATION_MATRIX_ID).value<QVariantList>();
    if (!rotML.isEmpty()) {
             rotMatrix.load(rotML);
    }
}

void GLFrame::writeStateToMap( QVariantMap& state )
{
    state[CAMERA_STATE_POSITION_X_ID] = QVariant::fromValue(cameraPosition.x);
    state[CAMERA_STATE_POSITION_Y_ID] = QVariant::fromValue(cameraPosition.y);

    state[ZOOM_FACTOR_ID] = QVariant::fromValue(zoomFactor);
    state[ROTATION_MATRIX_ID] = rotMatrix.store();
}


void GLFrame::updateViewPort( int width, int height )
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set view settings
    GLfloat aspect = GLfloat(width) / height;
    gluPerspective(zoomFactor, aspect, cameraClipNear, cameraClipFar);
}

void GLFrame::updateViewPort()
{
    updateViewPort(glWidget->width(), glWidget->height());
}

GLFrameManager::~GLFrameManager()
{

}

void GLFrameManager::addGLFrame( GLFrame* glFrame)
{
    widgetFrameMap.insert(glFrame->getGLWidget(), glFrame);
}

GLFrame* GLFrameManager::getGLWidgetFrame(QOpenGLWidget *widget )
{
    if (widgetFrameMap.contains(widget)) {
        return widgetFrameMap.value(widget);
    } else  {
        return NULL;
    }

}

QList<GLFrame*> GLFrameManager::getGLFrames()
{
    return widgetFrameMap.values();
}

void GLFrameManager::setSyncLock(bool lockOn, QOpenGLWidget *syncWidget )
{
    syncLock = lockOn;
    if (lockOn) {
        GLFrame* syncFrame = getGLWidgetFrame(syncWidget);
        QVariantMap state;
        syncFrame->writeStateToMap(state);
        foreach(GLFrame* frame, widgetFrameMap.values()) {
            if (frame != syncFrame) {
                frame->makeCurrent();
                frame->setState(state);
                frame->updateViewPort();
                frame->updateGL();
            }
        }
    }
}

void GLFrameManager::removeGLWidgetFrame( QOpenGLWidget* widget )
{
    Q_ASSERT(widgetFrameMap.contains(widget));
    widgetFrameMap.remove(widget);
}

QList<GLFrame*> GLFrameManager::getActiveGLFrameList( GLFrame* currentFrame, bool syncModeOn )
{
    if (syncModeOn) {
        return widgetFrameMap.values();
    } else {
        QList<GLFrame*> lst;
        lst.append(currentFrame);
        return lst;
    }
}

void GLFrameManager::clear()
{
    widgetFrameMap.clear();
}

} //namespace
