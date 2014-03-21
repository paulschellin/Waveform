/*
 Waveform.hpp
 Waveform class provides functions frequently used in the RF simulations.
 Written by Paul Schellin at the Ohio State University, 2012-2013
 
 To Do:
		Fixes:
	 (none?)
 
		Readability:
 [x] Finish Doxygen comments.
 [ ] Remove unnecessary commented code.
		
		Safety:
 [+] Unit testing.
 [-] Implement better range checking. (how will the monitoring iterators impact this?)
 [-] Use exceptions and make sure memory checks are made.
 
 
 [x] Make a better copy constructor(s), namely a (deep) copy constructor.
 
 
 [x] Convert class to a template of either:
		  - A container template
		  - Two container (complete) types
		Default would be DiscreteSeries, but should be designed to accept any
		STL-container complient class/template. This may require DiscreteSeries
		to be modified to be complient itself.
 
 [x] Consider removing "samplingRateHz_", since it's included in DiscreteSeries
		and may or may not be included in the container(s) passed as template
		parameters.
 
 [x] Consider removing "numTimeBins_" as its functionality is duplicated in
		FftwDft1d in public member FftwDft1d::numElem.
		(I know it seems silly to care about small objects and PODs like this,
		but duplication means that there are multiple initializations,
		modification functions, and syncing to keep track of, for no gain)
 
 [ ] Define iterators of various sorts to manage and check for domain validity.
 
 [ ] Use some kind of smart pointer to encapsulate the validation checks, so
		that the code never needs to be duplicated, except for iterators.
 
 [x] Consider removing the state where validDomain_ == NeitherDomain. When the
		Waveform is in this state, many methods with throw an exception. I think
		this is improper form, and that instead of throwing an exception when
		an improper Waveform is used, the exception should be thrown when the
		Waveform was "made improper". Conceptualizing the possible states:
		  - After default construction: either can be modified since one does
				depend on the values of the other 
 
 [-] Consider changing "transform_" to be a shared_ptr. "reset()" can be called on
		the shared_ptr to allow the assignment operator (as well as domain
		container Setter functions) to modify a Waveform object's size, etc.
		instead of needing to start with a newly constructed Waveform object.
		Because any stl-complient container implementing "resize(...)" can
		intelligently handle changes in capacity, reducing the cost of
		shrinking or expanding according to a change in sample length/depth.
 
 [x] Reduce the number of constructor overloads. Currently there are 7:
		  - default constructor (allocating)
		  - size-based constructor (allocating)
		  - copy constructor (allocating && copying)
		  - freq DiscreteSeries copy constructor (allocating && copying)
		  - time DiscreteSeries copy constructor (allocating && copying)
		  - freq std::vector copy constructor (allocating && copying)
		  - time std::vector copy constructor (allocating && copying)
		The last four should be reduced to two constructors, each accepting one
		of the Waveform's template parameters (container types in this case).
		This means that the conversion to a container class must take place
		outside of the Waveform class.
		If possible to do without a runtime overhead, reducing that further to
		only one would be ideal.
 
 [-] Find out if shared_ptr<> is perhaps not the best to use, rather
		shared_array<> or something similar.
 
 
 [x] To keep the header file small, it may make sense to move the definition and
		development of the monitoring/proxy/observer smart pointer template to
		its own header file, perhaps along with the iterators.

 [-] Perhaps transition from the ...USE_SHARED_PTR macro and check if __cplusplus
 		is greater than 199711L?

 [ ] Determine if it is better to use a "flat" template parameter for the container
 		types or if nesting (i.e. template < template <typename...> class ContainerT, ...).
		It maybe be wise to use static asserts as well.

 [ ] Determine whether it is better to have so many iterator accessors brought to the surface
 		of the Waveform interface or if they should be left to the contained container
		classes.

 [ ] Move this to-do list to another file

 */

#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP 1
#pragma once


// Standard libraries
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <fstream>
#include <vector>
#include <string>
#include <complex>
#include <cmath>
#include <iomanip>
#include <sstream>


// Boost header files

#include <boost/operators.hpp>

#include <boost/range.hpp>


#include <boost/assert.hpp>
//#include <boost/accumulators/numeric/functional/vector.hpp>
//using namespace boost::numeric::operators;


#define WAVEFORM_USE_CBEGIN_CEND 1

/*!
 *	\addtogroup PS
 *	@{
 */

//! Namespace PS is the namespace used for classes, etc. created by the author (Paul Schellin)
namespace PS {
	using std::complex;
	using std::size_t;
	
	
	using boost::begin;		//	The begin() free function (not member function)
	using boost::end;		//	The end() free function (not member function)

	
	
	
	//!	PlaceholderTransformClass: Defines the minimum interface for a transform class.
	/*!
	 *	This class outlines the interface needed by some transform class.
	 *
	 *	Transforms could be (Fast) Fourier Transform, Laplace Transform, Wavelet
	 *	Transform, Hilbert, etc.
	 *
	 *	This dummy class exists so that FFTW and other libraries aren't necessary
	 *	to implement and test the Waveform class in the user's project.
	 *	
	 *	!!!!!	Important	!!!!!
	 *	The transform functions provided must satisfy criteria to work properly
	 *	with the Waveform class:
	 *
	 *		Unitary:	The functions must be unitary, that is, one function is the
	 *					inverse of the other (specifically, <em>not the scaled inverse</em>.
	 */
	class PlaceholderTransformClass {
	public:

		//!	Size and Domain "begin()" constructor
		/*	Deprecated due to lack of cohesion with STL idioms
		template <typename Iterator1, typename Iterator2>
		PlaceholderTransformClass (const unsigned size, Iterator1 first1, Iterator2 first2)
		{

		}
		*/
	
		//!	Iterator bounds constructor
		template <typename Iterator1, typename Iterator2>
		PlaceholderTransformClass (Iterator1 first1, Iterator1 last1, Iterator2 first2)
		{

		}

		//!	Boost::range constructor (Random Access Range)
		template <typename RandomAccessRange1, typename RandomAccessRange2>
		PlaceholderTransformClass (RandomAccessRange1& range1, RandomAccessRange2& range2)
		{

		}

		//!	Destructor
		~PlaceholderTransformClass (void)
		{

		}


		//!	Execute the forward transform
		void
		exec_transform (void)
		{

		}


		//!	Execute the inverse/reverse transform
		void
		exec_inverse_transform (void)
		{

		}
	};
	
	
	
	
	//!	Waveform class: Transform as-needed between time and freq domains.
	/*!
	 *	The Waveform class allows automatic transform (via FFTW) between time
	 *	and frequency domains.
	 *
	 *	Upon instantiation, one domain is made "active", and the other domain
	 *	will not be calculated until requested.
	 *
	 *	When one domain is modified, the other domain will be invalidated and
	 *	must be recalculated upon next request.
	 *
	 *	After a transform is completed, both domains will be valid until one is
	 *	modified.
	 */
	template< /*template<typename...> class*/ typename TimeContainer
			, /*template<typename...> class*/ typename FreqContainer = TimeContainer
			, typename TransformT = PlaceholderTransformClass
			>
	class Waveform
//		: boost::arithmetic1< Waveform<TimeContainer,FreqContainer,TransformT> >
	{
		
	public:

		//!	The type of the time domain
		typedef typename TimeContainer::value_type	TimeT;
		
		//!	The type of the frequency domain
		typedef typename FreqContainer::value_type	FreqT;
		
		//!	The "type" of the Waveform, inspired by std::map
		typedef typename std::pair<TimeT,FreqT> value_type;
		
		//!	The allocator type of the time domain container
		typedef typename TimeContainer::allocator_type TimeAllocT;
		
		//!	The allocator type of the frequency domain container
		typedef typename FreqContainer::allocator_type	FreqAllocT;

		//!	The iterator type of the time domain container
		typedef typename TimeContainer::iterator	TimeIterator;
		
		//!	The iterator type of the frequency domain container
		typedef typename FreqContainer::iterator	FreqIterator;
		

		//!	The const iterator type of the time domain container
		typedef typename TimeContainer::const_iterator	TimeConstIterator;
		
		//!	The const iterator type of the frequency domain container
		typedef typename FreqContainer::const_iterator	FreqConstIterator;
		
		
		
	private:

		//!
		/*!
		 *
		 *
		 *
		 */
		enum DomainSpecifier {TimeDomain, FreqDomain, EitherDomain};
		
		//!	Indicates the valid domain array(s)
		DomainSpecifier				validDomain_;

		//!	Container object for the time series array
		TimeContainer	timeSeries_;

		//!	Container object for the frequency spectrum array
		FreqContainer	freqSpectrum_;

		//!	Transform class object which wraps the forward and inverse transform functions
		TransformT		transform_;

		
	private: 
		//!	Default constructor
		/*! 
		 *	Because this initializes Waveform::transform_, which cannot be modified
		 *	post-initialization as currently written, it might make sense to
		 *	have the default constructor be private, and simply not used.
		 * Alternately, transform_ could be made into a shared_ptr so it is able
		 *	to be deleted/reset and re-new'd (or made via factory make_shared())
		 *	more safely. That would make Waveform Assignable with much less
		 * manual dynamic memory handling.
		 */
		Waveform(void)
			: validDomain_(EitherDomain)
		{ }
		
		
	public:
		
		//! Fill constructor
		Waveform(const size_t count)
			: validDomain_(EitherDomain)
			, timeSeries_(count)
			, freqSpectrum_(count)
			, transform_(timeSeries_, freqSpectrum_)
		{ }


		//! Copy constructor
		explicit Waveform(const Waveform& toCopy)
			: validDomain_(toCopy.validDomain_)
			, timeSeries_(toCopy.timeSeries_)
			, freqSpectrum_(toCopy.freqSpectrum_)
			, transform_(timeSeries_, freqSpectrum_)
		{ }
		
		
		//! Time domain copy constructor
		explicit Waveform(const TimeContainer& toCopy)
			: validDomain_(TimeDomain)
			, timeSeries_(toCopy)
			, freqSpectrum_(timeSeries_.size()/2 + 1)	// This might turn out to be wrong for non-even lengths, right?
			, transform_(timeSeries_, freqSpectrum_)
		{ }
		
		
		//! Frequency domain copy constructor
		explicit Waveform(const FreqContainer& toCopy)
			: validDomain_(FreqDomain)
			, timeSeries_((toCopy.size() - 1) * 2)
			, freqSpectrum_(toCopy)
			, transform_(timeSeries_, freqSpectrum_)
		{ }
			
		
		//!	Default destructor
		~Waveform (void) {}
		
		
		//!	Returns the size of the time domain container
		/*!
		 *	<This function seems redundant and will likely be
		 *	removed. Use Waveform::size() instead, since it's
		 *	STL-compliant (though this class is difficult to
		 *	design 100% in line with the STL idioms)>
		 */
		size_t
		GetSize	(void) //const
		{ return GetConstTimeSeries().size(); }
	

		//!	Returns the size of the time domain container
		size_t
		size (void) //const
		{ return GetConstTimeSeries().size(); }
		

		//!	Returns constant reference to the time domain container
		/*!
		 *	Pass "EitherDomain" because domain will not be modified.
		 *
		 *	Unfortunately the const keyword cannot be used here,
		 *	because ValidateDomain(...) could modify the class.
		 *	
		 *	If enough thought went into it, there could be a way
		 *	to separate these cases and use overloading between the
		 *	non-const and const member methods.
		 */
		const TimeContainer&
		GetConstTimeSeries (void)
		{ ValidateDomain(EitherDomain); return timeSeries_; }
		

		//!	Returns constant reference to the frequency domain container
		/*!
		 *	Pass "EitherDomain" because domain will not be modified.
		 *	Unfortunately the const keyword cannot be used here,
		 *	because ValidateDomain(...) could modify the class.
		 *	
		 *	If enough thought went into it, there could be a way
		 *	to separate these cases and use overloading between the
		 *	non-const and const member methods.
		 */
		const FreqContainer&
		GetConstFreqSpectrum (void)
		{ ValidateDomain(EitherDomain); return freqSpectrum_; }
		

		//!	Returns mutable reference to the time domain container
		/*!
		 *	If the time domain is not valid, the inverse/reverse transform
		 *	will automatically calculate the appropriate values behind the
		 *	scenes, given the valid frequency domain; returns when complete.
		 */
		TimeContainer&
		GetTimeSeries (void)
		{ ValidateDomain(TimeDomain); return timeSeries_; }
		

		//!	Returns mutable reference to the frequency domain container
		/*!
		 *	If the frequency domain is not valid, the forward transform
		 *	will automatically calculate the appropriate values behind the
		 *	scenes, given the valid time domain; returns when complete.
		 */
		FreqContainer&
		GetFreqSpectrum (void)
		{ ValidateDomain(FreqDomain); return freqSpectrum_; }
		
		


		//
		//
		//		Iterator Access Functions
		//
		//

		/*
			Because of the nature of the Waveform class, I'm most likely
			going to be removing the iterator functions from the library.

			The iterators will still be available from the containers
			themselves, of course, but because there is no obvious way
			to have just a "begin()" or "end()" function instead of
			"beginTime()" and "beginFreq()", and with there being no
			impact on performance either way, there is no good reason to
			keep the functions.

			Issues with not being able to mark "const" a function at the
			same time as checking the domain array validity (though there
			is a way to do it -- check in the function itself and throw an
			exception if the domain is not valid, but it's error prone and
			ugly) makes it difficult to consider this portion of the library
			intuitive or well-formed.

			The only reason iterators would be very useful is if they
			provided validation checks as well, but at the moment that
			portion of the library is not yet completed.
		 */


		//!	Returns read-only iterator to the first element in the time domain
		TimeConstIterator
		beginTime (void) const
		{ return TimeConstIterator (boost::begin(timeSeries_)); }
	

		//!	Returns read-only iterator to the first element in the freq domain
		FreqConstIterator
		beginFreq (void) const
		{ return FreqConstIterator (boost::begin(freqSpectrum_)); }
	

		//!	Returns read-only iterator to one past the last element
		TimeConstIterator
		endTime (void) const
		{ return TimeConstIterator (boost::end(timeSeries_)); }
	

		//!	Returns read-only iterator to one past the last element
		FreqConstIterator
		endFreq (void) const
		{ return FreqConstIterator (boost::end(freqSpectrum_)); }
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeIterator
		beginTime (void)
		{
			ValidateDomain(TimeDomain);
			return TimeIterator (boost::begin(timeSeries_));
		}
		

		//!	Returns read-only iterator to the first element in the freq domain
		FreqIterator
		beginFreq (void)
		{
			ValidateDomain(FreqDomain);
			return FreqIterator (boost::begin(freqSpectrum_));
		}
		

		//!	Returns read-only iterator to one past the last element
		TimeIterator
		endTime (void)
		{
			ValidateDomain(TimeDomain);
			return TimeIterator (boost::end(timeSeries_));
		}
		

		//!	Returns read-only iterator to one past the last element
		FreqIterator
		endFreq (void)
		{
			ValidateDomain(FreqDomain);
			return FreqIterator (boost::end(freqSpectrum_));
		}
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeConstIterator
		cbeginTime (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return TimeConstIterator (boost::begin(timeSeries_));
		#else
			return TimeConstIterator (timeSeries_.cbegin());
		#endif
		}
		
		//!	Returns read-only iterator to the first element in the freq domain
		FreqConstIterator
		cbeginFreq (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return FreqConstIterator (begin(freqSpectrum_));
		#else
			return FreqConstIterator (freqSpectrum_.cbegin());
		#endif
		}


		//!	Returns read-only iterator to one past the last element
		TimeConstIterator
		cendTime (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return TimeConstIterator (end(timeSeries_));
		#else
			return TimeConstIterator (timeSeries_.cend());
		#endif
		}


		//!	Returns read-only iterator to one past the last element
		FreqConstIterator
		cendFreq (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return FreqConstIterator (end(freqSpectrum_));
		#else
			return FreqConstIterator (freqSpectrum_.cend());
		#endif
		}
		

		//______________________________________________________________________
		//!	Validate and ensure that the specified domain is up-to-date
		/*!
		 *	This function is responsible for properly performing transforms
		 *	based on which domain(s) are valid as well as the domain which was
		 *	requested by the programmer.
		 *
		 *	As a reminder, DomainSpecifier is an enumeration which can be set
		 *	to one of 3 unique values: TimeDomain, FreqDomain, EitherDomain.
		 *
		 *	The variable validDomain_ maintains the current state of the system.
		 *	Each of the values of validDomain_ and their meaning are described
		 *	below:
		 *
		 *		TimeDomain		the time domain array timeSeries_ is valid,
		 *						while the contents of freqSpectrum_ should be
		 *						assumed to be out of date / invalid.
		 *
		 *		FreqDomain		the freq domain array freqSpectrum_ is valid,
		 *						while the contents of timeSeries_ should be
		 *						assumed to be out of date / invalid.
		 *
		 *		EitherDomain	this has to meanings, both of which ultimately
		 *						can be treated the same way:
		 *							1:	The last domain requested was done so
		 *								using one of the "Const" accessor
		 *								functions, so both of the domain arrays
		 *								may be read and modified from this
		 *								state.
		 *							2:	The Waveform was just constructed and
		 *								neither of the domain arrays contain
		 *								"useful" (useful to the user)
		 *								information. This means that either of
		 *								the domain arrays may be written to
		 *								without worrying about the validity of
		 *								the other domain. However, this also
		 *								means that neither of the domain arrays
		 *								can be read to provide valid data, but
		 *								since the user just initialized the
		 *								object they should be aware in any
		 *								circumstance that the arrays contain
		 *								only garbage.
		 *
		 *	The function parameter toValidate can take on those same three
		 *	values, this time with slightly different meanings:
		 *
		 *		TimeDomain		the time domain array was requested and
		 *						transforms should be performed as needed in
		 *						order to validate the domain.
		 *
		 *		FreqDomain		the freq domain array was requested and
		 *						transforms should be performed as needed in
		 *						order to validate the domain.
		 *
		 *		EitherDomain	the request for one of the domain arrays was
		 *						of the "Const" variety, so no matter which
		 *						domain the user wanted, both domain arrays
		 *						will be valid after the call. Using
		 *						EitherDomain like this allows the
		 *						implementation to be much simpler.
		 *
		 *	Because this is the core mechanism of the library, it's important
		 *	to note that the use of EitherDomain like this is fundamentally
		 *	reliant on the transform class used for the Waveform classes'
		 *	template parameter TransformT <em>providing two functions which
		 *	(along with the dataset) form a group (by the definition of group
		 *	theory)</em>. This means that the transform \f$\mathcal{F}\f$ and its
		 *	inverse \f$\mathcal{F^{-1}}\f$ must be unitary -- that is, they must
		 *	satisfy the composition criteria
		 *
		 *		\f$\mathcal{F^{-1}} \circ \mathcal{F} (x) = id (x)\f$ 
		 *	
		 *	(which is of course the definition of an inverse function!)
		 *	
		 *	This may seem obvious given the mathematical definition of, say,
		 *	the Fourier transform, but one of the most popular implementations,
		 *	FFTW3 (http://www.fftw.org), does not provide real-to-complex
		 *	forward- and inverse-transform functions which are true inverses of
		 *	the other -- the inverse transform of the "...dft_r2c_1d" transform
		 *	function is the "...dft_c2r_1d" function which results in a scaled
		 *	inverse. These transforms could be called "scaled unitary" or,
		 *	but must be rescaled in order to function properly in this library.
		 *
		 *	You can use transforms which are involutary functions (such as the
		 *	Laplace transform) by defining both "exec_transform()" and
		 *	"exec_inverse_transform()" (both are required functions of a
		 *	compatible TransformT class) to simply call the same function for
		 *	both.
		 */
		int
		ValidateDomain (const DomainSpecifier toValidate)
		{
			if (toValidate == validDomain_ || validDomain_ == EitherDomain) {
				//	There aren't any transforms to be performed
			}
			else if (toValidate == TimeDomain) {
				//if (validDomain_ == FreqDomain)
					transform_.exec_inverse_transform();
			}
			else if (toValidate == FreqDomain) {
				//if (validDomain_ == TimeDomain)
					transform_.exec_transform();
			}
			else if (toValidate == EitherDomain) {
				if (validDomain_ == TimeDomain) {
					transform_.exec_transform();
				} else if (validDomain_ == FreqDomain) {
					transform_.exec_inverse_transform();
				}
			}
			
			validDomain_ = toValidate;
			return 0;
		}
		


		//!	Swaps two values without throwing
		/*!
		 *	Because timeSeries_ and freqSpectrum_ are large
		 *	container objects, there's a good chance that
		 *	an overloaded swap function was written for them.
		 *
		 *	In the event that swap was not specialized for
		 *	one of the member object types, we bring 
		 *	std::swap into the scope so that way all swap
		 *	functions may be found using unqualified lookup.
		 *
		 *	This is necessary for the overloads to work.
		 *	The concept is coined "Argument-Dependant Lookup",
		 *	or ADL.
		 *
		 *	None of the swaps should throw, but the order in
		 *	which the member objects are swapped could still
		 *	matter.
		 *
		 *	It's also possible that the transform_ object
		 *	doesn't need to have any state of its own and
		 *	only wrap functions, but I'm not sure if there
		 *	is an easy way to attempt that.
		 */
		friend void	swap(Waveform& first, Waveform& second)
		{
			using std::swap;

			swap(first.validDomain_, second.validDomain_);

			swap(first.timeSeries_, second.timeSeries_);

			swap(first.freqSpectrum_, second.freqSpectrum_);

			swap(first.transform_, second.transform_);
		}


		//! Copy-assignment operator
		/*!
		 *	We need to make sure that copy assignment is without
		 *	errors, and we can do that using the copy-and-swap idiom.
		 *
		 *	The values of the lhs and rhs are swapped, not even
		 *	requiring a check for self-assignment.
		 *
		 *	Because the operator accepts rhs by value (and mutable),
		 *	once inside the function rhs is an rvalue, which means
		 *	that after the two are swapped, the swapped-away object
		 *	is a temporary rvalue at the end of the scope, destroyed
		 *	just as the previous value of rhs is returned as "*this",
		 *	so there is no opportunity for something to throw after
		 *	the old value was destroyed.
		 *
		 *	No need to worry about explicit calls to delete/ delete [],
		 *	and the compiler can basically optimize away the entire
		 *	transaction using Return Value Optimization (RVO)
		 */

		//template <typename WaveformT>
		Waveform&
		//operator= (const Waveform& rhs)
		//operator= (WaveformT rhs)
		operator= (Waveform rhs)
		{
			swap(*this, rhs);

			return *this;
		}


		//!	Move constructor (C++11)
		/*!
		 *	When rhs is just an rvalue, C++11 can make use of move semantics,
		 *	instead of copying values from memory to memory, the references
		 *	are swapped, in the exact same way they were for the copy assignment
		 *	operator.
		 *
		 *	This particular function also uses constructor delegation, a feature
		 *	which is not present for all compiler's implementations of C++11.
		 */
		Waveform(Waveform&& rhs)
			: Waveform()
		{
			swap(*this, rhs);
		}

		
		//
		//	Compound Assignment Functions
		//
		
		////////////////////////////////////////////////////////////////////////
		//					Arithmetic Operator Overloads
		////////////////////////////////////////////////////////////////////////
		//	Because Waveform requires two containers of floating-based types,
		//	we assume that the arithmetic operators work element-wise on these
		//	containers.
		//
		//	It would be ideal to be able to define the actual operators as a
		//	iterator-based implementation, such as:
		//
		//		operator+= (rhs)
		//		{
		//			std::transform(	  cbeginTime()
		//							, cendTime()
		//							, rhs.cbeginTime()
		//							, beginTime()
		//							, std::plus<TimeT, FreqT>()
		//						  );
		//			return *this;
		//		}
		//
		//	However, doing this would prevent specialization of the operators
		//	at a container level. For example, DiscreteSeries needs to be able
		//	to determine whether or not the time bins are the same when summing
		//	two timeSeries objects.
		//
		//	My solution to this is to require that any container (such as
		//	std::vector) also have the appropriate operators defined. Boost
		//	has definitions in boost/accumulators/numeric/functional/vector.hpp
		//	in the namespace boost::numeric::operators.
		////////////////////////////////////////////////////////////////////////
		

		//______________________________________________________________________
		//!	Add another Waveform
		/*!
		 *
		 */
		 /*
		Waveform&
		operator+= (Waveform& rhs)
		{
			//this->GetTimeSeries() += rhs.GetTimeSeries();
			
			//	Should assert that this->GetConstTimeSeries.size() == rhs.GetConstTimeSeries().size()
			
			BOOST_ASSERT(this->GetConstTimeSeries().size() == rhs.GetConstTimeSeries().size());
			for (std::size_t i = 0, max_size = this->GetConstTimeSeries().size(); i != max_size; ++i) {
				this->GetTimeSeries()[i] += rhs.GetConstTimeSeries()[i];
			}

			return *this;
		}
		*/
		

		//______________________________________________________________________
		//!	Subtract another Waveform
		/*!
		 *
		 */
		 /*
		Waveform&
		operator-= (Waveform& rhs)
		{
			//this->GetTimeSeries() -= rhs.GetConstTimeSeries();
			
			BOOST_ASSERT(this->GetConstTimeSeries().size() == rhs.GetConstTimeSeries().size());
			for (std::size_t i = 0, max_size = this->GetConstTimeSeries().size(); i != max_size; ++i) {
				this->GetTimeSeries()[i] -= rhs.GetConstTimeSeries()[i];
			}
			
			return *this;
		}
		*/
		

		//______________________________________________________________________
		//!	Multiply by another Waveform
		/*!
		 *
		 */
		/*
		Waveform&
		operator*= (Waveform& rhs)
		{
			//this->GetFreqSpectrum() *= rhs.GetConstFreqSpectrum();
			
			BOOST_ASSERT(this->GetConstTimeSeries().size() == rhs.GetConstTimeSeries().size());
			for (std::size_t i = 0, max_size = this->GetConstTimeSeries().size(); i != max_size; ++i) {
				this->GetTimeSeries()[i] *= rhs.GetConstTimeSeries()[i];
			}
			
			return *this;
		}
		*/
		

		//______________________________________________________________________
		//!	Divide by another Waveform
		/*!
		 *
		 */
		/*
		Waveform& operator/= (Waveform& rhs)
		{
			//this->GetFreqSpectrum() /= rhs.GetConstFreqSpectrum();

			BOOST_ASSERT(this->GetConstTimeSeries().size() == rhs.GetConstTimeSeries().size());
			for (std::size_t i = 0, max_size = this->GetConstTimeSeries().size(); i != max_size; ++i) {
				this->GetTimeSeries()[i] /= rhs.GetConstTimeSeries()[i];
			}


			return *this;
		}
		 */
		
	};
	
} // End of namespace PS

/*! @} End of Doxygen Groups*/

#endif
