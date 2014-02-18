#ifndef FFTWTRANSFORMCLASS_HPP
#define FFTWTRANSFORMCLASS_HPP 1

#include <complex>
#include <fftw3.h>
#include <boost/range.hpp>


/*
	Note that fftw complex values require some special care if you wish
	to use C++ std::complex<>.

	The website:
	http://www.fftw.org/doc/Complex-numbers.html
	explains why the "complex" header must be #include'd before the fftw
	header, and also why reinterpret_cast<fftw_complex*>() must be called.


 */


class FftwTransform {
	private:

	fftw_plan forwardPlan;
	fftw_plan inversePlan;


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
	FftwTransform (Iterator1 first1, Iterator1 last1, Iterator2 first2)
		: forwardPlan( fftw_plan_dft_r2c_1d ( std::distance(first1, last1)
											, &(*first1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( std::distance(first1, last1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, &(*first1)
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )

	
	{
		//init_(first1, last1, first2);
	}


	//!	Boost::range constructor (Random Access Range)
	template <typename RandomAccessRange1, typename RandomAccessRange2>
	FftwTransform (RandomAccessRange1& range1, RandomAccessRange2& range2)
		: forwardPlan( fftw_plan_dft_r2c_1d ( boost::distance(range1)
											, &(*boost::begin(range1))
											, reinterpret_cast<fftw_complex*>(&(*boost::begin(range2)))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( boost::distance(range1)
											, reinterpret_cast<fftw_complex*>(&(*boost::begin(range2)))
											, &(*boost::begin(range1))
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )
	
	{
		//init_(boost::begin(range1), boost::end(range1), boost::begin(range2));
	}



	~FftwTransform (void)
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



#endif
