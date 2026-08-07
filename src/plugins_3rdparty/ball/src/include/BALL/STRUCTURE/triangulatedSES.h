// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_TRIANGULATEDSES_H
#define BALL_STRUCTURE_TRIANGULATEDSES_H

#ifndef BALL_TRUCTURE_SOLVENTEXCLUDEDSURFACE_H
#	include <BALL/STRUCTURE/solventExcludedSurface.h>
#endif

#ifndef BALL_STRUCTURE_TRIANGLE_H
#	include <BALL/STRUCTURE/triangle.h>
#endif

#ifndef BALL_STRUCTURE_TRIANGLEEDGE_H
#	include <BALL/STRUCTURE/triangleEdge.h>
#endif

#ifndef BALL_STRUCTURE_TRIANGLEPOINT_H
#	include <BALL/STRUCTURE/trianglePoint.h>
#endif

#ifndef BALL_STRUCTURE_TRIANGULATEDSURFACE_H
#	include <BALL/STRUCTURE/triangulatedSurface.h>
#endif

#ifndef BALL_MATHS_ANGLE_H
#	include <BALL/MATHS/angle.h>
#endif

#ifndef BALL_MATHS_CIRCLE3_H
#	include <BALL/MATHS/circle3.h>
#endif

#ifndef BALL_MATHS_SPHERE3_H
#	include <BALL/MATHS/sphere3.h>
#endif

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

#include <list>
#include <vector>

namespace BALL
{
	class SESTriangulator;

	/** Generic TriangulatedSES Class.
			\ingroup Surface	
	*/
	class BALL_EXPORT TriangulatedSES	:	public TriangulatedSurface
	{
		
		public:

		/** @name Class friends

					- class SESTriangulator
				
		*/
		friend class SESTriangulator;

		BALL_CREATE(TriangulatedSES)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new TriangulatedSES object.
		*/
		TriangulatedSES()
			;

		/**	Copy constructor.
				Create a new TriangulatedSES object from another.
				@param	surface	the TriangulatedSES object to be copied
				@param	bool		ignored - just for interface consistency
		*/
		TriangulatedSES(const TriangulatedSES& surface, bool = true)
			;

		/** Detailed constructor.
				Create a new empty TriangulatedSES object and set its SES.
				@param	ses			a pointer to the corresponding SES
				@param	density	the density to use by triangulation
		*/
		TriangulatedSES(SolventExcludedSurface* ses, const double& density)
			;

		/**	Destructor.
				Destructs the TriangulatedSES object.
		*/
		virtual ~TriangulatedSES()
			;
		//@}

		/**	@name	Assignment
		*/
		//@{

		/**	Assign from another TriangulatedSES.
				@param	surface	the TriangulatedSES object to assign from
				@param	bool		ignored - just for interface consistency
		*/
		void set(const TriangulatedSES& surface, bool = true)
			;

		/**	Assign from another TriangulatedSES.
				@param	surface	the TriangulatedSES object to assign from
		*/
		TriangulatedSES& operator = (const TriangulatedSES& surface)
			;

		/** Set the density used by triangulation.
		*/
		void setDensity(const double& density)
			;

		/** Get the density used by triangulation.
		*/
		double getDensity() const
			;

		//@}

		/** @name Accessors
		*/
		//@{

		void compute()
;

		//@}

		protected:

		/*_	@name	Attributes
		*/
		//@{

		SolventExcludedSurface* ses_;

		double density_;

		//@}

	};



	/** Generic SESTriangulator Class.
			 \par
			
			 \par
	\ingroup Surface	
	*/
	class BALL_EXPORT SESTriangulator
	{
		
		public:

		BALL_CREATE(SESTriangulator)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new SESTriangulator object.
		*/
		SESTriangulator()
			;

		/** Detailed constructor.
				@param	tses			a pointer to the TriangulatedSES to compute
		*/
		SESTriangulator(TriangulatedSES* tses)
			;

		/**	Destructor.
				Destructs the SESTriangulator object.
		*/
		virtual ~SESTriangulator()
			;
		//@}

		/** @name Accessors
		*/
		//@{

		void run()
;

		private:

		void preProcessing()
			;

		void triangulateToricFaces()
			;

		void partitionSingularEdges()
			;

		void triangulateContactFaces()
			;

		void triangulateSphericFaces()
			;

		void triangulateToricFace
				(SESFace*	face,
				 const double&			probe_radius)
			;

		void triangulateFreeToricFace
				(SESFace*	face,
				 const double& probe_radius)
			;

		void triangulateSingularToricFace
				(SESFace*	face,
				 const double& probe_radius)
			;

		void triangulateNonSingularToricFace
				(SESFace*	face,
				 const double& probe_radius)
			;

		void partitionSingularEdge(SESEdge* singular_edge)
			;

		void partitionFreeSingularEdge(SESEdge* singular_edge)
			;

		void partitionNonFreeSingularEdge(SESEdge* singular_edge)
			;

		void triangulateContactFace
				(SESFace*				face,
				 const TSphere3<double>& sphere)
			;

		bool triangulateSphericFace
				(SESFace* 			face,
				 const TSphere3<double>&	sphere)
			;

		bool buildSphericTriangles
				(SESFace*					face,
				 TriangulatedSES&	part,
				 const TSphere3<double>&		sphere,
			 	 bool							convex = true)
			;

		bool buildFirstTriangle
				(SESFace*												face,
				 TriangulatedSES&								part,
				 std::list<TriangleEdge*>&			border,
				 const TSphere3<double>&									sphere,
				 const HashSet<TrianglePoint*>&	points,
			 	 bool														convex)
			;

		SESEdge* firstSESEdge(const std::list<SESEdge*>& sesedge)
			;

		void buildUnambiguousTriangle
				(TriangleEdge*						 edge,
				 TrianglePoint*						 point,
				 std::list<TriangleEdge*>& border,
				 const TSphere3<double>&						 sphere,
				 TriangulatedSES&					 part,
			 	 bool													 convex
				)
			;

		void buildAmbiguousTriangles
				(TriangleEdge*						 edge,
				 std::list<TrianglePoint*> points,
				 std::list<TriangleEdge*>& border,
				 const TSphere3<double>&						 sphere,
				 TriangulatedSES&					 part,
			 	 bool													 convex
				)
			;

		void createTriangleAndEdges
				(TriangleEdge*	edge,
				 TrianglePoint* point,
				 const TSphere3<double>& sphere,
				 TriangleEdge*& edge1,
				 bool&							old1,
				 TriangleEdge*& edge2,
				 bool&							old2,
				 Triangle*&			triangle,
			 	 bool								convex
				)
			;

		void partitionOfCircle
				(const TCircle3<double>&			circle,
				 const TVector3<double>&			p0,
				 const TAngle<double>&				phi,
				 Size								number_of_segments,
				 vector< TVector3<double> >&	partition,
				 bool								on_surface = true)
			;

		Size numberOfRefinements(const double& density, const double& radius)
			;

		void buildTemplateSpheres()
			;

		void buildTriangles
			 (SESEdge*											edge0,
				SESEdge*											edge1,
				SESEdge*											edge2,
				SESEdge*											edge3,
				const std::vector< TVector3<double> >&	centers,
				const std::vector< TVector3<double> >&	edge1_points,
				const std::vector< TVector3<double> >& edge3_points,
				const double&									probe_radius)
			;

		//@}

		protected:

		/*_	@name	Attributes
		*/
		//@{

		TriangulatedSES* tses_;
		std::vector<TrianglePoint*> point_;
		std::vector<std::list<TriangleEdge*> > edge_;
		HashMap<Size, std::list<TrianglePoint*> > template_spheres_;
		double sqrt_density_;

		//@}

	};

   
}	// namespace BALL


#endif	// BALL_STRUCTURE_TRIANGULATEDSES_H
