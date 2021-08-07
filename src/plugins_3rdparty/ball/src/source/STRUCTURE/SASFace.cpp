#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/SASEdge.h>
#include <BALL/STRUCTURE/SASFace.h>
#include <BALL/STRUCTURE/SASVertex.h>
#include <BALL/MATHS/sphere3.h>
#include <list>


namespace BALL
{

	SASFace::SASFace()
		: GraphFace< SASVertex,SASEdge,SASFace >(),
			orientation_(),
			sphere_()
	{
	}


	SASFace::SASFace(const SASFace& sasface, bool deep)
		: GraphFace< SASVertex,SASEdge,SASFace >(sasface,deep),
			orientation_(),
			sphere_(sasface.sphere_)
	{
		if (deep)
		{
			orientation_ = sasface.orientation_;
		}
	}


	SASFace::~SASFace()
	{
	}


	void SASFace::set(const SASFace& sasface, bool deep)
	{
		if (this != &sasface)
		{
			GraphFace< SASVertex,SASEdge,SASFace >::set(sasface,deep);
			if (deep)
			{
				orientation_ = sasface.orientation_;
			}
			sphere_.set(sasface.sphere_);
		}
	}


	SASFace& SASFace::operator = (const SASFace& sasface)
	{
		if (this != &sasface)
		{
			GraphFace< SASVertex,SASEdge,SASFace >::operator = (sasface);
			orientation_ = sasface.orientation_;
			sphere_.set(sasface.sphere_);
		}
		return *this;
	}


	void SASFace::setSphere(const TSphere3<double>& sphere)
	{
		sphere_ = sphere;
	}


	TSphere3<double> SASFace::getSphere() const
	{
		return sphere_;
	}


	bool SASFace::operator == (const SASFace&) const
	{
		return true;
	}


	bool SASFace::operator != (const SASFace&) const
	{
		return false;
	}


	bool SASFace::operator *= (const SASFace&) const
	{
		return true;
	}


	SASFace::OrientationIterator SASFace::beginOrientation()
	{
		return orientation_.begin();
	}


	SASFace::ConstOrientationIterator SASFace::beginOrientation() const
	{
		return orientation_.begin();
	}


	SASFace::OrientationIterator SASFace::endOrientation()
	{
		return orientation_.end();
	}


	SASFace::ConstOrientationIterator SASFace::endOrientation() const
	{
		return orientation_.end();
	}


	std::ostream& operator << (std::ostream& s, const SASFace& sasface)
	{
		s << "SASFACE" << sasface.getIndex() << "(";
		SASFace::ConstVertexIterator v = sasface.beginVertex();
		while (v != sasface.endVertex())
		{
			s << (*v)->getIndex() << ' ';
			v++;
		}
		s << "] [";
		SASFace::ConstEdgeIterator e = sasface.beginEdge();
		while (e != sasface.endEdge())
		{
			s << (*e)->getIndex() << ' ';
			e++;
		}
		s << "] [";
		SASFace::ConstOrientationIterator o = sasface.beginOrientation();
		while (o != sasface.endOrientation())
		{
			s << (*o ? "+ " : "- ");
			o++;
		}
		s << "])";
		return s;
	}


}	// namespace BALL
