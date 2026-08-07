// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_STRUCTURE_GRAPHEDGE_H
#define BALL_STRUCTURE_GRAPHEDGE_H

#ifndef BALL_COMMON_H
#	include <BALL/common.h>
#endif

#include <vector>

namespace BALL
{

	template <typename Vertex, typename Edge, typename Face>
	class GraphVertex;

	template <typename Vertex, typename Edge, typename Face>
	class GraphFace;

	template <typename Vertex, typename Edge, typename Face>
	class GraphTriangle;

	/** Generic GraphEdge Class.
  	\ingroup Surface
	*/
	template <typename Vertex, typename Edge, typename Face>
	class GraphEdge
	{
		public:

		/** @name Class friends

					- class GraphVertex<Vertex,Edge,Face>;
					- class GraphFace<Vertex,Edge,Face>;
					- class GraphTriangle<Vertex,Edge,Face>;
				
		*/
		friend class GraphVertex<Vertex,Edge,Face>;
		friend class GraphFace<Vertex,Edge,Face>;
		friend class GraphTriangle<Vertex,Edge,Face>;

		BALL_CREATE(GraphEdge)

		/**	@name	Constructors and Destructors
		*/
		//@{

		/**	Default constructor.
				This method creates a new GraphEdge object.
		*/
		GraphEdge();

		/**	Copy constructor.
				Create a new GraphEdge object from another.
				@param	edge	the GraphEdge object to be copied
				@param	deep	if deep = false, all pointers are set to NULL (default).
											Otherwise the new	GraphEdge object is linked to the
											neighbours of the old GraphEdge object.
		*/
		GraphEdge(const GraphEdge<Vertex,Edge,Face>& edge, bool deep = false);

		/**	Detailled constructor.
				Create a new GraphEdge object from a lot of nice objects
				@param	vertex1	assigned to the first vertex
				@param	vertex2	assigned to the second vertex
				@param	face1		assigned to the first face
				@param	face2		assigned to the second face
				@param	index		assigned to the index
		*/
		GraphEdge(Vertex*	vertex1,
							Vertex*	vertex2,
							Face*		face1,
							Face*		face2,
							Index		index);

		/**	Destructor.
				Destructs the GraphEdge object.
				The neighboured faces and vertices are {/bf not} destructed.
		*/
		virtual ~GraphEdge();

		//@}
		/**	@name	Assignment
		*/
		//@{

		/**	Assign from another GraphEdge.
				@param	edge	the GraphEdge object to assign from
				@param	deep	if deep = false, all pointers are set to NULL (default).
											Otherwise the GraphEdge object is linked to the
											neighbours of the GraphEdge object to assign from.
		*/
		void set(const GraphEdge<Vertex,Edge,Face>& edge, bool deep = false);

		/**	Assign from another GraphEdge.
				The GraphEdge object is linked to the neighbours of the GraphEdge	
				object to assign from.
				@param	edge	the GraphEdge object to assign from
		*/
		GraphEdge<Vertex,Edge,Face>& operator =
				(const GraphEdge<Vertex,Edge,Face>& edge);

		/**	Assign from a lot of nice objects
				@param	vertex0	assigned to the first vertex
				@param	vertex1	assigned to the second vertex
				@param	face0		assigned to the first face
				@param	face1		assigned to the second face
				@param	index		assigned to the index
		*/
		void set(Vertex*	vertex0,
						 Vertex*	vertex1,
						 Face*		face0,
						 Face*		face1,
						 Index		index);

		//@}
		/**	@name	Accessors
		*/
		//@{

		/** Set one of the two vertices of the GraphEdge.
				@param	i				the first vertex is changed if i = 0, the second
												otherwise
				@param	vertex	a pointer to the new vertex
		*/
		void setVertex(Position i, Vertex* vertex);

		/** Return one of the two vertices of the GraphEdge.
				@param	i
				@return	Vertex*	a pointer to the first vertex if i = 0,
												a pointer to the second vertex otherwise
		*/
		Vertex* getVertex(Position i) const;

		/** Set one of the two faces of the GraphEdge.
				@param	i			change the first face, if i = 0, the second otherwise
				@param	face	a pointer to the new face
		*/
		void setFace(Position i, Face* face);

		/** Return one of the two faces of the GraphEdge.
				@param	i
				@return	Face*	a pointer to the first face if i = 0,
											a pointer to the second face otherwise
		*/
		Face* getFace(Position i) const;

		/** Set the index of the GraphEdge.
				@param	index	the new index
		*/
		void setIndex(Index index);

		/** Return the index of the GraphEdge.
				@return	Index	the index of the GraphEdge
		*/
		Index getIndex() const;

		/** Return a pointer to the other vertex of the GraphEdge.
				If the given vertex is not incident to the GraphEdge, an exception is
				thrown.
				@param	vertex	one of the vertices of the GraphEdge
				@return	Vertex*	the other vertex
		*/
		Vertex* other(const Vertex* vertex) const
;

		/** Return a pointer to the other face of the GraphEdge.
				If the given face is not neighboured to the GraphEdge, an exception is
				thrown.
				@param	face	one of the faces of the GraphEdge
				@return	Face*	the other face
		*/
		Face* other(const Face* face) const
;

		/** Substitute a vertex by an other one.
				@param	old_vertex	the vertex that has to be substituted
				@param	new_vertex	the new vertex
				@return	bool				<b>true</b>, if the vertex can be substituted,
														<b>false</b> otherwise
		*/
		bool substitute(const Vertex* old_vertex, Vertex* new_vertex);

		/** Substitute a face by an other one.
				@param	old_vertex	the face that has to be substituted
				@param	new_vertex	the new face
				@return	bool			<b>true</b>, if the face can be substituted,
													<b>false</b> otherwise
		*/
		bool substitute(const Face* old_vertex, Face* new_vertex);

		/**	Delete a face of the GraphEdge.
				If the second face of the GraphEdge should be deleted, it is set to
				NULL. If the first one should be deleted, the second one is copied to
				first position and then set to NULL.
				@param	face	the face to delete
				@return	Face*	a pointer to the other face
		*/
		Face* remove(const Face* face);

		/*	Swap the two vertices of the GraphEdge
		*/
		void revert();

		//@}


		/**	@name	Predicates
		*/
		//@{

		/**	Equality operator.
				@return bool, <b>true</b>
		*/
		virtual bool operator == (const Edge&) const;

		/**	Inequality operator.
				@return	bool	<b>false</b>
		*/
		virtual bool operator != (const Edge&) const;

		/**	Similarity operator.
				@return	bool	<b>true</b>
		*/
		virtual bool operator *= (const Edge&) const;

		//@}

		protected:

		/*_	@name	Attributes
		*/
		//@{

		/*_ The vertices of the GraphEdge
		*/
		Vertex* vertex_[2];
		/*_ The faces of the GraphEdge
		*/
		Face* face_[2];
		/*_ The index of the GraphEdge
		*/
		Index index_;

		//@}

	};



	template <typename Vertex, typename Edge, typename Face>
	GraphEdge<Vertex,Edge,Face>::GraphEdge()
		:	index_(-1)
	{
		vertex_[0] = NULL;
		vertex_[1] = NULL;
		face_[0] = NULL;
		face_[1] = NULL;
	}


	template <typename Vertex, typename Edge, typename Face>
	GraphEdge<Vertex,Edge,Face>::
			GraphEdge(const GraphEdge<Vertex,Edge,Face>& edge, bool deep)
		:	index_(edge.index_)
	{
		if (deep)
		{
			vertex_[0] = edge.vertex_[0];
			vertex_[1] = edge.vertex_[1];
			face_[0] = edge.face_[0];
			face_[1] = edge.face_[1];
		}
		else
		{
			vertex_[0] = NULL;
			vertex_[1] = NULL;
			face_[0] = NULL;
			face_[1] = NULL;
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	GraphEdge<Vertex,Edge,Face>::GraphEdge
		(Vertex*	vertex1,
		 Vertex*	vertex2,
		 Face*		face1,
		 Face*		face2,
		 Index		index)
		: index_(index)
	{
		vertex_[0] = vertex1;
		vertex_[1] = vertex2;
		face_[0] = face1;
		face_[1] = face2;
	}


	template <typename Vertex, typename Edge, typename Face>
	GraphEdge<Vertex,Edge,Face>::~GraphEdge()
	{
	}


	template <typename Vertex, typename Edge, typename Face>
	void GraphEdge<Vertex,Edge,Face>::set
			(const GraphEdge<Vertex,Edge,Face>& edge, bool deep)
	{
		if (this != &edge)
		{
			if (deep)
			{
				vertex_[0] = edge.vertex_[0];
				vertex_[1] = edge.vertex_[1];
				face_[0] = edge.face_[0];
				face_[1] = edge.face_[1];
			}
			else
			{
				vertex_[0] = NULL;
				vertex_[1] = NULL;
				face_[0] = NULL;
				face_[1] = NULL;
			}
			index_ = edge.index_;
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	GraphEdge<Vertex,Edge,Face>& GraphEdge<Vertex,Edge,Face>::operator =
			(const GraphEdge<Vertex,Edge,Face>& edge)
	{
		if (this != &edge)
		{
			vertex_[0] = edge.vertex_[0];
			vertex_[1] = edge.vertex_[1];
			face_[0] = edge.face_[0];
			face_[1] = edge.face_[1];
			index_ = edge.index_;
		}
		return *this;
	}


	template <typename Vertex, typename Edge, typename Face>
	void GraphEdge<Vertex,Edge,Face>::set
		(Vertex*	vertex0,
		 Vertex*	vertex1,
		 Face*		face0,
		 Face*		face1,
		 Index		index)
	{
		vertex_[0] = vertex0;
		vertex_[1] = vertex1;
		face_[0] = face0;
		face_[1] = face1;
		index_ = index;
	}


	template <typename Vertex, typename Edge, typename Face>
	void GraphEdge<Vertex,Edge,Face>::setVertex(Position i, Vertex* vertex)
	{
		if (i == 0)
		{
			vertex_[0] = vertex;
		}
		else
		{
			vertex_[1] = vertex;
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	Vertex* GraphEdge<Vertex,Edge,Face>::getVertex(Position i) const
	{
		if (i == 0)
		{
			return vertex_[0];
		}
		else
		{
			return vertex_[1];
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	void GraphEdge<Vertex,Edge,Face>::setFace(Position i, Face* face)
	{
		if (i == 0)
		{
			face_[0] = face;
		}
		else
		{
			face_[1] = face;
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	Face* GraphEdge<Vertex,Edge,Face>::getFace(Position i) const
	{
		if (i == 0)
		{
			return face_[0];
		}
		else
		{
			return face_[1];
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	void GraphEdge<Vertex,Edge,Face>::setIndex(Index index)
	{
		index_ = index;
	}


	template <typename Vertex, typename Edge, typename Face>
	Index GraphEdge<Vertex,Edge,Face>::getIndex() const
	{
		return index_;
	}


	template <typename Vertex, typename Edge, typename Face>
	Vertex* GraphEdge<Vertex,Edge,Face>::other(const Vertex* vertex) const

	{
		if (vertex_[0] == vertex)
		{
			return vertex_[1];
		}
		else
		{
			if (vertex_[1] == vertex)
			{
				return vertex_[0];
			}
			else
			{
				throw Exception::GeneralException(__FILE__, __LINE__);
			}
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	Face* GraphEdge<Vertex,Edge,Face>::other(const Face* face) const

	{
		if (face_[0] == face)
		{
			return face_[1];
		}
		else
		{
			if (face_[1] == face)
			{
				return face_[0];
			}
			else
			{
				throw Exception::GeneralException(__FILE__, __LINE__);
			}
		}
	}


	template <typename Vertex, typename Edge, typename Face>
	bool GraphEdge<Vertex,Edge,Face>::substitute
			(const Vertex* old_vertex, Vertex* new_vertex)
	{
		if (vertex_[0] == old_vertex)
		{
			vertex_[0] = new_vertex;
		}
		else
		{
			if (vertex_[1] == old_vertex)
			{
				vertex_[1] = new_vertex;
			}
			else
			{
				return false;
			}
		}
		return true;
	}


	template <typename Vertex, typename Edge, typename Face>
	bool GraphEdge<Vertex,Edge,Face>::substitute
			(const Face* old_face, Face* new_face)
	{
		if (face_[0] == old_face)
		{
			face_[0] = new_face;
		}
		else
		{
			if (face_[1] == old_face)
			{
				face_[1] = new_face;
			}
			else
			{
				return false;
			}
		}
		return true;
	}


	template <typename Vertex, typename Edge, typename Face>
	Face* GraphEdge<Vertex,Edge,Face>::remove(const Face* face)
	{
		if (face_[1] == face)
		{
			face_[1] = NULL;
		}
		else
		{
			if (face_[0] == face)
			{
				face_[0] = face_[1];
				face_[1] = NULL;
			}
		}
		return face_[0];
	}


	template <typename Vertex, typename Edge, typename Face>
	void GraphEdge<Vertex,Edge,Face>::revert()
	{
		Vertex* tmp = vertex_[0];
		vertex_[0] = vertex_[1];
		vertex_[1] = tmp;
	}


	template <typename Vertex, typename Edge, typename Face>
	bool GraphEdge<Vertex,Edge,Face>::operator == (const Edge&) const
	{
		return true;
	}


	template <typename Vertex, typename Edge, typename Face>
	bool GraphEdge<Vertex,Edge,Face>::operator != (const Edge&) const
	{
		return false;
	}


	template <typename Vertex, typename Edge, typename Face>
	bool GraphEdge<Vertex,Edge,Face>::operator *= (const Edge&) const
	{
		return true;
	}



} // namespace BALL

#endif // BALL_STRUCTURE_GRAPHEDGE_H
