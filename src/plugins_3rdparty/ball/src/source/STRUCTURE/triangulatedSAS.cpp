#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

//#	include <BALL/KERNEL/atom.h>
//#	include <BALL/KERNEL/molecule.h>
//#	include <BALL/KERNEL/system.h>
//#	include <BALL/KERNEL/PTE.h>
//#	include <BALL/DATATYPE/string.h>

#include <BALL/STRUCTURE/solventAccessibleSurface.h>
#include <BALL/STRUCTURE/triangle.h>
#include <BALL/STRUCTURE/triangleEdge.h>
#include <BALL/STRUCTURE/trianglePoint.h>
#include <BALL/STRUCTURE/triangulatedSAS.h>
#include <BALL/STRUCTURE/triangulatedSurface.h>
#include <BALL/MATHS/angle.h>
#include <BALL/MATHS/matrix44.h>
#include <BALL/MATHS/plane3.h>
#include <BALL/MATHS/quaternion.h>
#include <BALL/MATHS/vector3.h>
#include <BALL/MATHS/vector4.h>

#include <list>
#include <vector>

namespace BALL
{

    void PartitionOfCircle(const TCircle3<double>& circle, std::list<Vector3>& partition)
    {
        Size density = 64;
        Vector3 center(circle.p.x,circle.p.y,circle.p.z);
        Vector3 normal(circle.n.x,circle.n.y,circle.n.z);
        Vector4 p;
        p.set(normal.y,-normal.x,0.0,0.0);
        if (p == Vector4::getZero())
        {
            p.set(normal.z,0.0,-normal.x,0.0);
        }
        p.normalize();
        p *= circle.radius;
        Quaternion rotate(normal,Angle(Constants::PI/density,true));
        Matrix4x4 rotation;
        rotate.getRotationMatrix(rotation);
        partition.push_back(Vector3(p.x,p.y,p.z)+center);
        for (Size i = 0; i < 2*density+1; i++)
        {
            p = rotation*p;
            partition.push_back(Vector3(p.x,p.y,p.z)+center);
        }
    }

    TriangulatedSAS::TriangulatedSAS()
     : TriangulatedSurface(),
       sas_(NULL),
       density_(4.5)
    {
    }

    TriangulatedSAS::TriangulatedSAS(const TriangulatedSAS& surface, bool deep)
     : TriangulatedSurface(surface,deep),
       sas_(surface.sas_),
       density_(surface.density_)
    {
    }

    TriangulatedSAS::TriangulatedSAS(SolventAccessibleSurface* sas, const double& density)
     : TriangulatedSurface(),
       sas_(sas),
       density_(density)
    {
    }

    TriangulatedSAS::~TriangulatedSAS()
    {
    }

    void TriangulatedSAS::set(const TriangulatedSAS& surface, bool)
    {
        if (this != &surface)
        {
            TriangulatedSurface::set(surface);
            sas_ = surface.sas_;
            density_ = surface.density_;
        }
    }

    TriangulatedSAS& TriangulatedSAS::operator = (const TriangulatedSAS& surface)
    {
        if (this != &surface)
        {
            TriangulatedSurface::operator = (surface);
            sas_ = surface.sas_;
            density_ = surface.density_;
        }

        return *this;
    }

    void TriangulatedSAS::setDensity(const double& density)
    {
        density_ = density;
    }

    double TriangulatedSAS::getDensity() const
    {
        return density_;
    }

    void TriangulatedSAS::compute()
    {
        SASTriangulator sast(this);
        sast.run();
    }

//////////////////////////////////////////////////

    SASTriangulator::SASTriangulator()
     : tsas_(NULL),
       sqrt_density_(0.0),
       edge_(),
       template_spheres_()
    {
    }

    SASTriangulator::SASTriangulator(TriangulatedSAS* tsas)
     : tsas_(tsas),
       sqrt_density_(sqrt(tsas->density_)),
       edge_(tsas_->sas_->number_of_edges_),
       template_spheres_()
    {
    }

    SASTriangulator::~SASTriangulator()
    {
    }

    void SASTriangulator::run()
    {
        // build template spheres with different densities and outside normal vectors
        buildTemplateSpheres();

        // use these to triangulate the sas faces
        for (Position i = 0; i < tsas_->sas_->number_of_faces_; i++)
        {
            triangulateFace(tsas_->sas_->faces_[i]);
        }
    }

    void SASTriangulator::triangulateFace(SASFace* face)
    {
        // store the planes of the SAS edges
        std::list< std::pair<TPlane3<double>, double> > planes;
        createPlanes(face,planes);

        // get template sphere
        TSphere3<double>* sphere = &face->sphere_;
        HashMap<Size,TriangulatedSurface>::ConstIterator s;

        s	= template_spheres_.find(numberOfRefinements(tsas_->density_, sphere->radius));

        TriangulatedSurface part = s->second;
        part.blowUp(sphere->radius);
        part.shift(sphere->p);

        // tag inside points
        tagPoints(part, planes);

        // remove inside triangles and then isolated edges and points
        removeInsideTriangles(part);
        part.deleteIsolatedEdges();
        part.deleteIsolatedPoints();

        // create HashGrid of triangle points
//		HashGrid3<TrianglePoint*> point_grid(createHashGrid(part));
        // create points on the cutting planes
//		createPoints(part,planes,point_grid);
        // create new triangles
//		createNewTriangles(part,point_grid);
        // remove isolated edges and points
//		part.deleteIsolatedEdges();
//		part.deleteIsolatedPoints();
        // join the triangulation of this sas face with the sas
        tsas_->join(part);
    }

    void SASTriangulator::createPlanes(SASFace* face,
         std::list< std::pair<TPlane3<double>, double> >& planes)
    {
        std::pair<TPlane3<double>, double> plane;
        std::list<SASEdge*>::iterator edge;
        std::list<bool>::iterator o = face->orientation_.begin();
        for (edge = face->edge_.begin(); edge != face->edge_.end(); edge++)
        {
            plane.first.p = (*edge)->circle_.p;
            if (*o)
            {
                plane.first.n = (*edge)->circle_.n;
            }
            else
            {
                plane.first.n = -(*edge)->circle_.n;
            }
            plane.second = plane.first.n*plane.first.p;
            planes.push_back(plane);
            o++;
        }
    }

    void SASTriangulator::tagPoints(TriangulatedSurface& part,
         const std::list< std::pair<TPlane3<double>, double> >& planes)
    {
        for (TriangulatedSurface::PointIterator p = part.beginPoint();
             p != part.endPoint();
             p++)
        {
            (*p)->index_ = 0;
            std::list< std::pair<TPlane3<double>, double> >::const_iterator plane = planes.begin();
            for (; plane != planes.end(); ++plane)
            {
                if (Maths::isLessOrEqual(plane->first.n*(*p)->point_, plane->second))
                {
                    (*p)->index_ = 1;
                    break;
                }
            }
        }
    }

    void SASTriangulator::removeInsideTriangles(TriangulatedSurface& part)
    {
        TriangulatedSurface::TriangleIterator t1, t2;

        t1 = part.beginTriangle();
        while (t1 != part.endTriangle())
        {
            if ((*t1)->vertex_[0]->index_ +
                (*t1)->vertex_[1]->index_ +
                (*t1)->vertex_[2]->index_  == 3)
            {
                t2 = t1;
                t2++;
                if (t2 == part.endTriangle())
                {
                    part.remove(t1);
                    t1 = part.endTriangle();
                }
                else
                {
                    part.remove(t1);
                    t1 = t2;
                }
            }
            else
            {
                t1++;
            }
        }
    }

    HashGrid3<TrianglePoint*> SASTriangulator::createHashGrid(const TriangulatedSurface& part)
    {
        double x_min = (*(part.beginPoint()))->point_.x;
        double y_min = (*(part.beginPoint()))->point_.y;
        double z_min = (*(part.beginPoint()))->point_.z;
        double x_max = (*(part.beginPoint()))->point_.x;
        double y_max = (*(part.beginPoint()))->point_.y;
        double z_max = (*(part.beginPoint()))->point_.z;
        TriangulatedSurface::ConstPointIterator p = part.beginPoint()++;
        while (p != part.endPoint())
        {
            x_min = Maths::min(x_min,(*p)->point_.x);
            y_min = Maths::min(y_min,(*p)->point_.y);
            z_min = Maths::min(z_min,(*p)->point_.z);
            x_max = Maths::max(x_min,(*p)->point_.x);
            y_max = Maths::max(y_max,(*p)->point_.y);
            z_max = Maths::max(z_max,(*p)->point_.z);
            p++;
        }
        //double dist = ses_->reduced_surface_->r_max_/3;
        double dist = 1.0;
        Position nx = (Position)((x_max-x_min)/dist+5);
        Position ny = (Position)((y_max-y_min)/dist+5);
        Position nz = (Position)((z_max-z_min)/dist+5);
        Vector3 origin(x_min-2*dist,y_min-2*dist,z_min-2*dist);
        HashGrid3<TrianglePoint*> grid = HashGrid3<TrianglePoint*>(origin,nx,ny,nz,dist);
        p = part.beginPoint()++;
        while (p != part.endPoint())
        {
            grid.insert(Vector3((*p)->point_.x,(*p)->point_.y,(*p)->point_.z),*p);
            p++;
        }
        return grid;
    }

    void SASTriangulator::createPoints(TriangulatedSurface& part,
             const std::list< std::pair<TPlane3<double>,double> >& planes,
             HashGrid3<TrianglePoint*>& grid)
    {
        TriangulatedSurface::EdgeIterator edge = part.beginEdge();
        std::list< std::pair<TPlane3<double>,double> >::const_iterator plane;
        while (edge != part.endEdge())
        {
            if ((*edge)->vertex_[0]->index_+(*edge)->vertex_[1]->index_ == 1)
            {
                // edge intersects one of the cutting planes
                TrianglePoint* v1;
                TrianglePoint* v2;
                if ((*edge)->vertex_[0]->index_ == 0)
                {
                    v1 = (*edge)->vertex_[0];
                    v2 = (*edge)->vertex_[1];
                }
                else
                {
                    v1 = (*edge)->vertex_[1];
                    v2 = (*edge)->vertex_[0];
                }
                TVector3<double> diff(v2->point_-v1->point_);
                TVector3<double> point(v2->point_);
                double min = 1.0;
                double div;
                Size counter = 0;
                for (plane = planes.begin(); plane != planes.end(); plane++)
                {
                    // intersecting point  =  v1 + lambda * (v2-v1)
                    div = plane->first.n*diff;
                    if (Maths::isNotZero(div))
                    {
                        double lambda = (plane->second-(plane->first.n*v1->point_))/div;
                        if (Maths::isGreaterOrEqual(lambda,0) &&
                             (Maths::isLessOrEqual(lambda,min)))
                        {
                            min = lambda;
                            point.set(v1->point_+(lambda*diff));
                            (*edge)->index_ = counter;
                        }
                    }
                    counter++;
                }
                v2->edges_.erase(*edge);
                TrianglePoint* new_point = vertexExists(point,grid);
                if (new_point == NULL)
                {
                    new_point = new TrianglePoint;
                    new_point->point_ = point;
                    new_point->index_ = -1;
                    new_point->edges_.insert(*edge),
                    part.insert(new_point);
                    grid.insert(Vector3(point.x,point.y,point.z),new_point);
                }
                if ((*edge)->vertex_[0] == v1)
                {
                    (*edge)->vertex_[1] = new_point;
                }
                else
                {
                    (*edge)->vertex_[0] = new_point;
                }
            }
            else
            {
                (*edge)->index_ = -1;
            }
            edge++;
        }
    }

    void SASTriangulator::createNewTriangles(TriangulatedSurface& part, HashGrid3<TrianglePoint*>& grid)
    {
        TriangulatedSurface::TriangleIterator t = part.beginTriangle();
        while (t != part.endTriangle())
        {
            Position type = 0;
            if ((*t)->vertex_[0]->index_ == 1)
            {
                type++;
            }
            if ((*t)->vertex_[1]->index_ == 1)
            {
                type += 2;
            }
            if ((*t)->vertex_[2]->index_ == 1)
            {
                type += 4;
            }
            switch (type)
            {
                case 0 :  break; // triangle not intersected
                case 1 :  onePointOutside(0,*t,part,grid);    break;
                case 2 :  onePointOutside(1,*t,part,grid);    break;
                case 3 :  twoPointsOutside(0,1,*t,part,grid); break;
                case 4 :  onePointOutside(2,*t,part,grid);    break;
                case 5 :  twoPointsOutside(2,0,*t,part,grid); break;
                case 6 :  twoPointsOutside(1,2,*t,part,grid); break;
                case 7 :  break; // should never happen
            }
            t++;
        }
    }

    void SASTriangulator::onePointOutside(Index outside, Triangle* t,
                                          TriangulatedSurface& part, HashGrid3<TrianglePoint*>& grid)
    {
        // get the relative indices of the intersected edges
        Position edge[3];
        Position i = 0;
        for (Position j = 0; j < 3; j++)
        {
            if (t->edge_[j]->index_ != -1)
            {
                edge[i] = j;
                i++;
            }
            else
            {
                edge[2] = j;
            }
        }
        // v1 = new vertex of the first intersected edge
        // v2 = old vertex of the first intersected edge
        // v3 = new vertex of the second intersected edge
        // v4 = old vertex of the second intersected edge
        TrianglePoint* v1;
        TrianglePoint* v2;
        TrianglePoint* v3;
        TrianglePoint* v4;
        Position test = ((t->edge_[edge[0]]->vertex_[0]->index_ == -1) ? 0 : 1);
        v1 = t->edge_[edge[0]]->vertex_[test];
        v2 = t->edge_[edge[0]]->vertex_[1-test];
        test = ((t->edge_[edge[1]]->vertex_[0]->index_ == -1) ? 0 : 1);
        v3 = t->edge_[edge[1]]->vertex_[test];
        v4 = t->edge_[edge[1]]->vertex_[1-test];

        // get the relative index of v4 which is needed
        // to compute the correct orientation of the first new triangle
        Index index = 0;
        for (Position j = 0; j < 3; j++)
        {
            if (t->vertex_[j] == v4)
            {
                index = j;
            }
        }
        // resize the original triangle
        t->vertex_[outside]->faces_.erase(t);
        t->vertex_[outside] = v1;
        v1->faces_.insert(t);
        // create a new triangle
        Triangle* new_triangle = new Triangle;
        new_triangle->vertex_[0] = v1;
        if ((outside-index == 1) || (outside-index == -2))
        {
            new_triangle->vertex_[1] = v4;
            new_triangle->vertex_[2] = v3;
        }
        else
        {
            new_triangle->vertex_[1] = v3;
            new_triangle->vertex_[2] = v4;
        }
        v1->faces_.insert(new_triangle);
        v3->faces_.insert(new_triangle);
        v4->faces_.insert(new_triangle);
        part.insert(new_triangle);
        // does the triangle intersect two different SAS edges?
        if (t->edge_[edge[0]]->index_ != t->edge_[edge[1]]->index_)
        {
            // ACHTUNG: pos MUSS NOCH BERECHNET WERDEN !!!
            TVector3<double> pos(v1->point_);
            TrianglePoint* new_point = vertexExists(pos,grid);
            if (new_point == NULL)
            {
                new_point = new TrianglePoint;
                new_point->point_ = pos;
                new_point->index_ = -1;
                part.insert(new_point);
                grid.insert(Vector3(pos.x,pos.y,pos.z),new_point);
            }
            Triangle* third_triangle = new Triangle;
            third_triangle->vertex_[0] = v1;
            if ((outside-index == 1) || (outside-index == -2))
            {
                third_triangle->vertex_[1] = v3;
                third_triangle->vertex_[2] = new_point;
            }
            else
            {
                third_triangle->vertex_[1] = new_point;
                third_triangle->vertex_[2] = v3;
            }
            v1->faces_.insert(third_triangle);
            v3->faces_.insert(third_triangle);
            new_point->faces_.insert(third_triangle);
            part.insert(third_triangle);
        }
    }

    void SASTriangulator::twoPointsOutside(Position outside1, Position outside2,
                                           Triangle* t, TriangulatedSurface& part,
                                           HashGrid3<TrianglePoint*>& grid)
    {
        // get the relative indices of the intersected edges
        Position edge[3];
        Position i = 0;
        for (Position j = 0; j < 3; j++)
        {
            if (t->edge_[j]->index_ != -1)
            {
                edge[i] = j;
                i++;
            }
            else
            {
                edge[2] = j;
            }
        }
        // v1 = new vertex of the first intersected edge
        // v2 = old vertex of the first intersected edge
        // v3 = new vertex of the second intersected edge
        TrianglePoint* v1;
        TrianglePoint* v2;
        TrianglePoint* v3;
        Position test = ((t->edge_[edge[0]]->vertex_[0]->index_ == -1) ? 0 : 1);
        v1 = t->edge_[edge[0]]->vertex_[test];
        v2 = t->edge_[edge[0]]->vertex_[1-test];
        test = ((t->edge_[edge[1]]->vertex_[0]->index_ == -1) ? 0 : 1);
        v3 = t->edge_[edge[1]]->vertex_[test];
        // resize the original triangle
        t->vertex_[outside1]->faces_.erase(t);
        t->vertex_[outside2]->faces_.erase(t);
        TLine3<double> line(v1->point_,v2->point_,TLine3<double>::FORM__TWO_POINTS);
        bool one = line.has(t->vertex_[outside1]->point_);
        if (one)
        {
            t->vertex_[outside1] = v1;
            t->vertex_[outside2] = v3;
        }
        else
        {
            t->vertex_[outside1] = v3;
            t->vertex_[outside2] = v1;
        }
        v1->faces_.insert(t);
        v2->faces_.insert(t);
        // does the triangle intersect two different SAS edges?
        if (t->edge_[edge[0]]->index_ != t->edge_[edge[1]]->index_)
        {
            // ACHTUNG: pos MUSS NOCH BERECHNET WERDEN !!!
            TVector3<double> pos(v1->point_);
            TrianglePoint* new_point = vertexExists(pos,grid);
            if (new_point == NULL)
            {
                new_point = new TrianglePoint;
                new_point->point_ = pos;
                new_point->index_ = -1;
                part.insert(new_point);
                grid.insert(Vector3(pos.x,pos.y,pos.z),new_point);
            }
            Triangle* new_triangle = new Triangle;
            new_triangle->vertex_[0] = t->vertex_[outside2];
            new_triangle->vertex_[1] = t->vertex_[outside1];
            new_triangle->vertex_[2] = new_point;
            v1->faces_.insert(new_triangle);
            v3->faces_.insert(new_triangle);
            new_point->faces_.insert(new_triangle);
            part.insert(new_triangle);
        }
    }

    TrianglePoint* SASTriangulator::vertexExists(const TVector3<double>& point,
             HashGrid3<TrianglePoint*>& grid)
    {
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 0.001;
        Vector3 p(point.x,point.y,point.z);
        HashGridBox3<TrianglePoint*>* box = grid.getBox(p);
        HashGridBox3<TrianglePoint*>::ConstBoxIterator b;
        HashGridBox3<TrianglePoint*>::ConstDataIterator d;
        if (box != NULL)
        {
            for (b = box->beginBox(); b != box->endBox(); b++)
            {
                for (d = b->beginData(); d != b->endData(); d++)
                {
                    if ((*d)->point_ == point)
                    {
                        Constants::EPSILON = epsilon;
                        return *d;
                    }
                }
            }
        }
        Constants::EPSILON = epsilon;
        return NULL;
    }

    Size SASTriangulator::numberOfRefinements(const double& density, const double& radius)
    {
        double test0 = (4.0*density*Constants::PI*radius*radius-12.0)/30.0;
        Size n = 0;
        if (Maths::isGreaterOrEqual(test0,0.0))
        {
            double test1 = 1;
            double test2 = 1;
            while (Maths::isLess(test2,test0))
            {
                test1 = test2;
                test2 *= 4;
                n++;
            }
            if (Maths::isLess(test2-test0,test0-test1))
            {
                n++;
            }
        }
        if (n > 4)
        {
            n = 4;
        }
        return n;
    }

    void SASTriangulator::buildTemplateSpheres()
    {
        TriangulatedSphere sphere;
        sphere.icosaeder(true);
        sphere.setIndices();
        template_spheres_[0] = sphere;
        sphere.refine(1,true);
        sphere.setIndices();
        template_spheres_[1] = sphere;
        sphere.refine(1,true);
        sphere.setIndices();
        template_spheres_[2] = sphere;
        sphere.refine(1,true);
        sphere.setIndices();
        template_spheres_[3] = sphere;
        sphere.refine(1,true);
        sphere.setIndices();
        template_spheres_[4] = sphere;
    }

}	//namespace BALL
