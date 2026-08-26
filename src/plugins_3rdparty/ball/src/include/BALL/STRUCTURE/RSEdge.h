// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_RSEDGE_H
#define BALL_STRUCTURE_RSEDGE_H

#ifndef BALL_STRUCTURE_GRAPHEDGE_H
#	include <BALL/STRUCTURE/graphEdge.h>
#endif

#ifndef BALL_MATHS_ANGLE_H
#	include <BALL/MATHS/angle.h>
#endif

#ifndef BALL_MATHS_CIRCLE3_H
#	include <BALL/MATHS/circle3.h>
#endif

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

namespace BALL
{
	class RSFace;
	class RSVertex;
	class ReducedSurface;
	class RSComputer;
	class SESEdge;
	class SESFace;
	class SESVertex;
	class SolventExcludedSurface;
	class SESComputer;
	class SESSingularityCleaner;
	class TriangulatedSES;
	class SESTriangulator;
	class SASEdge;
	class SASFace;
	class SASVertex;
	class SolventAccessibleSurface;
	class TriangulatedSAS;
	class SASTriangulator;

	/** Generic RSEdge Class.
			\ingroup Surface
	*/
	class BALL_EXPORT RSEdge	:	public GraphEdge< RSVertex,RSEdge,RSFace >
	{
		public:

		/** @name Class friends

					- class RSFace
					- class RSVertex
					- class ReducedSurface
					- class RSComputer
					- class SESEdge
					- class SESFace
					- class SESVertex
					- class SolventExcludedSurface
					- class SESComputer
					- class TSESSingularityCleaner
					- class TriangulatedSES
					- class SESTriangulator
					- class SASEdge
					- class SASFace
					- class SASVertex
					\itme class SolventAccessibleSurface
					- class TriangulatedSAS
					- class SASTriangulator
				
		*/
		friend class RSFace;
		friend class RSVertex;
		friend class ReducedSurface;
		friend class RSComputer;
		friend class SESEdge;
		friend class SESFace;
		friend class SESVertex;
		friend class SolventExcludedSurface;
		friend class SESComputer;
		friend class SESSingularityCleaner;
		friend class TriangulatedSES;
		friend class SESTriangulator;
		friend class SASEdge;
		friend class SASFace;
		friend class SolventAccessibleSurface;
		friend class SASVertex;
		friend class TriangulatedSAS;
		friend class SASTriangulator;

		BALL_CREATE(RSEdge)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new RSEdge object.
		*/
		RSEdge()
			;

		/**	Copy constructor.
				Create a new RSEdge object from another.
				@param	rsedge	the RSEdge object to be copied
				@param	deep		if deep = false, all pointers are set to NULL
												(default). Otherwise the new RSEdge object is linked
												to the neighbours of the old RSEdge object.
		*/
		RSEdge(const RSEdge& rsedge, bool deep = false)
			;

		/**	Detailled constructor.
				Create a new RSEdge object from a lot of nice objects
				@param	vertex1							assigned to the first vertex
				@param	vertex2							assigned to the second vertex
				@param	face1								assigned to the first face
				@param	face2								assigned to the second face
				@param	center_of_torus			assigned to the center of the torus
				@param	radius_of_torus			assigned to the radius of the torus
				@param	angle								assigned to th angle
				@param	circle1							assigned to the first contact circle
				@param	circle2							assigned to the second contact circle
				@param	intersection_point1	assigned to the first intersection point
																		(in singular case)
				@param	intersection_point2	assigned to the second intersection point
																		(in singular case)
				@param	singular
				@param	index								assigned to the index
		*/
		RSEdge(RSVertex* vertex1,
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
			;

		/**	Destructor.
				Destructs the RSEdge object.
				The neighboured faces and vertices are {/bf not} destructed.
		*/
		virtual ~RSEdge()
			;

		//@}
		/**	@name	Assignment
		*/
		//@{

		/**	Assign from another RSEdge.
				@param	rsedge	the RSEdge object to assign from
				@param	deep		if deep = false, all pointers are set to NULL
												(default). Otherwise the RSEdge object is linked to
												the neighbours of the RSEdge object to assign from.
		*/
		void set(const RSEdge& rsedge, bool deep = false)
			;

		/**	Assign from another RSEdge.
				The RSEdge object is linked to the neighbours of the RSEdge object to	
				assign from.
				@param	rsedge	the RSEdge object to assign from
		*/
		RSEdge& operator = (const RSEdge& rsedge)
			;

		/**	Assign from a lot of nice objects
				@param	vertex0							assigned to the first vertex
				@param	vertex1							assigned to the second vertex
				@param	face0								assigned to the first face
				@param	face1								assigned to the second face
				@param	center_of_torus			assigned to the center of the torus
				@param	radius_of_torus			assigned to the radius of the torus
				@param	angle								assigned to th angle
				@param	circle0							assigned to the first contact circle
				@param	circle1							assigned to the second contact circle
				@param	intersection_point0	assigned to the first intersection point
																		(in singular case)
				@param	intersection_point1	assigned to the second intersection point
																		(in singular case)
				@param	singular
				@param	index								assigned to the index
		*/
		void set(RSVertex* vertex0,
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
			;

		//@}
		/**	@name	Accessors
		*/
		//@{

		/** Set the center of the torus.
				Set the center of the torus described by the probe sphere when it
				rolls over the RSEdge.
				@param	center	the new center
		*/
		void setCenterOfTorus(const TVector3<double>& center)
			;

		/** Return the center of the torus.
				Return the center of the torus described by the probe sphere
				when it rolls over the RSEdge.
				@return	TVector3<double>	the center of the torus
		*/
		TVector3<double> getCenterOfTorus() const
			;

		/** Set the major radius of the torus.
				Set the major radius of the torus described by the probe sphere
				when it rolls over the RSEdge.
				@param	radius	the new radius
		*/
		void setMajorRadiusOfTorus(const double& radius)
			;

		/** Return the  major radius of the torus.
				Return the  major radius of the torus described by the probe sphere
				when it rolls over the RSEdge.
				@return	TVector3<double>	the major radius of the torus
		*/
		double getMajorRadiusOfTorus() const
			;

		/** Set the rotation angle.
				Set the rotation angle between the starting and ending position of
				the probe sphere when it rolls over the RSEdge.
				@param	angle	the new rotation angle
		*/
		void setAngle(const TAngle<double>& angle)
			;

		/** Return the rotation angle.
				Return the rotation angle between the starting and ending position
				of the probe sphere when it rolls over the RSEdge.
				@return	TAngle<double>	the rotation angle
		*/
		TAngle<double> getAngle() const
			;

		/** Set one of the two contact circles.
				Set one of the two contact circles of the probe sphere with the two
				RSVertices.
				@param	i				the first contact circle is changed if i = 0,
												the second otherwise
				@param	circle	the new contact circle
		*/
		void  setContactCircle(Position i, const TCircle3<double>&  circle)
			;

		/** Return one of the the contact circles.
				Return one of the the contact circles of the probe sphere with the
				two RSVertices
				@param	i
				@return	TCircle3<double>	the contact circle with the first RSVertex
														if i = 0, the contact circle with the second
														RSVertex otherwise
		*/
		TCircle3<double> getContactCircle(Position i) const
			;

		/** Set one of the intersection points.
				Set one of the intersection points of the probe sphere with the RSEdge.
				@param	i			the first intersection point is changed if i = 0,
											the second otherwise
				@param	point	the new intersection point
		*/
		void setIntersectionPoint(Position i, const TVector3<double>& point)
			;

		/** Return one of the intersection points.
				Return one of the intersection points of the probe sphere
				with the RSEdge. If the RSEdge not is singular, an exception
				is thrown.
				@return	TVector3<double>	the intersection point near to the first
														RSVertex if i = 0, the intersection point near
														to the second RSVertex otherwise
		*/
		TVector3<double> getIntersectionPoint(Position i) const
;

		/** Set singular
		*/
		void setSingular(bool singular)
			;

		//@}


		/**	@name	Predicates
		*/
		//@{

		/**	Equality operator.
				@return bool, <b>true</b> if all vertices and all faces are equal	
											modulo order, <b>false</b> otherwise
		*/
		virtual bool operator == (const RSEdge& rsedge) const
			;

		/**	Inequality operator.
				@return	bool	<b>false</b> if all vertices and all faces are equal	
											modulo order, <b>true</b> otherwise
		*/
		virtual bool operator != (const RSEdge& rsedge) const
			;

		/**	Similarity operator.
				@return	bool	<b>true</b> if all vertices are similar modulo order,
											<b>false</b> otherwise
		*/
		virtual bool operator *=(const RSEdge& rsedge) const
			;

		/** isSingular
				@return	bool	<b>true</b> if the RSEdge is singular,
											<b>false</b> otherwise
		*/
		bool isSingular() const
			;

		/** isFree
				@return	bool	<b>true</b> if the RSEdge is free, <b>false</b> otherwise
		*/
		bool isFree() const
			;

		//@}

		protected:

		/*_	@name	Attributes
		*/
		//@{

		/*_ The center of the torus described by the probe when ir rolls over
				the RSEdge
		*/
		TVector3<double> center_of_torus_;
		/*_ The major radius of the torus described by the probe when ir rolls
				over the RSEdge
		*/
		double radius_of_torus_;
		/*_ The rotation angle between the starting and ending position of the
				probe sphere when it rolls over the RSEdge
		*/
		TAngle<double> angle_;
		/*_ The contact circle of the probe sphere with the first RSVertex
		*/
		TCircle3<double> circle0_;
		/*_ The contact circle of the probe sphere with the second RSVertex
		*/
		TCircle3<double> circle1_;
		/*_ The intersection point of the probe sphere with the RSEdge near to
				the first RSVertex (in singular case).
		*/
		TVector3<double> intersection_point0_;
		/*_ The intersection point of the probe sphere with the RSEdge near to
				the second RSVertex (in singular case).
		*/
		TVector3<double> intersection_point1_;
		/*_ singular
		*/
		bool singular_;

		//@}

	};

	/**	@name	Storers
	*/
	//@{

	/**	Output- Operator
	*/
	BALL_EXPORT std::ostream& operator << (std::ostream& s, const RSEdge& rsedge);

	//@}


   
} // namespace BALL

#endif // BALL_STRUCTURE_RSEDGE_H
