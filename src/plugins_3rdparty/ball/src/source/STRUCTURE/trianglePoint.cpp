#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/triangleEdge.h>
#include <BALL/STRUCTURE/triangle.h>
#include <BALL/STRUCTURE/trianglePoint.h>

namespace BALL
{

	TrianglePoint::TrianglePoint()
		
		:	GraphVertex< TrianglePoint,TriangleEdge,Triangle >(),
			point_(),
			normal_()
	{
	}

	TrianglePoint::TrianglePoint(const TVector3<double>& point, const TVector3<double>& normal)
	 : GraphVertex<TrianglePoint,TriangleEdge,Triangle>(),
		 point_(point),
		 normal_(normal)
	{
	}

	TrianglePoint::TrianglePoint(const TrianglePoint& point, bool deep)
		
		:	GraphVertex< TrianglePoint,TriangleEdge,Triangle >
				(point,deep),
			point_(point.point_),
			normal_(point.normal_)
	{
	}


	TrianglePoint::~TrianglePoint()
		
	{
	}


	void TrianglePoint::set(const TrianglePoint& point, bool deep)
		
	{
		if (this != &point)
		{
			GraphVertex< TrianglePoint,TriangleEdge,Triangle >::set
					(point,deep);
			point_.set(point.point_);
			normal_.set(point.normal_);
		}
	}


	TrianglePoint& TrianglePoint:: operator =
			(const TrianglePoint& point)
		
	{
		if (this != &point)
		{
			GraphVertex< TrianglePoint,TriangleEdge,Triangle >::operator=
					(point);
			point_.set(point.point_);
			normal_.set(point.normal_);
		}
		return *this;
	}


	TVector3<double> TrianglePoint::getPoint() const
		
	{
		return point_;
	}


	void TrianglePoint::setPoint(const TVector3<double>& point)
		
	{
		point_ = point;
	}


	TVector3<double> TrianglePoint::getNormal() const
		
	{
		return normal_;
	}


	void TrianglePoint::setNormal(const TVector3<double>& normal)
		throw(Exception::DivisionByZero)
	{
		normal_ = normal;
		normal_.normalize();
	}


	bool TrianglePoint::operator == (const TrianglePoint& point) const
		
	{
		return (point_ == point.point_);
	}


	bool TrianglePoint::operator != (const TrianglePoint& point) const
		
	{
		return (point_ != point.point_);
	}


	bool TrianglePoint::operator *= (const TrianglePoint& point) const
		
	{
		return (point_ == point.point_);
	}


	std::ostream& operator << (std::ostream& s, const TrianglePoint& point)
	{
		s << "POINT";
		s << point.getIndex();
		s << "( " << point.getPoint() << " " << point.getNormal() << " {";
		TrianglePoint::ConstEdgeIterator e;
		for (e = point.beginEdge(); e != point.endEdge(); e++)
		{
			s << (*e)->getIndex() << " ";
		}
		s << "} [";
		TrianglePoint::ConstFaceIterator f;
		for (f = point.beginFace(); f != point.endFace(); f++)
		{
			s << (*f)->getIndex() << " ";
		}
		s << "] )";
		return s;
	}


}	// namespace BALL
