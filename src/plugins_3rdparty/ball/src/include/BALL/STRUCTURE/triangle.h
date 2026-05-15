// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_TRIANGLE_H
#define BALL_STRUCTURE_TRIANGLE_H

#ifndef BALL_STRUCTURE_GRAPHEDGE_H
#	include <BALL/STRUCTURE/graphEdge.h>
#endif

#ifndef BALL_STRUCTURE_GRAPHFACE_H
#	include <BALL/STRUCTURE/graphFace.h>
#endif

#ifndef BALL_STRUCTURE_GRAPHVERTEX_H
#	include <BALL/STRUCTURE/graphVertex.h>
#endif


namespace BALL
{

	class TriangleEdge;
	class TrianglePoint;
	class TriangulatedSurface;
	class TriangulatedSphere;
	class TriangulatedSES;
	class TriangulatedSAS;
	class SESTriangulator;
	class SASTriangulator;


	/** Generic TriangleEdge Class.
			\ingroup Surface
	*/
	class BALL_EXPORT Triangle
			:	public GraphTriangle< TrianglePoint,TriangleEdge,Triangle >
	{
		
		public:

		/** @name Class friends

					- class TriangleEdge
					- class TrianglePoint
					- class TriangulatedSurface
					- class TriangulatedSphere
					- class TriangulatedSES
					- class TriangulatedSAS
					- class SESTriangulator
					- class SASTriangulator
				
		*/
		friend class TriangleEdge;
		friend class TrianglePoint;
		friend class TriangulatedSurface;
		friend class TriangulatedSphere;
		friend class TriangulatedSES;
		friend class TriangulatedSAS;
		friend class SESTriangulator;
		friend class SASTriangulator;

		BALL_CREATE(Triangle)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new Triangle object.
		*/
		Triangle();

		/**
		 * Detailed constructor.
		 * Creates a new Triangle object. Its vertices are initialized with
		 * the passed TrianglePoint instances.
		 */
		Triangle(TrianglePoint* v1, TrianglePoint* v2, TrianglePoint* v3);

		/**
		 * Detailed constructor.
		 * Creates a new Triangle object. Its edges are initialized with
		 * the passed TriangleEdge instances.
		 * Its vertices are taken from the edges. This requires the edges
		 * to be properly oriented in a counter clockwise manner.
		 */
		Triangle(TriangleEdge* e1, TriangleEdge* e2, TriangleEdge* e3, bool flip_normal = false);

		/**	Copy constructor.
				Create a new Triangle object from another.
				@param	triangle	the Triangle object to be copied
				@param	deep			if deep = false, all pointers are set to NULL	
													(default). Otherwise the new Triangle object is
													linked to the neighbours of the old Triangle object.
		*/
		Triangle(const Triangle& triangle, bool deep = false);

		/**	Destructor.
				Destructs the Triangle object.
		*/
		virtual ~Triangle();

		//@}
		/**	@name	Assignments
		*/
		//@{

		/**	Assign from another Triangle.
				@param	triangle	the Triangle object to assign from
				@param	deep			if deep = false, all pointers are set to NULL	
													(default). Otherwise the Triangle object is linked	
													to the neighbours of the Triangle object to assign	
													from
		*/
		void set(const Triangle& triangle, bool deep = false);

		/**	Assign from another Triangle.
				The Triangle object is linked to the neighbours of the Triangle	
				object to assign from
				@param	triangle	the Triangle object to assign from
		*/
		Triangle& operator = (const Triangle& triangle);

		//@}
		/**	@name	Accessors
		*/
		//@{

		/** Set one of the three points of the Triangle.
				@param	i			the relative index of the point which should be set.	
											If i is greater three, an exception is thrown.
				@param	point	a pointer to the new point
		*/
		void setPoint(Position i, TrianglePoint* point)
;

		/** Return one of the three points of the Triangle.
				@param	i										the relative index of the point which	
																		should be given back. If i is greater	
																		three, an exception is thrown.
				@return	TrianglePoint*	a pointer to the asked point
		*/
		TrianglePoint* getPoint(Position i) const
;

		/**	Remove an edge from the Triangle.
				The edge is set to NULL.
				@param	edge	the edge to delete
		*/
		void remove(TriangleEdge* edge);

		//@}
		/**	@name	Predicates
		*/
		//@{

		/**	Equality operator.
				@return	<b>true</b>
		*/
		virtual bool operator == (const Triangle&) const;

		/**	Inequality operator.
				@return	<b>false</b>
		*/
		virtual bool operator != (const Triangle&) const;

		/**	Similarity operator.
				@return	<b>true</b>
		*/
		virtual bool operator *= (const Triangle&) const;
		
		//@}

	};


	/**	@name	Storers
	*/
	//@{

	/**	Output- Operator
	*/
	BALL_EXPORT std::ostream& operator << (std::ostream& s, const Triangle& triangle);

	//@}


}	// namespace BALL


#endif	// BALL_STRUCTURE_TRIANGLE_H
