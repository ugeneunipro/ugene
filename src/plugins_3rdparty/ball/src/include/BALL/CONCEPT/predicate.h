// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_CONCEPT_PREDICATE_H
#define BALL_CONCEPT_PREDICATE_H

#ifndef BALL_COMMON_GLOBAL_H
# include <BALL/COMMON/global.h>
#endif

#include <functional>

namespace BALL 
{
	// std::unary_function / std::binary_function were removed in C++17.
	// Provide equivalent typedefs so the predicates below stay usable with
	// the standard library, matching the old std::unary_function interface.
	template <typename Arg, typename Result>
	struct unary_function
	{
		typedef Arg argument_type;
		typedef Result result_type;
	};

	template <typename Arg1, typename Arg2, typename Result>
	struct binary_function
	{
		typedef Arg1 first_argument_type;
		typedef Arg2 second_argument_type;
		typedef Result result_type;
	};
		
	/**	@name	Predicates
			
			\ingroup ConceptsMiscellaneous
	*/
	//@{
	
	/**	Generic Unary Predicate Class
	*/
	template <typename T> 
	class UnaryPredicate 
		: public unary_function<T, bool> 
	{
		public:
		///
		virtual ~UnaryPredicate() {}

		///
		virtual bool operator() (const T& /* x */) const
			;
	};

	/**	Generic Binary Predicate Class
	*/
	template <typename T1, typename T2> 
	class BinaryPredicate 
		: public binary_function<T1, T2, bool> 
	{
		public:

		///
		virtual bool operator() (const T1& x, const T2& y) const
			;

		///
    virtual ~BinaryPredicate() {}
	};

	template <typename T> 
	bool UnaryPredicate<T>::operator() (const T& /* x */) const
		
	{
		return true;
	}

	template <typename T1, typename T2> 
	bool BinaryPredicate<T1, T2>::operator() (const T1&, const T2&) const
		
	{
		return true;
	}
	//@}
} // namespace BALL


#endif // BALL_CONCEPT_PREDICATE_H
