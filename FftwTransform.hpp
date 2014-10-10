#ifndef FFTWTRANSFORM_HPP
#define FFTWTRANSFORM_HPP 1
#pragma once

#include <complex>
#include <fftw3.h>
#include <boost/range.hpp>

#include <TransformTypes.hpp>

//#define NORMALIZE_INVERSE 1

//#define FFTWTRANSFORM_USE_INIT 1

/*
	Things / Features to keep in mind:

	 [ ] Using "Wisdom"
	 [ ] Supporting multi-dimensional transforms
	 [ ] Supporting multi-threading
	 [ ] SIMD alignment (fftw_malloc and fftw_alignment_of)
	 [ ] Making transforms have string names (fftw_sprint_plan)
	 [ ] Split arrays of real and imaginary components

	 [ ] Offloading alignment and such to an allocator can make it
	 		so that a plan could operate on a difference set of data
			each time through the advanced interface, meaning that
			the input and output arrays could be moved / modified
			with a bit more effort put into the design of these classes.

 */


/*
	Note that fftw complex values require some special care if you wish
	to use C++ std::complex<>.

	The website:
	http://www.fftw.org/doc/Complex-numbers.html
	explains why the "complex" header must be #include'd before the fftw
	header, and also why reinterpret_cast<fftw_complex*>() must be called.


 */

/*

	Currently supported "Plans":

		[ Forward Plan Name ]		[ Inverse Plan Name ]	[ Input Domain ]	[ Output Domain ]
		fftw_plan_dft_r2c_1d		fftw_plan_dft_c2r_1d	Real 1D array		Complex 1D array


	Eventually supported "Plans":
		[ Forward Plan Name ]		[ Inverse Plan Name ]	[ Input Domain ]	[ Output Domain ]
		fftw_plan_dft_1d			fftw_plan_dft_1d		Complex 1D array	Complex 1D array
		fftw_plan_dft_2d			fftw_plan_dft_2d		Complex 2D array	Complex 2D array
		fftw_plan_dft_3d			fftw_plan_dft_3d		Complex 3D array	Complex 3D array
		fftw_plan_dft_r2c_2d		fftw_plan_dft_c2r_2d	Real 2D array		Complex 2D array
		fftw_plan_dft_r2c_3d		fftw_plan_dft_c2r_3d	Real 3D array		Complex 3D array
		fftw_plan_dft_r2c			fftw_plan_dft_c2r		Real N-D array		Complex N-D array
	
		fftw_plan_r2r_1d			fftw_plan_r2r_1d		Real 1D array		Real 1D array
		fftw_plan_r2r_2d			fftw_plan_r2r_2d		Real 2D array		Real 2D array
		fftw_plan_r2r_3d			fftw_plan_r2r_3d		Real 3D array		Real 3D array
		fftw_plan_r2r				fftw_plan_r2r			Real N-D array		Real N-D array

		Note that the r2r plans are actually unified interfaces to many different types of
		transforms which are passed to the plan via the "fftw_r2r_kind" typed parameter.

		More information here: http://www.fftw.org/doc/More-DFTs-of-Real-Data.html#More-DFTs-of-Real-Data

		Useful kinds include:
		[ fftw_r2r_kind	]
		[ 	  name		]	[ Description ]
		FFTW_R2HC			Real to "Halfcomplex" output -- Like dft_r2c but sometimes faster
		FFTW_HC2R			Halfcomplex to Real -- different output than dft_c2r
		FFTW_DHT			Discrete Hartley transform


		
		The r2r kinds for the various REDFT and RODFT types supported by FFTW, along with the boundary conditions at both ends of the input array (n real numbers in[j=0..n-1]), are: 
		
		FFTW_REDFT00		(DCT-I): even around j=0 and even around j=n-1.
		FFTW_REDFT10		(DCT-II, “the” DCT): even around j=-0.5 and even around j=n-0.5.
		FFTW_REDFT01		(DCT-III, “the” IDCT): even around j=0 and odd around j=n.
		FFTW_REDFT11		(DCT-IV): even around j=-0.5 and odd around j=n-0.5.
		FFTW_RODFT00		(DST-I): odd around j=-1 and odd around j=n.
		FFTW_RODFT10		(DST-II): odd around j=-0.5 and odd around j=n-0.5.
		FFTW_RODFT01		(DST-III): odd around j=-1 and even around j=n-1.
		FFTW_RODFT11		(DST-IV): odd around j=-0.5 and even around j=n-0.5.
 */


/*

	Normal Interface:

		...	// malloc arrays aIn, aOut

		fftw_plan myPlan = fftw_plan_dft_r2c_1d ( aSize
												, aIn
												, aOut
												, myFlags
												);

		fftw_execute (myPlan);

		...	// do something with plan

		fftw_destroy_plan (myPlan);


	Advanced Interface:

		void fftw_execute_dft(
          const fftw_plan p,
          fftw_complex *in, fftw_complex *out);
     
    	void fftw_execute_split_dft(
          const fftw_plan p,
          double *ri, double *ii, double *ro, double *io);
		
		void fftw_execute_dft_r2c(
          const fftw_plan p,
          double *in, fftw_complex *out);
     
    	void fftw_execute_split_dft_r2c(
          const fftw_plan p,
          double *in, double *ro, double *io);
     
   		void fftw_execute_dft_c2r(
          const fftw_plan p,
          fftw_complex *in, double *out);
     
    	void fftw_execute_split_dft_c2r(
          const fftw_plan p,
          double *ri, double *ii, double *out);
     
    	void fftw_execute_r2r(
          const fftw_plan p,
          double *in, double *out);

		From <http://www.fftw.org/doc/New_002darray-Execute-Functions.html>:
			"These execute the plan to compute the corresponding transform on the input/output arrays specified by the subsequent arguments. The input/output array arguments have the same meanings as the ones passed to the guru planner routines in the preceding sections. The plan is not modified, and these routines can be called as many times as desired, or intermixed with calls to the ordinary fftw_execute."


 */



namespace Waveform {

namespace Transform {

class Fftw3_Dft_1d {
  public:
  	typedef InverseTypes::ScaledInverse inverse_type;
	
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

	/*
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
	*/

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
		: forwardPlan( fftw_plan_dft_r2c_1d ( std::distance(first1, last1)
											, &(*first1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, FFTW_ESTIMATE) )
		, inversePlan( fftw_plan_dft_c2r_1d ( std::distance(first1, last1)
											, reinterpret_cast<fftw_complex*>(&(*first2))
											, &(*first1)
											, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT) )

	{ }


	//!	Boost::range constructor (Random Access Range)
	template <typename RandomAccessRange1, typename RandomAccessRange2>
	Fftw3_Dft_1d (RandomAccessRange1& range1, RandomAccessRange2& range2)
		: Fftw3_Dft_1d (boost::begin(range1), boost::end(range1), boost::begin(range2))
	{
	
	}


	Fftw3_Dft_1d (const Fftw3_Dft_1d& to_copy)
		: forwardPlan(to_copy.forwardPlan)
		, inversePlan(to_copy.inversePlan)
	{

	}

	/*!
	 *	The maybe-copy constructor
	 *
	 *	There are cases when the constructor is expensive but reliant on
	 *	different data ranges.
	 *
	 *	This copies if the range is fine, otherwise does not.
	 *
	 *
	template <typename RandomAccessRange1, typename RandomAccessRange2>
	Fftw3_Dft_1d (const Fftw3_Dft_1d& to_copy
					, RandomAccessRange1& range1
					, RandomAccessRange2& range2)
		: forwardPlan(to_copy.forwardPlan)
		, inversePlan(to_copy.inversePlan)
	{

	}
	*/


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
  public:
	typedef InverseTypes::Inverse inverse_type;

  private:

//	fftw_plan forwardPlan;
//	fftw_plan inversePlan;

	//std::complex<double>*	first_;
	fftw_complex*	first_;
	std::size_t		length_;
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


	//!	Boost::range constructor (Random Access Range)
	template <typename RandomAccessRange1, typename RandomAccessRange2>
	Fftw3_Dft_1d_Normalized (RandomAccessRange1& range1, RandomAccessRange2& range2)
		: Fftw3_Dft_1d_Normalized(boost::begin(range1), boost::end(range1), boost::begin(range2))
	{ }


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
