// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_SESVERTEX_H
#define BALL_STRUCTURE_SESVERTEX_H

#ifndef BALL_STRUCTURE_GRAPHVERTEX_H
#	include <BALL/STRUCTURE/graphVertex.h>
#endif

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

namespace BALL
{
	class SESEdge;
	class SESFace;
	class SolventExcludedSurface;
	class SESComputer;
	class SESSingularityCleaner;
	class TriangulatedSES;
	class SESTriangulator;

	/** Generic SESVertex Class.
   \ingroup Surface   
	*/
	class BALL_EXPORT SESVertex : public GraphVertex< SESVertex,SESEdge,SESFace >
	{
		public:

		/** @name Class friends

					- class SESEdge
					- class SESFace
					- class SolventExcludedSurface
					- class SESComputer
					- class TSESSingulatityCleaner<double>
					- class TriangulatedSES
					- class SESTriangulator
				
		*/
		friend class SESEdge;
		friend class SESFace;
		friend class SolventExcludedSurface;
		friend class SESComputer;
		friend class SESSingularityCleaner;
		friend class TriangulatedSES;
		friend class SESTriangulator;

		BALL_CREATE(SESVertex)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new SESVertex object.
		*/
		SESVertex()
			;

		/**	Copy constructor.
				Create a new SESVertex object from another.
				@param	sesvertex	the SESVertex object to be copied
				@param	deep			if deep = false, all pointers are set to NULL
													(default). Otherwise the new SESVertex object is
													linked to the neighbours of the old SESVertex object.
		*/
		SESVertex(const SESVertex& sesvertex, bool deep = false)
			;

		/**	Detailled constructor.
				Create a new SESVertex object from some nice objects.
				@param	point		assigned to the point representet by the vertex
				@param	normal	assigned to the normal vector of the vertex
				@param	atom		assigned to the index of the closest atom
				@param	index		assigned to the index of the vertex
		*/
		SESVertex
				(const TVector3<double>&	point,
				 const TVector3<double>&	normal,
				 Index					atom,
				 Index					index)
			;

		/**	Destructor.
				Destructs the RSVertex object.
		*/
		virtual ~SESVertex()
			;

		//@}
		/**	@name	Assignment
		*/
		//@{

		/**	Assign from another SESVertex.
				@param	sesvertex	the SESVertex object to assign from
				@param	deep			if deep = false, all pointers are set to NULL
													(default). Otherwise the new SESVertex object is
													linked to the neighbours of the SESVertex object
													to assign from.
		*/
		void set(const SESVertex& sesvertex, bool deep = false)
			;

		/**	Assign from another SESVertex.
				The new SESVertex object is linked to the neighbours of the SESVertex	
				object to assign from.
				@param	sesvertex	the SESVertex object to assign from
		*/
		SESVertex& operator = (const SESVertex& sesvertex)
			;

		/**	Assign to two TVector3<double>'s and two indices.
				@param	point		assigned to the point represented by the vertex
				@param	normal	assigned to the normal vector of th vertex
				@param	atom		assigned to the index of the closest atom
				@param	index		assigned to the index of the vertex
		*/
		void set(const TVector3<double> point,
				const TVector3<double>& normal,
				Index atom,
				Index index)
			;

		/**	Set the point represented by the SESVertex.
				@param	point	the new point
		*/
		void setPoint(const TVector3<double>& point)
			;

		/**	Return the point represented by the SESVertex.
				@return	TVector3<double>	the point represented by the vertex
		*/
		TVector3<double> getPoint() const
			;

		/**	Set the normal vector of the SESVertex.
				@param	normal	the new normal vector
		*/
		void setNormal(const TVector3<double>& normal)
;

		/**	Return the normal vector of the SESVertex.
				@return	TVector3<double>	the normal vector of the vertex
		*/
		TVector3<double> getNormal() const
			;

		/**	Set the index of the closest atom of the SESVertex.
				@param	atom	the index of the closest atom
		*/
		void setAtom(Index atom)
			;

		/**	Return the index of the closest atom.
				@return	Index	the index of the closest atom
		*/
		Index getAtom() const
			;

		//@}

		/**	@name	Predicates
		*/
		//@{

		/**	Equality operator.
				@return	bool	<b>true</b>
		*/
		virtual bool operator == (const SESVertex&) const
			;

		/**	Inequality operator.
				@return	bool	<b>false</b>
		*/
		virtual bool operator != (const SESVertex&) const
			;

		/**	Similarity operator.
				@return	bool	<b>true</b>
		*/
		virtual bool operator *= (const SESVertex&) const
			;

		//@}

		protected:

		/**	@name	Attributes
		*/
		//@{

		/*_	The coordinates of the vertex.
		*/
		TVector3<double> point_;
		/*_	The normal vector of the vertex.
		*/
		TVector3<double> normal_;
		/*_	The index of the closest atom.
		*/
		Index atom_;

		//@}
	};

	/**	@name	Storers
	*/
	//@{

	/**	Output- Operator
	*/
	BALL_EXPORT std::ostream& operator << (std::ostream& s, const SESVertex& sesvertex);

	//@}

   
} // namespace BALL

#endif // BALL_SES_SESVERTEX_H
