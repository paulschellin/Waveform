#ifndef FFTWTRANSFORM_HPP
#define FFTWTRANSFORM_HPP 1
#pragma once

#include <complex>
#include <fftw3.h>
#include <boost/range.hpp>


//#define NORMALIZE_INVERSE 1

#define FFTWTRANSFORM_USE_INIT 1

/*
 	Note:

	I may be moving this class and all of those like it to be namespaced
	into PS::Waveform.

	It makes sense to keep theses classes in the same namespace as Waveform,
	similar to how boost treats extensions to serialize, accumulators, units,
	etc.

 */


/*
	Note that fftw complex values require some special care if you wish
	to use C++ std::complex<>.

	The website:
	http://www.fftw.org/doc/Complex-numbers.html
	explains why the "complex" header must be #include'd before the fftw
	header, and also why reinterpret_cast<fftw_complex*>() must be called.


 */


namespace Waveform {

namespace Transform {

class Fftw3_Dft_1d {
	private:

	fftw_plan forwardPlan;
	fftw_plan inversePlan;

	/*
		This init_ function is supposed to replace the lengthy initialization lists
		in each constructor. Because the only two objects are fftw_plan objects,
		there is absolutely no cost to move construction out of the initializer
		lists.

		The primary benefit is that it'll make it easier to maintain once additional
		FFTW transforms are written, so less code duplication.

	 */

	template <typename Iterator1, typename Iterator2>
	void
	init_ (Iterator1 first1, Iterator1 last1, Iterator2 first2)
	{
		forwardPlan = fftw_plan_dft_r2c_1d (std::distance(first1, last1)
											, &(*first1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, FFTW_ESTIMATE);

		inversePlan = fftw_plan_dft_c2r_1d ( std::distance(first1, last1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, &(*first1)
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
	}


	public:


	// Thinking about this constructor a bit, because unsigned/size_t matches
	// Iterator in type, the keyword "explicit" must be used somewhere.
	// The C++ STL interface seems to follow the idiom of all iterators,
	// which makes sense because it allows for more general containers to be
	// used. This particular version more closely resembles the idioms of
	// the C programming language.
	/*
	template <typename Iterator1, typename Iterator2>
	FftwTransform (const unsigned size, Iterator1 first1, Iterator2 first2)
		: forwardPlan( fftw_plan_dft_r2c_1d ( size
											, first1
											, reinterpret_cast<fftw_complex*>(first2)
											, FFTW_ESTIMATE)
		, inversePlan( fftw_plan_dft_c2r_1d ( size
											, reinterpret_cast<fftw_complex*>(first1)
											, first2
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT)
	{

	}
	*/


	//!	Iterator bounds constructor
	template <typename Iterator1, typename Iterator2>
	Fftw3_Dft_1d (Iterator1 first1, Iterator1 last1, Iterator2 first2)
#ifndef FFTWTRANSFORM_USE_INIT
		: forwardPlan( fftw_plan_dft_r2c_1d ( std::distance(first1, last1)
											, &(*first1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( std::distance(first1, last1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, &(*first1)
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )

	{ }
#else
	{
		init_(first1, last1, first2);
	}
#endif


	//!	Boost::range constructor (Random Access Range)
	template <typename RandomAccessRange1, typename RandomAccessRange2>
	Fftw3_Dft_1d (RandomAccessRange1& range1, RandomAccessRange2& range2)
#ifndef FFTWTRANSFORM_USE_INIT
		: forwardPlan( fftw_plan_dft_r2c_1d ( boost::distance(range1)
											, &(*boost::begin(range1))
											, reinterpret_cast<fftw_complex*>(&(*boost::begin(range2)))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( boost::distance(range1)
											, reinterpret_cast<fftw_complex*>(&(*boost::begin(range2)))
											, &(*boost::begin(range1))
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )
	{ }
#else
	{
		init_(boost::begin(range1), boost::end(range1), boost::begin(range2));
	}
#endif


	~Fftw3_Dft_1d (void)
	{
		fftw_destroy_plan(forwardPlan);
		fftw_destroy_plan(inversePlan);
	}

	void
	exec_transform (void)
	{
		fftw_execute(forwardPlan);
	}

	void
	exec_inverse_transform (void)
	{
		fftw_execute(inversePlan);
	}
};


class Fftw3_Dft_1d_Normalized {
	private:

//	fftw_plan forwardPlan;
//	fftw_plan inversePlan;

	//std::complex<double>*	first_;
	fftw_complex*	first_;
	std::size_t				length_;
	fftw_complex*	last_;
	//std::complex<double>*	last_;


	fftw_plan 				forwardPlan;
	fftw_plan 				inversePlan;

	/*
		This init_ function is supposed to replace the lengthy initialization lists
		in each constructor. Because the only two objects are fftw_plan objects,
		there is absolutely no cost to move construction out of the initializer
		lists.

		The primary benefit is that it'll make it easier to maintain once additional
		FFTW transforms are written, so less code duplication.

	 */

	template <typename Iterator1, typename Iterator2>
	void
	init_ (Iterator1 first1, Iterator1 last1, Iterator2 first2)
	{
		first_ = &(*first2);
		length_ = std::distance(first1, last1);
		last_ = &(*(first2 + length_ / 2 + 1));
		
		forwardPlan = fftw_plan_dft_r2c_1d  ( length_
											, &(*first1)
											, first_
											//, reinterpret_cast<fftw_complex*>(&(*first2))
											, FFTW_ESTIMATE);

		inversePlan = fftw_plan_dft_c2r_1d  ( length_
											, first_
											//, reinterpret_cast<fftw_complex*>(&(*first2))
											, &(*first1)
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);


	}


	public:


	// Thinking about this constructor a bit, because unsigned/size_t matches
	// Iterator in type, the keyword "explicit" must be used somewhere.
	// The C++ STL interface seems to follow the idiom of all iterators,
	// which makes sense because it allows for more general containers to be
	// used. This particular version more closely resembles the idioms of
	// the C programming language.
	/*
	template <typename Iterator1, typename Iterator2>
	FftwTransform (const unsigned size, Iterator1 first1, Iterator2 first2)
		: forwardPlan( fftw_plan_dft_r2c_1d ( size
											, first1
											, reinterpret_cast<fftw_complex*>(first2)
											, FFTW_ESTIMATE)
		, inversePlan( fftw_plan_dft_c2r_1d ( size
											, reinterpret_cast<fftw_complex*>(first1)
											, first2
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT)
	{

	}
	*/


	//!	Iterator bounds constructor
	template <typename Iterator1, typename Iterator2>
	Fftw3_Dft_1d_Normalized (Iterator1 first1, Iterator1 last1, Iterator2 first2)
#ifndef FFTWTRANSFORM_USE_INIT
		: first_(first2)
		, length_(std::distance(first1, last1))
		, last_(first_ + length_ / 2 + 1)
		, forwardPlan( fftw_plan_dft_r2c_1d ( length_
											, &(*first1)
											, first_
											//, reinterpret_cast<fftw_complex*>(&(*first2))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( length_
											, first_
											//, reinterpret_cast<fftw_complex*>(&(*first2))
											, &(*first1)
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )

	{ }
#else
	{
		init_(first1, last1, first2);
	}
#endif


	//!	Boost::range constructor (Random Access Range)
	template <typename RandomAccessRange1, typename RandomAccessRange2>
	Fftw3_Dft_1d_Normalized (RandomAccessRange1& range1, RandomAccessRange2& range2)
#ifndef FFTWTRANSFORM_USE_INIT
		: first_(boost::begin(range2))
		, length_(boost::distance(range1))
		, last_(boost::end(range2))
		, forwardPlan( fftw_plan_dft_r2c_1d ( length_
											, &(*boost::begin(range1))
											, first_
											//, reinterpret_cast<fftw_complex*>(&(*boost::begin(range2)))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( length_
											, first_
											//, reinterpret_cast<fftw_complex*>(&(*boost::begin(range2)))
											, &(*boost::begin(range1))
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )
	{ }
#else
	{
		init_(boost::begin(range1), boost::end(range1), boost::begin(range2));
	}
#endif


	~Fftw3_Dft_1d_Normalized (void)
	{
		fftw_destroy_plan(forwardPlan);
		fftw_destroy_plan(inversePlan);
	}

	void
	exec_transform (void)
	{
		fftw_execute(forwardPlan);
	}

	void
	exec_inverse_transform (void)
	{
		fftw_execute(inversePlan);

		/*
		std::for_each(first_, last_, [=](std::complex<double>& x)
		{x /= length_;});
		*/

		for (auto iter (first_); iter != last_; ++iter) {
			(*iter)[0] /= double(length_);
			(*iter)[1] /= double(length_);
		}
	}
};


}	//	namespace Transform
}	//	namespace Waveform


#endif
