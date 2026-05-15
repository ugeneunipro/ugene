// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_RSFACE_H
#define BALL_STRUCTURE_RSFACE_H

#ifndef BALL_STRUCTURE_GRAPHFACE_H
#	include <BALL/STRUCTURE/graphFace.h>
#endif

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

#ifndef BALL_DATATYPE_HASHSET_H
#	include <BALL/DATATYPE/hashSet.h>
#endif

namespace BALL
{
    class RSVertex;
    class RSEdge;
    class ReducedSurface;
    class RSComputer;
    class SolventExcludedSurface;
    class SESComputer;
    class SESSingularityCleaner;
    class SESEdge;
    class SESFace;
    class SESVertex;
    class TriangulatedSES;
    class SASTriangulator;
    class SolventAccessibleSurface;
    class SASEdge;
    class SASFace;
    class SASVertex;
    class TriangulatedSAS;
    class SESTriangulator;

    /** Generic RSFace Class.
    \ingroup Surface
    */
    class BALL_EXPORT RSFace	:	public GraphTriangle< RSVertex,RSEdge,RSFace >
    {
        public:

        /** @name Class friends

                    - class RSEdge
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
                    - class SolventAccessibleSurface;
                    - class TriangulatedSAS
                    - class SASTriangulator

        */
        friend class RSEdge;
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
        friend class SASVertex;
        friend class SolventAccessibleSurface;
        friend class TriangulatedSAS;
        friend class SASTriangulator;

        BALL_CREATE(RSFace)

        /**	@name	Constructors and Destructors
        */
        //@{

        /**	Default constructor.
                This method creates a new RSFace object.
        */
        RSFace()
            ;

        /**	Copy constructor.
                Create a new RSFace object from another.
                @param	rsface	the RSFace object to be copied
                @param	deep		if deep = false, all pointers are set to NULL
                                                (default). Otherwise the new RSFace object is linked
                                                to the neighbours of the old RSFace object.
        */
        RSFace(const RSFace& rsface, bool deep = false)
            ;

        /**	Detailled constructor.
                Create a new RSFace object from a lot of nice objects
                @param	vertex1		assigned to the first vertex
                @param	vertex2		assigned to the second vertex
                @param	vertex3		assigned to the third vertex
                @param	edge1			assigned to the first edge
                @param	edge2			assigned to the second edge
                @param	edge3			assigned to the third edge
                @param	center		assigned to the center
                @param	normal		assigned to the normal vector
                @param	singular
                @param	index			assigned to the index
        */
        RSFace(RSVertex* vertex1,
                RSVertex* vertex2,
                RSVertex* vertex3,
                RSEdge* edge1,
                RSEdge* edge2,
                RSEdge* edge3,
                const TVector3<double>& center,
                const TVector3<double>& normal,
                bool singular,
                Index index)
;

        /**	Destructor.
                Destructs the RSFace object.
        */
        virtual ~RSFace()
            ;

        //@}
        /**	@name	Assignment
        */
        //@{

        /**	Assign from another RSFace.
                @param	rsface	the RSFace object to assign from
                @param	deep		if deep = false, all pointers are set to NULL
                                                (default). Otherwise the RSFace object is linked to
                                                the neighbours of the RSFace object to assign from.
        */
        void set(const RSFace& rsface, bool deep = false)
            ;

        /**	Assign from another RSFace.
                The RSFace object is linked to the neighbours of the RSFace object to
                assign from.
                @param	rsface	the RSFace object to assign from
        */
        RSFace& operator = (const RSFace& rsface)
            ;

        /**	Assign to a lot of nice objects
                @param	vertex1		assigned to the first vertex
                @param	vertex2		assigned to the second vertex
                @param  vertex3   assigned to the third vertex
                @param	edge1			assigned to the first edge
                @param	edge2			assigned to the second edge
                @param  edge3			assigned to the third edge
                @param	center		assigned to the center
                @param	normal		assigned to the normal vector
                @param	singular
                @param	index			assigned to the index
        */
        void set(RSVertex* vertex1,
                RSVertex* vertex2,
                RSVertex* vertex3,
                RSEdge* edge1,
                RSEdge* edge2,
                RSEdge* edge3,
                const TVector3<double>& center,
                const TVector3<double>& normal,
                bool singular,
                Index index)
;

        //@}
        /**	@name	Predicates
        */
        //@{

        /**	Equality operator.
                @return bool	<b>true</b> if all vertices are similar modulo order
                                            and the centers are equal, <b>false</b> otherwise
        */
        virtual bool operator == (const RSFace& rsface) const
            ;

        /**	Inequality operator.
                @return	bool	<b>false</b> if all vertices are similar modulo order
                                            and the centers are equal, <b>true</b> otherwise
        */
        virtual bool operator != (const RSFace& rsface) const
            ;

        /**	Similarity operator.
                @return	bool	<b>true</b> if all vertices are similar modulo order,
                                            <b>false</b> otherwise
        */
        virtual bool operator *=(const RSFace& rsface) const
            ;

        /** isSingular
                @return	bool	<b>true</b> if the RSFace is singular,
                                            <b>false</b> otherwise
        */
        bool isSingular() const
            ;

        //@}
        /**	@name	Accessors
        */
        //@{

        /** Set the center of the probe sphere defining the RSFace.
                @param	center	the new center
        */
        void setCenter(const TVector3<double>& center)
            ;

        /** Return the center of the probe sphere defining the RSFace.
                @return	TVector3<double>	the center of the probe sphere
        */
        TVector3<double> getCenter() const
            ;

        /** Set the vector orthogonal to the RSFace.
                @param	normal	the new normal
        */
        void setNormal(const TVector3<double>& normal)
;

        /** Return the vector orthogonal to the RSFace.
                @return	TVector3<double>	the vector orthogonal to the RSFace.
        */
        TVector3<double> getNormal() const
            ;

        /** Set singular
        */
        void setSingular(bool singular)
            ;

        /** Remove a RSFace from a ReducedSurface graph structure.
                @param	edges			In this HashSet all edges are pushed which do not
                                                    belong to any face after this remove step.
                @param	vertices	In this HashSet all vertices are pushed which
                                                    possibliy are isolated after this remove step.
                @param	faces			In this HashSet all faces are pushed which have lost
                                                    a neighbour face during this remove step.
        */
        void remove
                (HashSet<RSEdge*>&		edges,
                 HashSet<RSVertex*>&	vertices,
                 HashSet<RSFace*>&		faces)
            ;

        //@}

        protected:

        /*_	@name	Attributes
        */
        //@{

        /*_ The center of the probe sphere defining the RSFace
        */
        TVector3<double> center_;
        /*_ The vector orthogonal to the RSFace
        */
        TVector3<double> normal_;
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
    BALL_EXPORT std::ostream& operator << (std::ostream& s, const RSFace& rsface);

    //@}

} // namespace BALL

#endif // BALL_STRUCTURE_RSFACE_H
