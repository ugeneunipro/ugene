#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/SESEdge.h>
#include <BALL/STRUCTURE/SESFace.h>
#include <BALL/STRUCTURE/SESVertex.h>
#include <BALL/STRUCTURE/RSEdge.h>
#include <BALL/STRUCTURE/RSFace.h>
#include <BALL/STRUCTURE/RSVertex.h>


namespace BALL
{

	SESFace::SESFace()
		
		: GraphFace< SESVertex,SESEdge,SESFace >(),
			type_(SESFace::TYPE_SPHERIC),
			rsvertex_(NULL),
			rsedge_(NULL),
			rsface_(NULL)
	{
	}


	SESFace::SESFace(const SESFace& sesface, bool deep)
		
		: GraphFace< SESVertex,SESEdge,SESFace >(sesface,deep),
			type_(sesface.type_),
			rsvertex_(NULL),
			rsedge_(NULL),
			rsface_(NULL)
	{
		if (deep)
		{
			rsvertex_  = sesface.rsvertex_;
			rsedge_ =  sesface.rsedge_;
			rsface_ =  sesface.rsface_;
		}
	}


	SESFace::SESFace(Type type,
			RSVertex* rsvertex,
			RSEdge* rsedge,
			RSFace* rsface)
		
		: GraphFace< SESVertex,SESEdge,SESFace >(),
			type_(type),
			rsvertex_(rsvertex),
			rsedge_(rsedge),
			rsface_(rsface)
	{
	}


	SESFace::~SESFace()
		
	{
	}


	void SESFace::set(const SESFace& sesface, bool deep)
		
	{
		if (this != &sesface)
		{
			GraphFace< SESVertex,SESEdge,SESFace >::set(sesface,deep);
			if (deep)
			{
				rsvertex_  = sesface.rsvertex_;
				rsedge_ =  sesface.rsedge_;
				rsface_ =  sesface.rsface_;
			}
			type_ = sesface.type_;
		}
	}


	SESFace& SESFace::operator = (const SESFace& sesface)
		
	{
		if (this != &sesface)
		{
			GraphFace< SESVertex,SESEdge,SESFace >::operator = (sesface);
			rsvertex_  = sesface.rsvertex_;
			rsedge_ =  sesface.rsedge_;
			rsface_ =  sesface.rsface_;
			type_ = sesface.type_;
		}
		return *this;
	}


	void SESFace::setType(SESFace::Type type)
		
	{
		type_ = type;
	}


	SESFace::Type SESFace::getType() const
		
	{
		return type_;
	}


	void SESFace::setRSVertex(RSVertex* rsvertex)
		
	{
		rsvertex_ = rsvertex;
	}


	RSVertex* SESFace::getRSVertex() const
		
	{
		return rsvertex_;
	}


	void SESFace::setRSEdge(RSEdge* rsedge)
		
	{
		rsedge_ = rsedge;
	}


	RSEdge* SESFace::getRSEdge() const
		
	{
		return rsedge_;
	}


	void SESFace::setRSFace(RSFace* rsface)
		
	{
		rsface_ = rsface;
	}


	RSFace* SESFace::getRSFace() const
		
	{
		return rsface_;
	}


	void SESFace::normalize(bool singular)
		
	{
		if ((type_ != SESFace::TYPE_TORIC) &&
				(type_ != SESFace::TYPE_TORIC_SINGULAR))
		{
			return;
		}
		if (rsedge_->isFree())
		{
			return;
		}
		if (singular == false)
		{
			normalizeNonSingularToricFace_();
		}
		else
		{
			normalizeSingularToricFace_();
		}
	}


	void SESFace::normalizeNonSingularToricFace_()
		
	{
		HashSet<SESVertex*> points;
		std::list<SESVertex*>::iterator v;
		for (v = vertex_.begin(); v != vertex_.end(); v++)
		{
			points.insert(*v);
		}
		SESEdge* edge0(0);
		SESEdge* edge1(0);
		SESEdge* edge2(0);
		SESEdge* edge3(0);
		SESVertex* p0(0);
		SESVertex* p1(0);
		SESVertex* p2(0);
		SESVertex* p3(0);
		std::list<SESEdge*>::iterator e = edge_.begin();
		while ((*e)->type_ != SESEdge::TYPE_CONCAVE)
		{
			e++;
		}
		edge0 = *e;								// edge0 = first concave edge
		e++;
		while ((*e)->type_ != SESEdge::TYPE_CONCAVE)
		{
			e++;
		}
		edge2 = *e;								// edge2 = second concave edge
		p0 = edge0->vertex_[0];
		p1 = edge0->vertex_[1];
		Size number_of_vertices = points.size();
		if (number_of_vertices == 4)
		{
			if (getEdge(p1,edge2->vertex_[0],edge1))
			{
				p2 = edge2->vertex_[0];
				p3 = edge2->vertex_[1];
			}
			else																					//	 p0 ____e3____ p3
			{																							//      |        |
				getEdge(p1,edge2->vertex_[1],edge1);				//      |        |
				p2 = edge2->vertex_[1];											//    e0|        |e2
				p3 = edge2->vertex_[0];											//      |        |
			}																							//      |________|
			getEdge(p0,p3,edge3);													//    p1   e1     p2
		}
		else
		{
			if ((p0 == edge2->vertex_[0]) || (p1 == edge2->vertex_[1]))
			{
				p2 = edge2->vertex_[1];
				p3 = edge2->vertex_[0];
			}
			else
			{
				p2 = edge2->vertex_[0];
				p3 = edge2->vertex_[1];
			}
			getEdge(p1,p2,edge1);
			getEdge(p0,p3,edge3);
		}
		edge_.clear();
		edge_.push_back(edge0);
		edge_.push_back(edge1);
		edge_.push_back(edge2);
		edge_.push_back(edge3);
		vertex_.clear();
		vertex_.push_back(p0);
		vertex_.push_back(p1);
		vertex_.push_back(p2);
		vertex_.push_back(p3);
	}


	void SESFace::normalizeSingularToricFace_()
		
	{
		SESEdge* edge0(0);
		SESEdge* edge1(0);
		SESEdge* edge2(0);
		SESEdge* edge3(0);
		SESEdge* edge4(0);
		SESEdge* edge5(0);
		SESVertex* p0(0);
		SESVertex* p1(0);
		SESVertex* p2(0);
		SESVertex* p3(0);
		SESVertex* p4(0);
		SESVertex* p5(0);
		findTriangle_(true,edge0,edge1,edge2,p0,p1,p2);
		findTriangle_(false,edge3,edge4,edge5,p3,p4,p5);
		if (edge1->circle_ != edge4->circle_)
		{
			SESEdge* temp = edge5;
			edge5 = edge4;
			edge4 = temp;
			SESVertex* tmp = p5;
			p5 = p3;
			p3 = tmp;
		}
		edge_.clear();
		edge_.push_back(edge0);
		edge_.push_back(edge1);
		edge_.push_back(edge2);
		edge_.push_back(edge3);
		edge_.push_back(edge4);
		edge_.push_back(edge5);
		vertex_.clear();
		vertex_.push_back(p0);
		vertex_.push_back(p1);
		vertex_.push_back(p2);
		vertex_.push_back(p3);
		vertex_.push_back(p4);
		vertex_.push_back(p5);
	}


	void SESFace::findTriangle_
				(bool first,
				 SESEdge*& edge0,
				 SESEdge*& edge1,
				 SESEdge*& edge2,
				 SESVertex*& vertex0,
				 SESVertex*& vertex1,
				 SESVertex*& vertex2)
			
	{
		std::list<SESEdge*>::iterator e;
		if (first)
		{
			// find first convex edge
			e = edge_.begin();
			while ((*e)->type_ != SESEdge::TYPE_CONVEX)
			{
				e++;
			}
		}
		else
		{
			// find second (last) convex edge
			e = edge_.end();
			do
			{
				e--;
			}
			while ((*e)->type_ != SESEdge::TYPE_CONVEX);
		}
		edge0 = *e;
		vertex0 = edge0->vertex_[0];
		vertex2 = edge0->vertex_[1];
		e = edge_.begin();
		while (e != edge_.end())
		{
			if (((*e)->vertex_[0] == vertex0) && ((*e) != edge0))
			{
				edge1 = *e;
				vertex1 = edge1->vertex_[1];
			}
			else
			{
				if (((*e)->vertex_[1] == vertex0) && ((*e) != edge0))
				{
					edge1 = *e;
					vertex1 = edge1->vertex_[0];
				}
			}
			e++;
		}
		while (e != edge_.begin())
		{
			e--;
			if ((((*e)->vertex_[0] == vertex1) && ((*e)->vertex_[1] == vertex2)) ||
					(((*e)->vertex_[1] == vertex1) && ((*e)->vertex_[0] == vertex2))		)
			{
				edge2 = *e;
			}
		}
	}


	bool SESFace::operator == (const SESFace&) const
		
	{
		return true;
	}


	bool SESFace::operator != (const SESFace&) const
		
	{
		return false;
	}


	bool SESFace::operator *= (const SESFace&) const
		
	{
		return true;
	}


	bool SESFace::isNeighbouredTo(SESFace* face) const
		
	{
		std::list<SESEdge*>::const_iterator e;
		for (e = edge_.begin(); e != edge_.end(); e++)
		{
			if ((*e)->face_[0] == face)
			{
				return true;
			}
			if ((*e)->face_[1] == face)
			{
				return true;
			}
		}
		return false;
	}


	bool SESFace::isFree() const
		
	{
		if (type_ != TYPE_TORIC)
		{
			return false;
		}
		return rsedge_->isFree();
	}


	bool SESFace::hasSingularEdges() const
		
	{
		SESFace::ConstEdgeIterator e;
		for (e = beginEdge(); e != endEdge(); e++)
		{
			if (*e != NULL)
			{
				if ((*e)->type_ == SESEdge::TYPE_SINGULAR)
				{
					return true;
				}
			}
		}
		return false;
	}


	std::ostream& operator << (std::ostream& s, const SESFace& sesface)
	{
		s << "SESFACE" << sesface.getIndex() << "("
			<< (sesface.getType() == SESFace::TYPE_CONTACT
						? "contact "
						: (sesface.getType() == SESFace::TYPE_SPHERIC
								? "spheric "
								: (sesface.getType() == SESFace::TYPE_TORIC
										? "toric "
										: "toric singular ")));
		if (sesface.getRSVertex() == NULL)
		{
			s << "(nil) ";
		}
		else
		{
			s << "(" << sesface.getRSVertex()->getIndex() << ") ";
		}
		if (sesface.getRSEdge() == NULL)
		{
			s << "(nil) ";
		}
		else
		{
			s << "(" << sesface.getRSEdge()->getIndex() << ") ";
		}
		if (sesface.getRSFace() == NULL)
		{
			s << "(nil) [";
		}
		else
		{
			s << "(" << sesface.getRSFace()->getIndex() << ") [";
		}
		SESFace::ConstVertexIterator v = sesface.beginVertex();
		while (v != sesface.endVertex())
		{
			s << (*v)->getIndex() << ' ';
			v++;
		}
		s << "] [";
		SESFace::ConstEdgeIterator e = sesface.beginEdge();
		while (e != sesface.endEdge())
		{
			s << (*e)->getIndex() << ' ';
			e++;
		}
		s << "])";
		return s;
	}


}	// namespace BALL
