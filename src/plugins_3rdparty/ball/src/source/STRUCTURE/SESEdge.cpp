#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/SESEdge.h>
#include <BALL/STRUCTURE/SESFace.h>
#include <BALL/STRUCTURE/SESVertex.h>
#include <BALL/STRUCTURE/RSEdge.h>
#include <BALL/MATHS/circle3.h>

namespace BALL
{

	SESEdge::SESEdge()
		
		: GraphEdge< SESVertex,SESEdge,SESFace >(),
			circle_(),
			rsedge_(NULL),
			type_(SESEdge::TYPE_CONCAVE)
	{
	}


	SESEdge::SESEdge(const SESEdge& sesedge, bool deep)
		
		: GraphEdge< SESVertex,SESEdge,SESFace >(sesedge,deep),
			circle_(sesedge.circle_),
			rsedge_(NULL),
			type_(sesedge.type_)
	{
		if (deep)
		{
			rsedge_ = sesedge.rsedge_;
		}
	}


	SESEdge::SESEdge(SESVertex* vertex0,
			SESVertex* vertex1,
			SESFace* face0,
			SESFace* face1,
			const TCircle3<double>& circle,
			RSEdge* rsedge,
			Type type,
			Index index)
		
		: GraphEdge< SESVertex,SESEdge,SESFace >
					(vertex0,vertex1,face0,face1,index),
			circle_(circle),
			rsedge_(rsedge),
			type_(type)
	{
	}


	SESEdge::~SESEdge()
		
	{
	}


	void SESEdge::set(const SESEdge& sesedge, bool deep)
		
	{
		if (this != &sesedge)
		{
			GraphEdge< SESVertex,SESEdge,SESFace >::set(sesedge,deep);
			if (deep)
			{
				rsedge_ = sesedge.rsedge_;
			}
			else
			{
				rsedge_ = NULL;
			}
			circle_.set(sesedge.circle_);
			type_ = sesedge.type_;
		}
	}


	SESEdge& SESEdge::operator = (const SESEdge& sesedge)
		
	{
		if (this != &sesedge)
		{
			GraphEdge< SESVertex,SESEdge,SESFace >::operator = (sesedge);
			rsedge_ = sesedge.rsedge_;
			circle_.set(sesedge.circle_);
			type_ = sesedge.type_;
		}
		return *this;
	}


	void SESEdge::set
			(SESVertex*			vertex0,
			 SESVertex*			vertex1,
			 SESFace*				face0,
			 SESFace*				face1,
			 const TCircle3<double>&	circle,
			 RSEdge*				rsedge,
			 Type								type,
			 Index							index)
		
	{
		GraphEdge< SESVertex,SESEdge,SESFace >::set
				(vertex0,vertex1,face0,face1,index);
		circle_.set(circle);
		rsedge_ = rsedge;
		type_ = type;
	}


	void SESEdge::setCircle(const TCircle3<double>& circle)
		
	{
		circle_ = circle;
	}


	TCircle3<double> SESEdge::getCircle() const
		
	{
		return circle_;
	}


	void SESEdge::setRSEdge(RSEdge* rsedge)
		
	{
		rsedge_ = rsedge;
	}


	RSEdge* SESEdge::getRSEdge() const
		
	{
		return rsedge_;
	}


	void SESEdge::setType(SESEdge::Type type)
		
	{
		type_ = type;
	}


	SESEdge::Type SESEdge::getType() const
		
	{
		return type_;
	}


	bool SESEdge::operator == (const SESEdge& sesedge) const
		
	{
		return (((vertex_[0] == sesedge.vertex_[0]) &&
						 (vertex_[1] == sesedge.vertex_[1])		) ||
						((vertex_[1] == sesedge.vertex_[0]) &&
						 (vertex_[0] == sesedge.vertex_[1])		)		);
	}


	bool SESEdge::operator != (const SESEdge& sesedge) const
		
	{
		return (((vertex_[0] != sesedge.vertex_[0]) ||
						 (vertex_[1] != sesedge.vertex_[1])		) &&
						((vertex_[1] != sesedge.vertex_[0]) ||
						 (vertex_[0] != sesedge.vertex_[1])		)		);
	}


	bool SESEdge::operator *= (const SESEdge&) const
		
	{
		return true;
	}


	bool SESEdge::isFree() const
		
	{
		if (rsedge_ == NULL)
		{
			return false;
		}
		else
		{
			return rsedge_->isFree();
		}
	}


	std::ostream& operator << (std::ostream& s, const SESEdge& sesedge)
	{
		return (s << "SESEDGE" << sesedge.getIndex() << "(["
							<< ((sesedge.getVertex(0) == NULL)
											? -2
											: sesedge.getVertex(0)->getIndex())
							<< ' '
							<< ((sesedge.getVertex(1) == NULL)
											? -2
											: sesedge.getVertex(1)->getIndex())
							<< "] ["
							<< ((sesedge.getFace(0) == NULL)
											? -2
											: sesedge.getFace(0)->getIndex())
							<< ' '
							<< ((sesedge.getFace(1) == NULL)
											? -2
											: sesedge.getFace(1)->getIndex())
							<< "] "
							<< sesedge.getCircle() << ' '
							<< ((sesedge.getRSEdge() == NULL)
											? -2
											: sesedge.getRSEdge()->getIndex())
							<< ((sesedge.getType() == SESEdge::TYPE_CONCAVE)
											? " concave)"
											: ((sesedge.getType() == SESEdge::TYPE_CONVEX)
															? " convex)"
															: " singular)"))
						);
	}


}	// namespace BALL
