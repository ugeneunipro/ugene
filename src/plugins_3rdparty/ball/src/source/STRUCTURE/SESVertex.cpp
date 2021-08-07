#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/SESEdge.h>
#include <BALL/STRUCTURE/SESFace.h>
#include <BALL/STRUCTURE/SESVertex.h>
#include <BALL/MATHS/vector3.h>


namespace BALL
{

	SESVertex::SESVertex()
		
		: GraphVertex< SESVertex,SESEdge,SESFace >(),
			point_(),
			normal_(),
			atom_(0)
	{
	}


	SESVertex::SESVertex(const SESVertex& sesvertex, bool deep)
		
		: GraphVertex< SESVertex,SESEdge,SESFace >(sesvertex,deep),
			point_(sesvertex.point_),
			normal_(sesvertex.normal_),
			atom_(sesvertex.atom_)
	{
	}


	SESVertex::SESVertex
			(const TVector3<double>&	point,
			 const TVector3<double>&	normal,
			 Index							atom,
			 Index							index)
		
		: GraphVertex< SESVertex,SESEdge,SESFace >(),
			point_(point),
			normal_(normal),
			atom_(atom)
	{
		index_ = index;
	}


	SESVertex::~SESVertex()
		
	{
	}


	void SESVertex::set(const SESVertex& sesvertex, bool deep)
		
	{
		if (this != &sesvertex)
		{
			GraphVertex< SESVertex,SESEdge,SESFace >::set
					(sesvertex,deep);
			point_.set(sesvertex.point_);
			normal_.set(sesvertex.normal_);
			atom_ = sesvertex.atom_;
		}
	}


	SESVertex& SESVertex::operator = (const SESVertex& sesvertex)
		
	{
		if (this != &sesvertex)
		{
			GraphVertex< SESVertex,SESEdge,SESFace >::operator =
					(sesvertex);
			point_.set(sesvertex.point_);
			normal_.set(sesvertex.normal_);
			atom_ = sesvertex.atom_;
		}
		return *this;
	}


	void SESVertex::set(const TVector3<double> point,
			const TVector3<double>& normal,
			Index atom,
			Index index)
		
	{
		point_ = point;
		normal_ = normal;
		atom_ = atom;
		index_ = index;
	}


	void SESVertex::setPoint(const TVector3<double>& point)
		
	{
		point_ = point;
	}


	TVector3<double> SESVertex::getPoint() const
		
	{
		return point_;
	}


	void SESVertex::setNormal(const TVector3<double>& normal)
		throw(Exception::DivisionByZero)
	{
		normal_ = normal;
		normal_.normalize();
	}


	TVector3<double> SESVertex::getNormal() const
		
	{
		return normal_;
	}


	void SESVertex::setAtom(Index atom)
		
	{
		atom_ = atom;
	}


	Index SESVertex::getAtom() const
		
	{
		return atom_;
	}


	bool SESVertex::operator == (const SESVertex&) const
		
	{
		return true;
	}


	bool SESVertex::operator != (const SESVertex&) const
		
	{
		return false;
	}


	bool SESVertex::operator *= (const SESVertex&) const
		
	{
		return true;
	}


	std::ostream& operator << (std::ostream& s, const SESVertex& sesvertex)
	{
		s << "SESVERTEX" << sesvertex.getIndex()
			<< "("
			<< sesvertex.getPoint() << " "
			<< sesvertex.getNormal() << " [";
		SESVertex::ConstEdgeIterator e;
		for (e = sesvertex.beginEdge(); e != sesvertex.endEdge(); e++)
		{
			s << (*e)->getIndex() << ' ';
		}
		s << "] [";
		SESVertex::ConstFaceIterator f;
		for (f = sesvertex.beginFace(); f != sesvertex.endFace(); f++)
		{
			s << (*f)->getIndex() << ' ';
		}
		s << "] " << sesvertex.getAtom() << ")";
		return s;
	}


}	// namespace BALL
