// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_DATATYPE_HASHGRID_H
#define BALL_DATATYPE_HASHGRID_H

#ifndef BALL_COMMON_H
#	include <BALL/common.h>
#endif

#ifndef BALL_CONCEPT_FORWARDITERATOR_H
#	include <BALL/CONCEPT/forwardIterator.h>
#endif

#ifndef BALL_CONCEPT_VISITOR_H
#	include <BALL/CONCEPT/visitor.h>
#endif

#ifndef BALL_CONCEPT_PROCESSOR_H
#	include <BALL/CONCEPT/processor.h>
#endif

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

#ifdef BALL_HAS_GNU_SLIST
#include <ext/slist>
#else
#include <list>
#endif

#include <algorithm>

namespace BALL 
{
	namespace __private
	{
		extern const char BALL_EXPORT neighbour_table_[27][3];
	}

	template <typename Item> class HashGrid3;

	/**	\defgroup GenericHash3D Three-dimensional Hash Grid
    	\ingroup  GenericHash
	*/
	//@{

	/**	Grid Box Class.
			These boxes represent the buckets of a threedimensional hash grid.
			Every such box contains a linear list of the objects that are
			contained in this box. This list is accessible through a
			DataIterator. \par

			\ingroup GenericHash3D
	*/
	template <typename Item>
	class HashGridBox3
	{
		public:

		/**	@name	Constructors and Destructors 
		*/	
		//@{

		///	Default constructor
		HashGridBox3(HashGrid3<Item>* parent);

		/// Clears the grid box
		void clear();

		/** Clears the grid box.
				Same as clear.
		*/
		void destroy();
	
		//@}
		/**	@name	Assignment 
		*/
		//@{

		//@}
		/**	@name	Accessors 
		*/
		//@{

		void setParent(HashGrid3<Item>* p);

		/**
		 * Return the indices of this box in the parent HashGrid
		 */
		void getIndices(Position& x, Position& y, Position& z);

		/** Find an item in the item list of this grid box.
				@param item the item to be searched for
				@return a pointer to the desired item or a NULL pointer, if the 
								item could not be found.
		*/
		Item* find(const Item &item);

		/// The const version of find()
		const Item* find(const Item& item) const;

		/** Counts all items in the data item list.
				@return the size of the data item list.
		*/
		Size getSize() const;

		/** Insert an item into the data item list of a grid box.
				@param item the item to  be inserted
		*/
		void insert(const Item& item);

		/** Remove the first occurrence of a certain item from the data item list.
				@param item the item to be removed
				@return bool - <tt>true</tt>, if the item could be removed, <tt>false</tt> 
								otherwise.
		*/
		bool remove(const Item& item);

		/** Remove all occurences of a certain item from the data item list.
				@param item the item to be removed
				@return bool - <tt>true</tt>, if the item could be removed, <tt>false</tt> 
								otherwise.
		*/
		bool removeAll(const Item& item);
			
		//@}			
		/**	@name	Miscellaneous 
		*/
		//@{
		
		/// Host method
		void host(Visitor<HashGridBox3> &visitor);
		//@}
		/**	@name	Predicates 
		*/
		//@{

		/// Equality operator
		bool operator == (const HashGridBox3& box) const;

		/// Inequality operator
		bool operator != (const HashGridBox3& box) const;

		/** Test whether an item is in the data item list
				@param item
				@return bool - <tt>true</tt> if instance has <tt>item</tt>, <tt>false</tt> otherwise.
		*/
		bool has(const Item& item) const;

		/** Test, whether this box is empty, i. e. the data item list contains nothing
				@return bool - <tt>true</tt>, if <tt>this</tt> is empty. <tt>false</tt> otherwise. 
		*/
		bool isEmpty() const;

		//@}
		/**	@name	Debugging and Diagnostics 
		*/
		//@{
		
		///
		bool isValid() const;
		///
		void dump(std::ostream& s = std::cout, Size depth = 0) const;

		//@}
		/**	@name	Internal Iterators 
		*/
		//@{

		///
		bool apply(UnaryProcessor<Item>& processor);

		///
		bool apply(UnaryProcessor< HashGridBox3<Item> >& processor);

		//@}
		/** @name	External Iterators 
		*/
		//@{
		typedef Position BoxIteratorPosition;
		
		class BoxIteratorTraits
		{
			public:

			BALL_CREATE_DEEP(BoxIteratorTraits)

			virtual ~BoxIteratorTraits() {}

			BoxIteratorTraits()
                : bound_(nullptr),
                  cur_box_(nullptr),
                  position_(0),
                  x_(0),
                  y_(0),
                  z_(0)
			{
			}

			BoxIteratorTraits(const HashGridBox3& box)
                : bound_((HashGridBox3 *)&box),
                  cur_box_(bound_),
                  position_(0),
                  x_(0),
                  y_(0),
                  z_(0)
			{
				bound_->getIndices(x_, y_, z_);
			}

			BoxIteratorTraits(const BoxIteratorTraits& traits, bool /* deep */ = true)
                : bound_(traits.bound_),
                  cur_box_(traits.cur_box_),
                  position_(traits.position_),
                  x_(traits.x_),
                  y_(traits.y_),
                  z_(traits.z_)
			{
			}

			HashGridBox3* getContainer()
			{
				return bound_;
			}

			const HashGridBox3* getContainer() const
			{
				return bound_;
			}

			bool isSingular() const
			{
				return (bound_ == 0);
			}
				
			BoxIteratorPosition& getPosition()
			{
				return position_;
			}

			const BoxIteratorPosition& getPosition() const
			{
				return position_;
			}

			bool operator == (const BoxIteratorTraits& traits) const
			{
				return (position_ == traits.position_);
			}

			bool operator != (const BoxIteratorTraits& traits) const
			{
				return (position_ != traits.position_);
			}
				
			bool isValid() const
			{
				return (bound_ != 0 && position_ < 27);
			}

			void invalidate()
			{
				bound_ = 0;
				position_ = 100;
			}

			void toBegin()
			{
				position_ = 0;
				cur_box_ = bound_;
			}

			bool isBegin() const
			{
				return (position_ == 0);
			}

			void toEnd()
			{
				position_ = 27;
				cur_box_ = 0;
			}

			bool isEnd() const
			{
				return (position_ == 27);
			}

			HashGridBox3<Item>& getData()
			{
				return *cur_box_;
			}

			const HashGridBox3<Item>& getData() const
			{
				return *cur_box_;
			}

			void forward()
			{
				++position_;
				// iterate to the next existing box
				while (     position_ < 27
				       && !(cur_box_ = bound_->parent->getBox(x_ + __private::neighbour_table_[position_][0],
				                                              y_ + __private::neighbour_table_[position_][1],
				                                              z_ + __private::neighbour_table_[position_][2])))
				{
					++position_;
				}
			}
		
			private:

			HashGridBox3<Item> *bound_;
			HashGridBox3<Item> *cur_box_;
			BoxIteratorPosition position_;
			Position x_, y_, z_;
		};

		friend class BoxIteratorTraits;

		/** BoxIterators iterate over all boxes that lie in the
				direct neighborhood to a box, and over the box itself. 
				Such an iterator traverses over 27 boxes.
		*/
		typedef ForwardIterator
			<HashGridBox3<Item>, HashGridBox3<Item>,
			BoxIteratorPosition, BoxIteratorTraits>
				BoxIterator;

		/// get the first box 
		BoxIterator beginBox()
		{
			return BoxIterator::begin(*this);
		}

		/// get the last box
		BoxIterator endBox()
		{
			return BoxIterator::end(*this);
		}


		/// This is the const version of  \link BoxIterator BoxIterator \endlink 
		typedef ConstForwardIterator
			<HashGridBox3<Item>, HashGridBox3<Item>,
			BoxIteratorPosition, BoxIteratorTraits>
				ConstBoxIterator;

		/// get the first box 
		ConstBoxIterator beginBox() const
		{
			return ConstBoxIterator::begin(*this);
		}

		/// get the last box
		ConstBoxIterator endBox() const
		{
			return ConstBoxIterator::end(*this);
		}

#ifdef BALL_HAS_GNU_SLIST
		typedef typename __gnu_cxx::slist<Item> DataContainer;
#else
		typedef typename std::list<Item> DataContainer;
#endif

		typedef typename DataContainer::iterator DataIteratorPosition;
		
		class DataIteratorTraits
		{
			public:

			BALL_CREATE_DEEP(DataIteratorTraits)

				virtual ~DataIteratorTraits() {}

			DataIteratorTraits()
			:	bound_(0),
				position_()
			{
			}
				
			DataIteratorTraits(const HashGridBox3& box)
			:	bound_((HashGridBox3 *)&box),
				position_(bound_->data.begin())
			{
			}
				
			DataIteratorTraits(const DataIteratorTraits& traits, bool /* deep */ = true)
			:	bound_(traits.bound_),
				position_(traits.position_)
			{
			}
				
			const DataIteratorTraits& operator = (const DataIteratorTraits &traits)
			{
				bound_ = traits.bound_;
				position_ = traits.position_;
				return *this;
			}

			HashGridBox3* getContainer()
			{
				return bound_;
			}

			const HashGridBox3* getContainer() const
			{
				return bound_;
			}

			bool isSingular() const
			{
				return (bound_ == 0);
			}
				
			DataIteratorPosition& getPosition()
			{
				return position_;
			}

			const DataIteratorPosition& getPosition() const
			{
				return position_;
			}

			bool operator == (const DataIteratorTraits &traits) const
			{
				return (position_ == traits.position_);
			}

			bool operator != (const DataIteratorTraits &traits) const
			{
				return (position_ != traits.position_);
			}

			bool isValid() const
			{
				return (bound_ != 0 && position_ != bound_->data.end());
			}

			void invalidate()
			{
				bound_ = 0;
				position_ = bound_->data.end();
			}

			void toBegin()
			{
				position_ = bound_->data.begin();
			}

			bool isBegin() const
			{
				return (position_ == bound_->data.begin());
			}

			void toEnd()
			{
				position_ = bound_->data.end();
			}

			bool isEnd() const
			{
				return (position_ == bound_->data.end());
			}

			Item& getData()
			{
				return *position_;
			}

			const Item& getData() const
			{
				return *position_;
			}

			void forward()
			{
				++position_;
			}
	
			private:

			HashGridBox3<Item>* 	bound_;
			DataIteratorPosition 	position_;
		};

		friend class DataIteratorTraits;

		/** Data iterator for grid boxes.
				This iterator traverses the list of data items store in a  \link HashGridBox3 HashGridBox3 \endlink .
		*/
		typedef ForwardIterator
			<HashGridBox3<Item>, Item,
			 DataIteratorPosition, DataIteratorTraits>
			DataIterator;

		///
		DataIterator beginData()
		{
			return DataIterator::begin(*this);
		}

		///
		DataIterator endData()
		{
			return DataIterator::end(*this);
		}


		/** Const data iterator for grid boxes.
				This is the const version of  \link DataIterator DataIterator \endlink 
		*/
		typedef ConstForwardIterator
			<HashGridBox3<Item>, Item,
			 DataIteratorPosition, DataIteratorTraits>
			ConstDataIterator;

		///
		ConstDataIterator beginData() const
		{
			return ConstDataIterator::begin(*this);
		}

		///
		ConstDataIterator endData() const
		{
			return ConstDataIterator::end(*this);
		}

		//@}	

		HashGrid3<Item>* parent;

		DataContainer data;
	};

	template<typename Item>  
	HashGridBox3<Item>::HashGridBox3(HashGrid3<Item>* p)
		: parent(p)
	{
	}

	template<typename Item>  
	void HashGridBox3<Item>::clear()
	{
		data.clear();
	}

	template<typename Item>  
	BALL_INLINE 
	void HashGridBox3<Item>::destroy()
	{
		clear();
	}

	template<typename Item>
	BALL_INLINE void HashGridBox3<Item>::setParent(HashGrid3<Item>* p)
	{
		parent = p;
	}

	template<typename Item>
	BALL_INLINE
	void HashGridBox3<Item>::getIndices(Position& x, Position& y, Position& z)
	{
		parent->getIndices(*this, x, y, z);
	}

	template<typename Item>  
	Item* HashGridBox3<Item>::find(const Item& item)
	{
		typename DataContainer::iterator found = std::find(data.begin(), data.end(), item);

		if (found != data.end())
		{
			return &(*found);
		}

		return 0;
	}

	template<typename Item>  
	BALL_INLINE 
	const Item* HashGridBox3<Item>::find(const Item& item) const
	{
		return const_cast<HashGridBox3*>(this)->find(item);
	}

	template<typename Item>  
	Size HashGridBox3<Item>::getSize() const
	{
		return data.size();
	}

	template<typename Item>  
	BALL_INLINE 
	void HashGridBox3<Item>::insert(const Item& item)
	{
		data.push_front(item);
	}

	template<typename Item>  
	bool HashGridBox3<Item>::remove(const Item& item)
	{
#ifdef BALL_HAS_GNU_SLIST
		if(data.empty())
		{
			return false;
		}

		if(*data.begin() == item)
		{
			data.pop_front();
			return true;
		}

		DataIteratorPosition prev = data.begin();
		DataIteratorPosition pos = prev;
		for(++pos; pos != data.end(); ++pos)
		{
			if(*pos == item)
			{
				data.erase_after(prev);
				return true;
			}

			prev = pos;
		}
		return false;
#else
		DataIteratorPosition pos = std::find(data.begin(), data.end(), item);

		if (pos != data.end())
		{
			data.erase(pos);

			return true;
		}

		return false;
#endif
	}

	template<typename Item>  
	bool HashGridBox3<Item>::removeAll(const Item& item)
	{
		data.remove(item);

		return true;
	}

	template <typename Item>
	BALL_INLINE 
	void HashGridBox3<Item>::host(Visitor< HashGridBox3<Item> >& visitor)
	{
		visitor.visit(*this);
	}

	template<typename Item>  
	bool HashGridBox3<Item>::operator == (const HashGridBox3<Item>& box) const
	{
		return (data == box.data);
	}

	template<typename Item>  
	BALL_INLINE 
	bool HashGridBox3<Item>::operator != (const HashGridBox3<Item>& box) const
	{
		return !(*this == box);
	}

	template<typename Item>  
	BALL_INLINE 
	bool HashGridBox3<Item>::has(const Item& item) const
	{
		return (std::find(data.begin(), data.end(), item) != data.end());
	}

	template<typename Item>  
	BALL_INLINE 
	bool HashGridBox3<Item>::isEmpty() const
	{
		return data.empty();
	}

	template<typename Item>  
	bool HashGridBox3<Item>::isValid() const
	{
		// this is no longer required...
		return true;
	}

	template<typename Item>  
	void HashGridBox3<Item>::dump(std::ostream& s, Size depth) const
	{
		BALL_DUMP_STREAM_PREFIX(s);
		
		BALL_DUMP_DEPTH(s, depth);
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  size: " << getSize() << std::endl;
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  data:" << std::endl;
		for (typename DataContainer::const_iterator d_it = data.begin(); d_it != data.end(); ++d_it)
		{
			BALL_DUMP_DEPTH(s, depth);
			s << "    " << *d_it << std::endl;
		}

		BALL_DUMP_STREAM_SUFFIX(s);
	}

	template <typename Item>
	bool HashGridBox3<Item>::apply(UnaryProcessor<Item>& processor)
	{
		if (processor.start() == false)
		{
			return false;
		}

		Processor::Result result;
			
		for (typename DataContainer::iterator d_it = data.begin(); d_it != data.end(); ++d_it)
		{
			result = processor(*d_it);

			if (result <= Processor::BREAK)
			{
				return (result == Processor::BREAK) ? true : false;
			}
		}

		return processor.finish(); 
	}

	template <typename Item>
	bool HashGridBox3<Item>::apply(UnaryProcessor< HashGridBox3<Item> >& processor)
	{
    if (processor.start() == false)
		{
      return false;
		}
 
		Processor::Result result;

		for (BoxIterator it = beginBox(); +it; ++it)
		{
			result = processor(*it);

			if (result <= Processor::BREAK)
			{
				return (result == Processor::BREAK) ? true : false;
			}
		}

		return processor.finish(); 
	}

	/**	Three-dimensional Hash Grid Class.
			????? \par
			
	*/
	template <typename Item>
	class HashGrid3
	{
		public:

		BALL_CREATE(HashGrid3)

		/**	@name	Constructors and destructor 
		*/
		//@{

		/// Default constructor
		HashGrid3();
			
		/** Constructor using origin, dimensions, and spacings of the grid.
				It creates a hashgrid at <tt>origin</tt> with axis-dependant spacings. 
				@param origin
				@param dimension_x
				@param dimension_y
				@param dimension_z
				@param spacing_x
				@param spacing_y
				@param spacing_z
		*/
		HashGrid3(const Vector3& origin, Size dimension_x, Size dimension_y,
				Size dimension_z, float spacing_x, float spacing_y, float spacing_z);

		/** Constructor using origin, dimensions, and a single spacing (only
				cubic grids)
		*/ 
		HashGrid3(const Vector3& origin, Size dimension_x, Size dimension_y, 
				Size dimension_z, float spacing);

		/** Constructor using two vectors and a single spacing.
				This constructor creates a hash grid at <tt>origin</tt> with spacing
				<tt>spacing</tt>. The vector <tt>size</tt> has to be relative to <tt>origin</tt>
				and defines the opposite corner of the grid, thereby setting the size 
				of the grid.
				@param origin a vector defining the origin of our cubic hash grid
				@param size a vector defining the opposite corner of the cubic grid
				@param spacing this float will be used as spacing in all three dimensions
		*/
		HashGrid3(const Vector3& origin, const Vector3& size, float spacing);

		/// Copy constructor
		HashGrid3(const HashGrid3& grid, bool deep = true);

		/// Destructor
		virtual ~HashGrid3();

		/// Clears the whole grid
		virtual void clear();

		/// Clears the HashGridBox3 at position (x, y, z)
		void clear(Position x, Position y, Position z);

		/// Clears the HashGridBox3 at position <tt>vector</tt>
		void clear(const Vector3 &vector);

		/// Destroys the grid (obsolete, only calls clear())
		void destroy();

		/// Destroys a box of the grid (obsolete, only calls clear())
		void destroy(Position x, Position y, Position z);

		/// Destroys a box of the grid (obsolete, only calls clear())
		void destroy(const Vector3& vector);

		//@}
		/**	@name	Assignment 
		*/
		//@{

		/// assigns the content of a hash grid (obsolete)
		void set(const Vector3& origin, const Vector3& unit, 
				Size	dimension_x, Size	dimension_y, Size	dimension_z);

		/// assigns the content of a hash grid (obsolete)
		void set(const Vector3& origin, float unit, Size size);

		/// assigns the content of a hash grid (obsolete)
		void set(const HashGrid3& grid, bool deep = true);

		/// Assignment operator
		const HashGrid3& operator = (const HashGrid3& grid);

		///
		void get(Vector3& origin, Vector3& unit, Size&	dimension_x, Size&	dimension_y, Size&	dimension_z) const; 

		///
		void get(HashGrid3& grid, bool deep = true) const;

		//@}
		/**	@name	Accessors 
		*/
		//@{

		/// Counts the non-empty boxes of a grid.
		Size countNonEmptyBoxes() const;
		
		/// Returns the size of a grid, i. e. ?????
		Size getSize() const;

		/// Returns the origin of the grid
		Vector3& getOrigin();

		/// Returns a const reference of the grid origin
		const Vector3& getOrigin() const;

		/// Returns the unit of the grid
		Vector3& getUnit();

		/// Returns the unit of the grid (const version)
		const Vector3& getUnit() const;

		/// Get the x dimension of the grid
		Size getSizeX() const;

		/// Get the y dimension of the grid
		Size getSizeY() const;

		/// Get the z dimension of the grid
		Size getSizeZ() const;

		/// Return the HashGridBox3 at position (x, y, z)
		HashGridBox3<Item>* getBox(Position x, Position y, Position z);

		/// Return the HashGridBox3 at position (x, y, z) (const version)
		const HashGridBox3<Item>* getBox(Position x, Position y, Position z) const;

		/// Return the HashGridBox3 at position vector 
		HashGridBox3<Item>* getBox(const Vector3& vector);

		/// Return the HashGridBox3 at position vector (const version)
		const HashGridBox3<Item>* getBox(const Vector3 &vector) const;

		/// Get the position indices of a HashGridBox3
		bool getIndices(const HashGridBox3<Item>& box, 
				Position& x, Position& y, Position& z) const;

		/// Insert an item at position (x, y, z)
		void insert(Position x, Position y, Position z, const Item& item);
		
		/// Insert an item at position <tt>vector</tt>
		void insert(const Vector3& vector, const Item& item);

		/// Remove an item from position (x, y ,z)
		bool remove(Position x, Position y, Position z, const Item& item);

		/// Remove an item from position <tt>vector</tt>
		bool remove(const Vector3& vector, const Item& item);

		//@}
		/**	@name Miscellaneous 
		*/
		//@{

		///
		void host(Visitor<HashGrid3>& visitor);

		//@}
		/**	@name	Predicates 
		*/
		//@{

		/// Equality operator
		bool operator == (const HashGrid3& grid) const;

		/// Inequality operator
		bool operator != (const HashGrid3& grid) const;

		/// Tests, whether <tt>this</tt> is empty
		bool isEmpty() const;

		//@}
		/**	@name	Debugging and Diagnostics 
		*/
		//@{

		/// Validity check
		virtual bool isValid() const;

		/// Dump the contents of a HashGrid3 to a stream
		virtual void dump(std::ostream& s = std::cout, Size depth = 0) const;

		//@}

		/**	@name Internal Iterators 
		*/
		//@{

		///
		bool apply(UnaryProcessor<Item> &processor);

		///
		bool apply(UnaryProcessor< HashGridBox3<Item> > &processor);

		/** Get the nearest item for a point
		 		@param distance number of adjacted boxes in each direction to search in
		*/
		const Item* getClosestItem(const Vector3& point, Size distance) const;

		/** Calculate the minimum required spacing to build a HashGrid3 with the given size
				and less than the given memory consumption.
				@param memory the amount of memory in bytes
				@param size   the diagonal of the grid
				@return the minimal needed spacing
		*/
		static float calculateMinSpacing(LongIndex memory, const Vector3& size);

		//@}
		/**	@name	External Iterators 
		*/
		//@{

		typedef Position BoxIteratorPosition;
		
		class BoxIteratorTraits
		{
			public:

			BALL_CREATE_DEEP(BoxIteratorTraits)

			virtual ~BoxIteratorTraits() {}

			BoxIteratorTraits()
				:	bound_(0),
					position_(0)
			{
			}
				
			BoxIteratorTraits(const HashGrid3 &grid)
				:	bound_((HashGrid3 *)&grid),
					position_(0)
			{
			}
				
			BoxIteratorTraits(const BoxIteratorTraits& traits, bool /* deep */ = true)
				:	bound_(traits.bound_),
					position_(traits.position_)
			{
			}
				
			const BoxIteratorTraits& operator = (const BoxIteratorTraits& traits)
			{
				bound_ = traits.bound_;
				position_ = traits.position_;
				return *this;
			}

			HashGrid3* getContainer()
			{
				return bound_;
			}

			const HashGrid3* getContainer() const
			{
				return bound_;
			}

			bool isSingular() const
			{
				return (bound_ == 0);
			}
				
			BoxIteratorPosition& getPosition()
			{
				return position_;
			}

			const BoxIteratorPosition& getPosition() const
			{
				return position_;
			}

			bool operator == (const BoxIteratorTraits& traits) const
			{
				return (position_ == traits.position_);
			}

			bool operator != (const BoxIteratorTraits& traits) const
			{
				return (position_ != traits.position_);
			}
				
			bool isValid() const
			{
				return (bound_ != 0 && position_ < bound_->getSize());
			}

			void invalidate()
			{
				bound_ = 0;
				position_ = bound_->getSize()+1;
			}

			void toBegin()
			{
				position_ = 0;
			}

			bool isBegin() const
			{
				return (position_ == 0);
			}

			void toEnd()
			{
				position_ = bound_->getSize();
			}

			bool isEnd() const
			{
				return (position_ == bound_->getSize());
			}

			HashGridBox3<Item>& getData()
			{
				return bound_->box_[position_];
			}

			const HashGridBox3<Item>& getData() const
			{
				return bound_->box_[position_];
			}

			void forward()
			{
				++position_;
			}
		
			private:

			HashGrid3<Item>* bound_;
			BoxIteratorPosition position_;
		};

		friend class BoxIteratorTraits;

		///
		typedef ForwardIterator
			<HashGrid3<Item>, HashGridBox3<Item>,
			 BoxIteratorPosition, BoxIteratorTraits>
			BoxIterator;

		/// 
		BoxIterator beginBox()
		{
			return BoxIterator::begin(*this);
		}

		///
		BoxIterator endBox()
		{
			return BoxIterator::end(*this);
		}


		///
		typedef ConstForwardIterator
			<HashGrid3<Item>, HashGridBox3<Item>,
			 BoxIteratorPosition, BoxIteratorTraits>
			ConstBoxIterator;

		/// 
		ConstBoxIterator beginBox() const
		{
			return ConstBoxIterator::begin(*this);
		}

		/// 
		ConstBoxIterator endBox() const
		{
			return ConstBoxIterator::end(*this);
		}

		//@}

		private:

		//_
		Index getIndex_(const HashGridBox3<Item>& box) const;

		//_
		Vector3 origin_;
		//_
		Vector3 unit_;
		//_
		Size dimension_x_;
		//_
		Size dimension_y_;
		//_
		Size dimension_z_;
		//_
		vector<HashGridBox3<Item> > box_;
	};

		
	//@}


	template <typename Item>
	HashGrid3<Item>::HashGrid3()
		: origin_(0,0,0),
			unit_(0,0,0),
			dimension_x_(0),
			dimension_y_(0),
			dimension_z_(0)
	{
	}

	template <typename Item>
	HashGrid3<Item>::HashGrid3
		(const Vector3 &originvector,
		 Size dimension_x, Size dimension_y, Size dimension_z,
		 float spacing_x, float spacing_y, float spacing_z)
		: origin_(originvector),
			unit_(spacing_x, spacing_y, spacing_z),
			dimension_x_(dimension_x),
			dimension_y_(dimension_y),
			dimension_z_(dimension_z),
		  box_(dimension_x * dimension_y * dimension_z, HashGridBox3<Item>(this))
	{
	}

	template <typename Item>
	HashGrid3<Item>::HashGrid3
		(const Vector3& origin,
		 Size dimension_x, Size dimension_y, Size dimension_z, float spacing)
		: origin_(origin),
		  unit_(spacing, spacing, spacing),
		  dimension_x_(dimension_x),
		  dimension_y_(dimension_y),
		  dimension_z_(dimension_z),
		  box_(dimension_x * dimension_y * dimension_z, HashGridBox3<Item>(this))
	{
	}

	// this constructor creates a linear array of HashGridBox3 objects.
	template <typename Item>
	HashGrid3<Item>::HashGrid3(const Vector3& origin, const Vector3& size,
			float spacing)
		: origin_(origin),
			unit_(spacing, spacing, spacing),
			dimension_x_((Size)(size.x / spacing + 1.0)),
			dimension_y_((Size)(size.y / spacing + 1.0)),
			dimension_z_((Size)(size.z / spacing + 1.0)),
		  box_(dimension_x_ * dimension_y_ * dimension_z_, HashGridBox3<Item>(this))
	{
	}

	template <typename Item>
	HashGrid3<Item>::HashGrid3(const HashGrid3<Item>& grid, bool deep)
	{
		set(grid, deep);
	}

	template <typename Item>
	HashGrid3<Item>::~HashGrid3()
	{
	}

	template <typename Item>
	void HashGrid3<Item>::clear()
	{
		Size size = dimension_x_ * dimension_y_ * dimension_z_;

		for (Position index = 0; index < (Position)size; ++index)
		{
			box_[index].clear();
		}
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::clear(Position x, Position y, Position z)
	{
		HashGridBox3<Item>* box = getBox(x, y, z);
		
		if (box != 0)
		{
			box->clear();
		}
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::clear(const Vector3& vector)
	{
		HashGridBox3<Item>* box = getBox(vector);
		
		if (box != 0)
		{
			box->clear();
		}
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::destroy()
	{
		clear();
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::destroy(Position x, Position y, Position z)
	{
		clear(x, y, z);
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::destroy(const Vector3 &vector)
	{
		clear(vector);
	}

	template <typename Item>
	void HashGrid3<Item>::set
		(const Vector3& origin, const Vector3& unit,
		 Size dimension_x, Size dimension_y, Size dimension_z)
	{
		origin_.set(origin);
		unit_.set(unit);
		dimension_x_ = dimension_x;
		dimension_y_ = dimension_y;
		dimension_z_ = dimension_z;
		box_.assign(getSize(), HashGridBox3<Item>(this));
	}

	template <typename Item>
	void HashGrid3<Item>::set(const Vector3& origin, float unit, Size size)
	{
		origin_.set(origin);
		unit_.set(unit, unit, unit);
		dimension_x_ = size;
		dimension_y_ = size;
		dimension_z_ = size;
		box_.assign(getSize(), HashGridBox3<Item>(this));
	}

	template <typename Item>
	void HashGrid3<Item>::set(const HashGrid3<Item>& grid, bool /* deep */)
	{
		origin_.set(grid.origin_);
		unit_.set(grid.unit_);
		dimension_x_ = grid.dimension_x_;
		dimension_y_ = grid.dimension_y_;
		dimension_z_ = grid.dimension_z_;
		box_ = grid.box_;

		for(Position i = 0; i < box_.size(); ++i)
		{
			box_[i].setParent(this);
		}
	}

	template <typename Item>
	BALL_INLINE 
	const HashGrid3<Item>& HashGrid3<Item>::operator = (const HashGrid3<Item> &grid)
	{
		set(grid);
		
		return *this;
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::get(Vector3 &origin, Vector3 &unit,
														Size& dimension_x, Size& dimension_y, Size& dimension_z) const
	{
		origin.set(origin_);
		unit.set(unit_);
		dimension_x = dimension_x_;
		dimension_y = dimension_y_;
		dimension_z = dimension_z_;
	}

	template <typename Item>
	BALL_INLINE void 
	HashGrid3<Item>::get(HashGrid3<Item> &grid, bool deep) const
	{
		grid.set(*this, deep);
	}

	template <typename Item>
	Size 
	HashGrid3<Item>::countNonEmptyBoxes() const
	{
		Size size = 0;

		for (Position i=0; i<27; ++i)
		{
			if (!box_[i].isEmpty())
				++size;
		}

		return size;
	}

	template <typename Item>
	BALL_INLINE 
	Size HashGrid3<Item>::getSize() const
	{
		return (dimension_x_ * dimension_y_ * dimension_z_);
	}

	template <typename Item>
	BALL_INLINE 
	Vector3& HashGrid3<Item>::getOrigin()
	{
		return origin_;
	}

	template <typename Item>
	BALL_INLINE 
	const Vector3& HashGrid3<Item>::getOrigin() const
	{
		return origin_;
	}

	template <typename Item>
	BALL_INLINE 
	Vector3& HashGrid3<Item>::getUnit()
	{
		return unit_;
	}

	template <typename Item>
	BALL_INLINE 
	const Vector3& HashGrid3<Item>::getUnit() const
	{
		return unit_;
	}

	template <typename Item>
	BALL_INLINE 
	Size HashGrid3<Item>::getSizeX() const
	{
		return dimension_x_;
	}

	template <typename Item>
	BALL_INLINE 
	Size HashGrid3<Item>::getSizeY() const
	{
		return dimension_y_;
	}

	template <typename Item>
	BALL_INLINE 
	Size HashGrid3<Item>::getSizeZ() const
	{
		return dimension_z_;
	}

	template <typename Item>
	const Item* HashGrid3<Item>::getClosestItem(const Vector3& point, Size dist) const
	{
		const HashGridBox3<Item>* box = getBox(point);
		if (!box) return 0;

		Position x, y, z;
		getIndices(*box, x, y, z);

		const Item* item = 0;
		float distance = FLT_MAX;

		// iterator over neighbour boxes
		for (Index xi = -(Index)dist; xi <= (Index)dist; xi++)
		{
			const Index xn = x + xi;
			for (Index yi = -(Index)dist; yi <= (Index)dist; yi++)
			{
				const Index yn = y + yi;
				for (Index zi = -(Index)dist; zi <= (Index)dist; zi++)
				{
					// iterate over all data items
					const HashGridBox3<Item>* const box_ptr = getBox(xn, yn, z+zi);	
					if (box_ptr != 0 && !box_ptr->isEmpty())
					{
						typename HashGridBox3<Item>::ConstDataIterator hit = box_ptr->beginData();
						for (;hit != box_ptr->endData(); hit++)
						{
							const float new_dist = ((*hit)->getPosition() - point).getSquareLength();
							if (new_dist < distance)
							{
								item = &*hit;
								distance = new_dist;
							}
						}
					}
				}
			}
		}

		return item;
	}

	template <typename Item>
	BALL_INLINE
	float HashGrid3<Item>::calculateMinSpacing(LongIndex memory, const Vector3& size)
	{
		LongSize memory_for_box = sizeof(HashGridBox3<Item>) + sizeof(HashGridBox3<Item>*);
		LongSize nr_boxes =(LongSize)floor((float)(memory / memory_for_box));

		return pow((double)((size.x * size.y * size.z) / nr_boxes), (double)(1.0 / 3.0));
	}

	template <typename Item>
	BALL_INLINE
	HashGridBox3<Item>* HashGrid3<Item>::getBox(Position x, Position y, Position z)
	{
		if (x >= (Position)dimension_x_ ||
			  y >= (Position)dimension_y_ ||
			  z >= (Position)dimension_z_)
		{
			return 0;
		} 
		else 
		{
			return &(box_[x * dimension_y_ * dimension_z_ + y * dimension_z_ + z]);
		}
	}

	template <typename Item>
	BALL_INLINE 
	const HashGridBox3<Item>* HashGrid3<Item>::getBox(Position x, Position y, Position z) const
	{
		return ((HashGrid3*)this)->getBox(x, y, z);
	}

	template <typename Item>
	BALL_INLINE 
	HashGridBox3<Item>* HashGrid3<Item>::getBox(const Vector3& vector)
	{
		float x = (vector.x - origin_.x) / unit_.x;
		float y = (vector.y - origin_.y) / unit_.y;
		float z = (vector.z - origin_.z) / unit_.z;

		// workaround for MSVC 7, dont change this !!!
		Index x1 = (Index) Maths::floor(x);
		Index y1 = (Index) Maths::floor(y);
		Index z1 = (Index) Maths::floor(z);

		return getBox(x1, y1, z1);
	}

	template <typename Item>
	BALL_INLINE 
	const HashGridBox3<Item>* HashGrid3<Item>::getBox(const Vector3& vector) const
	{
		return ((HashGrid3 *)this)->getBox(vector);
	}

	template <typename Item>
	BALL_INLINE 
	bool HashGrid3<Item>::getIndices
		(const HashGridBox3<Item>& box,
		 Position& x, Position& y, Position& z) const
	{
		Index index = getIndex_(box);
		
		if (index == INVALID_INDEX)
		{
			x = y = z = INVALID_POSITION;
			
			return false;
		}
		
		x = index / (dimension_y_ * dimension_z_);
		index  -= x * dimension_y_ * dimension_z_;
		y = index / dimension_z_;
		z = index - y * dimension_z_;
		
		return true;
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::insert
		(Position x, Position y, Position z, const Item& item)
	{
		HashGridBox3<Item>* box = getBox(x, y, z);
		
		if (box != 0)
		{
			box->insert(item);
		}
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::insert(const Vector3& vector, const Item& item)
	{
		HashGridBox3<Item> *box = getBox(vector);
		
		if (box != 0)
		{
			box->insert(item);
		}
	}

	template <typename Item>
	BALL_INLINE 
	bool HashGrid3<Item>::remove(Position x, Position y, Position z, const Item& item)
	{
		HashGridBox3<Item>* box = getBox(x, y, z);
		
		if (box != 0)
		{
			return box->remove(item);
		}
		
		return false;
	}

	template <typename Item>
	BALL_INLINE 
	bool HashGrid3<Item>::remove(const Vector3& vector, const Item& item)
	{
		HashGridBox3<Item>* box = getBox(vector);
		
		if (box != 0)
		{
			return box->remove(item);
		}

		return false;
	}

	template <typename Item>
	BALL_INLINE 
	void HashGrid3<Item>::host(Visitor< HashGrid3<Item> >& visitor)
	{
		visitor.visit(*this);
	}

	template <typename Item>
	BALL_INLINE 
	bool HashGrid3<Item>::operator == (const HashGrid3<Item>& grid) const
	{
		if (getSize() != grid.getSize()
				|| origin_ != grid.origin_
				|| unit_ != grid.unit_
				|| dimension_x_ != grid.dimension_x_
				|| dimension_y_ != grid.dimension_y_
				|| dimension_z_ != grid.dimension_z_)
		{
			return false;
		}

		return box_ == grid.box_;
	}

	template <typename Item>
	BALL_INLINE 
	bool HashGrid3<Item>::operator !=	(const HashGrid3<Item>& grid) const
	{
		return !(*this == grid);
	}

	template <typename Item>
	BALL_INLINE 
	bool HashGrid3<Item>::isEmpty() const
	{
		return (getSize() == 0);
	}

	template <typename Item>
	bool HashGrid3<Item>::isValid() const
	{
		Size size = getSize();
		
		for (Position index = 0; index < (Position)size; ++index)
		{
			if (box_[index].isValid() == false)
			{
				return false;
			}
		}

		return true;
	}

	template <typename Item>
	void HashGrid3<Item>::dump(std::ostream& s, Size depth) const
	{
		BALL_DUMP_STREAM_PREFIX(s);
		
		BALL_DUMP_DEPTH(s, depth);
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  origin: " << origin_ << std::endl;
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  unit: " << unit_.z << std::endl;
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  dimension: " << dimension_x_ << " " 
			<< dimension_y_ << " " 
			<< dimension_z_ << std::endl;
		
		Size size = getSize();
		BALL_DUMP_DEPTH(s, depth);
		s << "  size: " << size << std::endl;

		BALL_DUMP_DEPTH(s, depth);
		s << "  non empty boxes: " << countNonEmptyBoxes() << std::endl;
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  boxes:" << std::endl;
		Position x, y, z;
		for (Position index = 0; index < (Position)size; ++index)
		{
			BALL_DUMP_DEPTH(s, depth);
			getIndices(box_[index], x, y, z);
			s << "    " << index << ". box: (" 
			  << x << ',' << y << ',' << z << ')' << std::endl;
			box_[index].dump(s, 1);
		}
		
		BALL_DUMP_DEPTH(s, depth);
		s << "  non-empty boxes:" << std::endl;

		for (Position i=0; i<27; ++i)
		{
			if (!box_[i].isEmpty())
				s << "    " << getIndex_(box_[i]) << std::endl;
		}
		BALL_DUMP_STREAM_SUFFIX(s);
	}

	template <typename Item>
	bool HashGrid3<Item>::apply(UnaryProcessor<Item>& processor)
	{
		if (processor.start() == false)
		{
			return false;
		}
		Processor::Result result;

		for (Position i=0; i<27; ++i)
		{
			HashGridBox3<Item>* box = &box_[i];
			for (typename HashGridBox3<Item>::DataIterator *item = box->beginData(); +item; ++item)
			{
				result = processor(*item);

				if (result <= Processor::BREAK)
				{
					return (result == Processor::BREAK) ? true : false;
				}
			}
		}

		return processor->finish();
	}

	template <typename Item>
	bool HashGrid3<Item>::apply(UnaryProcessor< HashGridBox3<Item> >& processor)
	{
		if (processor.start() == false)
		{
			return false;
		}

		Processor::Result result;

		for (Position i=0; i<27; ++i)
		{
			HashGridBox3<Item>* box = &box_[i];
			result = processor(*box);

			if (result <= Processor::BREAK)
			{
				return (result == Processor::BREAK) ? true : false;
			}
		}

		return processor->finish();
	}

	template <typename Item>
	BALL_INLINE 
	Index HashGrid3<Item>::getIndex_(const HashGridBox3<Item>& box) const
	{
		if ((&box < &box_[0]) || (&box - &box_[0]  >= getSize()))
		{
			return INVALID_INDEX;
		} 
		else 
		{
			return (Index)(&box - &box_[0]);
		}
	}
} // namespace BALL

#endif // BALL_DATATYPE_HASHGRID_H
