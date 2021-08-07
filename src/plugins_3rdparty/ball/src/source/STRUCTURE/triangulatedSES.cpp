#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/solventExcludedSurface.h>
#include <BALL/STRUCTURE/triangle.h>
#include <BALL/STRUCTURE/triangleEdge.h>
#include <BALL/STRUCTURE/trianglePoint.h>
#include <BALL/STRUCTURE/triangulatedSES.h>
#include <BALL/STRUCTURE/triangulatedSurface.h>
#include <BALL/MATHS/analyticalGeometry.h>
#include <BALL/MATHS/angle.h>
#include <BALL/MATHS/circle3.h>
#include <BALL/MATHS/matrix44.h>
#include <BALL/MATHS/plane3.h>
#include <BALL/MATHS/quaternion.h>
#include <BALL/MATHS/sphere3.h>
#include <BALL/MATHS/vector3.h>
#include <BALL/MATHS/vector4.h>
#include <list>
#include <vector>


namespace BALL
{

    TriangulatedSES::TriangulatedSES()
        :	TriangulatedSurface(),
            ses_(NULL),
            density_(4.5)
    {
    }

    TriangulatedSES::TriangulatedSES
            (const TriangulatedSES& surface, bool deep)
        :	TriangulatedSurface(surface,deep),
            ses_(surface.ses_),
            density_(surface.density_)
    {
    }

    TriangulatedSES::TriangulatedSES(SolventExcludedSurface* ses, const double& density)
        :	TriangulatedSurface(),
            ses_(ses),
            density_(density)
    {
    }

    TriangulatedSES::~TriangulatedSES()
    {
    }

    void TriangulatedSES::set(const TriangulatedSES& surface, bool)
    {
        if (this != &surface)
        {
            TriangulatedSurface::set(surface);
            ses_ = surface.ses_;
            density_ = surface.density_;
        }
    }

    TriangulatedSES& TriangulatedSES::operator = (const TriangulatedSES& surface)
    {
        if (this != &surface)
        {
            TriangulatedSurface::operator = (surface);
            ses_ = surface.ses_;
            density_ = surface.density_;
        }
        return *this;
    }

    void TriangulatedSES::setDensity(const double& density)
    {
        density_ = density;
    }

    double TriangulatedSES::getDensity() const
    {
        return density_;
    }

    void TriangulatedSES::compute()
        throw(Exception::GeneralException,Exception::DivisionByZero)
    {
        SESTriangulator sest(this);
        sest.run();
    }


    SESTriangulator::SESTriangulator()
        :	tses_(0),
            point_(),
            edge_(),
            template_spheres_(),
            sqrt_density_(0.0)
    {
    }

    SESTriangulator::SESTriangulator(TriangulatedSES* tses)
        :	tses_(tses),
            point_(tses->ses_->number_of_vertices_),
            edge_(tses->ses_->number_of_edges_),
            template_spheres_(),
            sqrt_density_(sqrt(tses_->density_))
    {
    }

    SESTriangulator::~SESTriangulator()
    {
        std::list<TrianglePoint*>::iterator i;
        for (i = template_spheres_[0].begin(); i != template_spheres_[0].end(); i++)
        {
            delete *i;
        }
        for (i = template_spheres_[1].begin(); i != template_spheres_[1].end(); i++)
        {
            delete *i;
        }
        for (i = template_spheres_[2].begin(); i != template_spheres_[2].end(); i++)
        {
            delete *i;
        }
        for (i = template_spheres_[3].begin(); i != template_spheres_[3].end(); i++)
        {
            delete *i;
        }
    }

    void SESTriangulator::run()
        throw(Exception::GeneralException,Exception::DivisionByZero)
    {
        preProcessing();
        triangulateToricFaces();
        partitionSingularEdges();
        triangulateContactFaces();
        triangulateSphericFaces();
        //std::list<TrianglePoint*>::iterator p;
        //for (p = tses_->points_.begin(); p != tses_->points_.end(); p++)
        //{
        //	(*p)->normal_.normalize();
        //}
    }

    void SESTriangulator::preProcessing()
    {
        tses_->ses_->clean(tses_->density_);
        tses_->ses_->splitSphericFaces();
        for (Position i = 0;  i < tses_->ses_->number_of_vertices_; i++)
        {
            point_[i] = new TrianglePoint;
            point_[i]->point_ = tses_->ses_->vertices_[i]->point_;
            point_[i]->normal_ = tses_->ses_->vertices_[i]->normal_;
            tses_->points_.push_back(point_[i]);
            tses_->number_of_points_++;
        }
        buildTemplateSpheres();
    }


    void SESTriangulator::triangulateToricFaces()

    {
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 1e-4;
        double probe_radius(tses_->ses_->reduced_surface_->probe_radius_);
        for (Position i = 0; i < tses_->ses_->number_of_toric_faces_; i++)
        {
            triangulateToricFace(tses_->ses_->toric_faces_[i],probe_radius);
        }
        Constants::EPSILON = epsilon;
    }


    void SESTriangulator::partitionSingularEdges()
    {
        std::list<SESEdge*>::iterator e;
        for (e = tses_->ses_->singular_edges_.begin();
                 e != tses_->ses_->singular_edges_.end();
                 e++)
        {
            partitionSingularEdge(*e);
        }
    }


    void SESTriangulator::triangulateContactFaces()
    {
        SESFace* current_face;
        TSphere3<double> sphere;
        for (Position i = 0; i < tses_->ses_->number_of_contact_faces_; i++)
        {
            current_face = tses_->ses_->contact_faces_[i];
            sphere.set(tses_->ses_->reduced_surface_
                                                                ->atom_[current_face->rsvertex_->atom_]);
            triangulateContactFace(current_face,sphere);
        }
    }


    void SESTriangulator::triangulateSphericFaces()
    {
        SESFace* current_face;
        TSphere3<double> sphere;
        sphere.radius = tses_->ses_->reduced_surface_->probe_radius_;
        std::list<SESFace*> not_triangulated_faces;
        for (Position i = 0; i < tses_->ses_->number_of_spheric_faces_; i++)
        {
            current_face = tses_->ses_->spheric_faces_[i];
            sphere.p.set(current_face->rsface_->center_);
            if (!triangulateSphericFace(current_face,sphere))
            {
                not_triangulated_faces.push_back(current_face);
            }
        }
        Position end = not_triangulated_faces.size();
        Position counter = 0;
        while ((!not_triangulated_faces.empty()) && (counter < end))
        {
            current_face = not_triangulated_faces.front();
            not_triangulated_faces.pop_front();
            bool ok = false;
            std::list<SESEdge*>::iterator e
                    = current_face->edge_.begin();
            while (!ok && (e != current_face->edge_.end()))
            {
                if (edge_[(*e)->index_].front()->face_[0] != NULL)
                {
                    sphere.p.set(current_face->rsface_->center_);
                    SESEdge::Type type = (*e)->type_;
                    (*e)->type_ = SESEdge::TYPE_CONCAVE;
                    ok = triangulateSphericFace(current_face,sphere);
                    (*e)->type_ = type;
                }
                e++;
            }
            if (ok)
            {
                end--;
                counter = 0;
            }
            else
            {
                not_triangulated_faces.push_back(current_face);
                counter++;
            }
        }
    }


    void SESTriangulator::triangulateToricFace
            (SESFace*	face,
             const double&			probe_radius)
    {
        if (face->isFree())
        {
            triangulateFreeToricFace(face,probe_radius);
        }
        else
        {
            if (face->rsedge_->isSingular())
            {
                triangulateSingularToricFace(face,probe_radius);
            }
            else
            {
                triangulateNonSingularToricFace(face,probe_radius);
            }
        }
    }


    void SESTriangulator::triangulateNonSingularToricFace
            (SESFace*	face,
             const double&			probe_radius)
    {
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        SESEdge* edge0 = *e;
        e++;
        SESEdge* edge1 = *e;
        e++;
        SESEdge* edge2 = *e;
        e++;
        SESEdge* edge3 = *e;
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        SESVertex* p0 = *v;
        v++;
        SESVertex* p1 = *v;

        // In how much segments the edge should be triangulated? This depends on the angle of the
        // corresponding RSEdge and the density.
        Size number_of_segments =
                (Size)Maths::round(face->rsedge_->angle_.value*edge3->circle_.radius*sqrt_density_);

        if (number_of_segments == 0)
        {
            number_of_segments++;
        }

        TAngle<double> psi(face->rsedge_->angle_.value/number_of_segments,true);

        // Get the normal vector of the rotation used to partition the SESEdges of the SESFace ...
        const TCircle3<double>& circle3 = edge3->circle_;
        const TCircle3<double>& circle1 = edge1->circle_;

        TVector3<double> normal = circle3.n;

        // ... and now partition the SESEdges ...
        vector< TVector3<double> > edge3_segments(number_of_segments+1);
        partitionOfCircle(circle3,edge3->vertex_[0]->point_,psi,number_of_segments,edge3_segments);

        edge3_segments[number_of_segments] = edge3->vertex_[1]->point_;

        if (edge3->vertex_[0] != p0)
        {
            TVector3<double> tmp;
            for (Position i = 0; i < (number_of_segments+1)/2; i++)
            {
                tmp = edge3_segments[i];
                edge3_segments[i] = edge3_segments[number_of_segments-i];
                edge3_segments[number_of_segments-i] = tmp;
            }
            normal.negate();
        }

        vector< TVector3<double> > edge1_segments(number_of_segments+1);
        partitionOfCircle(circle1,edge1->vertex_[0]->point_,psi,number_of_segments,edge1_segments);

        edge1_segments[number_of_segments] = edge1->vertex_[1]->point_;

        if (edge1->vertex_[0] != p1)
        {
            TVector3<double> tmp;
            for (Position i = 0; i < (number_of_segments+1)/2; i++)
            {
                tmp = edge1_segments[i];
                edge1_segments[i] = edge1_segments[number_of_segments-i];
                edge1_segments[number_of_segments-i] = tmp;
            }
        }
        TCircle3<double> center_circle(face->rsedge_->center_of_torus_,
                                                            normal,
                                                            face->rsedge_->radius_of_torus_);
        // ... and the "center_circle". This is the circle on which the centers oft the circle lie which
        // define the segments of the face.
        vector< TVector3<double> > centers(number_of_segments+1);
        partitionOfCircle(center_circle,edge0->circle_.p,psi,number_of_segments,centers);
        centers[number_of_segments] = edge2->circle_.p;

        // save an iterator to the last triangle
        //
        // workaround for MSVC from Andreas Moll: 22.07.06
        std::list<Triangle*>::iterator last_triangle = tses_->triangles_.end();
        bool at_start = false;
        if (tses_->number_of_triangles_ == 0)
        {
            at_start = true;
        }
        else
        {
            last_triangle--;
        }

        buildTriangles(edge0,edge1,edge2,edge3,centers,edge1_segments,edge3_segments, probe_radius);

        if (at_start)
        {
            last_triangle = tses_->triangles_.begin();
        }
        else
        {
            last_triangle++;
        }
        Triangle* test_triangle = *last_triangle;
        // swap the triangles if necessary
        TVector3<double> orth( (test_triangle->vertex_[1]->point_-test_triangle->vertex_[0]->point_) %
                                            (test_triangle->vertex_[2]->point_-test_triangle->vertex_[0]->point_));
        if (Maths::isGreater(orth*test_triangle->vertex_[0]->point_,orth*centers[0]))
        {
            std::list<Triangle*>::iterator t;
            for (t = last_triangle; t != tses_->triangles_.end(); t++)
            {
                TrianglePoint* temp = (*t)->vertex_[0];
                (*t)->vertex_[0] = (*t)->vertex_[1];
                (*t)->vertex_[1] = temp;
            }
        }
    }


    void SESTriangulator::triangulateFreeToricFace
            (SESFace*	face,
             const double&			probe_radius)
    {
        TVector3<double> normal(face->edge_.front()->circle_.n);
        TCircle3<double> circle1(face->edge_.front()->circle_);
        TCircle3<double> circle2(face->edge_.back()->circle_.p,
                                                normal,
                                                face->edge_.back()->circle_.radius);
        TCircle3<double> circle3(face->rsedge_->center_of_torus_,
                                                normal,
                                                face->rsedge_->radius_of_torus_);
        TVector3<double> orth(normal.y,-normal.x,0.0);
        if (orth == TVector3<double>::getZero())
        {
            orth.set(normal.z,0.0,-normal.x);
        }
        orth.normalize();
        TVector3<double> p1(circle1.p+(orth*circle1.radius));
        TVector3<double> p2(circle2.p+(orth*circle2.radius));
        TVector3<double> p3(circle3.p+(orth*circle3.radius));
        Size number_of_points
                = (Size)Maths::round(2*Constants::PI*circle1.radius*sqrt_density_);
        if (number_of_points == 0)
        {
            number_of_points++;
        }
        TAngle<double> phi(2*Constants::PI/number_of_points,true);
        std::vector< TVector3<double> > points1(number_of_points+1);
        std::vector< TVector3<double> > points2(number_of_points+1);
        std::vector< TVector3<double> > centers(number_of_points+1);
        partitionOfCircle(circle1,p1,phi,number_of_points,points1);
        partitionOfCircle(circle2,p2,phi,number_of_points,points2);
        partitionOfCircle(circle3,p3,phi,number_of_points,centers);
        points1.pop_back();
        points2.pop_back();
        centers.pop_back();
        SESEdge* dummy(NULL);
        buildTriangles(dummy,face->edge_.front(),dummy,face->edge_.back(),
                                     centers,points1,points2,probe_radius);
    }


    void SESTriangulator::triangulateSingularToricFace
            (SESFace*	face,
             const double&			probe_radius)

    {
        face->normalize(true);
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        //std::vector<SESEdge*> edge(6);
        SESEdge* edge[6];
        edge[0] = *e;
        e++;
        edge[1] = *e;
        e++;
        edge[2] = *e;
        e++;
        edge[3] = *e;
        e++;
        edge[4] = *e;
        e++;
        edge[5] = *e;
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        //std::vector<SESVertex*> vertex(6);
        SESVertex* vertex[6];
        vertex[0] = *v;
        v++;
        vertex[1] = *v;
        v++;
        vertex[2] = *v;
        v++;
        vertex[3] = *v;
        v++;
        vertex[4] = *v;
        v++;
        vertex[5] = *v;
        Size number_of_segments
                = (Size)Maths::round(face->rsedge_->angle_.value*
                                                         edge[1]->circle_.radius*sqrt_density_);
        if (number_of_segments == 0)
        {
            number_of_segments++;
        }
        TAngle<double> psi(face->rsedge_->angle_.value/number_of_segments,true);
        Position offset = 0;
        std::list<Triangle*>::iterator last = tses_->triangles_.end();
        last--;
        for (Position counter = 0; counter < 2; counter++)
        {
            std::vector< TVector3<double> > edge_segments(number_of_segments+1);
            partitionOfCircle(edge[0+offset]->circle_,
                                                edge[0+offset]->vertex_[0]->point_,
                                                psi,
                                                number_of_segments,
                                                edge_segments);
            edge_segments[number_of_segments] = edge[0+offset]->vertex_[1]->point_;

            TVector3<double> axis(edge[0+offset]->circle_.n);
            if (edge[0+offset]->vertex_[0] != vertex[0+offset])
            {
                TVector3<double> tmp;
                for (Position i = 0; i < (number_of_segments+1)/2; i++)
                {
                    tmp = edge_segments[i];
                    edge_segments[i] = edge_segments[number_of_segments-i];
                    edge_segments[number_of_segments-i] = tmp;
                }
                axis.negate();
            }
            std::vector< TVector3<double> > point;
            for (Position i = 0; i < number_of_segments+1; i++)
            {
                point.push_back(vertex[1+offset]->point_);
            }
            TCircle3<double> center_circle(face->rsedge_->center_of_torus_,
                                                                axis,
                                                                face->rsedge_->radius_of_torus_);

            vector< TVector3<double> > centers(number_of_segments+1);
            partitionOfCircle(center_circle,edge[1+offset]->circle_.p,psi,
                                                number_of_segments,centers);
            centers[number_of_segments] = edge[2+offset]->circle_.p;

            SESEdge* dummy(NULL);
            buildTriangles(edge[1+offset],edge[0+offset],edge[2+offset],
                                         dummy,centers,edge_segments,point,probe_radius);
            offset += 3;
            Triangle* test_triangle;
            std::list<Triangle*>::iterator start;
            last++;
            test_triangle = *last;
            start = last;
            last = tses_->triangles_.end();
            last--;
            TVector3<double> orth( (test_triangle->vertex_[1]->point_-
                                                        test_triangle->vertex_[0]->point_) %
                                                (test_triangle->vertex_[2]->point_-
                                                        test_triangle->vertex_[0]->point_));
            if (Maths::isGreater(orth*(test_triangle->vertex_[0]->point_-centers[0]),
                                                     0.0))
            {
                std::list<Triangle*>::iterator t;
                TrianglePoint* temp;
                for (t = start; t != tses_->triangles_.end(); t++)
                {
                    temp = (*t)->vertex_[0];
                    (*t)->vertex_[0] = (*t)->vertex_[1];
                    (*t)->vertex_[1] = temp;
                }
            }
        }
    }


    void SESTriangulator::partitionSingularEdge(SESEdge* singular_edge)

    {
        if (singular_edge->vertex_[0] == NULL)
        {
            partitionFreeSingularEdge(singular_edge);
        }
        else
        {
            partitionNonFreeSingularEdge(singular_edge);
        }
    }


    void SESTriangulator::partitionFreeSingularEdge
            (SESEdge* singular_edge)

    {
        TAngle<double> phi;
        phi.value = 2*Constants::PI;
        Size number_of_segments
                = (Size)Maths::round(phi.value*
                                                         singular_edge->circle_.radius*
                                                         sqrt_density_);
        if (number_of_segments == 0)
        {
            number_of_segments++;
        }
        TAngle<double> psi(phi.value/number_of_segments,true);
        std::vector< TVector3<double> > points(number_of_segments+1);
        partitionOfCircle(singular_edge->circle_,
                                            TVector3<double>::getZero(),psi,
                                            number_of_segments,points,false);
        points.pop_back();
        TrianglePoint* p0;
        TrianglePoint* p1;
        TrianglePoint* p2;
        TriangleEdge* e;
        p1 = new TrianglePoint;
        p1->point_ = points[0];
        p1->normal_ = singular_edge->circle_.p-p1->point_;
        tses_->points_.push_back(p1);
        tses_->number_of_points_++;
        p0 = p1;
        for (Position k = 1; k < points.size(); k++)
        {
            p2 = new TrianglePoint;
            p2->point_ = points[k];
            p2->normal_ = singular_edge->circle_.p-p2->point_;
            tses_->points_.push_back(p2);
            tses_->number_of_points_++;
            e = new TriangleEdge;
            e->vertex_[0] = p1;
            e->vertex_[1] = p2;
            tses_->edges_.push_back(e);
            tses_->number_of_edges_++;
            edge_[singular_edge->index_].push_back(e);
            p1->edges_.insert(e);
            p2->edges_.insert(e);
            p1 = p2;
        }
        e = new TriangleEdge;
        e->vertex_[0] = p1;
        e->vertex_[1] = p0;
        tses_->edges_.push_back(e);
        tses_->number_of_edges_++;
        edge_[singular_edge->index_].push_back(e);
        p1->edges_.insert(e);
        p0->edges_.insert(e);
    }


    void SESTriangulator::partitionNonFreeSingularEdge
            (SESEdge* singular_edge)

    {
        TAngle<double> phi(getOrientedAngle(singular_edge->vertex_[0]->point_-
                                                                                    singular_edge->circle_.p,
                                                                                singular_edge->vertex_[1]->point_-
                                                                                    singular_edge->circle_.p,
                                                                                singular_edge->circle_.n));
        Size number_of_segments
                = (Size)Maths::round(phi.value*
                                                         singular_edge->circle_.radius*
                                                         sqrt_density_);
        if (number_of_segments == 0)
        {
            number_of_segments++;
        }
        TAngle<double> psi(phi.value/number_of_segments,true);
        std::vector< TVector3<double> > points(number_of_segments+1);
        partitionOfCircle(singular_edge->circle_,
                                            singular_edge->vertex_[0]->point_,psi,
                                            number_of_segments,points);
        points.pop_back();
        points.push_back(singular_edge->vertex_[1]->point_);
        TrianglePoint* p1;
        TrianglePoint* p2;
        TriangleEdge* e;
        p1 = point_[singular_edge->vertex_[0]->index_];
        p1->normal_ = singular_edge->circle_.p-p1->point_;
        for (Position k = 1; k < points.size()-1; k++)
        {
            p2 = new TrianglePoint;
            p2->point_ = points[k];
            p2->normal_ = singular_edge->circle_.p-p2->point_;
            tses_->points_.push_back(p2);
            tses_->number_of_points_++;
            e = new TriangleEdge;
            e->vertex_[0] = p1;
            e->vertex_[1] = p2;
            tses_->edges_.push_back(e);
            tses_->number_of_edges_++;
            edge_[singular_edge->index_].push_back(e);
            p1->edges_.insert(e);
            p2->edges_.insert(e);
            p1 = p2;
        }
        p2 = point_[singular_edge->vertex_[1]->index_];
        p2->normal_ = singular_edge->circle_.p-p2->point_;
        e = new TriangleEdge;
        e->vertex_[0] = p1;
        e->vertex_[1] = p2;
        tses_->edges_.push_back(e);
        tses_->number_of_edges_++;
        edge_[singular_edge->index_].push_back(e);
        p1->edges_.insert(e);
        p2->edges_.insert(e);
    }


    void SESTriangulator::triangulateContactFace(SESFace*	face, const TSphere3<double>&	sphere)
    {
        Position number_of_edges = face->edge_.size();
        if (number_of_edges > 0)
        {
            // the face does not come from a free vertex
            if ((number_of_edges == 2) &&
                    (edge_[face->edge_.front()->index_].size() == 1) &&
                    (edge_[face->edge_.back()->index_].size() == 1)			)
            {
                return;
            }
            if ((number_of_edges == 3) &&
                    (edge_[face->edge_.front()->index_].size() == 1) &&
                    (edge_[(*(face->edge_.begin())++)->index_].size() == 1) &&
                    (edge_[face->edge_.back()->index_].size() == 1)			)
            {
                Triangle* triangle = new Triangle;
                HashSet<TrianglePoint*> points;
                points.insert(point_[face->edge_.front()->vertex_[0]->index_]);
                points.insert(point_[face->edge_.front()->vertex_[1]->index_]);
                points.insert(point_[face->edge_.back()->vertex_[0]->index_]);
                points.insert(point_[face->edge_.back()->vertex_[1]->index_]);
                HashSet<TrianglePoint*>::Iterator p = points.begin();
                for (Position i = 0; i < 3; i++)
                {
                    triangle->vertex_[i] = *p;
                    p++;
                }
                TVector3<double> normal((triangle->vertex_[0]->point_- triangle->vertex_[1]->point_	) %
                                        (triangle->vertex_[0]->point_- triangle->vertex_[2]->point_	)	 );
                if (Maths::isGreater(normal*(sphere.p-triangle->vertex_[1]->point_), 0.0))
                {
                    TrianglePoint* tmp = triangle->vertex_[0];
                    triangle->vertex_[0] = triangle->vertex_[1];
                    triangle->vertex_[1] = tmp;
                }
                tses_->triangles_.push_back(triangle);
                tses_->number_of_triangles_++;
                return;
            }

            // get a template sphere for the face to triangulate
            HashMap<Size, std::list<TrianglePoint*> >::ConstIterator s
                    = template_spheres_.find(numberOfRefinements(tses_->density_,	sphere.radius));

            std::list<TrianglePoint*>::const_iterator p;
            TrianglePoint* point;
            TriangulatedSES part;
            for (p = s->second.begin(); p != s->second.end(); p++)
            {
                point = new TrianglePoint;
                point->point_ = (*p)->point_;
                point->normal_ = (*p)->normal_;
                part.points_.push_back(point);
                part.number_of_points_++;
            }
            part.blowUp(sphere.radius);

            // cut the face with all its edges
            TPlane3<double> plane;
            std::list<SESEdge*>::iterator edge;
            for (edge = face->edge_.begin(); edge != face->edge_.end(); edge++)
            {
                plane.p = (*edge)->circle_.p-sphere.p;
                plane.n = (*edge)->circle_.n;
                part.cut(plane, 0.05);
            }
            part.shift(sphere.p);
            buildSphericTriangles(face, part, sphere);
            part.deleteIsolatedPoints();
            tses_->join(part);
        }
        else
        {
            // the face comes from a free vertex
            TriangulatedSphere part;
            part.icosaeder();
            part.refine(numberOfRefinements(tses_->density_,sphere.radius));
            part.blowUp(sphere.radius);
            part.shift(sphere.p);
            tses_->join(part);
        }
    }

    bool SESTriangulator::triangulateSphericFace(SESFace* face, const TSphere3<double>& sphere)
    {
        TriangulatedSES part;
        bool ok = buildSphericTriangles(face,part,sphere,false);
        if (ok)
        {
            tses_->join(part);
            return true;
        }
        else
        {
            return false;
        }
    }


    bool SESTriangulator::buildSphericTriangles(SESFace* face, TriangulatedSES&	part,
                                                const TSphere3<double>& sphere, bool convex)
    {
        HashSet<TrianglePoint*> points;
        std::list<TrianglePoint*>::iterator p;
        for (p = part.points_.begin(); p != part.points_.end(); p++)
        {
            points.insert(*p);
        }
        std::list<TriangleEdge*>::const_iterator c;
        std::list<SESEdge*>::const_iterator e;
        for (e = face->edge_.begin(); e != face->edge_.end(); e++)
        {
            for (c = edge_[(*e)->index_].begin();
                     c != edge_[(*e)->index_].end(); c++)
            {
                points.insert((*c)->vertex_[0]);
                points.insert((*c)->vertex_[1]);
            }
        }
        std::list<TriangleEdge*> border;
        if (!buildFirstTriangle(face,part,border,sphere,points,convex))
        {
            return false;
        }
        while (!border.empty())
        {
            TriangleEdge* edge = border.front();
            border.pop_front();
            Triangle* start_triangle = edge->face_[0];
            TrianglePoint* third_point
                    = start_triangle->third(edge->vertex_[0],edge->vertex_[1]);
            TVector3<double> normal((third_point->point_-edge->vertex_[0]->point_) %
                                                 (third_point->point_-edge->vertex_[1]->point_)	);
            double test_value = normal*third_point->point_;
            Index p0 = start_triangle->getRelativeIndex(edge->vertex_[0]);
            Index p1 = start_triangle->getRelativeIndex(edge->vertex_[1]);
            Index diff = p1-p0;
            if ((convex && ((diff == -1) || (diff == 2))) ||
                    (!convex && ((diff == 1) || (diff == -2)))	)
            {
                TrianglePoint* tmp = edge->vertex_[0];
                edge->vertex_[0] = edge->vertex_[1];
                edge->vertex_[1] = tmp;
            }

            HashSet<TrianglePoint*>::Iterator next = points.begin();
            for (; +next; ++next)
            {
                TrianglePoint* tpoint = *next;
                if (tpoint != edge->vertex_[0] &&
                        tpoint != edge->vertex_[1] &&
                        tpoint != third_point)
                {
                    break;
                }
            }

            if (!+next) continue;

            std::list<TrianglePoint*> third;
            third.push_back(*next);
            normal.set(((*next)->point_-edge->vertex_[1]->point_) %
                                 ((*next)->point_-edge->vertex_[0]->point_)	);
            test_value = normal*edge->vertex_[0]->point_;
            next++;
            while (+next)
            {
                if ((*next != edge->vertex_[0]) && (*next != edge->vertex_[1]) &&
                        (*next != third_point))
                {
                    double this_value = normal*(*next)->point_;
                    if (Maths::isGreater(this_value,test_value))
                    {
                        third.clear();
                        third.push_back(*next);
                        normal.set(((*next)->point_-edge->vertex_[1]->point_) %
                                             ((*next)->point_-edge->vertex_[0]->point_)	);
                        test_value = normal*edge->vertex_[0]->point_;
                    }
                    else
                    {
                        if (Maths::isEqual(this_value,test_value))
                        {
                            third.push_back(*next);
                        }
                    }
                }
                next++;
            }
            switch (third.size())
            {
                case 0 :	break;
                case 1 :	buildUnambiguousTriangle(edge,third.front(),border,
                                                                                     sphere,part,convex);
                                    break;
                default :	buildAmbiguousTriangles(edge,third,border,
                                                                                    sphere,part,convex);
                                    break;
            }
        }
        return true;
    }


    bool SESTriangulator::buildFirstTriangle
            (SESFace*												face,
             TriangulatedSES&								part,
             std::list<TriangleEdge*>&			border,
             const TSphere3<double>&									sphere,
             const HashSet<TrianglePoint*>&	points,
                bool															convex)
    {
        SESEdge* first_sesedge = firstSESEdge(face->edge_);
        if (first_sesedge == NULL)
        {
            return false;
        }
        TriangleEdge* edge = edge_[first_sesedge->index_].front();
        HashSet<TrianglePoint*> same_edge;
        std::list<TriangleEdge*>::const_iterator e;
        for (e = edge_[first_sesedge->index_].begin();
                 e != edge_[first_sesedge->index_].end(); e++)
        {
            same_edge.insert((*e)->vertex_[0]);
            same_edge.insert((*e)->vertex_[1]);
        }
        // orientiere die edge so, wie sie im existierenden Dreieck NICHT
        // orientiert ist (so soll sie im neuen Dreieck orientiert sein)
        TVector3<double> edge_vector(edge->vertex_[0]->point_-edge->vertex_[1]->point_);
        Triangle* triangle = edge->face_[0];
        Index p0 = triangle->getRelativeIndex(edge->vertex_[0]);
        Index p1 = triangle->getRelativeIndex(edge->vertex_[1]);
        Index diff = p1-p0;
        if ((diff == 1) || (diff == -2))
        {
            TrianglePoint* tmp = edge->vertex_[0];
            edge->vertex_[0] = edge->vertex_[1];
            edge->vertex_[1] = tmp;
            edge_vector = -edge_vector;
        }
        HashSet<TrianglePoint*>::ConstIterator next = points.begin();
        HashSet<TrianglePoint*>::ConstIterator test;
        std::list<TrianglePoint*> third;
        TVector3<double> normal;
        while (next != points.end())
        {
            if (!same_edge.has(*next))
            {
                normal.set(edge_vector % (edge->vertex_[0]->point_-(*next)->point_));
                double test_value = normal*edge->vertex_[0]->point_;
                bool is_convex = true;
                test = points.begin();
                while (test != points.end())
                {
                    if ((*test != *next) &&
                            (*test != edge->vertex_[0]) &&
                            (*test != edge->vertex_[1]))
                    {
                        double this_value = normal*(*test)->point_;
                        if ((Maths::isGreater(this_value,test_value) && convex) ||
                                (Maths::isLess(this_value,test_value) && !convex))
                        {
                            is_convex = false;
                        }
                    }
                    test++;
                }
                if (is_convex)
                {
                    third.push_back(*next);
                }
            }
            next++;
        }
        std::list<TrianglePoint*> real_third;
        if (third.size() == 1)
        {
            real_third.push_back(third.front());
        }
        else
        {
            // normal vector of the old triangle (shows out of the molecule)
            normal.set((triangle->vertex_[0]->point_-triangle->vertex_[1]->point_) %
                                 (triangle->vertex_[0]->point_-triangle->vertex_[2]->point_)	);
            TAngle<double> min(3*Constants::PI,true);
            std::list<TrianglePoint*>::iterator p = third.begin();
            TVector3<double> new_normal;
            TAngle<double> angle;
            while (p != third.end())
            {
                new_normal.set(edge_vector % (edge->vertex_[0]->point_-(*p)->point_));
                angle = getOrientedAngle(normal,new_normal,edge_vector);
                if (angle <= min)
                {
                    if (angle < min)
                    {
                        real_third.clear();
                        min = angle;
                    }
                    real_third.push_back(*p);
                }
                p++;
            }
        }
        switch (real_third.size())
        {
            case 0 :	break;
            case 1 :	buildUnambiguousTriangle(edge,real_third.front(),border,
                                                                                 sphere,part,convex);
                                break;
            default :	buildAmbiguousTriangles(edge,real_third,border,
                                                                                sphere,part,convex);
                                break;
        }
        return true;
    }


    SESEdge* SESTriangulator::firstSESEdge
            (const std::list<SESEdge*>& sesedge)

    {
        bool found = false;
        std::list<SESEdge*>::const_iterator e = sesedge.begin();
        while (e != sesedge.end())
        {
            found = false;
            while ((found == false) && (e != sesedge.end()))
            {
                if ((*e)->type_ == SESEdge::TYPE_SINGULAR)
                {
                    e++;
                }
                else
                {
                    found = true;
                }
            }
            if (found)
            {
                if (edge_[(*e)->index_].size() == 1)
                {
                    TriangleEdge* edge = edge_[(*e)->index_].front();
                    TVector3<double> diff(edge->vertex_[0]->point_-edge->vertex_[1]->point_);
                    if (diff*diff < 0.01)
                    {
                        e++;
                    }
                    else
                    {
                        return *e;
                    }
                }
                else
                {
                    return *e;
                }
            }
        }
        return NULL;
    }


    void SESTriangulator::buildUnambiguousTriangle
            (TriangleEdge*						 edge,
             TrianglePoint*						 point,
             std::list<TriangleEdge*>& border,
             const TSphere3<double>&						 sphere,
             TriangulatedSES&					 part,
                bool													convex
            )

    {
        TriangleEdge* edge1;
        TriangleEdge* edge2;
        Triangle* triangle;
        bool old1, old2;
        createTriangleAndEdges(edge,point,sphere,
                                                     edge1,old1,edge2,old2,triangle,convex);
        if (old1 == true)
        {
            if (edge1->face_[0] == NULL)
            {
                edge1->face_[0] = triangle;
            }
            else
            {
                edge1->face_[1] = triangle;
            }
            border.remove(edge1);
        }
        else
        {
            edge1->vertex_[0]->edges_.insert(edge1);
            edge1->vertex_[1]->edges_.insert(edge1);
            edge1->face_[0] = triangle;
            part.edges_.push_back(edge1);
            part.number_of_edges_++;
            border.push_back(edge1);
        }
        if (old2 == true)
        {
            if (edge2->face_[0] == NULL)
            {
                edge2->face_[0] = triangle;
            }
            else
            {
                edge2->face_[1] = triangle;
            }
            border.remove(edge2);
        }
        else
        {
            edge2->vertex_[0]->edges_.insert(edge2);
            edge2->vertex_[1]->edges_.insert(edge2);
            edge2->face_[0] = triangle;
            part.edges_.push_back(edge2);
            part.number_of_edges_++;
            border.push_back(edge2);
        }
        edge->face_[1] = triangle;
        triangle->vertex_[0]->faces_.insert(triangle);
        triangle->vertex_[1]->faces_.insert(triangle);
        triangle->vertex_[2]->faces_.insert(triangle);
        part.triangles_.push_back(triangle);
        part.number_of_triangles_++;
    }


    void SESTriangulator::buildAmbiguousTriangles
            (TriangleEdge*						 edge,
             std::list<TrianglePoint*> points,
             std::list<TriangleEdge*>& border,
             const TSphere3<double>&						 sphere,
             TriangulatedSES&					 part,
                bool												 convex
            )

    {
        std::list<TrianglePoint*>::iterator p;
        points.push_back(edge->vertex_[0]);
        points.push_back(edge->vertex_[1]);
        std::list<TriangleEdge*> planar_edges;
        planar_edges.push_back(edge);
        TriangleEdge* edge0;
        TriangleEdge* edge1;
        TriangleEdge* edge2;
        Triangle* triangle;
        bool old1, old2;
        while (!planar_edges.empty())
        {
            edge0 = planar_edges.front();
            planar_edges.pop_front();
            p = points.begin();
            bool built = false;
            while ((p != points.end()) && (built == false))
            {
                if ((*p == edge0->vertex_[0]) || (*p == edge0->vertex_[1]))
                {
                    p++;
                }
                else
                {
                    createTriangleAndEdges(edge0,*p,sphere,edge1,old1,edge2,old2,triangle,convex);
                    if (edge0 == edge)
                    {
                        // the first triangle has not to be tested
                        built = true;
                    }
                    else
                    {
                        Triangle* old_triangle = edge0->face_[0];
                        Index i1 = old_triangle->getRelativeIndex(edge0->vertex_[0]);
                        Index i2 = old_triangle->getRelativeIndex(edge0->vertex_[1]);
                        bool back = false;
                        if ((i1-i2 == 1) || (i1-i2 == -2))
                        {
                            back = true;
                        }
                        i1 = triangle->getRelativeIndex(edge0->vertex_[0]);
                        i2 = triangle->getRelativeIndex(edge0->vertex_[1]);
                        if (back)
                        {
                            if ((i1-i2 == -1) || (i1-i2 == 2))
                            {
                                built = true;
                            }
                        }
                        else
                        {
                            if ((i1-i2 == 1) || (i1-i2 == -2))
                            {
                                built = true;
                            }
                        }
                    }
                    if (built)
                    {
                        edge0->face_[1] = triangle;
                        triangle->vertex_[0]->faces_.insert(triangle);
                        triangle->vertex_[1]->faces_.insert(triangle);
                        triangle->vertex_[2]->faces_.insert(triangle);
                        if (old1 == true)
                        {
                            if (edge1->face_[0] == NULL)
                            {
                                edge1->face_[0] = triangle;
                            }
                            else
                            {
                                edge1->face_[1] = triangle;
                            }
                            border.remove(edge1);
                            planar_edges.remove(edge1);
                        }
                        else
                        {
                            edge1->face_[0] = triangle;
                            border.push_back(edge1);
                            planar_edges.push_back(edge1);
                            edge1->vertex_[0]->edges_.insert(edge1);
                            edge1->vertex_[1]->edges_.insert(edge1);
                            part.edges_.push_back(edge1);
                            part.number_of_edges_++;
                        }
                        if (old2 == true)
                        {
                            if (edge2->face_[0] == NULL)
                            {
                                edge2->face_[0] = triangle;
                            }
                            else
                            {
                                edge2->face_[1] = triangle;
                            }
                            border.remove(edge2);
                            planar_edges.remove(edge2);
                        }
                        else
                        {
                            edge2->face_[0] = triangle;
                            border.push_back(edge2);
                            planar_edges.push_back(edge2);
                            edge2->vertex_[0]->edges_.insert(edge2);
                            edge2->vertex_[1]->edges_.insert(edge2);
                            part.edges_.push_back(edge2);
                            part.number_of_edges_++;
                        }
                        border.remove(edge0);
                        part.triangles_.push_back(triangle);
                        part.number_of_triangles_++;
                    }
                    else
                    {
                        p++;
                        delete triangle;
                        if (old1 == false)
                        {
                            delete edge1;
                        }
                        if (old2 == false)
                        {
                            delete edge2;
                        }
                    }
                }
            }
        }
    }


    void SESTriangulator::createTriangleAndEdges
            (TriangleEdge*	edge,
             TrianglePoint* point,
             const TSphere3<double>& sphere,
             TriangleEdge*& edge1,
             bool&							old1,
             TriangleEdge*& edge2,
             bool&							old2,
             Triangle*&			triangle,
             bool								convex
            )

    {
        // build two new edges
        edge1	= new TriangleEdge;
        edge1->vertex_[0] = edge->vertex_[0];
        edge1->vertex_[1] = point;
        edge2 = new TriangleEdge;
        edge2->vertex_[0] = point;
        edge2->vertex_[1] = edge->vertex_[1];
        // do the edges exist ?
        TriangleEdge* test = edge->vertex_[0]->has(edge1);
        if (test != NULL)
        {
            delete edge1;
            edge1 = test;
            old1 = true;
        }
        else
        {
            old1 = false;
        }
        test = edge->vertex_[1]->has(edge2);
        if (test != NULL)
        {
            delete edge2;
            edge2 = test;
            old2 = true;
        }
        else
        {
            old2 = false;
        }
        // build the new triangle
        triangle = new Triangle;
        triangle->vertex_[0] = edge->vertex_[1];
        triangle->vertex_[1] = edge->vertex_[0];
        triangle->vertex_[2] = point;
        triangle->edge_[0] = edge;
        triangle->edge_[1] = edge1;
        triangle->edge_[2] = edge2;
        // swap triangle if necessary
        TVector3<double> test_vector((triangle->vertex_[1]->point_-
                                                                triangle->vertex_[0]->point_)%
                                                        (triangle->vertex_[2]->point_-
                                                                triangle->vertex_[0]->point_)	);
        double test_value = test_vector*(sphere.p-triangle->vertex_[0]->point_);
        if ((Maths::isGreater(test_value,0) && convex) ||
                (Maths::isLess(test_value,0) && !convex)			)
        {
            TrianglePoint* temp = triangle->vertex_[0];
            triangle->vertex_[0] = triangle->vertex_[1];
            triangle->vertex_[1] = temp;
        }
    }


    void SESTriangulator::partitionOfCircle(const TCircle3<double>&	circle, const TVector3<double>&	p0, const TAngle<double>&	phi,
                                            Size number_of_segments, vector< TVector3<double> >& partition, bool on_surface)
    {
        TVector4<double> p;
        if (on_surface)
        {
            TVector3<double> p_(p0-circle.p);
            p.set(p_.x, p_.y, p_.z, 0.0);
        }
        else
        {
            p.set(circle.n.y,-circle.n.x,0.0,0.0);
            if (p == TVector4<double>::getZero())
            {
                p.set(circle.n.z,0.0,-circle.n.x,0.0);
            }
            p.normalize();
            p *= circle.radius;
        }

        TQuaternion<double> rotate(circle.n, phi);
        TMatrix4x4<double>  rotation;
        rotate.getRotationMatrix(rotation);

        partition[0] = TVector3<double>(p.x,p.y,p.z)+circle.p;
        for (Size i = 0; i < number_of_segments; i++)
        {
            p = rotation*p;
            partition[i+1] = TVector3<double>(p.x,p.y,p.z)+circle.p;
        }
    }


    Size SESTriangulator::numberOfRefinements(const double& density, const double& radius)
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
        if (n > 3)
        {
            n = 3;
        }
        return n;
    }


    void SESTriangulator::buildTemplateSpheres()

    {
        TriangulatedSphere::ConstPointIterator p;
        TrianglePoint* point;
        TriangulatedSphere sphere;
        sphere.icosaeder(true);
        for (p = sphere.beginPoint(); p != sphere.endPoint(); p++)
        {
            point = new TrianglePoint(**p);
            template_spheres_[0].push_back(point);
        }
        sphere.refine(1,true);
        for (p = sphere.beginPoint(); p != sphere.endPoint(); p++)
        {
            point = new TrianglePoint(**p);
            template_spheres_[1].push_back(point);
        }
        sphere.refine(1,true);
        for (p = sphere.beginPoint(); p != sphere.endPoint(); p++)
        {
            point = new TrianglePoint(**p);
            template_spheres_[2].push_back(point);
        }
        sphere.refine(1,true);
        for (p = sphere.beginPoint(); p != sphere.endPoint(); p++)
        {
            point = new TrianglePoint(**p);
            template_spheres_[3].push_back(point);
        }
    }


    void SESTriangulator::buildTriangles
         (SESEdge*											edge0,
            SESEdge*											edge1,
            SESEdge*											edge2,
            SESEdge*											edge3,
            const std::vector< TVector3<double> >&	centers,
            const std::vector< TVector3<double> >&	edge1_points,
            const std::vector< TVector3<double> >&	edge3_points,
            const double&													probe_radius)

    {
        TAngle<double> pi(Constants::PI,true);
        TAngle<double> psi;
        // How much triangles should be built in each segment of the face?
        // Attention: number_of_triangles is the half of the number of triangles
        // for each segment.
        GetAngle(edge1_points[0]-centers[0],edge3_points[0]-centers[0],psi);
        Size number_of_triangles
                = (Size)Maths::round(psi.value*probe_radius*sqrt_density_);
        if (number_of_triangles == 0)
        {
            number_of_triangles++;
        }
        TAngle<double> phi(psi/number_of_triangles,true);
        Size number_of_segments = centers.size()-1;
        // How much TrianglePoints lie on the face?
        // Remark: edge3 == NULL iff the face is singular
        Size number_of_points
                = ((edge3 != NULL) ? (number_of_segments+1)*(number_of_triangles+1)
                                                     : (number_of_segments+1)* number_of_triangles + 1);
        std::vector<TrianglePoint*> points(number_of_points);
        if (edge3 == NULL)
        {
            // singular face
            // get the index of the SESVertex on the top of the face
            // (remember: singular faces are two triangles)
            Index index;
            if (edge0->vertex_[0]->point_ == edge3_points[0])
            {
                index = edge0->vertex_[0]->index_;
            }
            else
            {
                index = edge0->vertex_[1]->index_;
            }
            TrianglePoint* top = point_[index];
            top->normal_ = centers[number_of_segments]-top->point_;
            points[(number_of_segments+1)*number_of_triangles] = top;
        }
        Position end
                = ((edge3 != NULL) ? number_of_triangles+1 : number_of_triangles);
        for (Position i = 0; i <= number_of_segments; i++)
        {
            TCircle3<double> circle(centers[i],
                                                 (edge1_points[i]-centers[i])%
                                                 (edge3_points[i]-centers[i]),
                                                 probe_radius);
            std::vector< TVector3<double> > line(number_of_triangles+1);
            partitionOfCircle(circle,edge1_points[i],phi,number_of_triangles,line);

            for (Position j = 0; j < end; j++)
            {
                // get the index of the point if it just exists (point on the corner)
                Index index = -1;
                if (edge0 != NULL)
                {
                    // not free face
                    if ((i == 0) && (j == 0))
                    {
                        double dist1 = line[0].getSquareDistance(edge0->vertex_[0]->point_);
                        double dist2 = line[0].getSquareDistance(edge0->vertex_[1]->point_);
                        index = ((dist1 < dist2) ? edge0->vertex_[0]->index_
                                                                         : edge0->vertex_[1]->index_);
                    }
                    if ((i == 0) && (j == number_of_triangles) && (edge3 != NULL))
                    {
                        double dist1 = line[number_of_triangles].getSquareDistance(edge0->vertex_[0]->point_);
                        double dist2 = line[number_of_triangles].getSquareDistance(edge0->vertex_[1]->point_);
                        index = ((dist1 < dist2) ? edge0->vertex_[0]->index_
                                                                         : edge0->vertex_[1]->index_);
                    }
                    if ((i == number_of_segments) && (j == 0))
                    {
                        double dist1 = line[0].getSquareDistance(edge2->vertex_[0]->point_);
                        double dist2 = line[0].getSquareDistance(edge2->vertex_[1]->point_);
                        index = ((dist1 < dist2) ? edge2->vertex_[0]->index_
                                                                         : edge2->vertex_[1]->index_);
                    }
                    if ((i == number_of_segments) && (j == number_of_triangles) && (edge3 != NULL))
                    {
                        double dist1 = line[number_of_triangles].getSquareDistance(edge2->vertex_[0]->point_);
                        double dist2 = line[number_of_triangles].getSquareDistance(edge2->vertex_[1]->point_);
                        index = ((dist1 < dist2) ? edge2->vertex_[0]->index_
                                                                         : edge2->vertex_[1]->index_);
                    }
                }
                TrianglePoint* point;
                if (index == -1)
                {
                    // create a new point if it not exists yet
                    point = new TrianglePoint;
                    tses_->points_.push_back(point);
                    tses_->number_of_points_++;
                    point->point_ = line[j];
                    point->normal_ = centers[i]-line[j];
                }
                else
                {
                    point = point_[index];
                }
                points[i*end+j] = point;
            }
        }
        // number of edges to create
        Size edge_number = ((edge3 != NULL) ? 3*number_of_segments*number_of_triangles +
                                                                                    number_of_segments+number_of_triangles
                                                                                : 3*number_of_segments*number_of_triangles-
                                                                                    2*number_of_segments+number_of_triangles-1	 );
        std::vector<TriangleEdge*> edges(edge_number);
        // build the vertical edges
        Position offset;
        Position offset2;
        TriangleEdge* new_edge;
        for (Position j = 0; j <= number_of_segments; j++)
        {
            offset = j*end;
            for (Position i = 0; i < end-1; i++)
            {
                offset2 = offset+i;
                new_edge = new TriangleEdge;
                tses_->edges_.push_back(new_edge);
                tses_->number_of_edges_++;
                new_edge->vertex_[0] = points[offset2];
                new_edge->vertex_[0]->edges_.insert(new_edge);
                new_edge->vertex_[1] = points[offset2+1];
                new_edge->vertex_[1]->edges_.insert(new_edge);
                if ((j == 0) && (edge0 != 0))
                {
                    edge_[edge0->index_].push_back(new_edge);
                }
                else
                {
                    if ((j == number_of_segments) && (edge0 != 0))
                    {
                        edge_[edge2->index_].push_back(new_edge);
                    }
                }
                edges[offset2-j] = new_edge;
            }
        }
        // build the horizontal edges
        Position add1 = (end-1)*(number_of_segments+1);
        for (Position j = 0; j < number_of_segments; j++)
        {
            offset = j*end;
            for (Position i = 0; i < end; i++)
            {
                offset2 = offset+i;
                new_edge = new TriangleEdge;
                tses_->edges_.push_back(new_edge);
                tses_->number_of_edges_++;
                new_edge->vertex_[0] = points[offset2];
                new_edge->vertex_[0]->edges_.insert(new_edge);
                new_edge->vertex_[1] = points[offset2+end];
                new_edge->vertex_[1]->edges_.insert(new_edge);
                if ((i == 0) && (edge1 != 0))
                {
                    edge_[edge1->index_].push_back(new_edge);
                }
                else
                {
                    if ((i == end-1) && (edge3 != 0))
                    {
                        edge_[edge3->index_].push_back(new_edge);
                    }
                }
                edges[offset2+add1] = new_edge;
            }
        }
        // buid the diagonal edges
        Position add2 = add1+end*number_of_segments;
        for (Position j = 0; j < number_of_segments; j++)
        {
            offset = j*end-j;
            for (Position i = 0; i < end-1; i++)
            {
                offset2 = offset+i;
                new_edge = new TriangleEdge;
                tses_->edges_.push_back(new_edge);
                tses_->number_of_edges_++;
                new_edge->vertex_[0] = points[offset2+j];
                new_edge->vertex_[0]->edges_.insert(new_edge);
                new_edge->vertex_[1] = points[offset2+j+end+1];
                new_edge->vertex_[1]->edges_.insert(new_edge);
                edges[offset2+add2] = new_edge;
            }
        }
        // number of triangles to build
        Size triangle_number = 2*number_of_segments*(end-1);
        std::vector<Triangle*> triangles(triangle_number);
        // build the triangles
        for (Position i = 0; i < end-1; i++)
        {
            for (Position j = 0; j < number_of_segments; j++)
            {
                offset2 = j*end+i;
                Triangle* t1 = new Triangle;
                    tses_->triangles_.push_back(t1);
                    tses_->number_of_triangles_++;
                    t1->vertex_[0] = points[offset2];
                    t1->vertex_[1] = points[offset2+1];
                    t1->vertex_[2] = points[offset2+1+end];
                    t1->edge_[0] = edges[offset2-j];
                    t1->edge_[1] = edges[offset2+add1+1];
                    t1->edge_[2] = edges[offset2-j+add2];
                    t1->vertex_[0]->faces_.insert(t1);
                    t1->vertex_[1]->faces_.insert(t1);
                    t1->vertex_[2]->faces_.insert(t1);
                    t1->edge_[0]->face_[0] = t1;
                    t1->edge_[1]->face_[0] = t1;
                    t1->edge_[2]->face_[0] = t1;
                    triangles[2*(number_of_segments*i+j)] = t1;
                Triangle* t2 = new Triangle;
                    tses_->triangles_.push_back(t2);
                    tses_->number_of_triangles_++;
                    t2->vertex_[0] = points[offset2];
                    t2->vertex_[1] = points[offset2+1+end];
                    t2->vertex_[2] = points[offset2+end];
                    t2->edge_[0] = edges[offset2-j+end-1];
                    t2->edge_[1] = edges[offset2+add1];
                    t2->edge_[2] = edges[offset2-j+add2];
                    t2->vertex_[0]->faces_.insert(t2);
                    t2->vertex_[1]->faces_.insert(t2);
                    t2->vertex_[2]->faces_.insert(t2);
                    t2->edge_[0]->face_[0] = t2;
                    t2->edge_[1]->face_[0] = t2;
                    t2->edge_[2]->face_[0] = t2;
                    triangles[2*(number_of_segments*i+j)+1] = t2;
            }
        }
        // close free faces
        if (edge0 == NULL)
        {
            offset = number_of_segments*end;
            TriangleEdge* edge = new TriangleEdge;
                tses_->edges_.push_back(edge);
                tses_->number_of_edges_++;
                edge->vertex_[0] = points[offset];
                edge->vertex_[1] = points[0];
                points[offset]->edges_.insert(edge);
                points[0]->edges_.insert(edge);
                edge_[edge1->index_].push_back(edge);
            for (Position i = 0; i < number_of_triangles; i++)
            {
                offset2 = offset+i;
                Triangle* t1 = new Triangle;
                    tses_->triangles_.push_back(t1);
                    tses_->number_of_triangles_++;
                    t1->vertex_[0] = points[offset2];
                    t1->vertex_[1] = points[i+1];
                    t1->vertex_[2] = points[i];
                    t1->edge_[0] = edge;
                    edge = new TriangleEdge;
                        tses_->edges_.push_back(edge);
                        tses_->number_of_edges_++;
                        edge->vertex_[0] = points[offset2];
                        edge->vertex_[1] = points[i+1];
                        points[offset2]->edges_.insert(edge);
                        points[i+1]->edges_.insert(edge);
                    t1->edge_[1] = edge;
                    t1->edge_[2] = edges[i];
                    t1->vertex_[0]->faces_.insert(t1);
                    t1->vertex_[1]->faces_.insert(t1);
                    t1->vertex_[2]->faces_.insert(t1);
                    t1->edge_[0]->face_[0] = t1;
                    t1->edge_[1]->face_[0] = t1;
                    t1->edge_[2]->face_[0] = t1;
                    triangles.push_back(t1);
                Triangle* t2 = new Triangle;
                    tses_->triangles_.push_back(t2);
                    tses_->number_of_triangles_++;
                    t2->vertex_[0] = points[offset2];
                    t2->vertex_[1] = points[offset2+1];
                    t2->vertex_[2] = points[i+1];
                    t2->edge_[0] = edge;
                    t2->edge_[1] = edges[offset2-number_of_segments];
                    edge = new TriangleEdge;
                        tses_->edges_.push_back(edge);
                        tses_->number_of_edges_++;
                        edge->vertex_[0] = points[offset2+1];
                        edge->vertex_[1] = points[i+1];
                        points[offset2+1]->edges_.insert(edge);
                        points[i+1]->edges_.insert(edge);
                    t2->edge_[2] = edge;
                    t2->vertex_[0]->faces_.insert(t2);
                    t2->vertex_[1]->faces_.insert(t2);
                    t2->vertex_[2]->faces_.insert(t2);
                    t2->edge_[0]->face_[0] = t2;
                    t2->edge_[1]->face_[0] = t2;
                    t2->edge_[2]->face_[0] = t2;
                    triangles.push_back(t2);
            }
            edge_[edge3->index_].push_back(edge);
        }
        // get the top of a singular face
        if (edge3 == NULL)
        {
            Position last = (number_of_segments+1)*number_of_triangles;
            offset = (end-1)*(number_of_segments+2);
            TriangleEdge* edge = new TriangleEdge;
                tses_->edges_.push_back(edge);
                tses_->number_of_edges_++;
                edge->vertex_[0] = points[number_of_triangles-1];
                edge->vertex_[1] = points[last];
                edge->vertex_[0]->edges_.insert(edge);
                edge->vertex_[1]->edges_.insert(edge);
                edge_[edge0->index_].push_back(edge);
            for (Position i = 0; i < number_of_segments; i++)
            {
                Triangle* t = new Triangle;
                    tses_->triangles_.push_back(t);
                    tses_->number_of_triangles_++;
                    t->vertex_[0] = points[number_of_triangles*(i+1)-1];
                    t->vertex_[1] = points[last];
                    t->vertex_[2] = points[number_of_triangles*(i+2)-1];
                    t->edge_[0] = edge;
                    t->edge_[1] = edges[offset+i*end];
                    edge = new TriangleEdge;
                        tses_->edges_.push_back(edge);
                        tses_->number_of_edges_++;
                        edge->vertex_[0] = points[number_of_triangles*(i+2)-1];
                        edge->vertex_[1] = points[last];
                        edge->vertex_[0]->edges_.insert(edge);
                        edge->vertex_[1]->edges_.insert(edge);
                        if (i == number_of_segments-1)
                        {
                            edge_[edge2->index_].push_back(edge);
                        }
                    t->edge_[2] = edge;
                    t->vertex_[0]->faces_.insert(t);
                    t->vertex_[1]->faces_.insert(t);
                    t->vertex_[2]->faces_.insert(t);
                    t->edge_[0]->face_[0] = t;
                    t->edge_[1]->face_[0] = t;
                    t->edge_[2]->face_[0] = t;
                    triangles.push_back(t);
            }
        }
    }


}	// namespace BALL
