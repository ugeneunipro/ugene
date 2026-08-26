// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_TRIANGLEPOINT_H
#define BALL_STRUCTURE_TRIANGLEPOINT_H

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

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
	class Triangle;
	class TriangulatedSurface;
	class TriangulatedSphere;
	class TriangulatedSES;
	class SESTriangulator;
	class TriangulatedSAS;
	class SASTriangulator;


	/** Generic TriangleEdge Class.
			\ingroup Surface
	*/
	class BALL_EXPORT TrianglePoint
			:	public GraphVertex< TrianglePoint,TriangleEdge,Triangle >
	{
		
		public:

		/** @name Class friends

					- class Triangle
					- class TriangleEdge
					- class TriangulatedSurface
					- class TriangulatedSphere
					- class TriangulatedSES
					- class SESTriangulator
					- class TriangulatedSAS
					- class SASTriangulator
				
		*/
		friend class Triangle;
		friend class TriangleEdge;
		friend class TriangulatedSurface;
		friend class TriangulatedSphere;
		friend class TriangulatedSES;
		friend class SESTriangulator;
		friend class TriangulatedSAS;
		friend class SASTriangulator;

		BALL_CREATE(TrianglePoint)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new TrianglePoint object.
		*/
		TrianglePoint()
			;

		/** Detailed constructor.
 				This method creates a new TrianglePoint object with the given
				point and normal.
    */
		TrianglePoint(const TVector3<double>& point, const TVector3<double>& normal = TVector3<double>(0.));

		/**	Copy constructor.
				Create a new TrianglePoint object from another.
				@param	point	the TrianglePoint object to be copied
				@param	deep	if deep = false, all pointers are set to NULL (default).	
											Otherwise the new TrianglePoint object is linked to the	
											neighbours of the old TrianglePoint object.
		*/
		TrianglePoint(const TrianglePoint& point, bool deep = false)
			;

		/**	Destructor.
				Destructs the TrianglePoint object.
		*/
		virtual ~TrianglePoint()
			;

		//@}
		/**	@name	Assignments
		*/
		//@{

		/**	Assign from another TrianglePoint.
				@param	point	the TrianglePoint object to assign from
				@param	deep	if deep = false, all pointers are set to NULL	
											(default). Otherwise the new TrianglePoint object is	
											linked to the neighbours of the TrianglePoint object to	
											assign from.
		*/
		void set(const TrianglePoint& point, bool deep = false)
			;

		/**	Assign from another TrianglePoint.
				The new TrianglePoint object is linked to the neighbours of the	
				TrianglePoint object to assign from.
				@param	point	the TrianglePoint object to assign from
		*/
		TrianglePoint& operator = (const TrianglePoint& point)
			;

		//@}
		/**	@name	Accessors
		*/
		//@{

		/** Get the point
		*/
		TVector3<double> getPoint() const
			;

		/** Set the point
		*/
		void setPoint(const TVector3<double>& point)
			;

		/** Get the normal of the TrianglePoint
		*/
		TVector3<double> getNormal() const
			;

		/** Set the normal of the TrianglePoint
		*/
		void setNormal(const TVector3<double>& normal)
;

		//@}

		/**	@name	Predicates
		*/
		//@{

		/** Equality operator
				@return	bool	<b>true</b> if the TrianglePoints lie on the same point,	
											<b>false</b> otherwise.
		*/
		virtual bool operator == (const TrianglePoint& point) const
			;

		/** Inequality operator
				@return	bool	<b>false</b> if the TrianglePoints lie on the same point,	
											<b>true</b> otherwise.
		*/
		virtual bool operator != (const TrianglePoint& point) const
			;

		/** Similarity operator
				@return	bool	<b>true</b> if the TrianglePoints lie on the same point,	
											<b>false</b> otherwise.
		*/
		virtual bool operator *= (const TrianglePoint& point) const
			;

		//@}

		protected:

		/*_ The point itself
		*/
		TVector3<double> point_;
		/*_ The normal vector of the point
		*/
		TVector3<double> normal_;

	};


	/**	@name	Storers
	*/
	//@{

	/**	Output- Operator
	*/
	BALL_EXPORT std::ostream& operator << (std::ostream& s, const TrianglePoint& point);

	//@}


}	// namespace BALL


#endif	// BALL_STRUCTURE_TRIANGLEPOINT_H
