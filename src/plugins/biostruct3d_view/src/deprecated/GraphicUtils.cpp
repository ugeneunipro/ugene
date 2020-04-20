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

#include <QtOpenGL>
#include <U2Algorithm/GeomUtils.h>
#include <math.h>
#include "GraphicUtils.h"
#include <U2Core/Log.h>

#include <U2Core/BioStruct3D.h>
#include "BioStruct3DColorScheme.h"


namespace U2 {


///////////////////////////////////////////////////////////////////////////////////////////
/// Color4f

Color4f::Color4f()
{
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    color[3] = 1.0;
}

Color4f::Color4f(float r, float g, float b, float a)
{
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
}

Color4f::Color4f( const QColor& qc)
{
    color[0] = qc.redF();
    color[1] = qc.greenF();
    color[2] = qc.blueF();
    color[3] = qc.alphaF();
}

Color4f::Color4f(const Color4f &c) {
    for (int i = 0; i < 4; ++i) {
        color[i] = c.color[i];
    }
}

float Color4f::operator[] (unsigned int i ) const
{
    assert(i < 4);
    return color[i];
}

float& Color4f::operator[] (unsigned int i) {
    assert(i < 4);
    return color[i];
}

bool Color4f::operator== (const Color4f &a) const
{
    //Do not affect alpha channel
    if (color[0]==a.color[0] && color[1]==a.color[1] && color[2]==a.color[2])
    {
        return true;
    }
    else
    {
        return false;
    }
}

void glDrawCylinder(GLUquadric* pObj, const Vector3D& p1, const Vector3D& p2, double thickness, float renderDetailLevel)
{
    int numSlices = (8 * renderDetailLevel);
    int numStacks = 1;
    static Vector3D zAxis(0.0, 0.0, 1.0);

    Vector3D vec(p2 - p1);
    float length = vec.length();
    float rotAngle = Rad2Deg* acos( vec.z / length );
    Vector3D rotAxis = vector_cross(zAxis, vec);

    glPushMatrix();
    glTranslatef(p1.x, p1.y, p1.z);
    glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
    gluCylinder(pObj, thickness, thickness, length, numSlices, numStacks);
    glPopMatrix();
}

/* class Helix3D : public Object3D */
Helix3D::Helix3D(const Color4f& cl, const Vector3D& c, const Vector3D& n, float r )
    : Object3D(cl), cterm(c), nterm(n), radius(r)
{
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    Vector3D vec(nterm - cterm);
    length = vec.length();
    rotAngle = Rad2Deg* acos( vec.z / length );
    Vector3D zAxis(0.0, 0.0, 1.0);
    rotAxis = vector_cross(zAxis, vec);
}


void Helix3D::draw(float renderDetailLevel)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color.getConstData());
    int numSlices = 10 * renderDetailLevel;
    int numStacks = 1;
    static float smallLength = 1.2f;

    radius=1.0f;

    glPushMatrix();
        glTranslatef(cterm.x, cterm.y, cterm.z);
        glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
        glPushMatrix();
            glScalef(1,1,-1);
            gluDisk(pObj, 0, radius, numSlices, numSlices);
        glPopMatrix();
        gluCylinder(pObj, radius, radius, length - smallLength, numSlices, numStacks);
        glTranslatef(0, 0, length - smallLength);
        glPushMatrix();
            glScalef(1,1,-1);
            gluDisk(pObj, 0, radius*1.2f, numSlices, numSlices);
        glPopMatrix();
        gluCylinder(pObj, radius*1.2f, radius*0.4f, smallLength, numSlices, numStacks);
        glTranslatef(0, 0, smallLength);
        gluDisk(pObj, 0, radius*0.4f, numSlices, numStacks);
    glPopMatrix();
}

Helix3D::~Helix3D()
{
    gluDeleteQuadric(pObj);
}

/* class Strand3D : public Object3D */
Strand3D::Strand3D( const Color4f& cl, const Vector3D& c, const Vector3D& n, const Vector3D& up )
    : Object3D(cl), cterm(c), nterm(n), upVector(up)
{

    Vector3D vec(nterm - cterm);
    length = vec.length();
    rotAngle = Rad2Deg* acos( vec.z / length );
    Vector3D zAxis(0.0, 0.0, 1.0);
    rotAxis = vector_cross(zAxis, vec);

}

void Strand3D::draw(float renderDetailLevel)
{
    Q_UNUSED(renderDetailLevel);

    float width = 1.5f;
    float height = 0.4f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color.getConstData());

    glPushMatrix();
    glTranslatef(cterm.x, cterm.y, cterm.z);
    glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
    glCreateArrowPrimitive(width, height, length);
    glPopMatrix();

}

Strand3D::~Strand3D()
{

}


void glDrawHalfWorm(const Vector3D& p0, const Vector3D& p1,
                                  const Vector3D& p2, const Vector3D& p3,
                                  double radius, bool cap1, bool cap2,
                                  double tension, float renderDetailLevel)
{
    int i, j, k, m, offset=0;
    Vector3D R1, R2, Qt, p, dQt, H, V;
    double len, MG[4][3], T[4], t, prevlen=0.0, cosj, sinj;
    GLfloat *Nx=NULL, *Ny=NULL, *Nz=NULL, *Cx=NULL, *Cy=NULL, *Cz=NULL,
        *pNx=NULL, *pNy=NULL, *pNz=NULL, *pCx=NULL, *pCy=NULL, *pCz=NULL, *tmp;

    /*
    * The Hermite matrix Mh.
    */
    static double Mh[4][4] = {
        { 2, -2,  1,  1},
        {-3,  3, -2, -1},
        { 0,  0,  1,  0},
        { 1,  0,  0,  0}
    };

    /*
    * Variables that affect the curve shape
    *   a=b=0 = Catmull-Rom
    */
    double a = tension,         /* tension    (adjustable)  */
        c = 0,                  /* continuity (should be 0) */
        b = 0;                  /* bias       (should be 0) */

    int wormSegments = 10 * renderDetailLevel;
    int wormSides = 10 * renderDetailLevel;

    /*
    if (wormSides % 2) {
        WARNINGMSG("worm sides must be an even number");
        ++wormSides;
    }
    */

    GLfloat *fblock = NULL;

    /* First, calculate the coordinate points of the center of the worm,
    * using the Kochanek-Bartels variant of the Hermite curve.
    */
    R1 = 0.5 * (1 - a) * (1 + b) * (1 + c) * (p1 - p0) + 0.5 * (1 - a) * (1 - b) * (1 - c) * ( p2 - p1);
    R2 = 0.5 * (1 - a) * (1 + b) * (1 - c) * (p2 -  p1) + 0.5 * (1 - a) * (1 - b) * (1 + c) * (p3 - p2);

    /*
    * Multiply MG=Mh.Gh, where Gh = [ P(1) P(2) R(1) R(2) ]. This
    * 4x1 matrix of vectors is constant for each segment.
    */
    for (i = 0; i < 4; ++i) {   /* calculate Mh.Gh */
        MG[i][0] = Mh[i][0] * p1.x + Mh[i][1] * p2.x + Mh[i][2] * R1.x + Mh[i][3] * R2.x;
        MG[i][1] = Mh[i][0] * p1.y + Mh[i][1] * p2.y + Mh[i][2] * R1.y + Mh[i][3] * R2.y;
        MG[i][2] = Mh[i][0] * p1.z + Mh[i][1] * p2.z + Mh[i][2] * R1.z + Mh[i][3] * R2.z;
    }

    for (i = 0; i <= wormSegments; ++i) {

        /* t goes from [0,1] from P(1) to P(2) (and we want to go halfway only),
        and the function Q(t) defines the curve of this segment. */
        t = (0.5 / wormSegments) * i;
        /*
        * Q(t)=T.(Mh.Gh), where T = [ t^3 t^2 t 1 ]
        */
        T[0] = t * t * t;
        T[1] = t * t;
        T[2] = t;
        //T[3] = 1;
        Qt.x = T[0] * MG[0][0] + T[1] * MG[1][0] + T[2] * MG[2][0] + MG[3][0] /* *T[3] */ ;
        Qt.y = T[0] * MG[0][1] + T[1] * MG[1][1] + T[2] * MG[2][1] + MG[3][1] /* *T[3] */ ;
        Qt.z = T[0] * MG[0][2] + T[1] * MG[1][2] + T[2] * MG[2][2] + MG[3][2] /* *T[3] */ ;

        if (radius == 0.0) {
            if (i > 0) {
                glBegin(GL_LINES);
                glVertex3d(p.x, p.y, p.z);
                glVertex3d(Qt.x, Qt.y, Qt.z);
                glEnd();
            }
            /* save to use as previous point for connecting points together */
            p = Qt;

        } else {
            /* construct a circle of points centered at and
            in a plane normal to the curve at t - these points will
            be used to construct the "thick" worm */

            /* allocate single block of storage for two circles of points */
            if (!Nx) {
                fblock = new GLfloat[12 * wormSides];
                Nx = fblock;
                Ny = &Nx[wormSides];
                Nz = &Nx[wormSides * 2];
                Cx = &Nx[wormSides * 3];
                Cy = &Nx[wormSides * 4];
                Cz = &Nx[wormSides * 5];
                pNx = &Nx[wormSides * 6];
                pNy = &Nx[wormSides * 7];
                pNz = &Nx[wormSides * 8];
                pCx = &Nx[wormSides * 9];
                pCy = &Nx[wormSides * 10];
                pCz = &Nx[wormSides * 11];
            }

            /*
            * The first derivative of Q(t), d(Q(t))/dt, is the slope
            * (tangent) at point Q(t); now T = [ 3t^2 2t 1 0 ]
            */
            T[0] = t * t * 3;
            T[1] = t * 2;
            //T[2] = 1;
            //T[3] = 0;
            dQt.x = T[0] * MG[0][0] + T[1] * MG[1][0] + MG[2][0] /* *T[2] + T[3]*MG[3][0] */ ;
            dQt.y = T[0] * MG[0][1] + T[1] * MG[1][1] + MG[2][1] /* *T[2] + T[3]*MG[3][1] */ ;
            dQt.z = T[0] * MG[0][2] + T[1] * MG[1][2] + MG[2][2] /* *T[2] + T[3]*MG[3][2] */ ;

            /* use cross product of [1,0,0] x normal as horizontal */
            H.set(0.0, -dQt.z, dQt.y);
            if (H.length() < 0.000001) /* nearly colinear - use [1,0.1,0] instead */
                H.set(0.1 * dQt.z, -dQt.z, dQt.y - 0.1 * dQt.x);
            H.normalize();

            /* and a vertical vector = normal x H */
            V = vector_cross(dQt, H);
            V.normalize();

            /* finally, the worm circumference points (C) and normals (N) are
            simple trigonometric combinations of H and V */
            for (j = 0; j < wormSides; ++j) {
                cosj = cos(2 * PI * j / wormSides);
                sinj = sin(2 * PI * j / wormSides);
                Nx[j] = H.x * cosj + V.x * sinj;
                Ny[j] = H.y * cosj + V.y * sinj;
                Nz[j] = H.z * cosj + V.z * sinj;
                Cx[j] = Qt.x + Nx[j] * radius;
                Cy[j] = Qt.y + Ny[j] * radius;
                Cz[j] = Qt.z + Nz[j] * radius;
            }

            /* figure out which points on the previous circle "match" best
            with these, to minimize envelope twisting */
            if (i > 0) {
                for (m = 0; m < wormSides; ++m) {
                    len = 0.0;
                    for (j = 0; j < wormSides; ++j) {
                        k = j + m;
                        if (k >= wormSides)
                            k -= wormSides;
                        len += (Cx[k] - pCx[j]) * (Cx[k] - pCx[j]) +
                            (Cy[k] - pCy[j]) * (Cy[k] - pCy[j]) +
                            (Cz[k] - pCz[j]) * (Cz[k] - pCz[j]);
                    }
                    if (m == 0 || len < prevlen) {
                        prevlen = len;
                        offset = m;
                    }
                }
            }

            /* create triangles from points along this and previous circle */
            if (i > 0) {
                glBegin(GL_TRIANGLE_STRIP);
                for (j = 0; j < wormSides; ++j) {
                    k = j + offset;
                    if (k >= wormSides) k -= wormSides;
                    glNormal3d(Nx[k], Ny[k], Nz[k]);
                    glVertex3d(Cx[k], Cy[k], Cz[k]);
                    glNormal3d(pNx[j], pNy[j], pNz[j]);
                    glVertex3d(pCx[j], pCy[j], pCz[j]);
                }
                glNormal3d(Nx[offset], Ny[offset], Nz[offset]);
                glVertex3d(Cx[offset], Cy[offset], Cz[offset]);
                glNormal3d(pNx[0], pNy[0], pNz[0]);
                glVertex3d(pCx[0], pCy[0], pCz[0]);
                glEnd();
            }

            /* put caps on the end */
            if (cap1 && i == 0) {
                dQt.normalize();
                glBegin(GL_POLYGON);
                glNormal3d(-dQt.x, -dQt.y, -dQt.z);
                for (j = wormSides - 1; j >= 0; --j) {
                        glVertex3d(Cx[j], Cy[j], Cz[j]);
                }
                glEnd();
            }
            else if (cap2 && i == wormSegments) {
                dQt.normalize();
                glBegin(GL_POLYGON);
                glNormal3d(dQt.x, dQt.y, dQt.z);
                for (j = 0; j < wormSides; ++j) {
                    k = j + offset;
                    if (k >= wormSides) k -= wormSides;
                        glVertex3d(Cx[k], Cy[k], Cz[k]);
                }
                glEnd();
            }

            /* store this circle as previous for next round; instead of copying
            all values, just swap pointers */
#define SWAPPTR(p1,p2) tmp=(p1); (p1)=(p2); (p2)=tmp
            SWAPPTR(Nx, pNx);
            SWAPPTR(Ny, pNy);
            SWAPPTR(Nz, pNz);
            SWAPPTR(Cx, pCx);
            SWAPPTR(Cy, pCy);
            SWAPPTR(Cz, pCz);
        }
    }

    delete[] fblock;
}

void glDrawHalfBond( GLUquadric *pObj, const Vector3D& p1, const Vector3D&p2, double thickness, float renderDetailLevel)
{
    Vector3D middle = (p1 + p2) / 2;
    glDrawCylinder(pObj, p2, middle, thickness, renderDetailLevel);
}

void glDrawAtom( GLUquadric* pObj, const Vector3D& pos, double r, float renderDetailLevel)
{
    int numSlices = 10  * renderDetailLevel;
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    gluSphere(pObj, r, numSlices, numSlices);
    glPopMatrix();
}

/*
/ This function and helix drawing approach are taken from VMD (www.ks.uiuc.edu/Research/vmd/)
/ Find x = a*i + b where i = 0..n-1
*/
static void least_squares(int n, const float *x, float *a, float *b) {
    float sum = 0;
    int i;
    for (i=0; i<n; i++) {    // find the sum of x
        sum += x[i];
    }
    float d = (float(n)-1.0f) / 2.0f;
    float t, sum_t2 = 0.0f;
    *a = 0.0f;
    for (i=0; i<n; i++) {
        t = (i - d);
        sum_t2 += t*t;
        *a += t*x[i];
    }
    *a /= sum_t2;
    *b = (sum/float(n) - d*(*a));
}



QPair<Vector3D,Vector3D> calcBestAxisThroughPoints( const QVector<Vector3D>& points )
{
    float a[3], b[3];
    int n = points.count();
    QVector<float> buf;
    buf.resize(3*n);
    for (int i = 0; i < n; i++) {
        buf[i] = points[i].x;
        buf[n + i] = points[i].y;
        buf[2*n + i] = points[i].z;
    }

    least_squares(n, buf.data(), a, b);
    least_squares(n, buf.data() + n, a + 1, b + 1);
    least_squares(n, buf.data() + 2*n, a + 2, b + 2);

    Vector3D pointA(b[0], b[1], b[2]);
    Vector3D pointB(a[0]*(n-1) + b[0], a[1]*(n-1) + b[1], a[2]*(n-1) + b[2]);

    return QPair<Vector3D,Vector3D>(pointA, pointB);
}

Vector3D calcMiddlePoint( const QVector<Vector3D>& points )
{
    Vector3D point(0,0,0);
    foreach (Vector3D v, points) {
        point += v;
    }
    return (point / points.count());
}

Vector3D projectPointOnAxis( const Vector3D& point, const Vector3D& axisUnitVector, const Vector3D& axisPoint )
{
    Vector3D projection(point - axisPoint);
    float projectedLength = vector_dot(projection,axisUnitVector);
    projection = projectedLength*axisUnitVector + axisPoint;

    return projection;

}

void glCreateArrowPrimitive( float width, float height, float length )
{
    float widthOffset = 1;
    float heightOffset = 1;

    GLfloat b00[3] = {-width / 2, -height / 2, 0 };
    GLfloat b01[3] = {-width / 2, height / 2, 0 };
    GLfloat b02[3] = {width / 2, height / 2, 0 };
    GLfloat b03[3] = {width / 2, -height / 2 , 0 };
    GLfloat b04[3] = {-width / 2, -height / 2, length };
    GLfloat b05[3] = {-width / 2, height / 2, length };
    GLfloat b06[3] = {width / 2, height / 2, length };
    GLfloat b07[3] = {width / 2, -height / 2, length };
    GLfloat b08[3] = {- (width + widthOffset) / 2, height / 2, length };
    GLfloat b09[3] = {- (width + widthOffset) / 2, -height / 2, length };
    GLfloat b10[3] = { (width  + widthOffset) / 2, height / 2, length };
    GLfloat b11[3] = {(width + widthOffset) / 2, -height / 2, length };
    GLfloat b12[3] = {0, height / 2, length + heightOffset  };
    GLfloat b13[3] = {0, -height / 2, length + heightOffset };



    static GLfloat n00[3] = { 1, 0, 0 } ;
    static GLfloat n01[3] = { -1, 0, 0 } ;
    static GLfloat n02[3] = { 0, 1, 0 } ;
    static GLfloat n03[3] = { 0, -1, 0 } ;
    static GLfloat n05[3] = { 0, 0, -1 } ;
    static GLfloat n06[3] = { -0.7071f, 0, 0.7071f } ;
    static GLfloat n07[3] = { 0.7071f, 0, 0.7071f } ;


    glBegin(GL_QUADS);

    // Draw arrow body as brick

    glNormal3fv(n02);
    glVertex3fv(b05);
    glVertex3fv(b06); //glNormal3fv(n02);
    glVertex3fv(b02); //glNormal3fv(n02);
    glVertex3fv(b01); //glNormal3fv(n02);

    glNormal3fv(n01);
    glVertex3fv(b04);
    glVertex3fv(b05); //glNormal3fv(n01);
    glVertex3fv(b01); //glNormal3fv(n01);
    glVertex3fv(b00); //glNormal3fv(n01);

    glNormal3fv(n03);
    glVertex3fv(b07);
    glVertex3fv(b04); //glNormal3fv(n03);
    glVertex3fv(b00); //glNormal3fv(n03);
    glVertex3fv(b03); //glNormal3fv(n03);

    glNormal3fv(n00);
    glVertex3fv(b06);
    glVertex3fv(b07); //glNormal3fv(n00);
    glVertex3fv(b03); //glNormal3fv(n00);
    glVertex3fv(b02); //glNormal3fv(n00);

    glNormal3fv(n05);
    glVertex3fv(b00);
    glVertex3fv(b01); //glNormal3fv(n05);
    glVertex3fv(b02); //glNormal3fv(n05);
    glVertex3fv(b03); //glNormal3fv(n05);

    // Draw arrow head

    glNormal3fv(n05);
    glVertex3fv(b09);
    glVertex3fv(b08);
    glVertex3fv(b10);
    glVertex3fv(b11);

    glNormal3fv(n06);
    glVertex3fv(b08);
    glVertex3fv(b09);
    glVertex3fv(b13);
    glVertex3fv(b12);

    glNormal3fv(n07);
    glVertex3fv(b12);
    glVertex3fv(b13);
    glVertex3fv(b11);
    glVertex3fv(b10);

    glEnd();

    glBegin(GL_TRIANGLES);

    // Continue arrow head

    glNormal3fv(n02);
    glVertex3fv(b08);
    glVertex3fv(b12); //glNormal3fv(n02);
    glVertex3fv(b10); //glNormal3fv(n02);

    glNormal3fv(n03);
    glVertex3fv(b13);
    glVertex3fv(b09); //glNormal3fv(n03);
    glVertex3fv(b11); //glNormal3fv(n03);

    glEnd();

}

void accFrustum(GLdouble left, GLdouble right, GLdouble bottom,
                GLdouble top, GLdouble _near, GLdouble _far, GLdouble pixdx,
                GLdouble pixdy, GLdouble eyedx, GLdouble eyedy,
                GLdouble focus)
{
    GLdouble xwsize, ywsize;
    GLdouble dx, dy;
    GLint viewport[4];

    glGetIntegerv (GL_VIEWPORT, viewport);

    xwsize = right - left;
    ywsize = top - bottom;
    dx = -( pixdx*xwsize / (GLdouble) viewport[2] + eyedx * _near / focus );
    dy = -(pixdy*ywsize/(GLdouble) viewport[3] + eyedy * _near/ focus);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (left + dx, right + dx, bottom + dy, top + dy, _near, _far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef (-eyedx, -eyedy, 0.0);
}

void accPerspective(GLdouble fovy, GLdouble aspect,
                    GLdouble _near, GLdouble _far, GLdouble pixdx, GLdouble pixdy,
                    GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
    GLdouble fov2,left,right,bottom,top;
    fov2 = ((fovy*PI) / 180.0) / 2.0;

    top = _near / (cos(fov2) / sin(fov2));
    bottom = -top;
    right = top * aspect;
    left = -right;

    accFrustum (left, right, bottom, top, _near, _far, pixdx, pixdy, eyedx, eyedy, focus);
}

} //namespace
