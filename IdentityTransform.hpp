#ifndef IDENTITYTRANSFORM_HPP
#define IDENTITYTRANSFORM_HPP 1
#pragma once

#include <typeinfo>
#include <iterator>
#include <functional>
#include <utility>
#include <boost/range.hpp>

#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>





/*
	For the Identity transform class, we don't care about the underlaying
	type at all. Realistically, we only need to care about the two types
	of iterators (or boost::range instances) so that the boost::copy function
	can be called on those containers.

	The question becomes


 */





namespace Waveform {

namespace Transform {



//template <typename RandomAccessRange1, typename RandomAccessRange2>
template <typename Iterator1, typename Iterator2 = Iterator1>
//template <typename Container1, typename Container2 = Container1>
class Identity {
  private:
  	
	/*
	typedef typename Container1::iterator Iterator1;
	typedef typename Container2::iterator Iterator2;
	*/

	/*
	typedef std::pair<Iterator1, Iterator1> RandomAccessRange1;
	typedef std::pair<Iterator2, Iterator2> RandomAccessRange2;
	 */

	typedef boost::iterator_range<Iterator1> RandomAccessRange1;
	typedef boost::iterator_range<Iterator2> RandomAccessRange2;
	 
  private:

  public:

	typedef typename std::iterator_traits<Iterator1>::value_type input_type;
	typedef typename std::iterator_traits<Iterator2>::value_type output_type;

	typedef std::function< output_type ( input_type ) >	transform_type;


	RandomAccessRange1	range1_;
	RandomAccessRange2	range2_;

	

  public:

	/*
	typedef typename RandomAccessRange1::iterator Iterator1;
	typedef typename RandomAccessRange2::iterator Iterator2;
	*/

	//template <typename Iterator1, typename Iterator2>
	Identity (Iterator1 first1, Iterator1 last1, Iterator2 first2)
		: range1_ (std::make_pair(first1, last1))
		, range2_ (std::make_pair(first2, first2 + boost::distance(range1_)))
	{

	}


	template <typename RandomAccessRangeIn1, typename RandomAccessRangeIn2>
	Identity (RandomAccessRangeIn1& range1, RandomAccessRangeIn2& range2)
		: range1_ (range1)
		, range2_ (range2)
		/*
		: range1_ (boost::make_iterator_range(boost::begin(range1), boost::end(range1)))
		, range2_ (boost::make_iterator_range(boost::begin(range2), boost::end(range2)))
		*/
		//: range1_ (boost::begin(range1), boost::end(range1))
		//, range2_ (boost::begin(range2), boost::end(range2))
	{

	}


	~Identity (void)
	{

	}

	void
	exec_transform(void)
	{
		boost::copy(range1_, boost::begin(range2_));
	}

	void
	exec_inverse_transform(void)
	{
		boost::copy(range2_, boost::begin(range1_));
	}

};



}	//	namespace Transform
}	//	namespace Waveform


#endif
