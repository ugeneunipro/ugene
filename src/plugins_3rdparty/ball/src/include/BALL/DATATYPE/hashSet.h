// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_DATATYPE_HASHSET_H
#define BALL_DATATYPE_HASHSET_H

#ifndef BALL_COMMON_H
#	include <BALL/common.h>
#endif

#ifndef BALL_COMMON_HASH_H
#	include <BALL/COMMON/hash.h>
#endif

#ifndef BALL_CONCEPT_FORWARDITERATOR_H
#	include <BALL/CONCEPT/forwardIterator.h>
#endif

#ifndef BALL_CONCEPT_VISITOR_H
#	include <BALL/CONCEPT/visitor.h>
#endif

#ifndef BALL_DATATYPE_FOREACH_H
#	include <BALL/DATATYPE/forEach.h>
#endif

#ifndef BALL_CONCEPT_PREDICATE_H
#	include <BALL/CONCEPT/predicate.h>
#endif

#ifndef BALL_CONCEPT_PROCESSOR_H
#	include <BALL/CONCEPT/processor.h>
#endif

#include <algorithm>


namespace BALL
{
  /** Generic Hash Set Class.
    	\ingroup  GenericHash
  */
 	template <class Key>
	class HashSet
	{
		public:

		/**
		*/
		typedef Key ValueType;
			
		/**
		*/
		typedef Key KeyType;

		/**
		*/
		typedef Key* PointerType;

		// --- EXTERNAL ITERATORS
		struct Node
		{
			Node*				next;
			ValueType		value;

			Node(const KeyType& my_key, const Node* my_next)
				: next(const_cast<Node*>(my_next)),
					value(const_cast<ValueType&>(my_key))
			{
			}
		};

		typedef Node* IteratorPosition;
	
		class IteratorTraits
		{

			friend class HashSet<Key>;
			public:

			IteratorTraits()
				:	bound_(0),
					position_(0),
					bucket_(0)
			{
			}
			
			IteratorTraits(const HashSet& hash_set)
				:	bound_(const_cast<HashSet*>(&hash_set)),
					position_(0),
					bucket_(0)
			{
			}
			
			IteratorTraits(const IteratorTraits& traits)
				:	bound_(traits.bound_),
					position_(traits.position_),
					bucket_(traits.bucket_)
			{
			}
			
			IteratorTraits& operator = (const IteratorTraits& traits)
			{
				bound_ = traits.bound_;
				position_ = traits.position_;
				bucket_ = traits.bucket_;
		
				return *this;
			}

			HashSet* getContainer()
			{
				return bound_;
			}
			
			const HashSet* getContainer() const
			{
				return bound_;
			}
			
			bool isSingular() const
			{
				return (bound_ == 0);
			}
			
			IteratorPosition& getPosition()
			{
				return position_;
			}

			const IteratorPosition& getPosition() const
			{
				return position_;
			}

			bool operator == (const IteratorTraits& traits) const
			{
				return (position_ == traits.position_);
			}

			bool operator != (const IteratorTraits& traits) const
			{
				return (position_ != traits.position_);
			}
			
			bool isValid() const
			{
				return ((bound_ != 0) && (position_ != 0)
											&& (bucket_ < (Position)bound_->bucket_.size()));
			}

			void invalidate()
			{
				bound_ = 0;
				position_ = 0;
				bucket_ = INVALID_POSITION;
			}
			
			void toBegin()
			{
				for (bucket_ = 0;  bucket_ < (Position)bound_->bucket_.size();  ++bucket_)
				{
					position_ = bound_->bucket_[bucket_];

					if (position_ != 0)
					{
						return;
					}
				}
			}

			bool isBegin() const
			{
				for (Position bucket = 0; bucket < (Position)bound_->bucket_.size();  ++bucket)
				{
					if (bound_->bucket_[bucket_] != 0)
					{
						if (position_ == bound_->bucket_[bucket_])
						{
							return true;
						} 
						else 
						{
							return false;
						}
					}
				}

				return false;
			}

			void toEnd()
			{
				position_ = 0;
			}
			
			bool isEnd() const
			{
				return (position_ == 0);
			}
			
			ValueType& getData()
			{
				return position_->value;
			}

			const ValueType& getData() const
			{
				return position_->value;
			}

			void forward()
			{
				position_ = position_->next;

				if (position_ != 0)
				{
					return;
				}

				for (++bucket_;  bucket_ < (Position)bound_->bucket_.size();  ++bucket_)
				{
					position_ = bound_->bucket_[bucket_];

					if (position_ != 0)
					{
						return;
					}
				}
			} 

			protected:

			HashSet*						bound_;
			IteratorPosition		position_;
			Position						bucket_;
		};
		friend class IteratorTraits;

		/**	@name	Enums
		*/
		//@{

		enum
		{
			/// Initial capacity of the hash set
			INITIAL_CAPACITY          = 4,
			/// Initial number of buckets
			INITIAL_NUMBER_OF_BUCKETS = 3
		};

		//@}

		/**	@name	Exceptions
		*/
		//@{

		/**	Illegal key exception.
				Thrown if access to a non-existent key is required by the constant
				version of  \link operator [] operator [] \endlink .
		*/
		class IllegalKey
			:	public Exception::GeneralException
		{
			public:
			IllegalKey(const char* file, int line)
				:	Exception::GeneralException(file, line)
			{
			}
		};

		//@}

		/**	@name	Type definitions
		*/
		//@{
			
		///
		typedef ForwardIterator<HashSet<Key>, ValueType, PointerType, IteratorTraits> Iterator;

		///
		typedef ConstForwardIterator <HashSet<Key>, ValueType, PointerType, IteratorTraits>	ConstIterator;

		// STL compatibility stuff
		///
		typedef Iterator iterator;
		///
		typedef ConstIterator const_iterator;
		///
		typedef Key					value_type;
		///
		typedef Key					key_type;
		///
		typedef Key*				pointer;
		///
		typedef const Key*	const_pointer;
		///
		typedef Key&				reference;
		///
		typedef const Key&	const_reference;
		///
		typedef Size				size_type;
		///
		typedef Index				difference_type;			
		//@}

		/**	@name Constructors and Destructors 
		*/
		//@{

		/**	Default Constructor.
		*/
		HashSet(Size initial_capacity = INITIAL_CAPACITY, Size number_of_buckets = INITIAL_NUMBER_OF_BUCKETS);
			
		/**	Copy Constructor.
		*/
		HashSet(const HashSet& hash_set);

		/**	Destructor.
		*/
		virtual ~HashSet()
		{
			destroy();
			deleteBuckets_();
		}

		/**	Clear the hash set.
				Remove all nodes from all buckets.
				The capacity and the number of buckets remain unchanged.
		*/
		virtual void clear();
	
		/**	Clear the hash set.
				Remove all nodes from all buckets.
				The capacity and the number of buckets remain unchanged.
				Simply calls clear;
		*/
		void destroy();

		//@}

		/**	@name Assignment 
		*/
		//@{

		/** Assign this HashSet with the contents of another HashSet
				@param hash_set the HashSet to assign from
		*/
		void set(const HashSet& hash_set);

		/** Assign this HashSet with the contents of another HashSet
				@param rhs the HashSet to assign from
		*/
		const HashSet& operator = (const HashSet& rhs);

		/** Assign another HashSet with the contents of this HashSet
				@param hash_set the HashSet to assign to
		*/
		void get(HashSet& hash_set) const;

		/**	Swap the contents of two hash sets.
		*/
		void swap(HashSet& hash_set);

		//@}

		/**	@name	Accessors
		*/
		//@{

		/**	Return the number of buckets.
		*/
		Size getBucketSize() const;

		/** Return the capcacity of the hash set.
		*/
		Size getCapacity() const;

		/**	Return the number of elements in the hash set.
		*/
		Size getSize() const;
			
		/**	Return the number of elements in the hash set.
		*/
		Size size() const;

    /** Find the element whose key is <tt>key</tt>.
    */
 		Iterator find(const Key& key);
	
    /** Find the element whose key is <tt>key</tt>.
    */
		ConstIterator find(const Key& key) const;

		/**	Insert a new entry into the hash set.
		*/
		std::pair<Iterator, bool> insert(const ValueType& item);

		/**	Insert a new entry into the hash set.
				For STL compatibility. The value of <tt>pos</tt> is ignored.
		*/
		Iterator insert(Iterator pos, const ValueType& item);

		/**	Erase element with key <tt>key</tt>.
				@return Size the number of elements erased (0 or 1)
		*/
		Size erase(const KeyType& key);

		/**	Erase element at a given position.
		 *	@param pos an iterator pointing to the element to delete
		 *  @throw Exception::IncompatibleIterators does not point into this HashSet
		 *  @throw Exception::InvalidIterator if pos is invalid
		 */
		void erase(Iterator pos);

		/**	Erase a range of elements.
		 *	Erase all elements in the range <tt>f - l</tt>.
		 *  @throw Exception::IncompatibleIterators if one of the arguments does not point into this HashSet
		 */
		void erase(Iterator f, Iterator l);

		//@}

		/**	@name	Operators
		*/
		//@{
		/**	Intersection operator.
				Replace the contents of the current hash set by
				its intersection with <tt>rhs</tt>.
		*/
		const HashSet& operator &= (const HashSet& rhs);
		
		/**	Union operator.
				Replace the contents of the current hash set by
				its union with <tt>rhs</tt>.
		*/
		const HashSet& operator |= (const HashSet& rhs);
		
		/**	Intersection operator.
				Compute the intersection of the two hash sets.
				The left-hand set is not modified.
		*/
		HashSet operator & (const HashSet& rhs) const;
		
		/**	Union operator.
				Compute the union of the two hash sets.
				The left-hand set is not modified.
		*/
		HashSet operator | (const HashSet& rhs) const;

		/**	Union operator.
				@see operator|
		*/
		HashSet operator + (const HashSet& rhs) const;

		/**	Difference operator.
				Computes the difference of the two sets, i.e. constructs a
				set containing the the elements of <tt>this</tt> set that are not
				contained in <tt>rhs</tt>.
		*/
		HashSet operator - (const HashSet& rhs) const;

		/**	Union operator.
				@see operator|=
		*/
		const HashSet& operator += (const HashSet& rhs);

		/**	Difference operator.
				Remove all elements contained in <tt>rhs</tt> from the set.
		*/
		const HashSet& operator -= (const HashSet& rhs);
		//@}

		/**	@name Miscellaneous
		*/
		//@{

		/**	Host a visitor for all set entries.
		*/
		virtual void host(Visitor<HashSet<Key> >& visitor);
		//@}

		/**	@name	Predicates
		*/
		//@{

		/**	Test whether the set contains the key <tt>key</tt>.
		*/
		bool has(const Key& key) const;

		/**	Test whether the set is empty.
		*/
		bool isEmpty() const;

		/**	Compare two hash sets.
		*/
		bool operator == (const HashSet& hash_set) const;

		/**	Compare two hash sets.
		*/
		bool operator != (const HashSet& hash_set) const;
		//@}

		/**	@name	Debugging and Diagnostics
		*/
		//@{

		/**	Return true if the hash set is consistent.
				Condition: the number of entries in all buckets has to be equal the 
				stored number of entries (getSize()).
		*/
		bool isValid() const;

		/** Dump the constent of this instance to an ostream.
		*/
		virtual void dump(std::ostream& s = std::cout, Size depth = 0) const;

		//@}

		// --- INTERNAL ITERATORS

		/**	@name Iteration
		*/
		//@{
		/** Apply a processor to all keys in this instance.
				@return true if the processor could be applied.
		*/
		bool apply(UnaryProcessor<ValueType>& processor);
		//@}



		/**
		*/
		Iterator begin()
		{
			return Iterator::begin(*this);
		}

		/**
		*/
		Iterator end()
		{
			return Iterator::end(*this);
		}

		/**
		*/
		ConstIterator begin() const
		{
			return ConstIterator::begin(*this);
		}

		/**
		*/
		ConstIterator end() const
		{
			return ConstIterator::end(*this);
		}


		protected:

		virtual Node* newNode_(const ValueType& value, Node* next) const;

		virtual void deleteNode_(Node* node) const;
	
		virtual HashIndex hash(const Key& key) const;

		virtual bool needRehashing_() const;

		virtual void rehash();

		private:

		void deleteBuckets_();

		Position hashBucket_(const Key& key) const;

		void rehash_();

		// --- ATTRIBUTES

		/*_	The number of elements in the hash set
		*/
		Size	size_;

		/*_	The capacity - usually the number of buckets
		*/
		Size	capacity_;

		/*_	Buckets are stored as a vector of linked lists of Nodes 
		*/
		vector<Node*>	bucket_;
	};


	template <class Key>
	HashSet<Key>::HashSet(Size initial_capacity, Size number_of_buckets)
		:	size_(0),
			capacity_(initial_capacity),
			bucket_(number_of_buckets)
	{
		for (Position bucket = 0; bucket < (Position)bucket_.size(); ++bucket)
		{
			bucket_[bucket] = 0;
		}
	}

	template <class Key>
	HashSet<Key>::HashSet(const HashSet& hash_set)
		:	size_(hash_set.size_),
			capacity_(hash_set.capacity_),
			bucket_(hash_set.bucket_.size())
	{
		Node* item = 0;
		
		for (Position bucket = 0; bucket < (Position)bucket_.size(); ++bucket)
		{
			bucket_[bucket] = 0;

			for (item = hash_set.bucket_[bucket]; item != 0; item = item->next)
			{
				bucket_[bucket]	= newNode_(item->value, bucket_[bucket]);
			}
		}
	}

	template <class Key>
	void HashSet<Key>::set(const HashSet& hash_set)
	{
		if (&hash_set == this)
		{
			return;
		}

		destroy();
		
		deleteBuckets_();

		size_ = hash_set.size_;
		capacity_ = hash_set.capacity_;
		bucket_.resize(hash_set.bucket_.size());

		Node* item = 0;
		
		for (Position bucket = 0; bucket < (Position)bucket_.size(); ++bucket)
		{
			bucket_[bucket] = 0;

			for (item = hash_set.bucket_[bucket]; item != 0; item = item->next)
			{
				bucket_[bucket]	= newNode_(item->value, bucket_[bucket]);
			}
		}
	}

	template <class Key>
	void HashSet<Key>::clear()
	{
		Node* node = 0;
		Node* next_node = 0;
		
		for (Position bucket = 0; bucket < (Position)bucket_.size(); ++bucket)
		{
			for (node = bucket_[bucket]; node != 0; node = next_node)
			{
				next_node = node->next;
				deleteNode_(node);
			}
			
			bucket_[bucket] = 0;
		}

		size_ = 0;
	}

	template <class Key>
	BALL_INLINE 
	void HashSet<Key>::destroy()
	{
		clear();
	}

	template <class Key>
	BALL_INLINE 
	const HashSet<Key>& HashSet<Key>::operator = (const HashSet& hash_set)
	{
		set(hash_set);
		return *this;
	}

	template <class Key>
	BALL_INLINE 
	void HashSet<Key>::get(HashSet& hash_set) const

	{
		hash_set.set(*this);
	}

	template <class Key>
	BALL_INLINE 
	void HashSet<Key>::swap(HashSet& hash_set)
	{
		std::swap(size_, hash_set.size_);
		std::swap(capacity_, hash_set.capacity_);
		std::swap(bucket_, hash_set.bucket_);
	}

	template <class Key>
	BALL_INLINE 
	const HashSet<Key>& HashSet<Key>::operator &= (const HashSet& rhs)
	{
		// Store all elements that are not part of the intersection
		// in a list for subsequent deletion.
		std::list<Key> erase_list;
		for (Iterator it = begin(); it != end(); ++it)
		{
			if (!rhs.has(*it))
			{
				erase_list.push_back(*it);
			}
		}

		// erase all elements not part of the intersection
		typename list<Key>::iterator list_it = erase_list.begin();
		for (; list_it != erase_list.end(); ++list_it)
		{
			erase(*list_it);
		}

		return *this;
	}

	template <class Key>
	BALL_INLINE 
	const HashSet<Key>& HashSet<Key>::operator |= (const HashSet<Key>& rhs)
	{
		// Compute the union of both sets by inserting every element of the
		// rhs set.
		for (ConstIterator it = rhs.begin(); it != rhs.end(); ++it)
		{
			insert(*it);
		}

		return *this;
	}

	template <class Key>
	BALL_INLINE 
	const HashSet<Key>& HashSet<Key>::operator += (const HashSet<Key>& rhs)
	{
		return operator |= (rhs);
	}

	template <class Key>
	BALL_INLINE 
	HashSet<Key> HashSet<Key>::operator & (const HashSet<Key>& rhs) const
	{
		// Create an empty hash set...
		HashSet<Key> tmp;
		ConstIterator it = begin();
		
		// ...and copy all the elements contained in the rhs hash set.
		for (; +it; ++it)
		{
			if (rhs.has(*it))
			{
				tmp.insert(*it);
			}
		}

		return tmp;
	}

	template <class Key>
	BALL_INLINE 
	HashSet<Key> HashSet<Key>::operator - (const HashSet<Key>& rhs) const
	{
		// Create an empty hash set...
		HashSet<Key> tmp;
		ConstIterator it = begin();
		
		// ...and copy all the elements contained in this set and not in the rhs hash set.
		for (; +it; ++it)
		{
			if (!rhs.has(*it))
			{
				tmp.insert(*it);
			}
		}

		return tmp;
	}

	template <class Key>
	BALL_INLINE 
	const HashSet<Key>& HashSet<Key>::operator -= (const HashSet<Key>& hash_set) 
	{
		// avoid memory corruption caused by iterating over freed space when
		// deleting myself
		if (this == &hash_set)
		{
			clear();
		}
		else
		{
			// erase all elements which are contained in this and hash_set 
			typename HashSet<Key>::ConstIterator it = hash_set.begin();
			for (; it != hash_set.end(); ++it)
			{
				if (has(*it))
				{
					erase(*it);
				}
			}
		}
		return *this;
	}

	template <class Key>
	BALL_INLINE 
	HashSet<Key> HashSet<Key>::operator | (const HashSet<Key>& rhs) const
	{
		HashSet<Key> tmp(*this);
		tmp |= rhs;

		return tmp;
	}

	template <class Key>
	BALL_INLINE 
	HashSet<Key> HashSet<Key>::operator + (const HashSet<Key>& rhs) const
	{
		return operator | (rhs);
	}

	template <class Key>
	BALL_INLINE 
	Size HashSet<Key>::getBucketSize() const
	{
		return (Size)bucket_.size();
	}

	template <class Key>
	BALL_INLINE 
	Size HashSet<Key>::getCapacity() const
	{
		return capacity_;
	}

	template <class Key>
	BALL_INLINE 
	Size HashSet<Key>::getSize() const
	{
		return size_;
	}

	template <class Key>
	BALL_INLINE 
	Size HashSet<Key>::size() const
	{
		return size_;
	}

	template <class Key>
	typename HashSet<Key>::Iterator HashSet<Key>::find(const Key& key)
	{
		Iterator it = end();
		Position bucket = hashBucket_(key);
		Node* node_ptr = bucket_[bucket];
		while (node_ptr != 0)
		{
			if (node_ptr->value == key)
			{
				it.getTraits().position_ = node_ptr;
				it.getTraits().bucket_ = bucket;
				break;
			}
			node_ptr = node_ptr->next;
		}

		return it;
	}
		
	template <class Key>
	BALL_INLINE 
	typename HashSet<Key>::ConstIterator HashSet<Key>::find(const Key& key) const
	{
		return (const_cast<HashSet*>(this))->find(key);
	}

	template <class Key>
	std::pair<typename HashSet<Key>::Iterator, bool> HashSet<Key>::insert
		(const ValueType& item)
	{
		Iterator it = find(item);
		if (it == end())
		{
			if (needRehashing_() == true)
			{
				rehash_();
			}
			
			Position bucket = hashBucket_(item);

			Node* next = bucket_[bucket];
			bucket_[bucket] = newNode_(item, next);
			
			++size_;
			it.getTraits().position_ = bucket_[bucket];
			it.getTraits().bucket_ = bucket;
		}

		return std::pair<Iterator, bool>(it, true);
	}

	template <class Key>
	typename HashSet<Key>::Iterator HashSet<Key>::insert
		(typename HashSet<Key>::Iterator /* pos */, const ValueType& item)
	{
		return insert(item).first;
	}

	template <class Key>
	Size HashSet<Key>::erase(const KeyType& key)
	{
		Position	bucket = hashBucket_(key);
		Node*			previous = 0;
		Node*			node_ptr = bucket_[bucket];
		
		while (node_ptr != 0 && node_ptr->value != key)
		{
			previous = node_ptr;
			node_ptr = node_ptr->next;
		}

		if (node_ptr == 0)
		{
			return false;
		}

		if (node_ptr == bucket_[bucket])
		{
			bucket_[bucket] = node_ptr->next;
		} 
		else 
		{
			previous->next = node_ptr->next;
		}

		deleteNode_(node_ptr);
		--size_;

		return 1;
	}

	template <class Key>
	void HashSet<Key>::erase(Iterator pos)
	{
		if (pos.getTraits().bound_ != this)
		{
			throw Exception::IncompatibleIterators(__FILE__, __LINE__);
		}

		if ((pos == end()) || (size_ == 0))
		{
			return;
		}
				
		if (pos.getTraits().position_ == bucket_[pos.getTraits().bucket_])
		{
			bucket_[pos.getTraits().bucket_] = pos.getTraits().position_->next;
		} 
		else 
		{
			// walk over all nodes in this bucket and identify the predecessor
			// of the node refered to by the iterator pos
			Node* prev = bucket_[pos.getTraits().bucket_];
			for (; (prev != 0) && (prev->next != pos.getTraits().position_); prev = prev->next) {};
			if (prev != 0)
			{
				// remove the node and reconnect the list
				prev->next = pos.getTraits().position_->next;
			}
			else 
			{
				throw Exception::InvalidIterator(__FILE__, __LINE__);
			}
		}

		// delete the node and decrement the set size
		deleteNode_(pos.getTraits().position_);
		--size_;
	}

	template <class Key>
	void HashSet<Key>::erase(Iterator f, Iterator l)
	{
		if (f.getTraits().bound_ != this || l.getTraits().bound_ != this)
		{
			throw Exception::IncompatibleIterators(__FILE__, __LINE__);
		}
		
		if (f == end())
		{
			return;
		}

		Position last_bucket = l.getTraits().bucket_;
		if (l == end())
		{
			last_bucket = (Position)(bucket_.size() - 1);
		}

		if (f.getTraits().bucket_ > last_bucket)
		{
			// empty range - l < f
			return;
		}

		// count the deleted entries to correct the set size
		Size no_deletions = 0;

		Position bucket = f.getTraits().bucket_;
		for (; bucket <= last_bucket; bucket++)
		{
			if (bucket_[bucket] == 0)
			{
				// skip all empty buckets
				continue;
			}


			if ((bucket == f.getTraits().bucket_) && (bucket_[bucket] != f.getTraits().position_))
			{
				// find the predecessor of f
				Node* n = bucket_[bucket];
				Node* next;
				for (; (n->next != f.getTraits().position_) && (n->next != 0); n = n->next) {};
				
				if (bucket == last_bucket)
				{
					// delete everything from f to l in this bucket

					next = n->next;
					n->next = l.getTraits().position_;
					for (n = next; (n != 0) && (n != l.getTraits().position_); n = next)
					{
						next = n->next;
						deleteNode_(n);
						no_deletions++;
					}
				}
				else
				{
					// delete everything from f to the end in this bucket

					if (n != 0)
					{
						// mark the end of the list
						next = n->next;
						n->next = 0;

						// delete all remaining nodes
						for (n = next; n != 0; n = next)
						{
							next = n->next;
							deleteNode_(n);
							no_deletions++;
						}
					}
				}
			} 
			// if the current bucket lies between the first and the last bucket...
			else if (bucket < last_bucket)
			{
				// ...delete the whole bucket
				Node* next;
				for (Node* n = bucket_[bucket]; n != 0; n = next)
				{
					next = n->next;
					deleteNode_(n);
					no_deletions++;
				}
				bucket_[bucket] = 0;
			}
			else if (bucket == last_bucket)
			{
				// we delete everything in this bucket up to the iterator l

				// find the predecessor of l
				Node* n = bucket_[bucket];
				Node* next;
				for (; (n != 0) && (n != l.getTraits().position_); n = next)
				{
					next = n->next;
					deleteNode_(n);
					no_deletions++;
				}

				bucket_[bucket] = l.getTraits().position_;
			}
		}

		// correct the set size
		size_ -= no_deletions;
	}

	template <class Key>
	BALL_INLINE 
	void HashSet<Key>::host(Visitor<HashSet<Key> >& visitor)
	{
		visitor.visit(*this);
	}
		
	template <class Key>
	BALL_INLINE 
	bool HashSet<Key>::has(const Key& key) const	
	{
		return (find(key) != end());
	}

	template <class Key>
	BALL_INLINE 
	bool HashSet<Key>::isEmpty() const
	{
		return (size_ == 0);
	}

	template <class Key>
	bool HashSet<Key>::operator == (const HashSet& hash_set) const
	{
		if (size_ != hash_set.size_)
		{
			return false;
		}

		ConstIterator it1 = begin();
		ConstIterator it2 = hash_set.begin();
		while (it1 != end())
		{
			if (*it1 != *it2)
			{
				return false;
			}
			it1++;
			it2++;
		}
		return true;
	}

	template <class Key>
	BALL_INLINE
	bool HashSet<Key>::operator != (const HashSet& hash_set) const
	{
		return !(*this == hash_set);
	}

	template <class Key>
	bool HashSet<Key>::isValid() const
	{
		Size size = 0;
		Node* node = 0;

		for (Position bucket = 0; bucket < (Position)bucket_.size(); ++bucket)
		{
			for (node = bucket_[bucket]; node != 0; node = node->next)
			{
				++size;

				if (node->next == 0)
				{
					break;
				}
			}
		}

		return (size_ == size);
	}      

	template <class Key>
	void HashSet<Key>::dump(std::ostream& s, Size depth) const
	{
		BALL_DUMP_STREAM_PREFIX(s);

		BALL_DUMP_DEPTH(s, depth);

		BALL_DUMP_DEPTH(s, depth);
		s << "  size: " << getSize() << std::endl;

		BALL_DUMP_DEPTH(s, depth);
		s << "  # buckets: " << getBucketSize() << std::endl;

		BALL_DUMP_DEPTH(s, depth);
		s << "  capacity: " << getCapacity() << std::endl;

		BALL_DUMP_DEPTH(s, depth);
		s << "  load factor: " << (float)size_ / (float)bucket_.size() << std::endl;

		for (Position bucket = 0; bucket < (Position)bucket_.size(); ++bucket)
		{
			BALL_DUMP_DEPTH(s, depth);
			s << "    bucket " << bucket << ": ";
			for (Node* ptr = bucket_[bucket]; ptr != 0; ptr = ptr->next)
			{
				s << "(" << (void*)ptr << ") ";
			}
			s << "(0)" << std::endl;
		}

		BALL_DUMP_STREAM_SUFFIX(s);
	} 
 
	template <class Key>
	bool HashSet<Key>::apply(UnaryProcessor<ValueType>& processor)
	{
    if (processor.start() == false)
		{
			return false;
		}

    Processor::Result result;

		Iterator it = begin();
		while (it != end())
		{
			result = processor(*it);
			if (result <= Processor::BREAK)
			{
				return (result == Processor::BREAK);
			}
			it++;
		}

    return processor.finish();
	}

	template <class Key>
	BALL_INLINE 
	HashIndex HashSet<Key>::hash(const Key& key) const
	{
		return (HashIndex)Hash(key);
	}

	template <class Key>
	BALL_INLINE 
	void HashSet<Key>::rehash()
	{
		capacity_ = (Size)getNextPrime((Size)bucket_.size() << 1);
	}

  template <class Key>
  void HashSet<Key>::deleteBuckets_()
  {
    Size i = 0;
    Node* node = 0;
    Node* next_node = 0;
    for (i = 0; i < bucket_.size(); i++)
    {
      node = bucket_[i];
      while (node != 0)
      {
        next_node = node->next;
        delete node;
        node = next_node;
			}
      bucket_[i] = 0;
		}
	}

	template <class Key>
	BALL_INLINE 
	typename HashSet<Key>::Node* HashSet<Key>::newNode_
		(const ValueType& value, typename HashSet<Key>::Node* next) const
	{
		return new Node(value, next);
	}

	template <class Key>
	BALL_INLINE 
	void HashSet<Key>::deleteNode_(typename HashSet<Key>::Node* node) const
	{
		delete node;
	}

	template <class Key>
	BALL_INLINE 
	bool HashSet<Key>::needRehashing_() const
	{
		return (size_ >= capacity_);
	}

	template <class Key>
	BALL_INLINE 
	HashIndex HashSet<Key>::hashBucket_(const Key& key) const
	{
		return (Position)((HashIndex)hash(key) % (HashIndex)bucket_.size());
	}

	template <class Key>
	void HashSet<Key>::rehash_()
  {
    // calculate the new number of buckets (in capacity_)
    rehash();

    // save the old contents
    vector<Node*> old_buckets(bucket_);

    // resize the bucket vector and initialize it with zero
    bucket_.clear();
    bucket_.resize(capacity_);
    Position i;
    for (i = 0; i < capacity_; i++)
    {
      bucket_[i] = 0;
		}

    // rehash the old contents into the new buckets
    Node* node;
    Node* next_node;
    for (Position i = 0; i < (Position)old_buckets.size(); ++i)
    {
      for (node = old_buckets[i]; node != 0; node = next_node)
      {
        next_node = node->next;
        Position new_bucket = hashBucket_(node->value);
        node->next = bucket_[new_bucket];
        bucket_[new_bucket] = node;
			}
		}
	}
} // namespace BALL

#endif // BALL_DATATYPE_HASHSET_H
