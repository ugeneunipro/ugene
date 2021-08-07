#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/solventAccessibleSurface.h>
#include <BALL/STRUCTURE/SASEdge.h>
#include <BALL/STRUCTURE/SASFace.h>
#include <BALL/STRUCTURE/SASVertex.h>
#include <BALL/STRUCTURE/reducedSurface.h>
#include <BALL/MATHS/sphere3.h>

#include <vector>

namespace BALL
{

	SolventAccessibleSurface::SolventAccessibleSurface()
	 : number_of_vertices_(0),
	   vertices_(),
	   number_of_edges_(0),
	   edges_(),
	   number_of_faces_(0),
	   faces_(),
	   reduced_surface_(NULL)
	{
	}

	SolventAccessibleSurface::SolventAccessibleSurface(const SolventAccessibleSurface&, bool)
	 : number_of_vertices_(0),
	   vertices_(),
	   number_of_edges_(0),
	   edges_(),
	   number_of_faces_(0),
	   faces_(),
	   reduced_surface_(NULL)
	{				// ????? !!!
		//for (Position i = 0; i < number_of_vertices_; i++)
		//{
		//	vertices_[i] = new SASVertex(*ses.vertices_[i],false);
		//}
		//for (Position i = 0; i < number_of_edges_; i++)
		//{
		//	edges_[i] = new SASVertex(*ses.edges_[i],false);
		//}
		// ::std::list<SASEdge*>::const_iterator se;
		//for (se = ses.singular_edges_.begin(); se != ses.singular_edges_.end(); se++)
		//{
		//	singular_edges_.push_back(edges_[(*se)->index_]);
		//}
		//for (Position i = 0; i < number_of_contact_faces_; i++)
		//{
		//	contact_faces_[i] = new SASFace(*ses.contact_faces_[i],false);
		//}
		//for (Position i = 0; i < number_of_toric_faces_; i++)
		//{
		//	toric_faces_[i] = new SASFace(*ses.toric_faces_[i],false);
		//}
		//for (Position i = 0; i < number_of_spheric_faces_; i++)
		//{
		//	spheric_faces_[i] = new SASFace(*ses.spheric_faces_[i],false);
		//}
	}

	SolventAccessibleSurface::SolventAccessibleSurface(ReducedSurface* reduced_surface)
	 : number_of_vertices_(0),
	   vertices_(),
	   number_of_edges_(0),
	   edges_(),
	   number_of_faces_(0),
	   faces_(),
	   reduced_surface_(reduced_surface)
	{
	}

	SolventAccessibleSurface::~SolventAccessibleSurface()
	{
		clear();
	}

	void SolventAccessibleSurface::setVertex(SASVertex* vertex, Position i)
		throw(Exception::IndexOverflow)
	{
		if (i < number_of_vertices_)
		{
			vertices_[i] = vertex;
		}
		else
		{
			throw Exception::IndexOverflow(__FILE__, __LINE__, i, number_of_vertices_+1);
		}
	}

	SASVertex* SolventAccessibleSurface::getVertex(Position i) const
		throw(Exception::IndexOverflow)
	{
		if (i < number_of_vertices_)
		{
			return vertices_[i];
		}
		else
		{
			throw Exception::IndexOverflow(__FILE__, __LINE__, i, number_of_vertices_+1);
		}
	}

	Size SolventAccessibleSurface::numberOfVertices() const
	{
		return number_of_vertices_;
	}
		
	void SolventAccessibleSurface::setEdge(SASEdge* edge, Position i)
		throw(Exception::IndexOverflow)
	{
		if (i < number_of_edges_)
		{
			edges_[i] = edge;
		}
		else
		{
			throw Exception::IndexOverflow(__FILE__, __LINE__, i, number_of_edges_+1);
		}
	}

	SASEdge* SolventAccessibleSurface::getEdge(Position i) const
		throw(Exception::IndexOverflow)
	{
		if (i < number_of_edges_)
		{
			return edges_[i];
		}
		else
		{
			throw Exception::IndexOverflow(__FILE__, __LINE__, i, number_of_edges_+1);
		}
	}

	Size SolventAccessibleSurface::numberOfEdges() const
	{
		return number_of_edges_;
	}
		
	void SolventAccessibleSurface::setFace(SASFace* face, Position i)
		throw(Exception::IndexOverflow)
	{
		if (i < number_of_faces_)
		{
			faces_[i] = face;
		}
		else
		{
			throw Exception::IndexOverflow(__FILE__, __LINE__, i, number_of_faces_+1);
		}
	}

	SASFace* SolventAccessibleSurface::getFace(Position i) const
		throw(Exception::IndexOverflow)
	{
		if (i < number_of_faces_)
		{
			return faces_[i];
		}
		else
		{
			throw Exception::IndexOverflow(__FILE__, __LINE__, i, number_of_faces_+1);
		}
	}

	Size SolventAccessibleSurface::numberOfFaces() const
	{
		return number_of_faces_;
	}

	void SolventAccessibleSurface::setRS(ReducedSurface* rs)
	{
		reduced_surface_ = rs;
	}

	ReducedSurface* SolventAccessibleSurface::getRS() const
	{
		return reduced_surface_;
	}

	void SolventAccessibleSurface::clear()
	{
		for (Position i = 0; i < number_of_vertices_; i++)
		{
			delete vertices_[i];
		}
		
		for (Position i = 0; i < number_of_edges_; i++)
		{
			delete edges_[i];
		}
		
		for (Position i = 0; i < number_of_faces_; i++)
		{
			delete faces_[i];
		}

		vertices_.clear();
		edges_.clear();
		faces_.clear();

		number_of_vertices_ = 0;
		number_of_edges_ = 0;
		number_of_faces_ = 0;
	}

	void SolventAccessibleSurface::compute()
	{
		preProcessing();
		get();
	}

	void SolventAccessibleSurface::preProcessing()
	{
		number_of_vertices_ = reduced_surface_->number_of_faces_;
		number_of_edges_    = reduced_surface_->number_of_edges_;
		number_of_faces_    = reduced_surface_->number_of_vertices_;
		
		for (Position i = 0; i < number_of_vertices_; i++)
		{
			SASVertex* vertex = new SASVertex;
			vertex->index_ = i;
			vertices_.push_back(vertex);
		}

		for (Position i = 0; i < number_of_edges_; i++)
		{
			SASEdge* edge = new SASEdge;
			edge->index_ = i;
			edges_.push_back(edge);
		}

		for (Position i = 0; i < number_of_faces_; i++)
		{
			SASFace* face = new SASFace;
			face->index_ = i;
			faces_.push_back(face);
		}
	}

	void SolventAccessibleSurface::get()
	{
		for (Position i = 0; i < number_of_vertices_; i++)
		{
			createVertex(i);
		}

		for (Position i = 0; i < number_of_edges_; i++)
		{
			createEdge(i);
		}

		for (Position i = 0; i < number_of_faces_; i++)
		{
			createFace(i);
		}
	}

	void SolventAccessibleSurface::createVertex(Position j)
	{
		SASVertex* vertex = vertices_[j];
		vertex->index_ = j;
		RSFace* rsface = reduced_surface_->faces_[j];
		vertex->point_ = rsface->center_;

		for (Position i = 0; i < 3; i++)
		{
			vertex->edges_.insert(edges_[rsface->edge_[i]->index_]);
			vertex->faces_.insert(faces_[rsface->vertex_[i]->index_]);
		}
	}

	void SolventAccessibleSurface::createEdge(Position j)
	{
		SASEdge* edge = edges_[j];
		edge->index_ = j;
		RSEdge* rsedge = reduced_surface_->edges_[j];

		if (rsedge->face_[0] != NULL)
		{
			edge->vertex_[0] = vertices_[rsedge->face_[0]->index_];
			edge->vertex_[1] = vertices_[rsedge->face_[1]->index_];
		}
		else
		{
			edge->vertex_[0] = NULL;
			edge->vertex_[1] = NULL;
		}

		edge->face_[0] = faces_[rsedge->vertex_[0]->index_];
		edge->face_[1] = faces_[rsedge->vertex_[1]->index_];
		edge->angle_ = rsedge->angle_;
		edge->circle_.p = rsedge->center_of_torus_;
		edge->circle_.radius = rsedge->radius_of_torus_;

		TSphere3<double> atom1(reduced_surface_->atom_[rsedge->vertex_[0]->atom_]);
		TSphere3<double> atom2(reduced_surface_->atom_[rsedge->vertex_[1]->atom_]);
		
		edge->circle_.n = atom1.p-atom2.p;
	}


	void SolventAccessibleSurface::createFace(Position j)
	{
		SASFace* face = faces_[j];
		face->index_ = j;
		RSVertex* rsvertex = reduced_surface_->vertices_[j];
		HashSet<RSEdge*>::Iterator e;
		for (e = rsvertex->edges_.begin(); e != rsvertex->edges_.end(); e++)
		{
			face->edge_.push_back(edges_[(*e)->index_]);
			if ((*e)->vertex_[0]->index_ == face->index_)
			{
				face->orientation_.push_back(true);
			}
			else
			{
				face->orientation_.push_back(false);
			}
		}
		HashSet<RSFace*>::Iterator f;
		for (f = rsvertex->faces_.begin(); f != rsvertex->faces_.end(); f++)
		{
			face->vertex_.push_back(vertices_[(*f)->index_]);
		}
		face->sphere_ = reduced_surface_->atom_[rsvertex->atom_];
		face->sphere_.radius += reduced_surface_->probe_radius_;
	}

	std::ostream& operator << (std::ostream& s, const SolventAccessibleSurface& sas)
	{
		s << "Vertices:\n";
		for (Position i = 0; i < sas.numberOfVertices(); i++)
		{
			if (sas.getVertex(i) != NULL)
			{
				s << "  " << *(sas.getVertex(i)) << "\n";
			}
			else
			{
				s << "  --\n";
			}
		}
		s << "Edges:\n";
		for (Position i = 0; i < sas.numberOfEdges(); i++)
		{
			if (sas.getEdge(i) != NULL)
			{
				s << "  " << *(sas.getEdge(i)) << "\n";
			}
			else
			{
				s << "  --\n";
			}
		}
		s << "Faces:\n";
		for (Position i = 0; i < sas.numberOfFaces(); i++)
		{
			if (sas.getFace(i) != NULL)
			{
				s << "  " << *(sas.getFace(i)) << "\n";
			}
			else
			{
				s << "  --\n";
			}
		}
		return s;
	}

}	// namespace BALL
