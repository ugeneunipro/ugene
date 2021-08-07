#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/SASEdge.h>
#include <BALL/STRUCTURE/SASFace.h>
#include <BALL/STRUCTURE/SASVertex.h>
#include <BALL/MATHS/vector3.h>


namespace BALL
{

	SASVertex::SASVertex()
		
		: GraphVertex< SASVertex,SASEdge,SASFace >(),
			point_()
	{
	}


	SASVertex::SASVertex(const SASVertex& sasvertex, bool deep)
		
		: GraphVertex< SASVertex,SASEdge,SASFace >(sasvertex,deep),
			point_(sasvertex.point_)
	{
	}


	SASVertex::SASVertex(const TVector3<double>& point, Index index)
		
		: GraphVertex< SASVertex,SASEdge,SASFace >(),
			point_(point)
	{
		index_ = index;
	}


	SASVertex::~SASVertex()
		
	{
	}


	void SASVertex::set(const SASVertex& sasvertex, bool deep)
		
	{
		if (this != &sasvertex)
		{
			GraphVertex< SASVertex,SASEdge,SASFace >::set(sasvertex,deep);
			point_ = sasvertex.point_;
		}
	}


	SASVertex& SASVertex::operator = (const SASVertex& sasvertex)
		
	{
		if (this != &sasvertex)
		{
			GraphVertex< SASVertex,SASEdge,SASFace >::operator =
					(sasvertex);
			point_ = sasvertex.point_;
		}
		return *this;
	}


	void SASVertex::set(const TVector3<double> point, Index index)
		
	{
		point_ = point;
		index_ = index;
	}


	void SASVertex::setPoint(const TVector3<double>& point)
		
	{
		point_ = point;
	}


	TVector3<double> SASVertex::getPoint() const
		
	{
		return point_;
	}


	bool SASVertex::operator == (const SASVertex&) const
		
	{
		return true;
	}


	bool SASVertex::operator != (const SASVertex&) const
		
	{
		return false;
	}


	bool SASVertex::operator *= (const SASVertex&) const
		
	{
		return true;
	}


	std::ostream& operator << (std::ostream& s, const SASVertex& sasvertex)
	{
		s << "SASVERTEX"
			<< sasvertex.getIndex()
			<< "(" << sasvertex.getPoint()
			<< " [";
		 SASVertex::ConstEdgeIterator e;
		for (e = sasvertex.beginEdge(); e != sasvertex.endEdge(); e++)
		{
			s << (*e)->getIndex() << ' ';
		}
		s << "] [";
		 SASVertex::ConstFaceIterator f;
		for (f = sasvertex.beginFace(); f != sasvertex.endFace(); f++)
		{
			s << (*f)->getIndex() << ' ';
		}
		s << "] )";
		return s;
	}


}	// namespace BALL
