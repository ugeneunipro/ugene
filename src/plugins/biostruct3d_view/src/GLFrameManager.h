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

#ifndef _U2_GL_FRAME_MANAGER_H_
#define _U2_GL_FRAME_MANAGER_H_

#include <U2Core/Matrix44.h>

#include <QVariantList>

class QOpenGLWidget;

namespace U2 {

class Vector3D;

/*!
* @brief Class for OpenGL 3d scene manipulation
*/
class GLFrame {
    static const GLfloat DEFAULT_ZOOM;
    //static const Vector3D DEFAULT_CAMERA;
public:
    GLFrame(QOpenGLWidget* widget);

    void setState(const QVariantMap& state);
    void writeStateToMap(QVariantMap& states);
    QOpenGLWidget* getGLWidget() { return glWidget; }
    void makeCurrent() { glWidget->makeCurrent(); }
    void updateViewPort(int width, int height);
    void updateViewPort();
    void updateGL() { glWidget->update(); }
    GLfloat getZoomFactor() const { return zoomFactor; }
    float* getRotationMatrix() { return rotMatrix.data(); }
    void rotateCamera(const Vector3D& rotAxis, float rotAngle);

    void setCameraClip(float clipNear, float clipFar);

    const Vector3D getCameraPosition() const;
    void setCameraPosition(const Vector3D &v);
    void setCameraPosition(float x, float y, float z);

    void performZoom(float delta);
    void performShift(float deltaX, float deltaY);

private:
    QOpenGLWidget* glWidget;
    Matrix44 rotMatrix;
    float cameraClipNear, cameraClipFar;

    GLfloat zoomFactor;
    Vector3D cameraPosition;
};

class GLFrameManager {
    QMap<QOpenGLWidget*, GLFrame*> widgetFrameMap;
    bool syncLock;

public:
    GLFrameManager() : syncLock(false) {}
    ~GLFrameManager();
    bool getSyncLock() const { return syncLock; }
    void setSyncLock(bool lockOn, QOpenGLWidget* syncWidget);
    void addGLFrame(GLFrame* frame);
    GLFrame* getGLWidgetFrame(QOpenGLWidget* widget);
    void removeGLWidgetFrame(QOpenGLWidget *widget);
    void clear();
    QList<GLFrame*> getGLFrames();
    QList<GLFrame*> getActiveGLFrameList(GLFrame* currentFrame, bool syncModeOn);

};

} // namespace


#endif // _U2_GL_FRAME_MANAGER_H_


