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

	Triangle::Triangle()
		:	GraphTriangle< TrianglePoint,TriangleEdge,Triangle >()
	{
	}

	Triangle::Triangle(TrianglePoint* v1, TrianglePoint* v2, TrianglePoint* v3)
		:	GraphTriangle< TrianglePoint,TriangleEdge,Triangle >()
	{
		vertex_[0] = v1;
		vertex_[1] = v2;
		vertex_[2] = v3;
	}

	Triangle::Triangle(TriangleEdge* e1, TriangleEdge* e2, TriangleEdge* e3, bool flip_normal)
		:	GraphTriangle< TrianglePoint,TriangleEdge,Triangle >()
	{
		edge_[0] = e1;
		edge_[1] = e2;
		edge_[2] = e3;

		if(flip_normal) {
			vertex_[0] = e1->vertex_[1];
			vertex_[1] = e1->vertex_[0];
		} else {
			vertex_[0] = e1->vertex_[0];
			vertex_[1] = e1->vertex_[1];
		}

		vertex_[2] = ((e2->vertex_[0] != e1->vertex_[0]) &&
		              (e2->vertex_[0] != e1->vertex_[1]))
		             ? e2->vertex_[0]
		             : e2->vertex_[1];

		vertex_[0]->faces_.insert(this);
		vertex_[1]->faces_.insert(this);
		vertex_[2]->faces_.insert(this);
	}

	Triangle::Triangle(const Triangle& triangle, bool deep)
		:	GraphTriangle< TrianglePoint,TriangleEdge,Triangle >
				(triangle,deep)
	{
	}


	Triangle::~Triangle()
	{
	}


	void Triangle::set(const Triangle& triangle, bool deep)
	{
		if (this != &triangle)
		{
			GraphTriangle< TrianglePoint,TriangleEdge,Triangle >::set
					(triangle,deep);
		}
	}


	Triangle& Triangle::operator = (const Triangle& triangle)
	{
		if (this != &triangle)
		{
			GraphTriangle< TrianglePoint,TriangleEdge,Triangle >::
					operator = (triangle);
		}
		return *this;
	}


	void Triangle::setPoint(Position i, TrianglePoint* vertex)
		throw(Exception::IndexOverflow)
	{
		if (i > 3) {
			throw Exception::IndexOverflow(__FILE__,__LINE__,i,2);
		}

		vertex_[i] = vertex;
	}


	TrianglePoint* Triangle::getPoint(Position i) const
		throw(Exception::IndexOverflow)
	{
		if (i > 3) {
			throw Exception::IndexOverflow(__FILE__,__LINE__,i,2);
		}

		return vertex_[i];
	}


	void Triangle::remove(TriangleEdge* edge)
	{
		for (Position i = 0; i < 3; i++)
		{
			if (edge_[i] == edge)
			{
				edge_[i] = NULL;
			}
		}
	}


	bool Triangle::operator == (const Triangle&) const
	{
		return true;
	}


	bool Triangle::operator != (const Triangle&) const
	{
		return false;
	}


	bool Triangle::operator *= (const Triangle&) const
	{
		return true;
	}


	std::ostream& operator << (std::ostream& s, const Triangle& triangle)
	{
		s << "TRIANGLE"
			<< triangle.getIndex()
			<< "(["
			<< (triangle.getVertex(0) == NULL
							? -2
							: triangle.getVertex(0)->getIndex()) << ' '
			<< (triangle.getVertex(1) == NULL
							? -2
							: triangle.getVertex(1)->getIndex()) << ' '
			<< (triangle.getVertex(2) == NULL
							? -2
							: triangle.getVertex(2)->getIndex())
			<< "] ["
			<< (triangle.getEdge(0) == NULL
							? -2
							: triangle.getEdge(0)->getIndex()) << ' '
			<< (triangle.getEdge(1) == NULL
							? -2
							: triangle.getEdge(1)->getIndex()) << ' '
			<< (triangle.getEdge(2) == NULL
							? -2
							: triangle.getEdge(2)->getIndex())
			<< "])";
		return s;
	}


}	// namespace BALL
