#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/RSEdge.h>
#include <BALL/STRUCTURE/RSFace.h>
#include <BALL/STRUCTURE/RSVertex.h>
#include <BALL/MATHS/vector3.h>

namespace BALL
{

	RSFace::RSFace()
		
		: GraphTriangle< RSVertex,RSEdge,RSFace >(),
			center_(),
			normal_(),
			singular_(false)
	{
	}


	RSFace::RSFace(const RSFace& rsface, bool deep)
		
		:	GraphTriangle< RSVertex,RSEdge,RSFace >(rsface,deep),
			center_(rsface.center_),
			normal_(rsface.normal_),
			singular_(rsface.singular_)
	{
	}


	RSFace::RSFace(RSVertex* vertex1,
			RSVertex* vertex2,
			RSVertex* vertex3,
			RSEdge* edge1,
			RSEdge* edge2,
			RSEdge* edge3,
			const TVector3<double>& center,
			const TVector3<double>& normal,
			bool singular,
			Index index)
		throw(Exception::DivisionByZero)
		:	GraphTriangle< RSVertex,RSEdge,RSFace >
					(vertex1,vertex2,vertex3,edge1,edge2,edge3,index),
			center_(center),
			normal_(normal),
			singular_(singular)
	{
		normal_.normalize();
	}


	RSFace::~RSFace()
		
	{
	}


	void RSFace::set(const RSFace& rsface, bool deep)
		
	{
		if (this != &rsface)
		{
			GraphTriangle< RSVertex,RSEdge,RSFace >::set(rsface,deep);
			center_.set(rsface.center_);
			normal_.set(rsface.normal_);
			singular_ = rsface.singular_;
		}
	}


	RSFace& RSFace::operator = (const RSFace& rsface)
		
	{
		if (this != &rsface)
		{
			GraphTriangle< RSVertex,RSEdge,RSFace >::operator = (rsface);
			center_.set(rsface.center_);
			normal_.set(rsface.normal_);
			singular_ = rsface.singular_;
		}
		return *this;
	}


	void RSFace::set(RSVertex* vertex1,
			RSVertex* vertex2,
			RSVertex* vertex3,
			RSEdge* edge1,
			RSEdge* edge2,
			RSEdge* edge3,
			const TVector3<double>& center,
			const TVector3<double>& normal,
			bool singular,
			Index index)
		throw(Exception::DivisionByZero)
	{
		GraphTriangle< RSVertex,RSEdge,RSFace >::set
				(vertex1,vertex2,vertex3,edge1,edge2,edge3,index);
		center_.set(center);
		normal_.set(normal);
		singular_ = singular;
		normal_.normalize();
	}


	bool RSFace::operator == (const RSFace& rsface) const
		
	{
		return (
						(center_ == rsface.center_) 														&&
						(((vertex_[0]->atom_ == rsface.vertex_[0]->atom_) &&
							(vertex_[1]->atom_ == rsface.vertex_[1]->atom_) &&
							(vertex_[2]->atom_ == rsface.vertex_[2]->atom_)		)	||
						 ((vertex_[0]->atom_ == rsface.vertex_[0]->atom_) &&
							(vertex_[1]->atom_ == rsface.vertex_[2]->atom_) &&
							(vertex_[2]->atom_ == rsface.vertex_[1]->atom_)		)	||
						 ((vertex_[0]->atom_ == rsface.vertex_[1]->atom_) &&
							(vertex_[1]->atom_ == rsface.vertex_[0]->atom_) &&
							(vertex_[2]->atom_ == rsface.vertex_[2]->atom_)		)	||
						 ((vertex_[0]->atom_ == rsface.vertex_[1]->atom_) &&
							(vertex_[1]->atom_ == rsface.vertex_[2]->atom_) &&
							(vertex_[2]->atom_ == rsface.vertex_[0]->atom_)		)	||
						 ((vertex_[0]->atom_ == rsface.vertex_[2]->atom_) &&
							(vertex_[1]->atom_ == rsface.vertex_[0]->atom_) &&
							(vertex_[2]->atom_ == rsface.vertex_[1]->atom_)		)	||
						 ((vertex_[0]->atom_ == rsface.vertex_[2]->atom_) &&
							(vertex_[1]->atom_ == rsface.vertex_[1]->atom_) &&
							(vertex_[2]->atom_ == rsface.vertex_[0]->atom_)		)			)
					 );
	}


	bool RSFace::operator != (const RSFace& rsface) const
		
	{
		return ( ! (*this == rsface) );
	}


	bool RSFace::operator *= (const RSFace& rsface) const
		
	{
		return (
						((vertex_[0]->atom_ == rsface.vertex_[0]->atom_) &&
						 (vertex_[1]->atom_ == rsface.vertex_[1]->atom_) &&
						 (vertex_[2]->atom_ == rsface.vertex_[2]->atom_)		)	||
						((vertex_[0]->atom_ == rsface.vertex_[0]->atom_) &&
						 (vertex_[1]->atom_ == rsface.vertex_[2]->atom_) &&
						 (vertex_[2]->atom_ == rsface.vertex_[1]->atom_)		)	||
						((vertex_[0]->atom_ == rsface.vertex_[1]->atom_) &&
						 (vertex_[1]->atom_ == rsface.vertex_[0]->atom_) &&
						 (vertex_[2]->atom_ == rsface.vertex_[2]->atom_)		)	||
						((vertex_[0]->atom_ == rsface.vertex_[1]->atom_) &&
						 (vertex_[1]->atom_ == rsface.vertex_[2]->atom_) &&
						 (vertex_[2]->atom_ == rsface.vertex_[0]->atom_)		)	||
						((vertex_[0]->atom_ == rsface.vertex_[2]->atom_) &&
						 (vertex_[1]->atom_ == rsface.vertex_[0]->atom_) &&
						 (vertex_[2]->atom_ == rsface.vertex_[1]->atom_)		)	||
						((vertex_[0]->atom_ == rsface.vertex_[2]->atom_) &&
						 (vertex_[1]->atom_ == rsface.vertex_[1]->atom_) &&
						 (vertex_[2]->atom_ == rsface.vertex_[0]->atom_)		)
					 );
	}


	bool RSFace::isSingular() const
		
	{
		return singular_;
	}


	void RSFace::setCenter(const TVector3<double>& center)
		
	{
		center_ = center;
	}


	TVector3<double> RSFace::getCenter() const
		
	{
		return center_;
	}


	void RSFace::setNormal(const TVector3<double>& normal)
		throw(Exception::DivisionByZero)
	{
		normal_ = normal;
		normal_.normalize();
	}


	TVector3<double> RSFace::getNormal() const
		
	{
		return normal_;
	}


	void RSFace::setSingular(bool singular)
		
	{
		singular_ = singular;
	}


	void RSFace::remove
			(HashSet<RSEdge*>&		edges,
			 HashSet<RSVertex*>&	vertices,
			 HashSet<RSFace*>&		faces)
		
	{
		vertex_[0]->faces_.erase(this);
		vertex_[1]->faces_.erase(this);
		vertex_[2]->faces_.erase(this);
		if (edge_[0] != NULL)
		{
			if (edge_[0]->face_[1] == NULL)
			{
				edge_[0]->vertex_[0]->edges_.erase(edge_[0]);
				edge_[0]->vertex_[1]->edges_.erase(edge_[0]);
				vertices.insert(edge_[0]->vertex_[0]);
				vertices.insert(edge_[0]->vertex_[1]);
				edges.insert(edge_[0]);
				edge_[0] = NULL;
			}
			else
			{
				faces.insert(edge_[0]->remove(this));
			}
		}
		if (edge_[1] != NULL)
		{
			if (edge_[1]->face_[1] == NULL)
			{
				edge_[1]->vertex_[0]->edges_.erase(edge_[1]);
				edge_[1]->vertex_[1]->edges_.erase(edge_[1]);
				vertices.insert(edge_[1]->vertex_[0]);
				vertices.insert(edge_[1]->vertex_[1]);
				edges.insert(edge_[1]);
				edge_[1] = NULL;
			}
			else
			{
				faces.insert(edge_[1]->remove(this));
			}
		}
		if (edge_[2] != NULL)
		{
			if (edge_[2]->face_[1] == NULL)
			{
				edge_[2]->vertex_[0]->edges_.erase(edge_[2]);
				edge_[2]->vertex_[1]->edges_.erase(edge_[2]);
				vertices.insert(edge_[2]->vertex_[0]);
				vertices.insert(edge_[2]->vertex_[1]);
				edges.insert(edge_[2]);
				edge_[2] = NULL;
			}
			else
			{
				faces.insert(edge_[2]->remove(this));
			}
		}
	}


	std::ostream& operator << (std::ostream& s, const RSFace& rsface)
	{
		s << "RSFACE"
			<< rsface.getIndex()
			<< "(["
			<< (rsface.getVertex(0) == NULL
									? -2
									: rsface.getVertex(0)->getIndex()) << ' '
			<< (rsface.getVertex(1) == NULL
									? -2
									: rsface.getVertex(1)->getIndex()) << ' '
			<< (rsface.getVertex(2) == NULL
									? -2
									: rsface.getVertex(2)->getIndex())
			<< "] ["
			<< (rsface.getEdge(0) == NULL
									? -2
									: rsface.getEdge(0)->getIndex()) << ' '
			<< (rsface.getEdge(1) == NULL
									? -2
									: rsface.getEdge(1)->getIndex()) << ' '
			<< (rsface.getEdge(2) == NULL
									? -2
									: rsface.getEdge(2)->getIndex())
			<< "] "
			<< rsface.getCenter() << ' '
			<< rsface.getNormal() << ' '
			<< (rsface.isSingular() ? "true" : "false") << ")";
		return s;
	}

} // namespace BALL
