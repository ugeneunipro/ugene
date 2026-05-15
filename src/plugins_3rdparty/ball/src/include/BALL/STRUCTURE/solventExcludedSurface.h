// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_SOLVENTEXCLUDEDSURFACE_H
#define BALL_STRUCTURE_SOLVENTEXCLUDEDSURFACE_H

#ifndef BALL_STRUCTURE_SESEDGE_H
#	include <BALL/STRUCTURE/SESEdge.h>
#endif

#ifndef BALL_STRUCTURE_SESFACE_H
#	include <BALL/STRUCTURE/SESFace.h>
#endif

#ifndef BALL_STRUCTURE_SESVERTEX_H
#	include <BALL/STRUCTURE/SESVertex.h>
#endif

#ifndef BALL_STRUCTURE_REDUCEDSURFACE_H
#	include <BALL/STRUCTURE/reducedSurface.h>
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

#ifndef BALL_MATHS_VECTOR4_H
#	include <BALL/MATHS/vector4.h>
#endif

#ifndef BALL_DATATYPE_HASHGRID_H
#	include <BALL/DATATYPE/hashGrid.h>
#endif

#ifndef BALL_DATATYPE_HASHMAP_H
#	include <BALL/DATATYPE/hashMap.h>
#endif

#include <vector>
#include <list>


namespace BALL
{
	class SESComputer;
	class SESSingularityCleaner;
	class TriangulatedSES;
	class SESTriangulator;

	/** Generic SolventExcludedSurface Class.
	\ingroup Surface			
	*/
	class BALL_EXPORT SolventExcludedSurface
	{
		public:

		/** @name Class friends

					- class SESComputer
					- class SESSingularityCleaner
					- class TriangulatedSES
					- class SESTriangulator
				
		*/
		friend class SESComputer;
		friend class SESSingularityCleaner;
		friend class TriangulatedSES;
		friend class SESTriangulator;

		BALL_CREATE(SolventExcludedSurface)

  	/**	@name	Type definitions
		*/
		//@{

		typedef std::vector<SESVertex*>::iterator
						VertexIterator;
		typedef std::vector<SESVertex*>::const_iterator
						ConstVertexIterator;
		typedef std::vector<SESEdge*>::iterator
						EdgeIterator;
		typedef std::vector<SESEdge*>::const_iterator
						ConstEdgeIterator;
		typedef std::list<SESEdge*>::iterator
						SingularEdgeIterator;
		typedef std::list<SESEdge*>::const_iterator
						ConstSingularEdgeIterator;
		typedef std::vector<SESFace*>::iterator
						ContactFaceIterator;
		typedef std::vector<SESFace*>::const_iterator
						ConstContactFaceIterator;
		typedef std::vector<SESFace*>::iterator
						SphericFaceIterator;
		typedef std::vector<SESFace*>::const_iterator
						ConstSphericFaceIterator;
		typedef std::vector<SESFace*>::iterator
						ToricFaceIterator;
		typedef std::vector<SESFace*>::const_iterator
						ConstToricFaceIterator;

		//@}
		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new SolventExcludedSurface object.
		*/
		SolventExcludedSurface();

		/**	Copy constructor.
				Create a new SolventExcludedSurface object from another.
				@param	ses		the SolventExcludedSurface object to be copied
				@param	bool	ignored - just for interface consistency
		*/
		SolventExcludedSurface
				(const SolventExcludedSurface& ses, bool = false);

		/** Detailed constructor
		*/
		SolventExcludedSurface(ReducedSurface* reduced_surface);

		/**	Destructor.
				As there are no dynamic	data structures, nothing happens.
		*/
		virtual ~SolventExcludedSurface()
			;

		//@}
		/**	@name	Accessors
		*/
		//@{

		void clear();

		void clean(const double& density);

		/**	Computes the solvent excluded surface from a ReducedSurface object
		*/
		void compute()
;

		void splitSphericFaces()
			;

		bool check()
			;

		//@}
		/**	@name	External Iterators
		*/
		//@{

		VertexIterator beginVertex()
			;
		ConstVertexIterator beginVertex() const
			;
		VertexIterator endVertex()
			;
		ConstVertexIterator endVertex() const
			;

		EdgeIterator beginEdge()
			;
		ConstEdgeIterator beginEdge() const
			;
		EdgeIterator endEdge()
			;
		ConstEdgeIterator endEdge() const
			;

		SingularEdgeIterator beginSingularEdge()
			;
		ConstSingularEdgeIterator beginSingularEdge() const
			;
		SingularEdgeIterator endSingularEdge()
			;
		ConstSingularEdgeIterator endSingularEdge() const
			;

		ContactFaceIterator beginContactFace()
			;
		ConstContactFaceIterator beginContactFace() const
			;
		ContactFaceIterator endContactFace()
			;
		ConstContactFaceIterator endContactFace() const
			;

		SphericFaceIterator beginSphericFace()
			;
		ConstSphericFaceIterator beginSphericFace() const
			;
		SphericFaceIterator endSphericFace()
			;
		ConstSphericFaceIterator endSphericFace() const
			;

		ToricFaceIterator beginToricFace()
			;
		ConstToricFaceIterator beginToricFace() const
			;
		ToricFaceIterator endToricFace()
			;
		ConstToricFaceIterator endToricFace() const
			;

		//@}

		private:

		void splitSphericFace(Position i)
			;

		void deleteSmallToricFace(SESFace* face)
			;

		void deleteSmallSingularToricFace(SESFace* face)
			;

		bool cleanToricFace(SESFace* face, const double& sqrt_density)
			;

		bool cleanSingularToricFace(SESFace* face, const double& sqrt_density)
			;

		void cleanVertices()
			;

		void cleanEdges()
			;

		void cleanContactFaces()
			;

		void cleanToricFaces()
			;

		void cleanSphericFaces()
			;


		protected:

    /*_ the number of vertices of the solvent exluded surface
    */
		Position number_of_vertices_;
    /*_ the vertices of the solvent exluded surface
    */
		::std::vector<SESVertex*> vertices_;
    /*_ the number of edges of the solvent exluded surface
    */
		Position number_of_edges_;
    /*_ the edges of the solvent exluded surface
    */
		::std::vector<SESEdge*> edges_;
    /*_ the number of singular edges of the solvent exluded surface
    */
		Position number_of_singular_edges_;
    /*_ the singular edges of the solvent exluded surface
    */
		::std::list<SESEdge*> singular_edges_;
    /*_ the number of contact faces of the solvent exluded surface
    */
		Position number_of_contact_faces_;
    /*_ the contact faces of the solvent exluded surface
    */
		::std::vector<SESFace*> contact_faces_;
    /*_ the number of toric reentrant faces of the solvent exluded surface
    */
		Position number_of_toric_faces_;
    /*_ the toric reentrant faces of the solvent exluded surface
    */
		::std::vector<SESFace*> toric_faces_;
    /*_ the number of spheric reentrant faces of the solvent exluded surface
    */
		Position number_of_spheric_faces_;
    /*_ the spheric reentrant faces of the solvent exluded surface
    */
		::std::vector<SESFace*> spheric_faces_;
		/*_ the corresponding reduced surface
		*/
		ReducedSurface* reduced_surface_;

	};


	/**	@name	Storers
	*/
	//@{

	/**	Output- Operator
	*/
	BALL_EXPORT std::ostream& operator <<
			(std::ostream& s, const SolventExcludedSurface& ses);

	//@}



	/** Generic SESComputer Class.
	\ingroup Surface			
	*/
	class BALL_EXPORT SESComputer
	{
		public:

		BALL_CREATE(SESComputer)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new SESComputer object.
		*/
		SESComputer()
			;

		/** Detailed constructor
		*/
		SESComputer(SolventExcludedSurface* ses)
			;

		/**	Destructor.
				As there are no dynamic	data structures, nothing happens.
		*/
		virtual ~SESComputer()
			;

		//@}
		/**	@name	Accessors
		*/
		//@{

		/**	Computes the solvent excluded surface
		*/
		void run()
;

		//@}

		private:

		/*_	@name	SES computation (private)
		*/
		//@{

		void preProcessing()
			;

		void get()
			;

		void createSphericFace(Position j)
			;

		SESVertex* createVertex
			(const TVector3<double>& probe_center,
			 Index index)
			;

		void pushVertex
			(SESFace* face,
			 const TSphere3<double>& probe,
			 RSVertex* rsvertex)
			;

		SESEdge* createConcaveEdge
			(SESFace* spheric_face,
			 Position p1,
			 Position p2,
			 Index index,
			 const double& radius_of_probe)
			;

		void pushConcaveEdge
			(SESFace* face,
			 Position p1,
			 Position p2,
			 const double& radius_of_probe)
			;

		SESEdge* createConvexEdge
			(SESFace* toric_face,
			 RSVertex* rsvertex)
			;

		void createToricFace(Position i)
			;

		void treatSingularToricFace(Position i)
			;

		void createFreeToricFace(Position i)
			;

		SESVertex* createSingularVertex
				(Position ip,
				 const TVector3<double>& dir,
				 SESFace* face0,
				 SESFace* face1,
				 SESFace* face2,
				 SESEdge* edge0,
				 SESEdge* edge1,
				 SESEdge* edge2)
			;

		void updateEdge
				(SESEdge*		edge,
				 SESVertex*	vertex1,
				 SESVertex*	vertex2,
				 bool						is_new)
			;

		void getPoint
			(const TVector3<double>& p1,
			 const TVector3<double>& p2,
			 const double&						dist,
			 TVector3<double>&				result);

		Index vertexExists(const TVector3<double>& point);

		//@}


		protected:

		/*_ a pointer to the solvent excluded surface to compute
		*/
		SolventExcludedSurface* ses_;
		/*_ a HashGrid to store and find the vertices in an efficient way
		*/
		HashGrid3<Index> vertex_grid_;
	};




	/** Generic SESComputer Class.
	\ingroup Surface			
	*/
	class BALL_EXPORT SESSingularityCleaner
	{
		public:

		BALL_CREATE(SESSingularityCleaner)

		typedef std::pair< std::pair<TAngle<double>,Index>,TVector3<double> > Intersection;

		struct ProbeIntersection
		{
			TVector3<double> point[2];
		};

		friend class SESComputer;

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new SESComputer object.
		*/
		SESSingularityCleaner()
			;

		/** Detailed constructor
		*/
		SESSingularityCleaner
				(SolventExcludedSurface*	ses,
				 HashGrid3<Index>*				vertex_grid_)
			;

		/**	Destructor.
				As there are no dynamic	data structures, nothing happens.
		*/
		virtual ~SESSingularityCleaner()
			;

		//@}
		/**	@name	Accessors
		*/
		//@{

		/**	Solves the singularities
		*/
		bool run()
;

		//@}

		private:

		/**	@name	Treatment of singularities (private)
		*/
		//@{

		Index vertexExists(TVector3<double> point)
			;

		void treatSingularities()
			;

		void getSingularFaces(std::list<SESFace*>& faces)
			;

		bool treatFirstCategory()
			;

		void treatSecondCategory()
			;

		void getFirstCategoryFaces(std::list<SESFace*>& first_category_faces)
			;

		void noCut(SESFace* face1, SESFace* face2)
			;

		void twoCuts(SESFace* face1, SESFace* face2)
			;

		void treatSingularEdge
			(SESEdge*								edge,
			 HashGrid3<Position>&		grid,
			 ::std::list<SESEdge*>&	deletable_edges)
			;

		void getIntersectionsOfSingularEdge
			(SESEdge*									edge,
			 const TAngle<double>&							phi,
			 HashGrid3<Position>&			grid,
			 std::list<Intersection>& intersections)
			;

		bool getIntersectionPointsAndAngles
			(const TCircle3<double>&	circle,
			 const TVector3<double>&	point,
			 Position				index1,
			 Position				index2,
			 Position				probe_index,
			 TAngle<double>&					phi1,
			 TVector3<double>&				point1,
			 TAngle<double>&					phi2,
			 TVector3<double>&				point2)
			;

		bool isIntersection
			 (const TAngle<double>&	 min_phi,
				const TAngle<double>&	 max_phi,
				const TAngle<double>&	 phi,
				const TVector3<double>& middle,
				const TSphere3<double>& probe)
			;

		void buildEndEdges
			(SESEdge*												edge,
			 const std::list<Intersection>&	min,
			 const std::list<Intersection>&	max,
			 SESVertex*&										vertex1,
			 SESVertex*&										vertex2,
			 Index&													actual_min,
			 Index&													actual_max)
			;

		void buildEdge
			(SESEdge*								edge,
			 Index									face1,
			 Index&									face2,
			 Index									end,
			 SESVertex*&						vertex,
			 const HashSet<Index>&	indices,
			 bool										minimum);

		void getExtrema
				(const std::list<Intersection>& intersections,
				 std::list<Intersection>& min,
				 std::list<Intersection>& max);

		void buildEndEdge
			(SESEdge*												edge,
			 const std::list<Intersection>&	extrema,
			 SESVertex*&										vertex,
			 Index&													actual_extremum,
			 bool														min);

		bool probeIntersection
			 (Index		 face1,
				Index		 face2,
				Index		 face3,
				TVector3<double>& point1,
				TVector3<double>& point2);

		void sort
				(SESFace* face1,
				 SESFace* face2,
				 std::vector<SESEdge*>& sesedge1,
				 std::vector<SESEdge*>& sesedge2,
				 std::vector<SESVertex*>& sesvertex1,
				 std::vector<SESVertex*>& sesvertex2);

		void sort
			 (Index		u1, Index		u2, Index		u3,
				Index&	s1,	Index&	s2, Index&	s3);

		//@}


		protected:

		/*_ a pointer to the solvent excluded surface to compute
		*/
		SolventExcludedSurface* ses_;
		/*_ a pointer to a HashGrid to store and find the vertices
				in an efficient way
		*/
		HashGrid3<Index>* vertex_grid_;
		/*_ for each triple of probe spheres its intersections
		*/
		HashMap< Position,
						 HashMap< Position,
											HashMap< Position,
															 ProbeIntersection*
														 >
										>
					 > probe_intersections_;

	};

   
} // namespace BALL

#endif  // BALL_STRUCTURE_SOLVENTEXCLUDEDSURFACE_H
