#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/RSEdge.h>
#include <BALL/STRUCTURE/RSFace.h>
#include <BALL/STRUCTURE/RSVertex.h>
#include <BALL/MATHS/angle.h>
#include <BALL/MATHS/circle3.h>
#include <BALL/MATHS/vector3.h>

namespace BALL
{

	RSEdge::RSEdge()
		
		: GraphEdge< RSVertex,RSEdge,RSFace >(),
			center_of_torus_(),
			radius_of_torus_(0.0),
			angle_(),
			circle0_(),
			circle1_(),
			intersection_point0_(),
			intersection_point1_(),
			singular_(false)
	{
	}


	RSEdge::RSEdge(const RSEdge& rsedge, bool deep)
		
		: GraphEdge< RSVertex,RSEdge,RSFace >(rsedge,deep),
			center_of_torus_(rsedge.center_of_torus_),
			radius_of_torus_(rsedge.radius_of_torus_),
			angle_(rsedge.angle_),
			circle0_(rsedge.circle0_),
			circle1_(rsedge.circle1_),
			intersection_point0_(rsedge.intersection_point0_),
			intersection_point1_(rsedge.intersection_point1_),
			singular_(rsedge.singular_)
	{
	}


	RSEdge::RSEdge(RSVertex* vertex1,
			RSVertex* vertex2,
			RSFace* face1,
			RSFace* face2,
			const TVector3<double>& center_of_torus,
			const double& radius_of_torus,
			const TAngle<double>& angle,
			const TCircle3<double>& circle1,
			const TCircle3<double>& circle2,
			const TVector3<double>& intersection_point1,
			const TVector3<double>& intersection_point2,
			bool singular,
			Index index)
		
		: GraphEdge< RSVertex,RSEdge,RSFace >
				(vertex1,vertex2,face1,face2,index),
			center_of_torus_(center_of_torus),
			radius_of_torus_(radius_of_torus),
			angle_(angle),
			circle0_(circle1),
			circle1_(circle2),
			intersection_point0_(intersection_point1),
			intersection_point1_(intersection_point2),
			singular_(singular)
	{
	}


	RSEdge::~RSEdge()
		
	{
	}


	void RSEdge::set(const RSEdge& rsedge, bool deep)
		
	{
		if (this != &rsedge)
		{
			GraphEdge< RSVertex,RSEdge,RSFace >::set(rsedge,deep);
			center_of_torus_ = rsedge.center_of_torus_;
			radius_of_torus_ = rsedge.radius_of_torus_;
			angle_ = rsedge.angle_;
			circle0_ = rsedge.circle0_;
			circle1_ = rsedge.circle1_;
			intersection_point0_ = rsedge.intersection_point0_;
			intersection_point1_ = rsedge.intersection_point1_;
			singular_ = rsedge.singular_;
		}
	}


	RSEdge& RSEdge::operator = (const RSEdge& rsedge)
		
	{
		if (this != &rsedge)
		{
			GraphEdge< RSVertex,RSEdge,RSFace >::operator = (rsedge);
			center_of_torus_ = rsedge.center_of_torus_;
			radius_of_torus_ = rsedge.radius_of_torus_;
			angle_ = rsedge.angle_;
			circle0_ = rsedge.circle0_;
			circle1_ = rsedge.circle1_;
			intersection_point0_ = rsedge.intersection_point0_;
			intersection_point1_ = rsedge.intersection_point1_;
			singular_ = rsedge.singular_;
		}
		return *this;
	}


	void RSEdge::set(RSVertex* vertex0,
			RSVertex* vertex1,
			RSFace* face0,
			RSFace* face1,
			const TVector3<double>& center_of_torus,
			const double& radius_of_torus,
			const TAngle<double>& angle,
			const TCircle3<double>& circle0,
			const TCircle3<double>& circle1,
			const TVector3<double>& intersection_point0,
			const TVector3<double>& intersection_point1,
			bool singular,
			Index index)
		
	{
		GraphEdge< RSVertex,RSEdge,RSFace >::set
				(vertex0,vertex1,face0,face1,index);
		center_of_torus_ = center_of_torus;
		radius_of_torus_ = radius_of_torus;
		angle_ = angle;
		circle0_ = circle0;
		circle1_ = circle1;
		intersection_point0_ = intersection_point0;
		intersection_point1_ = intersection_point1;
		singular_ = singular;
	}


	void RSEdge::setCenterOfTorus(const TVector3<double>& center)
		
	{
		center_of_torus_ = center;
	}


	TVector3<double> RSEdge::getCenterOfTorus() const
		
	{
		return center_of_torus_;
	}


	void RSEdge::setMajorRadiusOfTorus(const double& radius)
		
	{
		radius_of_torus_ = radius;
	}


	double RSEdge::getMajorRadiusOfTorus() const
		
	{
		return radius_of_torus_;
	}


	void RSEdge::setAngle(const TAngle<double>& angle)
		
	{
		angle_ = angle;
	}


	TAngle<double> RSEdge::getAngle() const
		
	{
		return angle_;
	}


	void  RSEdge::setContactCircle(Position i, const TCircle3<double>&  circle)
		
	{
		if (i == 0)
		{
			circle0_ = circle;
		}
		else
		{
			circle1_ = circle;
		}
	}


	TCircle3<double> RSEdge::getContactCircle(Position i) const
		
	{
		if (i == 0)
		{
			return circle0_;
		}
		else
		{
			return circle1_;
		}
	}


	void RSEdge::setIntersectionPoint(Position i, const TVector3<double>& point)
		
	{
		if (i == 0)
		{
			intersection_point0_ = point;
		}
		else
		{
			intersection_point1_ = point;
		}
	}


	TVector3<double> RSEdge::getIntersectionPoint(Position i) const
		throw(Exception::GeneralException)
	{
		if (singular_ == false)
		{
			throw Exception::GeneralException(__FILE__, __LINE__);
		}
		if (i == 0)
		{
			return intersection_point0_;
		}
		else
		{
			return intersection_point1_;
		}
	}


	void RSEdge::setSingular(bool singular)
		
	{
		singular_ = singular;
	}


	bool RSEdge::operator == (const RSEdge& rsedge) const
		
	{
		return ((
						 ((vertex_[0] == rsedge.vertex_[0]) &&
							(vertex_[1] == rsedge.vertex_[1])    ) ||
						 ((vertex_[0] == rsedge.vertex_[1]) &&
							(vertex_[1] == rsedge.vertex_[0])    )		) &&
						(((face_[0] == rsedge.face_[0]) &&
							(face_[1] == rsedge.face_[1])    ) ||
						 ((face_[0] == rsedge.face_[1]) &&
							(face_[1] == rsedge.face_[0])    )		)
					 );
	}


	bool RSEdge::operator != (const RSEdge& rsedge) const
		
	{
		return ( ! (*this == rsedge) );
	}


	bool RSEdge::operator *= (const RSEdge& rsedge) const
		
	{
		return (
						((vertex_[0]->atom_ == rsedge.vertex_[0]->atom_) &&
						 (vertex_[1]->atom_ == rsedge.vertex_[1]->atom_)    ) ||
						((vertex_[0]->atom_ == rsedge.vertex_[1]->atom_) &&
						 (vertex_[1]->atom_ == rsedge.vertex_[0]->atom_)    )
					 );
	}


	bool RSEdge::isSingular() const
		
	{
		return singular_;
	}


	bool RSEdge::isFree() const
		
	{
		return (face_[0] == NULL);
	}


	std::ostream& operator << (std::ostream& s, const RSEdge& rsedge)
	{
		s << "RSEDGE" << rsedge.getIndex()
			<< "(["
			<< (rsedge.getVertex(0) == NULL
									?	-2
									: rsedge.getVertex(0)->getIndex()) << ' '
			<< (rsedge.getVertex(1) == NULL
									? -2
									: rsedge.getVertex(1)->getIndex())
			<< "] ["
			<< (rsedge.getFace(0) == NULL
									? -2
									: rsedge.getFace(0)->getIndex()) << ' '
			<< (rsedge.getFace(1) == NULL
									? -2
									: rsedge.getFace(1)->getIndex())
			<< "] "
			<< rsedge.getCenterOfTorus() << ' '
			<< rsedge.getMajorRadiusOfTorus() << ' ' << rsedge.getAngle() << ' '
			<< rsedge.getContactCircle(0) << ' '
			<< rsedge.getContactCircle(1) << ' ';
		bool singular(rsedge.isSingular());
		if (singular)
		{
			s << rsedge.getIntersectionPoint(0) << ' '
				<< rsedge.getIntersectionPoint(1) << " true)";
		}
		else
		{
			s << TVector3<double>::getZero() << ' '
				<< TVector3<double>::getZero() << " false)";
		}
		return s;
	}


} // namespace BALL
