// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_REDUCEDSURFACE_H
#define BALL_STRUCTURE_REDUCEDSURFACE_H

#ifndef BALL_MATHS_COMMON_H
#	include <BALL/MATHS/common.h>
#endif

#ifndef BALL_MATHS_SIMPLEBOX3_H
#	include <BALL/MATHS/simpleBox3.h>
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

#ifndef BALL_DATATYPE_HASHMAP_H
#	include <BALL/DATATYPE/hashMap.h>
#endif

#ifndef BALL_DATATYPE_HASHSET_H
#	include <BALL/DATATYPE/hashSet.h>
#endif

#ifndef BALL_COMMON_EXCEPTION_H
#	include <BALL/COMMON/exception.h>
#endif

#ifndef BALL_STRUCTURE_RSEDGE_H
#	include <BALL/STRUCTURE/RSEdge.h>
#endif

#ifndef BALL_STRUCTURE_RSFACE_H
#	include <BALL/STRUCTURE/RSFace.h>
#endif

#ifndef BALL_STRUCTURE_RSVERTEX_H
#	include <BALL/STRUCTURE/RSVertex.h>
#endif

#include <set>
#include <list>
#include <deque>
#include <vector>

namespace BALL
{
    struct SortedPosition2
    {
        SortedPosition2(Position a1, Position a2)
            : a(a1), b(a2)
        {
            if (a > b) std::swap(a, b);
        }

        bool operator==(const SortedPosition2& pos) const
        {
            return (a == pos.a) && (b == pos.b);
        }

        bool operator<(const SortedPosition2& pos) const
        {
            bool result;

            if (a < pos.a)
            {
                result = true;
            }
            else if (a > pos.a)
            {
                result = false;
            }
            else
            {
                result = b < pos.b;
            }

            return result;
        }

        Position a;
        Position b;
    };

    struct SortedPosition3
    {
        SortedPosition3(Position a1, Position a2, Position a3)
            : a(a1), b(a2), c(a3)
        {
            if (a > b) std::swap(a, b);
            if (a > c) std::swap(a, c);
            if (b > c) std::swap(b, c);
        }

        bool operator==(const SortedPosition3& pos) const
        {
            return (a == pos.a) && (b == pos.b) && (c == pos.c);
        }

        bool operator<(const SortedPosition3& pos) const
        {
            bool result;

            if (a < pos.a)
            {
                result = true;
            }
            else if (a > pos.a)
            {
                result = false;
            }
            else
            {
                // a == pos.a, check b next
                if (b < pos.b)
                {
                    result = true;
                }
                else if ( b > pos.b)
                {
                    result = false;
                }
                else
                {
                    result = c < pos.c;
                }
            }

            return result;
        }

        Position a;
        Position b;
        Position c;
    };
}

#if defined(BALL_HAS_UNORDERED_MAP) || defined(BALL_HAS_HASH_MAP)
#ifdef BALL_EXTEND_HASH_IN_STD_NS
namespace std
{
#endif // BALL_EXTEND_HASH_IN_STD_NS

    namespace BALL_MAP_NAMESPACE {
        template<>
        struct hash<BALL::SortedPosition2> : public std::unary_function<BALL::SortedPosition2, size_t>
        {
            inline size_t operator()(const BALL::SortedPosition2& p) const
            {
                return 5323 * p.a + 1847 * p.b;
            }
        };

        template<>
        struct hash<BALL::SortedPosition3> : public std::unary_function<BALL::SortedPosition3, size_t>
        {
            inline size_t operator()(const BALL::SortedPosition3& p) const
            {
                return 5323 * p.a + 1847 * p.b + 2347 * p.c;
            }
        };
    }

#ifdef BALL_EXTEND_HASH_IN_STD_NS
}
#endif // BALL_EXTEND_HASH_IN_STD_NS

#endif

namespace BALL
{
    class RSComputer;
    class SolventExcludedSurface;
    class SESComputer;
    class SESSingularityCleaner;
    class TriangulatedSES;
    class SolventAccessibleSurface;
    class TriangulatedSAS;
    class SESTriangulator;

    // temp
    template <class Key, class T>
    class HashMap;

    /** Generic ReducedSurface Class.
    \ingroup Surface
    */
    class BALL_EXPORT ReducedSurface
    {
        public:

        /** @name Class friends

                    - class RSComputer
                    - class SolventExcludedSurface
                    - class TriangulatedSES
                    - class SESComputer
                    - class SESSingularityCleaner
                    - class SolventAccessibleSurface
                    - class TriangulatedSAS
                    - class SESTriangulator

        */
        friend class RSComputer;
        friend class SolventExcludedSurface;
        friend class SESComputer;
        friend class SESSingularityCleaner;
        friend class SolventAccessibleSurface;
        friend class TriangulatedSES;
        friend class TriangulatedSAS;
        friend class SESTriangulator;

        BALL_CREATE(ReducedSurface)

        /**	@name	Constructors and Destructors
        */
        //@{

        /**	Default constructor.
                All components are initialized to <tt>(T)0</tt> or <tt>NULL</tt>,
                respectivly.
        */
        ReducedSurface();

        /**	Copy constructor.
                @param reduced_surface the ReducedSurface object to be copied
                @param bool ignored - just for interface consistency
        */
        ReducedSurface(const ReducedSurface& reduced_surface, bool = true);

        /**	Detailed constructor.
                Create a new ReducedSurface object from a list of spheres.
        */
        ReducedSurface(const std::vector< TSphere3<double> >& spheres,
                       const double& probe_radius);

        /**	Destructor.
        */
        virtual ~ReducedSurface();

        //@}
        /** @name Assignment
        */
        //@{

        /**	Assign from another ReducedSurface.
                @param reduced_surface the ReducedSurface object to assign from
        */
        void operator = (const ReducedSurface& reduced_surface);

        /**	Assign from another ReducedSurface.
                @param reduced_surface the ReducedSurface object to assign from
        */
        void set(const ReducedSurface& reduced_surface);

        /** Delete all vertices, edges and faces.
        */
        void clear();

        /** Remove all NULL-pointers
        */
        void clean();

        //@}
        /**	@name	Accessors
        */
        //@{

        /** Return the number of atoms.
                @return Size the number of atoms
        */
        Size numberOfAtoms() const;

        /** Return the number of rsvertices.
                @return Size the number of rsvertices
        */
        Size numberOfVertices() const;

        /** Return the number of rsedges.
                @return Size the number of rsedges
        */
        Size numberOfEdges() const;

        /** Return the number of rsfaces.
                @return Size the number of rsfaces
        */
        Size numberOfFaces() const;

        /** Return the probe radius.
                @return double the radius of the used probe sphere
        */
        double getProbeRadius() const;

        /** Return the i'th sphere.
                @param	i	the index of the sphere that should be given back
                @return TSphere3<double>, the i'th sphere
        */
    TSphere3<double> getSphere(Position i) const
;

        /** Return the i'th rsvertex.
                @param	i	the index of the rsvertex that should be given back
                @return RSVertex, the i'th rsvertex
        */
        RSVertex* getVertex(Position i) const
;

        /** Return the i'th rsedge.
                @param	i	the index of the rsedge that should be given back
                @return RSEdge, the i'th rsedge
        */
        RSEdge* getEdge(Position i) const
;

        /** Return the i'th rsface.
                @param	i	the index of the rsface that should be given back
                @return RSFace, the i'th rsface
        */
        RSFace* getFace(Position i) const
;

        /** Insert a new RSVertex.
                @param	rsvertex	a pointer to the RSVertex to insert
        */
        void insert(RSVertex* rsvertex);

        /** Insert a new RSEdge.
                @param	rsedge	a pointer to the RSEdge to insert
        */
        void insert(RSEdge* rsedge);

        /** Insert a new RSFace.
                @param	rsface	a pointer to the RSFace to insert
        */
        void insert(RSFace* rsface);

        /** Return the maximal radius of te atoms of te molecule
                @return double	the maximal radius
        */
        double getMaximalRadius() const;

        /** Return the bounding box of the centers of the atoms of the molecule
                @return TSimpleBox3<double>	the bounding box
        */
        TSimpleBox3<double> getBoundingBox() const;

        /** Delete a pair of similar faces.
                @param	face1	a pointer to the first face
                @param	face2	a pointer to the second face
        */
        void deleteSimilarFaces(RSFace* face1, RSFace* face2);

        /** Get the angle between two RSFaces.
                @param	face1		a pointer to the first face
                @param	face2		a pointer to the second face
                @param	vertex1	a pointer to the first vertex to roll over
                @param	vertex2	a pointer to the second vertex to roll over
                @param	angle		the computed angle
                @param	check		you kan ask to check whether the angle can be computed
        */
        bool getAngle(RSFace*   face1,   RSFace*   face2,
                      RSVertex* vertex1, RSVertex* vertex2,
                      TAngle<double>& angle, bool check = false) const;

        /** Compute the reduced surface.
        */
        void compute()
            ;

        //@}

        private:

        /*_ Test whether a ReducedSurface object can be copied.
        */
        bool canBeCopied(const ReducedSurface& reduced_surface);

        /*_ Copy a ReducedSurface object.
        */
        void copy(const ReducedSurface& reduced_surface);

        /*_
        */
        void correctEdges(RSFace* face1, RSFace* face2,
                          RSEdge* edge1, RSEdge* edge2);

        /*_
        */
        void joinVertices(RSFace*   face1,   RSFace*   face2,
                          RSVertex* vertex1, RSVertex* vertex2);

        /*_
        */
        void findSimilarVertices(RSFace* face1, RSFace* face2,
                                 std::vector<RSVertex*>& rsvertex1,
                                 std::vector<RSVertex*>& rsvertex2);

        /*_
        */
        void findSimilarEdges(RSFace* face1, RSFace* face2,
                              std::vector<RSEdge*>& rsedge1,
                              std::vector<RSEdge*>& rsedge2);

        protected:

        /*_ the number of atoms of the reduced surface
        */
        Size number_of_atoms_;

        /*_ the atoms of the molecule
        */

        std::vector< TSphere3<double> > atom_;

        /*_	probe radius
        */
        double probe_radius_;

        /*_ the number of vertices of the reduced surface
        */
        Size number_of_vertices_;

        /*_ the vertices of the reduced surface
        */
        std::vector< RSVertex* > vertices_;

        /*_ the number of edges of the reduced surface
        */
        Size number_of_edges_;

        /*_ the edges of the reduced surface
        */
        std::vector< RSEdge* > edges_;

        /*_ the number of faces of the reduced surface
        */
        Size number_of_faces_;

        /*_ the faces of the reduced surface
        */
        std::vector< RSFace* > faces_;

        /*_ maximal radius of all atoms
        */
        double r_max_;

        /*_ bounding SimpleBox of the atom centers of the molecule
        */
        TSimpleBox3<double> bounding_box_;
    };

    /**	@name	Storers
    */
    //@{

    /**	Output- Operator.
            Prints out a ReducedSurface
    */
    BALL_EXPORT std::ostream& operator << (std::ostream& s, const ReducedSurface& rs);

    //@}

    /** Generic RSComputer Class.
            \ingroup Surface
    */
    class BALL_EXPORT RSComputer
    {
        public:

        BALL_CREATE(RSComputer)

        /**	@name	Enums
        */
        //@{

        /** status of the probe positions of three atoms:
                <tt>0</tt> ok
                <tt>1</tt> not ok
                <tt>2</tt> not tested
        */
        enum ProbeStatus
        {
            STATUS_OK = 0,
            STATUS_NOT_OK,
            STATUS_NOT_TESTED
        };

        /** status of an atom
                <tt>0</tt> on surface
                <tt>1</tt> inside
                <tt>2</tt> outside
        */
        enum AtomStatus
        {
            STATUS_ON_SURFACE = 0,
            STATUS_INSIDE,
            STATUS_UNKNOWN
        };
        //@}

        struct ProbePosition
        {
            ProbeStatus status[2];
            TVector3<double> point[2];
        };

        /**	@name	Constructors and Destructors
        */
        //@{

        /**	Default constructor.
                All components are initialized to <tt>(T)0</tt> or <tt>NULL</tt>,
                respectivly.
        */
        RSComputer();

        /**	Detailed constructor.
        */
        RSComputer(ReducedSurface* rs);

        /**	Destructor.
        */
        virtual ~RSComputer();

        //@}
        /**	@name	Accessors
        */
        //@{

        /** Compute the reduced surface
        */
        void run()
            ;

        //@}


        private:

        /*_ @name Computing reduced surface
        */
        //@{

        /*_
        */
        void preProcessing();

        /*_ Compute a RSComponent.
        */
        void getRSComponent()
            ;

        /*_ Treat all edges of a face.
            @param	face	the RSFace to be treated
        */
        bool treatFace(RSFace* face)
            ;

        /*_ Roll over an edge that belongs to only one face and find the other one.
                @param	edge	the RSEdge to be treated
        */
        bool treatEdge(RSEdge* edge)
            ;

        /*_ Treat an ambiguous situation.
                All vertices on an ambiguous atom are deleted with all its edges and
                faces. The radius of the atom is decreased by 10 EPSILON.
                @param	atom	the index of the atom
        */
        void correct(Index atom);

        /*_ Check all new created vertices for extensions
        */
        void extendComponent()
            ;

        /*_ Find a third atom rolling over two vertices starting on a face.
                From all atoms which can be touched by the probe sphere when it
                touches the given two vertices we choose the one with smallest
                rotation angle.
                If the rotation angle equals zero, the probe sphere can touch four
                atoms and an exception is thrown.
                If no atom can be found an exception is thrown.
            @param	vertex1	the first vertex
            @param	vertex2	the second vertex
            @param	face		the starting face
            @param	probe		the new probe sphere
            @param	phi			the rotation angle
            @return	Index		index of the found atom
        */
        Index thirdAtom(RSVertex* vertex1, RSVertex* vertex2,
                        RSFace* face, TSphere3<double>& probe, TAngle<double>& phi)
            ;

        //@}
        /*_ @name Finding a start position
        */
        //@{

        /*_ Find a start position
            @param	vertex		a pointer to the found vertex, if only a vertex
                                                can be found
            @param	edge			a pointer to the found edge, if only an edge can be
                                                found
            @param	face			a pointer to the found face, if a face can be found
            @return Position	0, if no start position is found,
                                                1, if a single vertex is found,
                                                2, if an edge is found,
                                                3, if a face is found
        */
        Position getStartPosition()
;

        //@}
        /*_ @name Finding a first face
        */
        //@{

        /*_ Try to find a starting face
            @return	RSFace*	a pointer to the found face, if a face can be found,
                                                    NULL otherwise
        */
        RSFace* findFirstFace()
;

        /*_ Try to find a starting face in a given direction
            @param	direction		search in x-direction, if direction is 0,
                                                    search in y-direction, if direction is 1,
                                                    search in z-direction, if direction is 2
            @param	extrem			search in min direction, if extrem is 0,
                                                    search in max direction, if extrem is 1
            @return	RSFace*	a pointer to the found face, if a face can be found,
                                                    NULL otherwise
        */
        RSFace* findFace(Position direction, Position extrem)
;

        //@}
        /*_ @name Finding a first edge
        */
        //@{

        /*_ Try to find a starting edge
            @return	RSEdge*	a pointer to the found edge, if a face can be found,
                                                    NULL otherwise
        */
        RSEdge* findFirstEdge();

        /*_ Try to find a starting edge in a given direction
            @param	direction		search in x-direction, if direction is 0,
                                                    search in y-direction, if direction is 1,
                                                    search in z-direction, if direction is 2
            @param	extrem			search in min direction, if extrem is 0,
                                                    search in max direction, if extrem is 1
            @return	RSEdge*	a pointer to the found edge, if a face can be found,
                                                    NULL otherwise
        */
        RSEdge* findEdge(Position direction, Position extrem);

        //@}
        /*_ @name Finding a first vertex
        */
        //@{

        /*_ Try to find a single atom
            @return	RSVertex*	a pointer to the found vertex, if a vertex can be
                                                        found, NULL otherwise
        */
        RSVertex* findFirstVertex();

        /*_ Find a single atom in a given direction
            @param	direction	search in x-direction, if direction is 0,
                                                search in y-direction, if direction is 1,
                                                search in z-direction, if direction is 2
            @param	extrem		search in min direction, if extrem is 0,
                                                search in max direction, if extrem is 1
            @return	Index			the index of the found atom
        */
        Index findFirstAtom(Position direction, Position extrem);

        /*_ Find a second atom close enougth to the first atom in a given direction
            @param	atom1			the index of the first atom
            @param	direction	search in x-direction, if direction is 0,
                                                search in y-direction, if direction is 1,
                                                search in z-direction, if direction is 2
            @param	extrem		search in min direction, if extrem is 0,
                                                search in max direction, if extrem is 1
            @return	Index			the index of the found atom
        */
        Index findSecondAtom(Index atom, Position direction, Position extrem);

        /*_ Find a second atom close enougth to the first two atoms
            @param	atom1			the index of the first atom
            @param	atom2			the index of the second atom
            @param	atom_list	a HashSet of the indices of all candidate atoms
            @return	::std::list< ::std::pair< Index,TSphere3<double> > >
                                                a list of all candidates with their probe spheres
        */
        void findThirdAtom(Index atom1, Index atom2, const std::deque<Index>& third,
                           std::deque< std::pair< Index,TSphere3<double> > >& atoms);

        //@}
        /*_ @name Some utilities
        */
        //@{

        /*_ Find all atoms close enougth to two given atoms.
            The indices of all atoms which can be touched by the probe sphere when
            it touches the given atoms are computed.
            @param	atom1				the index of the first given atom
            @param	atom2				the index of the second given atom
            @param	output_list	list of all atoms close enougth to the given atoms
        */
        const std::deque<Index>& neighboursOfTwoAtoms(const SortedPosition2& pos);

        /*_ Find all atoms close enougth to three given atoms.
            The indices of all atoms which can be touched by the probe sphere when
            it touches the given atoms are computed.
            @param	atom1				the index of the first given atom
            @param	atom2				the index of the second given atom
            @param	atom3				the index of the third given atom
            @param	output_list	list of all atoms close enougth to the given atoms
        */
        void neighboursOfThreeAtoms(Index atom1, Index atom2, Index atom3,
                                    std::deque<Index>& output_list);

    /*_ Get the extrem coordinate of a circle in a given direction
        @param	circle		the circle
            @param	direction	search in x-direction, if direction is 0,
                                                search in y-direction, if direction is 1,
                                                search in z-direction, if direction is 2
            @param	extrem		search in min direction, if extrem is 0,
                                                search in max direction, if extrem is 1
            @return	double					the extrem coordinate
    */
        double getCircleExtremum(const TCircle3<double>& circle,
                                 Position direction, Position extrem);

        //@}
        /*_ @name Creating / updating edges / faces
        */
        //@{

        /*_ Create a free edge from two vertices if it is a free edge
            @param	vertex1			a pointer to the first vertex
            @param	vertex2			a pointer to the second vertex
            @return	RSEdge* a pointer to the created free edge, if there is one,
                                                    NULL otherwise
        */
        RSEdge* createFreeEdge(RSVertex* vertex1, RSVertex* vertex2);

        /*_ Get the circle described by the center of the probe sphere and the two
                contact circles with the atoms when the probe sphere rolls over two
                atoms
                @param	atom1		the index of the first atom
                @param	atom2		the index of the second atom
                @param	circle1	the circle described by the center of the probe sphere
                @param	circle2	the contact circle with atom1
                @param	circle3	the contact circle with atom2
                @return	bool		true, if the probe sphere can touch both atoms,
                                                false, otherwise
        */
        bool getCircles(Index atom1, Index atom2, TCircle3<double>& circle1,
                        TCircle3<double>& circle2, TCircle3<double>& circle3);

        /*_ Get the normal vector of the face described by three atoms and a probe
            @param	atom1				the index of the first atom
            @param	atom2				the index of the second atom
            @param	atom3				the index of the third atom
            @param	probe				the probe sphere lying on atom1, atom2, atom3
            @return	TVector3<double>	the normal vector
        */
        TVector3<double> getFaceNormal(const TSphere3<double>& atom1, const TSphere3<double>& atom2,
                                       const TSphere3<double>& atom3, const TSphere3<double>& probe);

        /*_ Update a face and it's edges
            @param	v1		the first vertex of the face
            @param	v2		the second vertex of the face
            @param	v3		the third vertex of the face
            @param	e1		the first edge
            @param	e2		the second edge
            @param	e3		the third edge
            @param	f			the face
            @param	probe	the probe sphere of the face
        */
        void updateFaceAndEdges(RSVertex* v1, RSVertex* v2, RSVertex* v3,
                                RSEdge*   e1, RSEdge*   e2, RSEdge*   e3,
                                RSFace* f, const TSphere3<double>& probe);

        /*_ Test, whether a face exists or not
            @param	face				a pointer to the face to be tested
            @return	RSFace*	a pointer to the face, if it exists, otherwise NULL
        */
        RSFace* faceExists(RSFace* face, const std::list< RSVertex* >& vertices);

        //@}
        /*_ @name Finding a probe sphere
        */
        //@{

        /*_ Get the centers of the probe sphere when it lies on three atoms
            @param	pos		the three atom's indices
            @param	c1		the first center
            @param	c2		the second center
            @return	bool	true, if the probe sphere can touch the three atoms,
                                        false, otherwise
        */
        bool centerOfProbe(const SortedPosition3& pos, TVector3<double>& c1, TVector3<double>& c2);

        /*_ Check,weather a probe sphere is inside an atom
            @param	probe	the probe sphere to be tested
            @return	bool	true, if the probe sphere is not intersecting any atom
                                        false, otherwise
        */
        bool checkProbe(const TSphere3<double>& probe, const SortedPosition3& pos);

        /*_
        */
        void correctProbePosition(Position atom);

        /*_
        */
        void correctProbePosition(const SortedPosition3& pos);

        /*_
        */
        void insert(RSVertex* vertex);

        /*_
        */
        void insert(RSEdge* edge);

        /*_
        */
        void insert(RSFace* face);

        //@}

        protected:


        /*_ a pointer to the reduced surface to compute
        */
        ReducedSurface* rs_;

        /*_ for each atom a list of its neighbours
        */
        std::vector< std::deque<Index> > neighbours_;

        /*_ for each atom a status
        */
        std::vector< AtomStatus > atom_status_;

        /*_ for each pair of atoms a list of its neighbours
        */
        HashMap< SortedPosition2, std::deque<Index> > neighbours_of_two_;

        /*_ for each triple of atoms its probe positions
        */
        HashMap< SortedPosition3, ProbePosition* > probe_positions_;

        /*_ all new created vertices which are not yet checked for extensions
        */
        HashSet<RSVertex*> new_vertices_;

        /*_ all new created faces which are not completely treated yet
        */
        HashSet<RSFace*> new_faces_;

        /*_ for each atom a list of the rsvertices of the atom
        */
        std::vector< std::list<RSVertex*> > vertices_;
    };
} // namespace BALL

#endif  // BALL_STRUCTURE_REDUCEDSURFACE_H
