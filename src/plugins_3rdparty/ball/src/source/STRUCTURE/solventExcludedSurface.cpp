#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/solventExcludedSurface.h>
#include <BALL/STRUCTURE/SESEdge.h>
#include <BALL/STRUCTURE/SESFace.h>
#include <BALL/STRUCTURE/SESVertex.h>
#include <BALL/STRUCTURE/reducedSurface.h>
#include <BALL/MATHS/analyticalGeometry.h>
#include <BALL/MATHS/angle.h>
#include <BALL/MATHS/circle3.h>
#include <BALL/MATHS/matrix44.h>
#include <BALL/MATHS/quaternion.h>
#include <BALL/MATHS/sphere3.h>
#include <BALL/MATHS/vector3.h>
#include <BALL/MATHS/vector4.h>
#include <BALL/DATATYPE/hashGrid.h>
#include <BALL/DATATYPE/hashMap.h>
#include <vector>
#include <list>
#include <iostream>


namespace BALL
{

    SolventExcludedSurface::SolventExcludedSurface()
        :	number_of_vertices_(0),
            vertices_(),
            number_of_edges_(0),
            edges_(),
            number_of_singular_edges_(0),
            singular_edges_(),
            number_of_contact_faces_(0),
            contact_faces_(),
            number_of_toric_faces_(0),
            toric_faces_(),
            number_of_spheric_faces_(0),
            spheric_faces_(),
            reduced_surface_(NULL)
    {
    }


    SolventExcludedSurface::SolventExcludedSurface
        (const SolventExcludedSurface& ses, bool)
        :	number_of_vertices_(ses.vertices_.size()),
            vertices_(number_of_vertices_),
            number_of_edges_(ses.edges_.size()),
            edges_(number_of_edges_),
            number_of_singular_edges_(0),
            singular_edges_(),
            number_of_contact_faces_(ses.contact_faces_.size()),
            contact_faces_(number_of_contact_faces_),
            number_of_toric_faces_(ses.toric_faces_.size()),
            toric_faces_(number_of_toric_faces_),
            number_of_spheric_faces_(ses.spheric_faces_.size()),
            spheric_faces_(number_of_spheric_faces_),
            reduced_surface_(NULL)
    {
        //for (Position i = 0; i < number_of_vertices_; i++)
        //{
        //	vertices_[i] = new SESVertex(*ses.vertices_[i],false);
        //}
        //for (Position i = 0; i < number_of_edges_; i++)
        //{
        //	edges_[i] = new SESVertex(*ses.edges_[i],false);
        //}
        //::std::list<SESEdge*>::const_iterator se;
        //for (se = ses.singular_edges_.begin(); se != ses.singular_edges_.end(); se++)
        //{
        //	singular_edges_.push_back(edges_[(*se)->index_]);
        //}
        //for (Position i = 0; i < number_of_contact_faces_; i++)
        //{
        //	contact_faces_[i] = new SESFace(*ses.contact_faces_[i],false);
        //}
        //for (Position i = 0; i < number_of_toric_faces_; i++)
        //{
        //	toric_faces_[i] = new SESFace(*ses.toric_faces_[i],false);
        //}
        //for (Position i = 0; i < number_of_spheric_faces_; i++)
        //{
        //	spheric_faces_[i] = new SESFace(*ses.spheric_faces_[i],false);
        //}
    }


    SolventExcludedSurface::SolventExcludedSurface
            (ReducedSurface* reduced_surface)
        :	number_of_vertices_(0),
            vertices_(),
            number_of_edges_(0),
            edges_(),
            number_of_singular_edges_(0),
            singular_edges_(),
            number_of_contact_faces_(0),
            contact_faces_(),
            number_of_toric_faces_(0),
            toric_faces_(),
            number_of_spheric_faces_(0),
            spheric_faces_(),
            reduced_surface_(reduced_surface)
    {
    }


    SolventExcludedSurface::~SolventExcludedSurface()
    {
        clear();
    }


    void SolventExcludedSurface::clear()
    {
        Position i;
        for (i = 0; i < number_of_vertices_; i++)
        {
            delete vertices_[i];
        }
        for (i = 0; i < number_of_edges_; i++)
        {
            delete edges_[i];
        }
        for (i = 0; i < number_of_contact_faces_; i++)
        {
            delete contact_faces_[i];
        }
        for (i = 0; i < number_of_toric_faces_; i++)
        {
            delete toric_faces_[i];
        }
        for (i = 0; i < number_of_spheric_faces_; i++)
        {
            delete spheric_faces_[i];
        }
        vertices_.clear();
        edges_.clear();
        singular_edges_.clear();
        contact_faces_.clear();
        toric_faces_.clear();
        spheric_faces_.clear();
        number_of_vertices_ = 0;
        number_of_edges_ = 0;
        number_of_singular_edges_ = 0;
        number_of_contact_faces_ = 0;
        number_of_toric_faces_ = 0;
        number_of_spheric_faces_ = 0;
    }


    void SolventExcludedSurface::clean(const double& density)
    {
        SESFace* face(0);
        bool done = false;
        double sqrt_density = sqrt(density);
        while (!done)
        {
            done = true;
            for (Position i = 0; i < toric_faces_.size(); i++)
            {
                face = toric_faces_[i];
                if (face != NULL)
                {
                    if (!face->isFree())
                    {
                        if (face->type_ == SESFace::TYPE_TORIC_SINGULAR)
                        {
                            if (cleanSingularToricFace(face,sqrt_density) == false)
                            {
                                done = false;
                            }
                        }
                        else
                        {
                            if (cleanToricFace(face,sqrt_density) == false)
                            {
                                done = false;
                            }
                        }
                    }
                }
            }
        }

        cleanVertices();
        cleanEdges();
        cleanContactFaces();
        cleanToricFaces();
        cleanSphericFaces();
    }


    bool SolventExcludedSurface::cleanSingularToricFace(SESFace* face, const double& sqrt_density)
    {
        face->normalize(true);
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        SESEdge* edge0 = *e;
        e++;
        e++;
        e++;
        SESEdge* edge3 = *e;
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        SESVertex* v0 = *v;
        v++;
        v++;
        SESVertex* v2 = *v;
        v++;
        SESVertex* v3 = *v;
        v++;
        v++;
        SESVertex* v5 = *v;
        bool del = false;
        bool set = false;
        SESEdge* edge = NULL;
        if (v0 == v2)
        {
            del = (edge0->rsedge_->angle_.value < Constants::PI);
            set = !del;
            edge = edge0;
        }
        else
        {
            if (v3 == v5)
            {
                del = (edge3->rsedge_->angle_.value < Constants::PI);
                set = !del;
                edge = edge3;
            }
            else
            {
                double exact_number_of_segments(face->rsedge_->angle_.value*
                                                                     edge3->circle_.radius*
                                                                     sqrt_density);
                del = (Maths::isLess(exact_number_of_segments,0.1));
            }
        }
        if (del)
        {
            deleteSmallSingularToricFace(face);
        }
        if (set)
        {
            edge->rsedge_->angle_.value = 2*Constants::PI;
        }
        return !del;
    }


    bool SolventExcludedSurface::cleanToricFace
            (SESFace* face,
             const double&			sqrt_density)

    {
        face->normalize(false);
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        e++;
        SESEdge* edge1 = *e;
        e++;
        e++;
        SESEdge* edge3 = *e;
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        SESVertex* v0 = *v;
        v++;
        SESVertex* v1 = *v;
        v++;
        SESVertex* v2 = *v;
        v++;
        SESVertex* v3 = *v;
        bool del = false;
        bool set = false;
        SESEdge* edge = NULL;
        if (v0 == v3)
        {
            del = (edge3->rsedge_->angle_.value < Constants::PI);
            set = !del;
            edge = edge3;
        }
        else
        {
            if (v1 == v2)
            {
                del = (edge1->rsedge_->angle_.value < Constants::PI);
                set = !del;
                edge = edge1;
            }
            else
            {
                double exact_number_of_segments(face->rsedge_->angle_.value*
                                                                     edge3->circle_.radius*
                                                                     sqrt_density);
                del =  (Maths::isLess(exact_number_of_segments,0.1));
            }
        }
        if (del)
        {
            deleteSmallToricFace(face);
        }
        if (set)
        {
            edge->rsedge_->angle_.value = 2*Constants::PI;
        }
        return !del;
    }


    void SolventExcludedSurface::cleanVertices()

    {
        while ((number_of_vertices_ > 0) &&
                     (vertices_[number_of_vertices_-1] == NULL))
        {
            vertices_.pop_back();
            number_of_vertices_--;
        }
        for (Position i = 0; i < number_of_vertices_; i++)
        {
            if (vertices_[i] == NULL)
            {
                vertices_[i] = vertices_[number_of_vertices_-1];
                vertices_[i]->index_ = i;
                vertices_.pop_back();
                number_of_vertices_--;
                while (vertices_[number_of_vertices_-1] == NULL)
                {
                    vertices_.pop_back();
                    number_of_vertices_--;
                }
            }
        }
    }


    void SolventExcludedSurface::cleanEdges()

    {
        while ((number_of_edges_ > 0) &&
                     (edges_[number_of_edges_-1] == NULL))
        {
            edges_.pop_back();
            number_of_edges_--;
        }
        for (Position i = 0; i < number_of_edges_; i++)
        {
            if (edges_[i] == NULL)
            {
                edges_[i] = edges_[number_of_edges_-1];
                edges_[i]->index_ = i;
                edges_.pop_back();
                number_of_edges_--;
                while (edges_[number_of_edges_-1] == NULL)
                {
                    edges_.pop_back();
                    number_of_edges_--;
                }
            }
        }
    }


    void SolventExcludedSurface::cleanContactFaces()

    {
        while ((number_of_contact_faces_ > 0) &&
                     (contact_faces_[number_of_contact_faces_-1] == NULL))
        {
            contact_faces_.pop_back();
            number_of_contact_faces_--;
        }
        for (Position i = 0; i < number_of_contact_faces_; i++)
        {
            if (contact_faces_[i] == NULL)
            {
                contact_faces_[i] = contact_faces_[number_of_contact_faces_-1];
                contact_faces_[i]->index_ = i;
                contact_faces_.pop_back();
                number_of_contact_faces_--;
                while (contact_faces_[number_of_contact_faces_-1] == NULL)
                {
                    contact_faces_.pop_back();
                    number_of_contact_faces_--;
                }
            }
        }
    }


    void SolventExcludedSurface::cleanToricFaces()

    {
        while ((number_of_toric_faces_ > 0) &&
                     (toric_faces_[number_of_toric_faces_-1] == NULL))
        {
            toric_faces_.pop_back();
            number_of_toric_faces_--;
        }
        for (Position i = 0; i < number_of_toric_faces_; i++)
        {
            if (toric_faces_[i] == NULL)
            {
                toric_faces_[i] = toric_faces_[number_of_toric_faces_-1];
                toric_faces_[i]->index_ = i;
                toric_faces_.pop_back();
                number_of_toric_faces_--;
                while (toric_faces_[number_of_toric_faces_-1] == NULL)
                {
                    toric_faces_.pop_back();
                    number_of_toric_faces_--;
                }
            }
        }
    }


    void SolventExcludedSurface::cleanSphericFaces()

    {
        while ((number_of_spheric_faces_ > 0) &&
                     (spheric_faces_[number_of_spheric_faces_-1] == NULL))
        {
            spheric_faces_.pop_back();
            number_of_spheric_faces_--;
        }
        for (Position i = 0; i < number_of_spheric_faces_; i++)
        {
            if (spheric_faces_[i] == NULL)
            {
                spheric_faces_[i] = spheric_faces_[number_of_spheric_faces_-1];
                spheric_faces_[i]->index_ = i;
                spheric_faces_.pop_back();
                number_of_spheric_faces_--;
                while (spheric_faces_[number_of_spheric_faces_-1] == NULL)
                {
                    spheric_faces_.pop_back();
                    number_of_spheric_faces_--;
                }
            }
        }
    }


    void SolventExcludedSurface::deleteSmallToricFace(SESFace* face)
    {
        SESEdge* edge[4];
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        for (Position i = 0; i < 4; i++)
        {
            edge[i] = *e;
            e++;
        }
        SESVertex* p[4];
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        for (Position i = 0; i < 4; i++)
        {
            p[i] = *v;
            v++;
        }
        SESFace* neighbour1 = edge[1]->other(face);
        SESFace* neighbour2 = edge[2]->other(face);
        SESFace* neighbour3 = edge[3]->other(face);
        if (p[0] != p[3])
        {
            p[0]->join(*p[3]);
            neighbour3->vertex_.remove(p[3]);
            p[3]->substitute(p[0]);
        }
        if (p[1] != p[2])
        {
            p[1]->join(*p[2]);
            neighbour1->vertex_.remove(p[2]);
            p[2]->substitute(p[1]);
        }
        p[0]->edges_.erase(edge[2]);
        p[0]->edges_.erase(edge[3]);
        p[1]->edges_.erase(edge[2]);
        p[1]->edges_.erase(edge[1]);
        p[0]->faces_.erase(face);
        p[1]->faces_.erase(face);
        edge[0]->substitute(face,neighbour2);
        neighbour2->substitute(edge[2],edge[0]);
        if (p[2] != p[1])
        {
            vertices_[p[2]->index_] = NULL;
            neighbour1->edge_.remove(edge[1]);
            delete p[2];
        }
        else
        {
            p[1]->faces_.erase(neighbour1);
            contact_faces_[neighbour1->index_] = NULL;
            delete neighbour1;
        }
        if (p[3] != p[0])
        {
            vertices_[p[3]->index_] = NULL;
            neighbour3->edge_.remove(edge[3]);
            delete p[3];
        }
        else
        {
            p[0]->faces_.erase(neighbour3);
            contact_faces_[neighbour3->index_] = NULL;
            delete neighbour3;
        }
        edges_[edge[1]->index_] = NULL;
        delete edge[1];
        edges_[edge[2]->index_] = NULL;
        delete edge[2];
        edges_[edge[3]->index_] = NULL;
        delete edge[3];
        toric_faces_[face->index_] = NULL;
        delete face;
        edge[0]->type_ = SESEdge::TYPE_SINGULAR;
        TAngle<double> phi
                (getOrientedAngle(edge[0]->vertex_[0]->point_-edge[0]->circle_.p,
                                                    edge[0]->vertex_[1]->point_-edge[0]->circle_.p,
                                                    edge[0]->circle_.n));
        if (phi.value > Constants::PI)
        {
            edge[0]->circle_.n.negate();
        }
        singular_edges_.push_back(edge[0]);
    }


    void SolventExcludedSurface::deleteSmallSingularToricFace
            (SESFace* face)

    {
        SESEdge* edge[6];
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        for (Position i = 0; i < 6; i++)
        {
            edge[i] = *e;
            e++;
        }
        SESVertex* p[6];
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        for (Position i = 0; i < 6; i++)
        {
            p[i] = *v;
            v++;
        }
        SESFace* neighbour0 = edge[0]->other(face);
        SESFace* neighbour2 = edge[2]->other(face);
        SESFace* neighbour3 = edge[3]->other(face);
        SESFace* neighbour5 = edge[5]->other(face);
        if (p[0] != p[2])
        {
            p[0]->join(*p[2]);
            neighbour0->vertex_.remove(p[2]);
            p[2]->substitute(p[0]);
        }
        if (p[3] != p[5])
        {
            p[3]->join(*p[5]);
            neighbour3->vertex_.remove(p[5]);
            p[5]->substitute(p[3]);
        }
        p[0]->edges_.erase(edge[0]);
        p[0]->edges_.erase(edge[2]);
        p[1]->edges_.erase(edge[2]);
        p[3]->edges_.erase(edge[3]);
        p[3]->edges_.erase(edge[5]);
        p[4]->edges_.erase(edge[5]);
        p[0]->faces_.erase(face);
        p[1]->faces_.erase(face);
        p[3]->faces_.erase(face);
        p[4]->faces_.erase(face);
        edge[1]->substitute(face,neighbour2);
        edge[4]->substitute(face,neighbour5);
        neighbour2->substitute(edge[2],edge[1]);
        neighbour5->substitute(edge[5],edge[4]);
        if (p[2] != p[0])
        {
            vertices_[p[2]->index_] = NULL;
            neighbour0->edge_.remove(edge[0]);
            delete p[2];
        }
        else
        {
            p[0]->faces_.erase(neighbour0);
            contact_faces_[neighbour0->index_] = NULL;
            delete neighbour0;
        }
        if (p[3] != p[5])
        {
            vertices_[p[5]->index_] = NULL;
            neighbour3->edge_.remove(edge[3]);
            delete p[5];
        }
        else
        {
            p[3]->faces_.erase(neighbour3);
            contact_faces_[neighbour3->index_] = NULL;
            delete neighbour3;
        }
        edges_[edge[0]->index_] = NULL;
        delete edge[0];
        edges_[edge[2]->index_] = NULL;
        delete edge[2];
        edges_[edge[3]->index_] = NULL;
        delete edge[3];
        edges_[edge[5]->index_] = NULL;
        delete edge[5];
        toric_faces_[face->index_] = NULL;
        delete face;
        edge[1]->type_ = SESEdge::TYPE_SINGULAR;
        TAngle<double> phi
                (getOrientedAngle(edge[1]->vertex_[0]->point_-edge[1]->circle_.p,
                                                    edge[1]->vertex_[1]->point_-edge[1]->circle_.p,
                                                    edge[1]->circle_.n));
        if (phi.value > Constants::PI)
        {
            edge[1]->circle_.n.negate();
        }
        edge[4]->type_ = SESEdge::TYPE_SINGULAR;
        phi = getOrientedAngle(edge[4]->vertex_[0]->point_-edge[4]->circle_.p,
                                                     edge[4]->vertex_[1]->point_-edge[4]->circle_.p,
                                                     edge[4]->circle_.n);
        if (phi.value > Constants::PI)
        {
            edge[4]->circle_.n.negate();
        }
        singular_edges_.push_back(edge[1]);
        singular_edges_.push_back(edge[4]);
    }


    void SolventExcludedSurface::compute()
        throw(Exception::GeneralException)
    {
        SESComputer sesc(this);
        sesc.run();
    }


    void SolventExcludedSurface::splitSphericFaces()
    {
        for (Position i = 0; i < number_of_spheric_faces_; i++)
        {
            splitSphericFace(i);
        }
    }


    void SolventExcludedSurface::splitSphericFace(Position i)

    {
        SESFace* face = spheric_faces_[i];
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        while (e != face->edge_.end())
        {
            if ((*e)->vertex_[0] == NULL)
            {
                return;
            }
            e++;
        }
        std::list<SESEdge*> contour;
        std::list<SESVertex*> vertices;
        e = face->edge_.begin();
        SESVertex* start = (*e)->vertex_[0];
        SESVertex* next = (*e)->vertex_[1];
        SESEdge* current = *e;
        contour.push_back(current);
        vertices.push_back(next);
        while (next != start)
        {
            for (e = face->edge_.begin(); e != face->edge_.end(); e++)
            {
                if (*e != current)
                {
                    if ((*e)->vertex_[0] == next)
                    {
                        contour.push_back(*e);
                        next = (*e)->vertex_[1];
                        vertices.push_back(next);
                        current = *e;
                    }
                    else
                    {
                        if ((*e)->vertex_[1] == next)
                        {
                            contour.push_back(*e);
                            next = (*e)->vertex_[0];
                            vertices.push_back(next);
                            current = *e;
                        }
                    }
                }
            }
        }
        if (contour.size() != face->edge_.size())
        {
            SESFace* new_face = new SESFace(*face,true);
            for (e = contour.begin(); e != contour.end(); e++)
            {
                new_face->edge_.remove(*e);
            }
            std::list<SESVertex*>::iterator v;
            for (v = vertices.begin(); v != vertices.end(); v++)
            {
                new_face->vertex_.remove(*v);
            }
            new_face->index_ = number_of_spheric_faces_;
            spheric_faces_.push_back(new_face);
            number_of_spheric_faces_++;
            face->edge_ = contour;
            face->vertex_ = vertices;
        }
    }


    bool SolventExcludedSurface::check()

    {
        for (Position i = 0; i < number_of_vertices_; i++)
        {
            if (vertices_[i]->edges_.size() != vertices_[i]->faces_.size())
            {
                return false;
            }
        }
        for (Position i = 0; i < number_of_spheric_faces_; i++)
        {
            if (spheric_faces_[i]->edge_.size() != spheric_faces_[i]->vertex_.size())
            {
                Index test = spheric_faces_[i]->edge_.size()-
                                         spheric_faces_[i]->vertex_.size();
                std::list<SESEdge*>::iterator e = spheric_faces_[i]->edge_.begin();
                while (e != spheric_faces_[i]->edge_.end())
                {
                    if ((*e)->vertex_[0] == NULL)
                    {
                        test--;
                    }
                    e++;
                }
                if (test != 0)
                {
                    return false;
                }
            }
        }
        return true;
    }


    SolventExcludedSurface::VertexIterator
            SolventExcludedSurface::beginVertex()

    {
        return vertices_.begin();
    }


    SolventExcludedSurface::ConstVertexIterator
            SolventExcludedSurface::beginVertex() const

    {
        return vertices_.begin();
    }


    SolventExcludedSurface::VertexIterator
            SolventExcludedSurface::endVertex()

    {
        return vertices_.end();
    }


    SolventExcludedSurface::ConstVertexIterator
            SolventExcludedSurface::endVertex() const

    {
        return vertices_.end();
    }


    SolventExcludedSurface::EdgeIterator
            SolventExcludedSurface::beginEdge()

    {
        return edges_.begin();
    }


    SolventExcludedSurface::ConstEdgeIterator
            SolventExcludedSurface::beginEdge() const

    {
        return edges_.begin();
    }


    SolventExcludedSurface::EdgeIterator
            SolventExcludedSurface::endEdge()

    {
        return edges_.end();
    }


    SolventExcludedSurface::ConstEdgeIterator
            SolventExcludedSurface::endEdge() const

    {
        return edges_.end();
    }


    SolventExcludedSurface::SingularEdgeIterator
            SolventExcludedSurface::beginSingularEdge()

    {
        return singular_edges_.begin();
    }


    SolventExcludedSurface::ConstSingularEdgeIterator
            SolventExcludedSurface::beginSingularEdge() const

    {
        return singular_edges_.begin();
    }


    SolventExcludedSurface::SingularEdgeIterator
            SolventExcludedSurface::endSingularEdge()

    {
        return singular_edges_.end();
    }


    SolventExcludedSurface::ConstSingularEdgeIterator
            SolventExcludedSurface::endSingularEdge() const

    {
        return singular_edges_.end();
    }


    SolventExcludedSurface::ContactFaceIterator
            SolventExcludedSurface::beginContactFace()

    {
        return contact_faces_.begin();
    }


    SolventExcludedSurface::ConstContactFaceIterator
            SolventExcludedSurface::beginContactFace() const

    {
        return contact_faces_.begin();
    }


    SolventExcludedSurface::ContactFaceIterator
            SolventExcludedSurface::endContactFace()

    {
        return contact_faces_.end();
    }


    SolventExcludedSurface::ConstContactFaceIterator
            SolventExcludedSurface::endContactFace() const

    {
        return contact_faces_.end();
    }


    SolventExcludedSurface::SphericFaceIterator
            SolventExcludedSurface::beginSphericFace()

    {
        return spheric_faces_.begin();
    }


    SolventExcludedSurface::ConstSphericFaceIterator
            SolventExcludedSurface::beginSphericFace() const

    {
        return spheric_faces_.begin();
    }


    SolventExcludedSurface::SphericFaceIterator
            SolventExcludedSurface::endSphericFace()

    {
        return spheric_faces_.end();
    }


    SolventExcludedSurface::ConstSphericFaceIterator
            SolventExcludedSurface::endSphericFace() const

    {
        return spheric_faces_.end();
    }


    SolventExcludedSurface::ToricFaceIterator
            SolventExcludedSurface::beginToricFace()

    {
        return toric_faces_.begin();
    }


    SolventExcludedSurface::ConstToricFaceIterator
            SolventExcludedSurface::beginToricFace() const

    {
        return toric_faces_.begin();
    }


    SolventExcludedSurface::ToricFaceIterator
            SolventExcludedSurface::endToricFace()

    {
        return toric_faces_.end();
    }


    SolventExcludedSurface::ConstToricFaceIterator
            SolventExcludedSurface::endToricFace() const

    {
        return toric_faces_.end();
    }


    std::ostream& operator << (std::ostream& s, const SolventExcludedSurface& ses)
    {
        s << "Vertices:\n";
        SolventExcludedSurface::ConstVertexIterator v;
        for (v = ses.beginVertex(); v != ses.endVertex(); v++)
        {
            if (*v != NULL)
            {
                s << "  " << **v << "\n";
            }
            else
            {
                s << "  --\n";
            }
        }
        s << "Edges:\n";
        SolventExcludedSurface::ConstEdgeIterator e;
        for (e = ses.beginEdge(); e != ses.endEdge(); e++)
        {
            if (*e != NULL)
            {
                s << "  " << **e << "\n";
            }
            else
            {
                s << "  --\n";
            }
        }
        s << "singular Edges:\n";
        SolventExcludedSurface::ConstSingularEdgeIterator se;
        for (se = ses.beginSingularEdge(); se != ses.endSingularEdge(); se++)
        {
            if (*se != NULL)
            {
                s << "  " << **se << "\n";
            }
            else
            {
                s << "  --\n";
            }
        }
        s << "contact Faces:\n";
        SolventExcludedSurface::ConstContactFaceIterator cf;
        for (cf = ses.beginContactFace(); cf != ses.endContactFace(); cf++)
        {
            if (*cf != NULL)
            {
                s << "  " << **cf << "\n";
            }
            else
            {
                s << "  --\n";
            }
        }
        s << "toric Faces:\n";
        SolventExcludedSurface::ConstToricFaceIterator tf;
        for (tf = ses.beginToricFace(); tf != ses.endToricFace(); tf++)
        {
            if (*tf != NULL)
            {
                s << "  " << **tf << "\n";
            }
            else
            {
                s << "  --\n";
            }
        }
        s << "spheric Faces:\n";
        SolventExcludedSurface::ConstSphericFaceIterator sf;
        for (sf = ses.beginSphericFace(); sf != ses.endSphericFace(); sf++)
        {
            if (*sf != NULL)
            {
                s << "  " << **sf << "\n";
            }
            else
            {
                s << "  --\n";
            }
        }
        return s;
    }

///////////////////////////////


    SESComputer::SESComputer()
        :	ses_(),
            vertex_grid_()
    {
    }


    SESComputer::SESComputer(SolventExcludedSurface* ses)
        :	ses_(ses),
            vertex_grid_()
    {
    }


    SESComputer::~SESComputer()
    {
    }


    void SESComputer::run()
        throw(Exception::GeneralException)
    {
        preProcessing();
        get();
        SESSingularityCleaner sessc(ses_,&vertex_grid_);
        while (!sessc.run())
        {
            ses_->clear();
            vertex_grid_.clear();
            preProcessing();
            get();
            sessc.vertex_grid_ = &vertex_grid_;
        }
    }


    void SESComputer::preProcessing()

    {
        ses_->clear();
        ses_->number_of_contact_faces_
                = ses_->reduced_surface_->number_of_vertices_;
        ses_->number_of_toric_faces_
                = ses_->reduced_surface_->number_of_edges_;
        ses_->number_of_spheric_faces_
                = ses_->reduced_surface_->number_of_faces_;
        SESFace* face;
        Position i;
        for (i = 0; i < ses_->number_of_contact_faces_; i++)
        {
            face = new SESFace;
            face->type_ = SESFace::TYPE_CONTACT;
            face->rsvertex_ = ses_->reduced_surface_->vertices_[i];
            face->rsedge_ = NULL;
            face->rsface_ = NULL;
            face->index_ = i;
            ses_->contact_faces_.push_back(face);
        }
        for (i = 0; i < ses_->number_of_toric_faces_; i++)
        {
            face = new SESFace;
            face->type_ = SESFace::TYPE_TORIC;
            face->rsvertex_ = NULL;
            face->rsedge_ = ses_->reduced_surface_->edges_[i];
            face->rsface_ = NULL;
            face->index_ = i;
            ses_->toric_faces_.push_back(face);
        }
        for (i = 0; i < ses_->number_of_spheric_faces_; i++)
        {
            face = new SESFace;
            face->type_ = SESFace::TYPE_SPHERIC;
            face->rsvertex_ = NULL;
            face->rsedge_ = NULL;
            face->rsface_ = ses_->reduced_surface_->faces_[i];
            face->index_ = i;
            ses_->spheric_faces_.push_back(face);
        }
        double x_min = ses_->reduced_surface_->bounding_box_.a.x;
        double y_min = ses_->reduced_surface_->bounding_box_.a.y;
        double z_min = ses_->reduced_surface_->bounding_box_.a.z;
        double x_max = ses_->reduced_surface_->bounding_box_.b.x;
        double y_max = ses_->reduced_surface_->bounding_box_.b.y;
        double z_max = ses_->reduced_surface_->bounding_box_.b.z;
        double dist = ses_->reduced_surface_->r_max_;
        Position nx = (Position)((x_max-x_min)/dist+5);
        Position ny = (Position)((y_max-y_min)/dist+5);
        Position nz = (Position)((z_max-z_min)/dist+5);
        Vector3 origin(x_min-2*dist,y_min-2*dist,z_min-2*dist);
        vertex_grid_.set(origin, Vector3(dist), nx, ny, nz);
    }


    void SESComputer::get()
    {
        for (Position i = 0; i < ses_->number_of_spheric_faces_; i++)
        {
            createSphericFace(i);
        }
        for (Position i = 0; i < ses_->number_of_toric_faces_; i++)
        {
            createToricFace(i);
        }
    }


    void SESComputer::createSphericFace(Position j)
    {
        SESFace* face = ses_->spheric_faces_[j];
        RSFace* rsface = face->rsface_;
        double probe_radius = ses_->reduced_surface_->probe_radius_;
        TSphere3<double> probe(rsface->center_,probe_radius);
        // create three vertices and push them to their faces
        // and in the list of vertices
        pushVertex(face,probe,rsface->vertex_[0]);
        pushVertex(face,probe,rsface->vertex_[1]);
        pushVertex(face,probe,rsface->vertex_[2]);
        // create three concace edges and push them to their faces
        // and in the list of edges
        pushConcaveEdge(face,0,1,probe_radius);
        pushConcaveEdge(face,1,2,probe_radius);
        pushConcaveEdge(face,2,0,probe_radius);
    }


    void SESComputer::pushVertex
        (SESFace*				face,
         const TSphere3<double>& probe,
         RSVertex*			rsvertex)
    {
        // Create a new vertex on the correct position ...
        SESVertex* vertex(createVertex(probe.p,rsvertex->atom_));
        // ... and push it to the face's vertices.
        face->vertex_.push_back(vertex);
        vertex->faces_.insert(face);
        // Get the RSEdges of the corresponding RSVertex ...
        RSEdge* tf1(0);
        RSEdge* tf2(0);
        face->rsface_->getEdges(rsvertex,tf1,tf2);
        // ... and push the vertex to these toric faces.
        ses_->toric_faces_[tf1->index_]->vertex_.push_back(vertex);
        vertex->faces_.insert(ses_->toric_faces_[tf1->index_]);
        ses_->toric_faces_[tf2->index_]->vertex_.push_back(vertex);
        vertex->faces_.insert(ses_->toric_faces_[tf2->index_]);
        // Push the vertex to the contact face of the corresponding RSVertex ...
        ses_->contact_faces_[rsvertex->index_]->vertex_.push_back(vertex);
        vertex->faces_.insert(ses_->contact_faces_[rsvertex->index_]);
        // ... and to the vertices of the SES.
        ses_->vertices_.push_back(vertex);
        Vector3 pos(vertex->point_.x,
                                vertex->point_.y,
                                vertex->point_.z);
        vertex_grid_.insert(pos,vertex->index_);
        ses_->number_of_vertices_++;
    }


    void SESComputer::pushConcaveEdge
        (SESFace* face,
         Position			p1,
         Position			p2,
         const double&			radius_of_probe)

    {
        RSFace* rsface = face->rsface_;
        RSEdge* rsedge(0);
        // get the corresponding RSEdge
        rsface->getEdge(rsface->getVertex(p1),rsface->getVertex(p2),rsedge);
        Index index = rsedge->index_;
        // create a new SESEdge
        SESEdge* edge(createConcaveEdge(face,p1,p2,index,radius_of_probe));
        // and push it to all it's places
        face->edge_.push_back(edge);
        ses_->toric_faces_[index]->edge_.push_back(edge);
        edge->vertex_[0]->edges_.insert(edge);
        edge->vertex_[1]->edges_.insert(edge);
        ses_->edges_.push_back(edge);
        ses_->number_of_edges_++;
    }


    SESVertex* SESComputer::createVertex
        (const TVector3<double>& probe_center,
         Index							index)

    {
        SESVertex* vertex = new SESVertex;
        TSphere3<double>* atom = &(ses_->reduced_surface_->atom_[index]);
        // get the position of the new vertex
        getPoint(atom->p,probe_center,atom->radius,vertex->point_);
        vertex->normal_.set(probe_center-vertex->point_);
        vertex->atom_ = index;
        vertex->index_ = ses_->number_of_vertices_;
        return vertex;
    }


    SESEdge* SESComputer::createConcaveEdge
        (SESFace* face,
         Position			p1,
         Position			p2,
         Index				index,
         const double&			radius_of_probe)

    {
        SESEdge* edge = new SESEdge;
        // set the vertices of the edge
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        for (Position i = 0; i < p1; i++)
        {
            v++;
        }
        edge->vertex_[0] = *v;
        v = face->vertex_.begin();
        for (Position i = 0; i < p2; i++)
        {
            v++;
        }
        edge->vertex_[1] = *v;
        // set the faces of the edge
        edge->face_[0] = face;
        edge->face_[1] = ses_->toric_faces_[index];
        //
        edge->rsedge_ = NULL;
        edge->type_ = SESEdge::TYPE_CONCAVE;
        edge->index_ = ses_->number_of_edges_;
        // compute the circle on which the edge lies
        RSFace* rsface = face->rsface_;
        TVector3<double> normal = (edge->vertex_[0]->point_-rsface->center_)%
                                                 (edge->vertex_[1]->point_-rsface->center_);
        edge->circle_.set(rsface->center_,normal,radius_of_probe);
        return edge;
    }


    void SESComputer::createToricFace(Position i)

    {
        SESFace* face = ses_->toric_faces_[i];
        if (face->isFree())
        {
            createFreeToricFace(i);
        }
        else
        {
            SESEdge* edge1 = createConvexEdge(face,face->rsedge_->vertex_[0]);
            SESEdge* edge2 = createConvexEdge(face,face->rsedge_->vertex_[1]);
            if (Maths::isEqual(face->rsedge_->angle_.value,Constants::PI))
            {
                RSFace* rsface1 = face->rsedge_->face_[0];
                RSFace* rsface2 = face->rsedge_->face_[1];
                RSVertex* rsvertex1 = face->rsedge_->vertex_[0];
                RSVertex* rsvertex2 = face->rsedge_->vertex_[1];
                RSVertex* rsvertex3 = rsface1->third(rsvertex1,rsvertex2);
                RSVertex* rsvertex4 = rsface2->third(rsvertex1,rsvertex2);
                TVector3<double> point1(ses_->reduced_surface_->atom_[rsvertex1->atom_].p);
                TVector3<double> point2(ses_->reduced_surface_->atom_[rsvertex2->atom_].p);
                TVector3<double> point3(ses_->reduced_surface_->atom_[rsvertex3->atom_].p);
                TVector3<double> point4(ses_->reduced_surface_->atom_[rsvertex4->atom_].p);
                TVector3<double> middle(edge1->circle_.n%
                                                     (edge1->vertex_[0]->point_-edge1->circle_.p));
                middle.normalize();
                middle *= edge1->circle_.radius;
                middle += edge1->circle_.p;
                TPlane3<double> plane(point1,point2,rsface1->center_);
                double test = plane.n*(point3-plane.p);
                if (test*(plane.n*(middle-plane.p)) > 0)
                {
                    edge1->revert();
                }
                middle.set(edge2->circle_.n%
                                     (edge2->vertex_[0]->point_-edge2->circle_.p));
                middle.normalize();
                middle *= edge2->circle_.radius;
                middle += edge2->circle_.p;
                if (test*(plane.n*(middle-plane.p)) > 0)
                {
                    edge2->revert();
                }
            }
            if (face->rsedge_->singular_)
            {
                treatSingularToricFace(i);
            }
        }
    }


    SESEdge* SESComputer::createConvexEdge(SESFace*	face, RSVertex*	rsvertex)
    {
        SESEdge* edge = new SESEdge;
        Index atom = rsvertex->atom_;
        Index index = rsvertex->index_;
        // find the first vertex of the toric face
        // which lies on the surface of the rsvertex
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        while ((*v)->atom_ != atom)
        {
            v++;
        }
        edge->vertex_[0] = *v;
        // find the second vertex of the toric face
        // which lies on the surface of the rsvertex
        v++;
        while ((*v)->atom_ != atom)
        {
            v++;
        }
        edge->vertex_[1] = *v;
        // set the faces of the edge
        edge->face_[0] = face;
        edge->face_[1] = ses_->contact_faces_[index];
        // set the rsedge, type and index of the edge
        edge->rsedge_ = face->rsedge_;
        edge->type_ = SESEdge::TYPE_CONVEX;
        edge->index_ = ses_->number_of_edges_;
        // compute the circle on which the edge lies
        TCircle3<double> circle0(edge->rsedge_->circle0_);
        TCircle3<double> circle1(edge->rsedge_->circle1_);
        if (edge->rsedge_->vertex_[0]->index_ == index)
        {
            edge->circle_.set(circle0.p,circle0.p-circle1.p,circle0.radius);
        }
        else
        {
            edge->circle_.set(circle1.p,circle1.p-circle0.p,circle1.radius);
        }
        TVector3<double> v1(edge->vertex_[0]->point_-edge->circle_.p);
        TVector3<double> v2(edge->vertex_[1]->point_-edge->circle_.p);
        TVector3<double> n(edge->circle_.n);
        TAngle<double> test_phi(getOrientedAngle(v1,v2,n));
        if ((test_phi.value-Constants::PI)*
                (edge->rsedge_->angle_.value-Constants::PI) < 0)
        {
            // test_phi smaller than PI, but expected to be greater or
            // test_phi greater than PI, but expected to be smaller
            edge->revert();
        }
        face->edge_.push_back(edge);
        ses_->contact_faces_[index]->edge_.push_back(edge);
        edge->vertex_[0]->edges_.insert(edge);
        edge->vertex_[1]->edges_.insert(edge);
        ses_->edges_.push_back(edge);
        ses_->number_of_edges_++;
        return edge;
    }


    void SESComputer::treatSingularToricFace(Position i)

    {
        SESFace* face = ses_->toric_faces_[i];
        face->normalize(false);
        SESEdge* edge[4];
        std::list<SESEdge*>::iterator e = face->edge_.begin();
        for (Position i = 0; i < 4; i++)
        {
            edge[i] = *e;
            e++;
        }
        SESVertex* p[4];
        std::list<SESVertex*>::iterator v = face->vertex_.begin();
        for (Position i = 0; i < 4; i++)
        {
            p[i] = *v;
            v++;
        }
        // compute the circle on which the singular edge lies
        SESFace* neighbour0 = edge[0]->other(face);
        SESFace* neighbour2 = edge[2]->other(face);
        double probe_radius = ses_->reduced_surface_->probe_radius_;
        TSphere3<double> probe1(neighbour0->rsface_->center_,probe_radius);
        TSphere3<double> probe2(neighbour2->rsface_->center_,probe_radius);
        TCircle3<double> intersection_circle;
        GetIntersection(probe1,probe2,intersection_circle);
        // create the new edges
        SESEdge* new_edge0 = new SESEdge(*edge[0],true);
        SESEdge* new_edge2 = new SESEdge(*edge[2],true);
        SESEdge* new_edge = new SESEdge(NULL,NULL,neighbour0,neighbour2,
                                                                                        intersection_circle,face->rsedge_,
                                                                                        SESEdge::TYPE_SINGULAR,-1);
        // create the new points
        Position ip1 = ((p[1]->atom_ == face->rsedge_->vertex_[0]->atom_) ? 0 : 1);
        Position ip3 = 1-ip1;
        SESVertex* new_point1
                = createSingularVertex(ip3,intersection_circle.p,
                                                             face,neighbour0,neighbour2,
                                                             edge[0],edge[2],new_edge);
        SESVertex* new_point3
                = createSingularVertex(ip1,intersection_circle.p,
                                                             face,neighbour0,neighbour2,
                                                             new_edge0,new_edge2,new_edge);
        // update the new edges
        updateEdge(edge[0],p[0],new_point1,false);
        updateEdge(edge[2],p[3],new_point1,false);
        updateEdge(new_edge0,p[1],new_point3,true);
        updateEdge(new_edge2,p[2],new_point3,true);
        // update the singular edge
        updateEdge(new_edge,new_point3,new_point1,true);
        ses_->singular_edges_.push_back(new_edge);
        ses_->number_of_singular_edges_++;
        // swap normal of new edge if necessary
        TAngle<double> phi(getOrientedAngle(new_point1->point_-intersection_circle.p,
                                                                     new_point3->point_-intersection_circle.p,
                                                                     intersection_circle.n));
        if ((face->rsedge_->angle_.value-Constants::PI)*
                (phi.value									-Constants::PI) < 0)
        {
            new_edge->circle_.n.negate();
        }
        // update the neighbour faces
        neighbour0->edge_.push_back(new_edge0);
        neighbour0->edge_.push_back(new_edge);
        neighbour0->vertex_.push_back(new_point1);
        neighbour0->vertex_.push_back(new_point3);
        neighbour2->edge_.push_back(new_edge2);
        neighbour2->edge_.push_back(new_edge);
        neighbour2->vertex_.push_back(new_point1);
        neighbour2->vertex_.push_back(new_point3);
        // update the toric face
        face->type_ = SESFace::TYPE_TORIC_SINGULAR;
        face->vertex_.push_back(new_point1);
        face->vertex_.push_back(new_point3);
        face->edge_.push_back(new_edge0);
        face->edge_.push_back(new_edge2);
        // update the vertices
        p[1]->edges_.erase(edge[0]);
        p[1]->edges_.insert(new_edge0);
        p[2]->edges_.erase(edge[2]);
        p[2]->edges_.insert(new_edge2);
    }


    SESVertex* SESComputer::createSingularVertex
            (Position ip,
             const TVector3<double>& dir,
             SESFace* face0,
             SESFace* face1,
             SESFace* face2,
             SESEdge* edge0,
             SESEdge* edge1,
             SESEdge* edge2)

    {
        SESVertex* vertex(0);
        TVector3<double> intersection_point(face0->rsedge_->getIntersectionPoint(ip));
        Index test = vertexExists(intersection_point);
        if (test == -1)
        {
            vertex = new SESVertex(intersection_point,
                                                                 dir-intersection_point,
                                                                 face0->rsedge_->getVertex(ip)->atom_,
                                                                 ses_->number_of_vertices_);
            ses_->vertices_.push_back(vertex);
            Vector3 pos(vertex->point_.x,
                                    vertex->point_.y,
                                    vertex->point_.z);
            vertex_grid_.insert(pos,vertex->index_);
            ses_->number_of_vertices_++;
        }
        else
        {
            vertex = ses_->vertices_[test];
        }
        vertex->edges_.insert(edge0);
        vertex->edges_.insert(edge1);
        vertex->edges_.insert(edge2);
        vertex->faces_.insert(face0);
        vertex->faces_.insert(face1);
        vertex->faces_.insert(face2);
        return vertex;
    }


    void SESComputer::updateEdge
            (SESEdge*		edge,
             SESVertex*	vertex1,
             SESVertex*	vertex2,
             bool						is_new)

    {
        if (edge->vertex_[0] == vertex1)
        {
            edge->vertex_[0] = vertex1;
            edge->vertex_[1] = vertex2;
        }
        else
        {
            edge->vertex_[0] = vertex2;
            edge->vertex_[1] = vertex1;
        }
        if (is_new)
        {
            edge->index_ = ses_->number_of_edges_;
            ses_->edges_.push_back(edge);
            ses_->number_of_edges_++;
        }
    }


    void SESComputer::createFreeToricFace(Position i)

    {
        SESFace* face(ses_->toric_faces_[i]);
        TCircle3<double> circle1(face->rsedge_->circle0_);
        TCircle3<double> circle2(face->rsedge_->circle1_);
        Index index1(face->rsedge_->vertex_[0]->index_);
        Index index2(face->rsedge_->vertex_[1]->index_);
        SESEdge* edge = new SESEdge;
            edge->type_ = SESEdge::TYPE_CONVEX;
            edge->vertex_[0] = NULL;
            edge->vertex_[1] = NULL;
            edge->rsedge_ = face->rsedge_;
            edge->face_[0] = face;
            edge->face_[1] = ses_->contact_faces_[index1];
            edge->circle_.set(circle1.p,circle1.p-circle2.p,circle1.radius);
            edge->index_ = ses_->number_of_edges_;
            face->edge_.push_back(edge);
            ses_->contact_faces_[index1]->edge_.push_back(edge);
            ses_->edges_.push_back(edge);
            ses_->number_of_edges_++;
        edge = new SESEdge;
            edge->type_ = SESEdge::TYPE_CONVEX;
            edge->vertex_[0] = NULL;
            edge->vertex_[1] = NULL;
            edge->rsedge_ = face->rsedge_;
            edge->face_[0] = face;
            edge->face_[1] = ses_->contact_faces_[index2];
            edge->circle_.set(circle2.p,circle2.p-circle1.p,circle2.radius);
            edge->index_ = ses_->number_of_edges_;
            face->edge_.push_back(edge);
            ses_->contact_faces_[index2]->edge_.push_back(edge);
            ses_->edges_.push_back(edge);
            ses_->number_of_edges_++;
    }


    void SESComputer::getPoint
        (const TVector3<double>& p1,
         const TVector3<double>& p2,
         const double&						dist,
         TVector3<double>&				result)

    {
        result.set(p2-p1);
        result.normalize();
        result *= dist;
        result += p1;
    }


    Index SESComputer::vertexExists(const TVector3<double>& point)

    {
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 0.001;
        Vector3 p(point.x,point.y,point.z);
        HashGridBox3<Index>* box = vertex_grid_.getBox(p);
        HashGridBox3<Index>::ConstBoxIterator b;
        HashGridBox3<Index>::ConstDataIterator d;
        if (box != NULL)
        {
            for (b = box->beginBox(); b != box->endBox(); b++)
            {
                for (d = b->beginData(); d != b->endData(); d++)
                {
                    if (ses_->vertices_[*d]->point_ == point)
                    {
                        Constants::EPSILON = epsilon;
                        return *d;
                    }
                }
            }
        }
        Constants::EPSILON = epsilon;
        return -1;
    }



///////////////////////////////


    SESSingularityCleaner::SESSingularityCleaner()
        :	ses_(),
            vertex_grid_(),
            probe_intersections_()
    {
    }


    SESSingularityCleaner::SESSingularityCleaner(SolventExcludedSurface* ses, HashGrid3<Index>* vertex_grid)
        :	ses_(ses),
            vertex_grid_(vertex_grid),
            probe_intersections_()
    {
    }


    SESSingularityCleaner::~SESSingularityCleaner()
    {
        // delete probe_intersections
        HashMap< Position,
                         HashMap< Position,
                                            HashMap< Position,
                                                             ProbeIntersection* > > >::Iterator pi1;
        HashMap< Position,
                         HashMap< Position,
                                            ProbeIntersection* > >::Iterator pi2;
        HashMap< Position,ProbeIntersection* >::Iterator pi3;
        for (pi1 = probe_intersections_.begin();
                 pi1 != probe_intersections_.end();
                 pi1++)
        {
            for (pi2 = pi1->second.begin(); pi2 != pi1->second.end(); pi2++)
            {
                for (pi3 = pi2->second.begin(); pi3 != pi2->second.end(); pi3++)
                {
                    delete pi3->second;
                }
            }
        }
    }


    bool SESSingularityCleaner::run()
        throw(Exception::GeneralException)
    {
        if (!treatFirstCategory())
        {
            return false;
        }
        if (ses_->number_of_singular_edges_ > 0)
        {
            treatSecondCategory();
        }
        return true;
    }


    bool SESSingularityCleaner::treatFirstCategory()
    {
        std::list<SESFace*> first_category_faces;
        getFirstCategoryFaces(first_category_faces);

        SESFace* face1(0);
        SESFace* face2(0);
        bool modified = false;
        std::list<SESFace*>::iterator f
                = first_category_faces.begin();
        while (f != first_category_faces.end())
        {
            face1 = *f;
            f++;
            face2 = *f;
            f++;
            switch (face1->edge_.size())
            {
                case 3 :	noCut(face1,face2);
                                    break;
                case 5 :	break;
                case 7 :	twoCuts(face1,face2);
                                    break;
                case 9 :	ses_->reduced_surface_->deleteSimilarFaces(face1->rsface_,
                                                                                                                         face2->rsface_);
                                    modified = true;
                                    break;
            }
        }
        if (modified)
        {
            ses_->reduced_surface_->clean();
            return false;
        }
        else
        {
            return true;
        }
    }


    void SESSingularityCleaner::getFirstCategoryFaces(std::list<SESFace*>& first_category_faces)
    {
        std::list<SESFace*> singular_faces;
        getSingularFaces(singular_faces);

        while (!singular_faces.empty())
        {
            SESFace* current = singular_faces.front();
            singular_faces.pop_front();
            std::list<SESFace*>::iterator i = singular_faces.begin();
            while (i != singular_faces.end())
            {
                if (*current->rsface_ *= *((*i)->rsface_))
                {
                    first_category_faces.push_back(current);
                    first_category_faces.push_back(*i);
                    singular_faces.erase(i);

                    break;
                }
                else
                {
                    i++;
                }
            }
        }
    }


    void SESSingularityCleaner::getSingularFaces(std::list<SESFace*>& faces)
    {
        for (Position i = 0; i < ses_->number_of_spheric_faces_; i++)
        {
            if (ses_->spheric_faces_[i]->rsface_->singular_ == true)
            {
                faces.push_back(ses_->spheric_faces_[i]);
            }
        }
    }


    void SESSingularityCleaner::noCut(SESFace* face1, SESFace* face2)
    {
        TCircle3<double> circle;
        double probe_radius = ses_->reduced_surface_->probe_radius_;
        TSphere3<double> s1(face1->rsface_->center_,probe_radius);
        TSphere3<double> s2(face2->rsface_->center_,probe_radius);
        GetIntersection(s1,s2,circle);
        // test whether the circle is really an edge
        TVector3<double> normal(face1->rsface_->normal_);
        TVector3<double> point1(ses_->reduced_surface_
                                                ->atom_[face1->rsface_->vertex_[0]->atom_].p);
        TVector3<double> point2(ses_->reduced_surface_
                                                ->atom_[face1->rsface_->vertex_[1]->atom_].p);
        TVector3<double> point3(ses_->reduced_surface_
                                                ->atom_[face1->rsface_->vertex_[2]->atom_].p);
        TVector3<double> u(normal%(point1-point2));
        TVector3<double> v(normal%(point2-point3));
        TVector3<double> w(normal%(point3-point1));
        TVector3<double> diff1(point1-circle.p);
        TVector3<double> diff2(point2-circle.p);
        double test1 = u*diff1;
        double test2 = v*diff2;
        double test3 = w*diff1;
        if ((Maths::isLess(test1,0.0) &&
                 Maths::isLess(test2,0.0) &&
                 Maths::isLess(test3,0.0)		) ||
                (Maths::isGreater(test1,0.0) &&
                 Maths::isGreater(test2,0.0) &&
                 Maths::isGreater(test3,0.0)		)	)
        {
            SESEdge* edge
                    = new SESEdge(NULL,NULL,face1,face2,circle,NULL,
                                                        SESEdge::TYPE_SINGULAR,
                                                        ses_->number_of_edges_);
            ses_->edges_.push_back(edge);
            ses_->singular_edges_.push_back(edge);
            ses_->number_of_edges_++;
            face1->edge_.push_back(edge);
            face2->edge_.push_back(edge);
        }
    }


    void SESSingularityCleaner::twoCuts
            (SESFace* face1, SESFace* face2)

    {
        std::vector<SESEdge*> sesedge1(7);
        std::vector<SESEdge*> sesedge2(7);
        std::vector<SESVertex*> sesvertex1(7);
        std::vector<SESVertex*> sesvertex2(7);
        sort(face1,face2,sesedge1,sesedge2,sesvertex1,sesvertex2);
        TCircle3<double> circle;
        TSphere3<double> sphere1(face1->rsface_->center_,
                                                ses_->reduced_surface_->probe_radius_);
        TSphere3<double> sphere2(face2->rsface_->center_,
                                                ses_->reduced_surface_->probe_radius_);
        GetIntersection(sphere1,sphere2,circle);
        TAngle<double> phi(getOrientedAngle(sesvertex1[0]->point_-circle.p,
                                                                     sesvertex1[2]->point_-circle.p,
                                                                     circle.n));
        if (phi.value > Constants::PI)
        {
            circle.n.negate();
        }
        SESEdge* new_edge1
                = new SESEdge(sesvertex1[0],sesvertex1[2],face1,face2,
                                                    circle,NULL,SESEdge::TYPE_SINGULAR,
                                                    ses_->number_of_edges_);
        ses_->edges_.push_back(new_edge1);
        ses_->singular_edges_.push_back(new_edge1);
        face1->edge_.push_back(new_edge1);
        face2->edge_.push_back(new_edge1);
        sesvertex1[0]->edges_.insert(new_edge1);
        sesvertex1[2]->edges_.insert(new_edge1);
        ses_->number_of_edges_++;
        ses_->number_of_singular_edges_++;
        SESEdge* new_edge2
                = new SESEdge(sesvertex1[3],sesvertex1[6],face1,face2,
                                                    circle,NULL,SESEdge::TYPE_SINGULAR,
                                                    ses_->number_of_edges_);
        ses_->edges_.push_back(new_edge2);
        ses_->singular_edges_.push_back(new_edge2);
        face1->edge_.push_back(new_edge2);
        face2->edge_.push_back(new_edge2);
        sesvertex1[3]->edges_.insert(new_edge2);
        sesvertex1[6]->edges_.insert(new_edge2);
        ses_->number_of_edges_++;
        ses_->number_of_singular_edges_++;
        if (sesedge1[2] == sesedge2[2])
        {
            ses_->edges_[sesedge1[2]->index_] = NULL;
            ses_->singular_edges_.remove(sesedge1[2]);
            sesvertex1[2]->edges_.erase(sesedge1[2]);
            sesvertex1[3]->edges_.erase(sesedge1[2]);
            face1->edge_.remove(sesedge1[2]);
            face2->edge_.remove(sesedge1[2]);
            delete sesedge1[2];
        }
        if (sesedge1[6] == sesedge2[6])
        {
            ses_->edges_[sesedge1[6]->index_] = NULL;
            ses_->singular_edges_.remove(sesedge1[6]);
            sesvertex1[6]->edges_.erase(sesedge1[6]);
            sesvertex1[0]->edges_.erase(sesedge1[6]);
            face1->edge_.remove(sesedge1[6]);
            face2->edge_.remove(sesedge1[6]);
            delete sesedge1[6];
        }
    }


    void SESSingularityCleaner::sort
            (SESFace* face1,
             SESFace* face2,
             std::vector<SESEdge*>& sesedge1,
             std::vector<SESEdge*>& sesedge2,
             std::vector<SESVertex*>& sesvertex1,
             std::vector<SESVertex*>& sesvertex2)

    {
        // find two equal vertices
        std::list<SESVertex*>::iterator v1 = face1->vertex_.begin();
        std::list<SESVertex*>::iterator v2;
        bool found = false;
        while (!found)
        {
            v2 = face2->vertex_.begin();
            while (!found && (v2 != face2->vertex_.end()))
            {
                if (*v2 == *v1)
                {
                    sesvertex1[0] = *v1;
                    sesvertex2[0] = *v2;
                    found = true;
                }
                v2++;
            }
            v1++;
        }
        // find first corresponding edges
        face1->getEdges(sesvertex1[0],sesedge1[0],sesedge1[1]);
        face2->getEdges(sesvertex2[0],sesedge2[0],sesedge2[1]);
        if (*sesedge1[0] == *sesedge2[1])
        {
            sesedge1[0] = sesedge1[1];
        }
        else
        {
            if (*sesedge1[1] == *sesedge2[0])
            {
                sesedge2[0] = sesedge2[1];
            }
            else
            {
                if (*sesedge1[0] == *sesedge2[0])
                {
                    sesedge1[0] = sesedge1[1];
                    sesedge2[0] = sesedge2[1];
                }
            }
        }
        // find remaining edges and vertices
        SESEdge* sesedge(0);
        sesvertex1[1] = sesedge1[0]->other(sesvertex1[0]);
        sesvertex2[1] = sesedge2[0]->other(sesvertex2[0]);
        for (Position i = 1; i < 6; i++)
        {
            face1->getEdges(sesvertex1[i],sesedge1[i],sesedge);
            if (sesedge != sesedge1[i-1])
            {
                sesedge1[i] = sesedge;
            }
            face2->getEdges(sesvertex2[i],sesedge2[i],sesedge);
            if (sesedge != sesedge2[i-1])
            {
                sesedge2[i] = sesedge;
            }
            sesvertex1[i+1] = sesedge1[i]->other(sesvertex1[i]);
            sesvertex2[i+1] = sesedge2[i]->other(sesvertex2[i]);
        }
        face1->getEdge(sesvertex1[0],sesvertex1[6],sesedge1[6]);
        face2->getEdge(sesvertex2[0],sesvertex2[6],sesedge2[6]);
        //
        SESVertex* sesvertex(0);
        if (sesvertex1[2] != sesvertex2[2])
        {
            for (Position i = 0; i < 3; i++)
            {
                sesvertex = sesvertex1[i];
                sesvertex1[i] = sesvertex1[6-i];
                sesvertex1[6-i] = sesvertex;
                sesvertex = sesvertex2[i];
                sesvertex2[i] = sesvertex2[6-i];
                sesvertex2[6-i] = sesvertex;
                sesedge = sesedge1[i];
                sesedge1[i] = sesedge1[5-i];
                sesedge1[5-i] = sesedge;
                sesedge = sesedge2[i];
                sesedge2[i] = sesedge2[5-i];
                sesedge2[5-i] = sesedge;
            }
        }
    }


    void SESSingularityCleaner::treatSecondCategory()
    {
        double x_min = ses_->spheric_faces_[0]->rsface_->center_.x;
        double y_min = ses_->spheric_faces_[0]->rsface_->center_.y;
        double z_min = ses_->spheric_faces_[0]->rsface_->center_.z;
        double x_max = ses_->spheric_faces_[0]->rsface_->center_.x;
        double y_max = ses_->spheric_faces_[0]->rsface_->center_.y;
        double z_max = ses_->spheric_faces_[0]->rsface_->center_.z;

        for (Position i = 1; i != ses_->number_of_spheric_faces_; i++)
        {
            x_min = std::min(x_min, ses_->spheric_faces_[i]->rsface_->center_.x);
            y_min = std::min(y_min, ses_->spheric_faces_[i]->rsface_->center_.y);
            z_min = std::min(z_min, ses_->spheric_faces_[i]->rsface_->center_.z);

            x_max = std::max(x_max, ses_->spheric_faces_[i]->rsface_->center_.x);
            y_max = std::max(y_max, ses_->spheric_faces_[i]->rsface_->center_.y);
            z_max = std::max(z_max, ses_->spheric_faces_[i]->rsface_->center_.z);
        }

        double dist = 2*ses_->reduced_surface_->probe_radius_;

        Position nx = (Position)((x_max-x_min)/dist+5);
        Position ny = (Position)((y_max-y_min)/dist+5);
        Position nz = (Position)((z_max-z_min)/dist+5);

        Vector3 origin(x_min-2*dist,y_min-2*dist,z_min-2*dist);
        HashGrid3<Position> grid(origin,nx,ny,nz,dist);
        Vector3 pos;
        for (Position i = 0; i != ses_->number_of_spheric_faces_; i++)
        {
            pos.set(ses_->spheric_faces_[i]->rsface_->center_.x,
                            ses_->spheric_faces_[i]->rsface_->center_.y,
                            ses_->spheric_faces_[i]->rsface_->center_.z);
            grid.insert(pos,i);
        }

        std::list<SESEdge*>::iterator edge;
        std::list<SESEdge*> deletable_edges;
        for (edge  = ses_->singular_edges_.begin();
                 edge != ses_->singular_edges_.end();
                 edge++)
        {
            treatSingularEdge(*edge, grid, deletable_edges);
        }

        for (edge = deletable_edges.begin(); edge != deletable_edges.end(); edge++)
        {
            (*edge)->face_[0]->edge_.remove(*edge);
            (*edge)->face_[1]->edge_.remove(*edge);
            (*edge)->vertex_[0]->edges_.erase(*edge);
            (*edge)->vertex_[1]->edges_.erase(*edge);
            ses_->edges_[(*edge)->index_] = NULL;
            ses_->singular_edges_.remove(*edge);
            delete *edge;
        }
    }

    void SESSingularityCleaner::treatSingularEdge(SESEdge* edge, HashGrid3<Position>& grid,
                                                  std::list<SESEdge*>& deletable_edges)
    {
        if (edge->vertex_[0] == NULL)
        {
            return;
        }
        TAngle<double> phi = getOrientedAngle(edge->vertex_[0]->point_-edge->circle_.p,
                                                                         edge->vertex_[1]->point_-edge->circle_.p,
                                                                         edge->circle_.n);
        std::list<Intersection> intersections;
        getIntersectionsOfSingularEdge(edge,phi,grid,intersections);
        if (!intersections.empty())
        {
            std::list<Intersection> min;
            std::list<Intersection> max;
            getExtrema(intersections,min,max);
            HashSet<Index> indices;
            std::list<Intersection>::const_iterator i;
            for (i = min.begin(); i != min.end(); i++)
            {
                indices.insert(i->first.second);
            }
            for (i = max.begin(); i != max.end(); i++)
            {
                indices.insert(i->first.second);
            }
            Index face1 = edge->face_[0]->index_;
            Index face2 = edge->face_[1]->index_;
            indices.insert(face1);
            indices.insert(face2);
            SESVertex* end_vertex1(0);
            SESVertex* end_vertex2(0);
            Index actual_min(0);
            Index actual_max(0);
            buildEndEdges(edge,min,max,end_vertex1,end_vertex2,
                                        actual_min,actual_max);
            Index next_face = actual_min;
            SESVertex* vertex = end_vertex1;
            while ((next_face != face2) && (vertex != NULL))
            {
                buildEdge(edge,face1,next_face,face2,vertex,indices,true);
            }
            if (next_face != face2)
            {
                next_face = actual_max;
                vertex = end_vertex2;
                while ((next_face != face2) && (vertex != NULL))
                {
                    buildEdge(edge,face1,next_face,face2,vertex,indices,false);
                }
            }
            face2 = face1;
            face1 = edge->face_[1]->index_;
            next_face = actual_min;
            vertex = end_vertex1;
            while ((next_face != face2) && (vertex != NULL))
            {
                buildEdge(edge,face1,next_face,face2,vertex,indices,true);
            }
            if (next_face != face2)
            {
                next_face = actual_max;
                vertex = end_vertex2;
                while ((next_face != face2) && (vertex != NULL))
                {
                    buildEdge(edge,face1,next_face,face2,vertex,indices,false);
                }
            }
            deletable_edges.push_back(edge);
        }
    }


    void SESSingularityCleaner::getExtrema
            (const std::list<Intersection>& intersections,
             std::list<Intersection>& min,
             std::list<Intersection>& max)

    {
        TAngle<double> min_phi(2*Constants::PI);
        TAngle<double> max_phi(0,true);
        std::list<Intersection>::const_iterator i;
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 0.0001;
        for (i = intersections.begin(); i != intersections.end(); i++)
        {
            if (i->first.first <= min_phi)
            {
                if (i->first.first < min_phi)
                {
                    min.clear();
                    min_phi = i->first.first;
                }
                min.push_back(*i);
            }
            if (i->first.first >= max_phi)
            {
                if (i->first.first > max_phi)
                {
                    max.clear();
                    max_phi = i->first.first;
                }
                max.push_back(*i);
            }
        }
        Constants::EPSILON = epsilon;
    }


    void SESSingularityCleaner::getIntersectionsOfSingularEdge
        (SESEdge*							edge,
         const TAngle<double>&					phi,
         HashGrid3<Position>&			grid,
         std::list<Intersection>& intersections)

    {
        TQuaternion<double> rotate(edge->circle_.n,phi/2);
        TMatrix4x4<double> rotation;
        rotate.getRotationMatrix(rotation);
        TVector4<double> middle_(edge->vertex_[0]->point_.x-edge->circle_.p.x,
                                                edge->vertex_[0]->point_.y-edge->circle_.p.y,
                                                edge->vertex_[0]->point_.z-edge->circle_.p.z,
                                                0);
        middle_ = rotation*middle_;
        TVector3<double> middle(middle_.x+edge->circle_.p.x,
                                             middle_.y+edge->circle_.p.y,
                                             middle_.z+edge->circle_.p.z);
        Index face1 = edge->face_[0]->index_;
        Index face2 = edge->face_[1]->index_;
        TAngle<double> phi1;
        TAngle<double> phi2;
        TVector3<double> point1;
        TVector3<double> point2;
        TSphere3<double> probe;
        probe.radius = ses_->reduced_surface_->probe_radius_;
        Intersection intersection;
        HashGridBox3<Position>* box(0);
        HashGridBox3<Position>::ConstBoxIterator b;
        HashGridBox3<Position>::ConstDataIterator d;
        Vector3 pos(edge->circle_.p.x,
                                edge->circle_.p.y,
                                edge->circle_.p.z);
        box = grid.getBox(pos);
        if (box != NULL)
        {
            for (b = box->beginBox(); b != box->endBox(); b++)
            {
                for (d = b->beginData(); d != b->endData(); d++)
                {
                    if (((Index)*d != face1) && ((Index)*d != face2))
                    {
                        if (getIntersectionPointsAndAngles
                                    (edge->circle_,edge->vertex_[0]->point_,
                                     edge->face_[0]->index_,edge->face_[1]->index_,
                                     ses_->spheric_faces_[*d]->index_,
                                     phi1,point1,phi2,point2))
                        {
                            probe.p = ses_->spheric_faces_[*d]->rsface_->center_;
                            if (isIntersection(phi1,phi2,phi,middle,probe))
                            {
                                intersection.first.second = *d;
                                intersection.first.first = phi1;
                                intersection.second = point1;
                                intersections.push_back(intersection);
                                intersection.first.first = phi2;
                                intersection.second = point2;
                                intersections.push_back(intersection);
                            }
                        }
                    }
                }
            }
        }
    }


    bool SESSingularityCleaner::getIntersectionPointsAndAngles
        (const TCircle3<double>& circle,
         const TVector3<double>& point,
         Position			index1,
         Position			index2,
         Position			probe_index,
         TAngle<double>&		phi1,
         TVector3<double>&	point1,
         TAngle<double>&		phi2,
         TVector3<double>&	point2)

    {
        if (probeIntersection(index1,
                                                    index2,
                                                    probe_index,
                                                    point1,
                                                    point2))
        {
            phi1 = getOrientedAngle(point-circle.p,
                                                            point1-circle.p,
                                                            circle.n);
            phi2 = getOrientedAngle(point-circle.p,
                                                            point2-circle.p,
                                                            circle.n);
            double epsilon = Constants::EPSILON;
            Constants::EPSILON = 0.001;
            if (Maths::isEqual(phi1.value,2*Constants::PI))
            {
                phi1.value = 0.0;
            }
            if (Maths::isEqual(phi2.value,2*Constants::PI))
            {
                phi2.value = 0.0;
            }
            Constants::EPSILON = epsilon;
            if (phi2 < phi1)
            {
                phi1.swap(phi2);
                point1.swap(point2);
            }
            return true;
        }
        else
        {
            return false;
        }
    }


    bool SESSingularityCleaner::isIntersection
         (const TAngle<double>&	 min_phi,
            const TAngle<double>&	 max_phi,
            const TAngle<double>&	 phi,
            const TVector3<double>& middle,
            const TSphere3<double>& probe)

    {
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 0.001;
        bool back;
        if (max_phi > phi)
        {
            back = false;
        }
        else
        {
            if (Maths::isNotZero(min_phi.value) || (max_phi < phi))
            {
                back = true;
            }
            else
            {
                double epsilon = Constants::EPSILON;
                Constants::EPSILON = 1e-6;
                if (Maths::isGreater(probe.p.getSquareDistance(middle),
                                                         probe.radius*probe.radius))
                {
                    back = false;
                }
                else
                {
                    back = true;
                }
                Constants::EPSILON = epsilon;
            }
        }
        Constants::EPSILON = epsilon;
        return back;
    }


    void SESSingularityCleaner::buildEndEdges
        (SESEdge*										edge,
         const std::list<Intersection>&	min,
         const std::list<Intersection>&	max,
         SESVertex*&								vertex1,
         SESVertex*&								vertex2,
         Index&													actual_min,
         Index&													actual_max)

    {
        buildEndEdge(edge,min,vertex1,actual_min,true);
        buildEndEdge(edge,max,vertex2,actual_max,false);
    }


    void SESSingularityCleaner::buildEndEdge
        (SESEdge*										edge,
         const std::list<Intersection>&	extrema,
         SESVertex*&								vertex,
         Index&													actual_extremum,
         bool														min)

    {
        vertex = NULL;
        std::list<Intersection>::const_iterator m;
        for (m = extrema.begin(); m != extrema.end(); m++)
        {
            Index test = vertexExists(m->second);
            if (test != -1)
            {
                vertex = ses_->vertices_[test];
                actual_extremum = m->first.second;
            }
        }
        if (vertex == NULL)
        {
            Intersection absolute_extremum = extrema.front();
            if (min)
            {
                for (m = extrema.begin(); m != extrema.end(); m++)
                {
                    if (m->first.first.value < absolute_extremum.first.first.value)
                    {
                        absolute_extremum = *m;
                    }
                }
            }
            else
            {
                for (m = extrema.begin(); m != extrema.end(); m++)
                {
                    if (m->first.first.value > absolute_extremum.first.first.value)
                    {
                        absolute_extremum = *m;
                    }
                }
            }
            actual_extremum = absolute_extremum.first.second;
            vertex = new SESVertex(absolute_extremum.second,
                                                                 edge->circle_.p-absolute_extremum.second,
                                                                 -2,ses_->number_of_vertices_);
            ses_->vertices_.push_back(vertex);
            Vector3 pos(vertex->point_.x,
                                    vertex->point_.y,
                                    vertex->point_.z);
            vertex_grid_->insert(pos,vertex->index_);
            ses_->number_of_vertices_++;
        }
        Position v = (min ? 0 : 1);
        if (vertex != edge->vertex_[v])
        {
            SESEdge* new_edge(0);
            new_edge = new SESEdge(*edge,true);
            new_edge->vertex_[1-v] = vertex;
            new_edge->rsedge_ = NULL;
            new_edge->index_ = ses_->number_of_edges_;
            ses_->edges_.push_back(new_edge);
            ses_->number_of_edges_++;
            ses_->singular_edges_.push_front(new_edge);
            ses_->number_of_singular_edges_++;
            new_edge->vertex_[0]->edges_.insert(new_edge);
            new_edge->vertex_[1]->edges_.insert(new_edge);
            new_edge->face_[0]->edge_.push_back(new_edge);
            new_edge->face_[1]->edge_.push_back(new_edge);
            new_edge->face_[0]->insert(new_edge->vertex_[1-v]);
            new_edge->face_[1]->insert(new_edge->vertex_[1-v]);
            vertex->faces_.insert(new_edge->face_[0]);
            vertex->faces_.insert(new_edge->face_[1]);
        }
    }


    void SESSingularityCleaner::buildEdge
        (SESEdge*						edge,
         Index									face1,
         Index&									face2,
         Index									end,
         SESVertex*&				vertex,
         const HashSet<Index>&	indices,
         bool										minimum)

    {
        SESFace* spheric_face1 = ses_->spheric_faces_[face1];
        SESFace* spheric_face2 = ses_->spheric_faces_[face2];
        TSphere3<double> probe1(spheric_face1->rsface_->center_,
                                             ses_->reduced_surface_->probe_radius_);
        TSphere3<double> probe2(spheric_face2->rsface_->center_,
                                             ses_->reduced_surface_->probe_radius_);
        TCircle3<double> circle;
        GetIntersection(probe1,probe2,circle);
        Index sign = (minimum ? -1 : 1);
        if (((probe1.p-edge->circle_.p)*edge->circle_.n)*
                ((probe1.p-circle.p)*circle.n)*sign						> 0)
        {
            circle.n.negate();
        }
        TVector3<double> point(vertex->point_);
        TVector3<double> point1;
        TVector3<double> point2;
        TAngle<double> phi1;
        TAngle<double> phi2;
        std::list< std::pair<TVector3<double>,Index> > min;
        TAngle<double> min_phi(2*Constants::PI,true);
        std::pair<TVector3<double>,Index> new_min;
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 0.001;
        HashSet<Index>::ConstIterator i;
        for (i = indices.begin(); i != indices.end(); i++)
        {
            if ((*i != face1) && (*i != face2))
            {
                if (getIntersectionPointsAndAngles(circle,point,face1,face2,*i,
                                                                                     phi1,point1,phi2,point2))
                {
                    if ((phi1 <= min_phi) && Maths::isGreater(phi1.value,0.0))
                    {
                        if (phi1 < min_phi)
                        {
                            min.clear();
                        }
                        min_phi = phi1;
                        new_min.first = point1;
                        new_min.second = *i;
                        min.push_back(new_min);
                    }
                    if ((phi2 <= min_phi) && Maths::isGreater(phi2.value,0.0))
                    {
                        if (phi2 < min_phi)
                        {
                            min.clear();
                        }
                        min_phi = phi2;
                        new_min.first = point2;
                        new_min.second = *i;
                        min.push_back(new_min);
                    }
                }
            }
        }
        Constants::EPSILON = epsilon;
        SESVertex* new_vertex = NULL;
        std::list< std::pair<TVector3<double>,Index> >::iterator m
                = min.begin();
        bool not_found = true;
        while (not_found && (m != min.end()))
        {
            if (m->second == end)
            {
                point2 = m->first;
                face2 = end;
                Index index = vertexExists(point2);
                if (index != -1)
                {
                    new_vertex = ses_->vertices_[index];
                }
                not_found = false;
            }
            m++;
        }
        if (not_found)
        {
            Index index = -1;
            m = min.begin();
            while ((index == -1) && (m != min.end()))
            {
                point2 = m->first;
                face2 = m->second;
                index = vertexExists(point2);
                if (index != -1)
                {
                    new_vertex = ses_->vertices_[index];
                }
                m++;
            }
        }
        if (spheric_face1->isNeighbouredTo(spheric_face2) == false)
        {
            point1.set(vertex->point_);
            if (new_vertex == NULL)
            {
                new_vertex = new SESVertex(point2,circle.p-point2,-2,
                                                                             ses_->number_of_vertices_);
                ses_->vertices_.push_back(new_vertex);
                Vector3 pos(new_vertex->point_.x,
                                        new_vertex->point_.y,
                                        new_vertex->point_.z);
                vertex_grid_->insert(pos,new_vertex->index_);
                ses_->number_of_vertices_++;
            }
            SESEdge* new_edge = new SESEdge;
            new_edge->vertex_[0] = vertex;
            new_edge->vertex_[1] = new_vertex;
            new_edge->face_[0] = spheric_face1;
            new_edge->face_[1] = spheric_face2;
            new_edge->type_ = SESEdge::TYPE_SINGULAR;
            new_edge->circle_ = circle;
            new_edge->rsedge_ = NULL;
            new_edge->index_ = ses_->number_of_edges_;
            ses_->edges_.push_back(new_edge);
            ses_->number_of_edges_++;
            ses_->singular_edges_.push_back(new_edge);
            ses_->number_of_singular_edges_++;
            spheric_face1->edge_.push_back(new_edge);
            spheric_face2->edge_.push_back(new_edge);
            vertex->edges_.insert(new_edge);
            new_vertex->edges_.insert(new_edge);
            spheric_face1->insert(vertex);
            spheric_face2->insert(vertex);
            spheric_face1->insert(new_vertex);
            spheric_face2->insert(new_vertex);
            vertex->faces_.insert(spheric_face1);
            vertex->faces_.insert(spheric_face2);
            new_vertex->faces_.insert(spheric_face1);
            new_vertex->faces_.insert(spheric_face2);
            vertex = new_vertex;
        }
        else
        {
            vertex = NULL;
        }
    }


    bool SESSingularityCleaner::probeIntersection
         (Index				 face1,
            Index				 face2,
            Index				 face3,
            TVector3<double>& point1,
            TVector3<double>& point2)

    {
        // sort the indices of the spheric faces
        sort(face1,face2,face3,face1,face2,face3);
        // try to find the intersection points
        HashMap< Position,
                                            HashMap< Position,
                                                             HashMap< Position,
                                                                                ProbeIntersection* > > >::Iterator i1;
        HashMap< Position,
                                            HashMap< Position,
                                                             ProbeIntersection* > >::Iterator i2;
        HashMap< Position,ProbeIntersection* >::Iterator i3;
        bool back = false;
        bool found = false;
        i1 = probe_intersections_.find(face1);
        if (i1 != probe_intersections_.end())
        {
            i2 = i1->second.find(face2);
            if (i2 != i1->second.end())
            {
                i3 = i2->second.find(face3);
                if (i3 != i2->second.end())
                {
                    found = true;
                    if (i3->second == NULL)
                    {
                        back = false;
                    }
                    else
                    {
                        point1 = i3->second->point[0];
                        point2 = i3->second->point[1];
                        back = true;
                    }
                }
            }
        }
        // if the intersection points are not computed yet, compute them now
        if (found == false)
        {
            TSphere3<double> s1(ses_->spheric_faces_[face1]->rsface_->center_,
                                 ses_->reduced_surface_->probe_radius_);
            TSphere3<double> s2(ses_->spheric_faces_[face2]->rsface_->center_,
                                 ses_->reduced_surface_->probe_radius_);
            TSphere3<double> s3(ses_->spheric_faces_[face3]->rsface_->center_,
                                 ses_->reduced_surface_->probe_radius_);
            if (GetIntersection(s1,s2,s3,point1,point2,false))
            {
                ProbeIntersection* intersection = new ProbeIntersection;
                intersection->point[0] = point1;
                intersection->point[1] = point2;
                probe_intersections_[face1][face2][face3] = intersection;
                back = true;
            }
            else
            {
                probe_intersections_[face1][face2][face3] = NULL;
                back = false;
            }
        }
        return back;
    }


    void SESSingularityCleaner::sort
         (Index		u1,
            Index		u2,
            Index		u3,
            Index&	s1,
            Index&	s2,
            Index&	s3)

    {
        s1 = u1;
        s2 = u2;
        s3 = u3;
        Index tmp;
        if (s1 > s2)
        {
            tmp = s1;
            s1 = s2;
            s2 = tmp;
        }
        if (s1 > s3)
        {
            tmp = s1;
            s1 = s3;
            s3 = tmp;
        }
        if (s2 > s3)
        {
            tmp = s2;
            s2 = s3;
            s3 = tmp;
        }
    }


    Index SESSingularityCleaner::vertexExists(TVector3<double> point)

    {
        double epsilon = Constants::EPSILON;
        Constants::EPSILON = 0.001;
        Vector3 p(point.x,point.y,point.z);
        HashGridBox3<Index>* box = vertex_grid_->getBox(p);
        HashGridBox3<Index>::ConstBoxIterator b;
        HashGridBox3<Index>::ConstDataIterator d;
        if (box != NULL)
        {
            for (b = box->beginBox(); b != box->endBox(); b++)
            {
                for (d = b->beginData(); d != b->endData(); d++)
                {
                    if (ses_->vertices_[*d]->point_ == point)
                    {
                        Constants::EPSILON = epsilon;
                        return *d;
                    }
                }
            }
        }
        Constants::EPSILON = epsilon;
        return -1;
    }



}	// namespace BALL
