/*
 Waveform.hpp
 Waveform class provides functions frequently used in the RF simulations.
 Written by Paul Schellin at the Ohio State University, 2012-2013
 
 To Do:
		Fixes:
	 (none?)
 
		Readability:
 [ ] Finish Doxygen comments.
 [ ] Remove unnecessary commented code.
		
		Safety:
 [ ] Unit testing.
 [ ] Implement better range checking. (how will the monitoring iterators impact this?)
 [ ] Use exceptions and make sure memory checks are made.
 
 
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
 
 [ ] Consider removing the state where validDomain_ == NeitherDomain. When the
		Waveform is in this state, many methods with throw an exception. I think
		this is improper form, and that instead of throwing an exception when
		an improper Waveform is used, the exception should be thrown when the
		Waveform was "made improper". Conceptualizing the possible states:
		  - After default construction: either can be modified since one does
				depend on the values of the other 
 
 [x] Consider changing "transform_" to be a shared_ptr. "reset()" can be called on
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
 
 [ ] Find out if shared_ptr<> is perhaps not the best to use, rather
		shared_array<> or something similar.
 
 
 [x] To keep the header file small, it may make sense to move the definition and
		development of the monitoring/proxy/observer smart pointer template to
		its own header file, perhaps along with the iterators.

 [ ] Perhaps transition from the ...USE_SHARED_PTR macro and check if __cplusplus
 		is greater than 199711L?

 [ ] Determine if it is better to use a "flat" template parameter for the container
 		types or if nesting (i.e. template < template <typename...> class ContainerT, ...).
		It maybe be wise to use static asserts as well.

 [ ] Determine whether it is better to have so many iterator accessors brought to the surface
 		of the Waveform interface or if they should be left to the contained container
		classes.

 */

#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP 1

#define WAVEFORM_USE_BOOST_SHARED_PTR 1


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

#ifdef WAVEFORM_USE_STD_SHARED_PTR
#include <memory>
#endif


// Boost header files
//#include <boost/accumulators/numeric/functional/vector.hpp>
//	The above included file defines operators for std::vector<...>, including:
//	lhs / rhs	for both scalar and vector rhs
//	lhs * rhs	for both scalar and vector rhs
//

#ifdef WAVEFORM_USE_BOOST_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/mem_fn.hpp>			//	For wrapping/calling suffix functions
#endif


#include <boost/operators.hpp>


#include <boost/range.hpp>


/*!
 *	\addtogroup PS
 *	@{
 */

//! Namespace PS is the namespace used for classes, etc. created by the author (Paul Schellin)
namespace PS {
	using std::complex;
	using std::size_t;
	
	
#ifdef WAVEFORM_USE_STD_SHARED_PTR
	using std::shared_ptr;
	using std::make_shared;
#elif defined(WAVEFORM_USE_BOOST_SHARED_PTR)
	using boost::shared_ptr;
	using boost::make_shared;
#endif
	
	using boost::begin;
	using boost::end;

	
	//!	Operator+=: Performs element-wise sum over templated argument types.
	/*!
	 *	It's likely that this functionality is either defined elsewhere,
	 *	or that this operator is going to mask the operator which should
	 *	have been called for some types.
	 *
	 *	Consider removing.
	 *
	 */
	template<typename Left, typename Right>
	std::vector<Left>&
	operator +=(std::vector<Left>& left, std::vector<Right> const& right)
	{
		for(std::size_t i = 0, size = left.size(); i != size; ++i)
		{
			left[i] += right[i];
		}
		return left;
	}
	
	
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
	class Waveform : boost::arithmetic1< Waveform<TimeContainer,FreqContainer,TransformT> > {
		
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
		
		DomainSpecifier				validDomain_;

		TimeContainer	timeSeries_;
		FreqContainer	freqSpectrum_;

		TransformT		transform_;

		/*
		shared_ptr<TimeContainer>	timeSeries_;
		shared_ptr<FreqContainer>	freqSpectrum_;
		
		shared_ptr<TransformT>			transform_;
		*/
		
	
		
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
			//, timeSeries_(make_shared<TimeContainer>())				// default initialize
			//, freqSpectrum_(make_shared<FreqContainer>())			// default initialize
			//, transform_()
			//, transform_(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin())
		{ }
		
		
	public:
		//! Fill constructor
/*	Need to remove shared pointers
		Waveform(const size_t count)
			: validDomain_(EitherDomain)
			, timeSeries_(make_shared<TimeContainer>(count))						// range-based initialization
			, freqSpectrum_(make_shared<FreqContainer>(timeSeries_->size()/2 + 1))	// range-based initialization
			, transform_(make_shared<TransformT>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
*/
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
		
		/*
			, timeSeries_(make_shared<TimeContainer>(toCopy.timeSeries_))
			, freqSpectrum_(make_shared<FreqContainer>(toCopy.freqSpectrum_))
			, transform_(make_shared<TransformT>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		*/
		
		//! Time domain copy constructor
		explicit Waveform(const TimeContainer& toCopy)
			: validDomain_(TimeDomain)
			, timeSeries_(toCopy)
			, freqSpectrum_(timeSeries_.size()/2 + 1)	// This might turn out to be wrong for non-even lengths, right?
			, transform_(timeSeries_, freqSpectrum_)
		{ }
		
		
		/*
			, timeSeries_(make_shared<TimeContainer>(toCopy))							// copy
			, freqSpectrum_(make_shared<FreqContainer>(timeSeries_->size()/2 + 1))		// range-based initialization
			, transform_(make_shared<TransformT>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		*/
		
		//! Frequency domain copy constructor
		explicit Waveform(const FreqContainer& toCopy)
			: validDomain_(FreqDomain)
			, timeSeries_((toCopy.size() - 1) * 2)
			, freqSpectrum_(toCopy)
			, transform_(timeSeries_, freqSpectrum_)
		{ }
			
		/*	
			, timeSeries_(make_shared<TimeContainer>((toCopy.size()-1)*2))			// range-based initialization
			, freqSpectrum_(make_shared<FreqContainer>(toCopy))						// copy
			, transform_(make_shared<TransformT>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		 */
		
		//!	Default destructor
		~Waveform (void) {}
		
		//
		// Accessor Function Declarations
		//
		
		
		
		//!	Returns the size of the time domain container
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
		//{ ValidateDomain(EitherDomain); return *timeSeries_; }
		

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
		//{ ValidateDomain(EitherDomain); return *freqSpectrum_; }
		

		//!	Returns mutable reference to the time domain container
		/*!
		 *	If the time domain is not valid, the inverse/reverse transform
		 *	will automatically calculate the appropriate values behind the
		 *	scenes, given the valid frequency domain; returns when complete.
		 */
		TimeContainer&
		GetTimeSeries (void)
		{ ValidateDomain(TimeDomain); return timeSeries_; }
		//{ ValidateDomain(TimeDomain); return *timeSeries_; }
		

		//!	Returns mutable reference to the frequency domain container
		/*!
		 *	If the frequency domain is not valid, the forward transform
		 *	will automatically calculate the appropriate values behind the
		 *	scenes, given the valid time domain; returns when complete.
		 */
		FreqContainer&
		GetFreqSpectrum (void)
		{ ValidateDomain(FreqDomain); return freqSpectrum_; }
		//{ ValidateDomain(FreqDomain); return *freqSpectrum_; }
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeConstIterator
		beginTime (void) const
		{ return TimeConstIterator (boost::begin(timeSeries_)); }
		//{ return TimeConstIterator (timeSeries_->begin()); }
		
		//!	Returns read-only iterator to the first element in the freq domain
		FreqConstIterator
		beginFreq (void) const
		{ return FreqConstIterator (boost::begin(freqSpectrum_)); }
		//{ return FreqConstIterator (freqSpectrum_->begin()); }
		
		//!	Returns read-only iterator to one past the last element
		TimeConstIterator
		endTime (void) const
		{ return TimeConstIterator (boost::end(timeSeries_)); }
		//{ return TimeConstIterator (timeSeries_->end()); }
		
		//!	Returns read-only iterator to one past the last element
		FreqConstIterator
		endFreq (void) const
		{ return FreqConstIterator (boost::end(freqSpectrum_)); }
		//{ return FreqConstIterator (freqSpectrum_->end()); }
		
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeIterator
		beginTime (void)
		{
			ValidateDomain(TimeDomain);
			return TimeIterator (boost::begin(timeSeries_));
			//return TimeIterator (timeSeries_->begin());
		}
		
		//!	Returns read-only iterator to the first element in the freq domain
		FreqIterator
		beginFreq (void)
		{
			ValidateDomain(FreqDomain);
			return FreqIterator (boost::begin(freqSpectrum_));
			//return FreqIterator (freqSpectrum_->begin());
		}
		
		//!	Returns read-only iterator to one past the last element
		TimeIterator
		endTime (void)
		{
			ValidateDomain(TimeDomain);
			return TimeIterator (boost::end(timeSeries_));
			//return TimeIterator (timeSeries_->end());
		}
		
		//!	Returns read-only iterator to one past the last element
		FreqIterator
		endFreq (void)
		{
			ValidateDomain(FreqDomain);
			return FreqIterator (boost::end(freqSpectrum_));
			//return FreqIterator (freqSpectrum_->end());
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

		/*
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return TimeConstIterator (timeSeries_->begin());
		#else
			return TimeConstIterator (timeSeries_->cbegin());
		#endif
		*/
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


/*
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return FreqConstIterator (freqSpectrum_->begin());
		#else
			return FreqConstIterator (freqSpectrum_->cbegin());
		#endif
*/

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

/*
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return TimeConstIterator (timeSeries_->end());
		#else
			return TimeConstIterator (timeSeries_->cend());
		#endif
*/

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


		/*
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return FreqConstIterator (freqSpectrum_->end());
		#else
			return FreqConstIterator (freqSpectrum_->cend());
		#endif
		*/
		}
		
		//______________________________________________________________________
		//!	Validate and ensure that the specified domain is up-to-date
		/*!
		 *	
		 */
		int
		ValidateDomain (const DomainSpecifier toValidate)
		{
			if (toValidate == validDomain_ || validDomain_ == EitherDomain) {
				
			}
			else if (toValidate == TimeDomain) {
				if (validDomain_ == FreqDomain) { transform_.exec_inverse_transform(); }
				//if (validDomain_ == FreqDomain) { transform_->exec_inverse_transform(); }
			}
			else if (toValidate == FreqDomain) {
				if (validDomain_ == TimeDomain) { transform_.exec_transform(); }
				//if (validDomain_ == TimeDomain) { transform_->exec_transform(); }
			}
			else if (toValidate == EitherDomain) {
				if (validDomain_ == TimeDomain) {
					transform_.exec_transform();
					//transform_->exec_transform();
				} else if (validDomain_ == FreqDomain) {
					transform_.exec_inverse_transform();
					//transform_->exec_transform();
				}
			}
			
			validDomain_ = toValidate;
			return 0;
		}
		
		/*
		template <typename WhichDomain>
		void ValidatePrefix (void)
		{ }
		
		template <>
		void ValidatePrefix<DS_T> (void)
		{ ValidateDomain(TimeDomain); }
		
		template <>
		void ValidatePrefix<DS_C> (void)
		{ ValidateDomain(FreqDomain); }
		*/
		/*
		void
		ValidateSuffix ()
		{
			
		}
		 */
		
		/*
		template <typename WhichDomain>
		int ValidateSuffix<WhichDomain> (void)
		{ return 0; }
		
		template <>
		int ValidateSuffix<DS_T> (void)
		{
			ValidateDomain(FreqDomain);
			return 0;
		}
		
		template <>
		int ValidateSuffix<DS_C> (void)
		{
			ValidateDomain(TimeDomain);
			return 0;
		}
		 */
		
		


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
		Waveform&
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

		//______________________________________________________________________
		//!	Copy-assignment operator
		/*
		Waveform&
		operator= (const Waveform& rhs)
		{
			if (&rhs != this) {				
				rhs.ValidateDomain(EitherDomain);
				
				timeSeries_.reset(make_shared<TimeContainer>(*rhs.timeSeries_));
				freqSpectrum_.reset(make_shared<FreqContainer>(*rhs.freqSpectrum_));
				
				
				//transform_.reset(make_shared<TransformT>(timeSeries_->size()
				//									, timeSeries_->begin()
				//									, freqSpectrum_->begin()));
				transform_.reset(make_shared<TransformT>(timeSeries_->size()
													, beginTime()
													, beginFreq()));
				
				validDomain_ = EitherDomain;
			}
			return *this;
		}
		*/
		
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
		//	std::vector)also have the appropriate operators defined. Boost
		//	has definitions in boost/accumulators/numeric/functional/vector.hpp
		//	in the namespace boost::numeric::operators.
		////////////////////////////////////////////////////////////////////////
		
		//______________________________________________________________________
		//!	Add another Waveform
		/*!
		 *	Because the Fourier Transform is linear, adding the Waveforms in
		 *	either domain will produce identical results.
		 */
		Waveform&
		operator+= (Waveform& rhs)
		{
			this->GetTimeSeries() += rhs.GetTimeSeries();
			return *this;
		}
		
		//______________________________________________________________________
		//!	Subtract another Waveform
		/*!
		 *	Because the Fourier Transform is linear, subtracting the Waveforms
		 *	in either domain will produce identical results.
		 */
		Waveform&
		operator-= (Waveform& rhs)
		{
			this->GetTimeSeries() -= rhs.GetTimeSeries();
			return *this;
		}
		
		//______________________________________________________________________
		//!	Multiply by another Waveform
		/*!
		 *	Because multiplication in the time domain has relatively few
		 *	physical uses (with the exception of convolution), this multiplies
		 *	in the frequency domain.
		 */
		Waveform&
		operator*= (Waveform& rhs)
		{
			this->GetFreqSpectrum() *= rhs.GetFreqSpectrum();
			return *this;
		}
		
		//______________________________________________________________________
		//!	Divide by another Waveform
		/*!
		 *	Because multiplication in the time domain has relatively few
		 *	physical uses (with the exception of convolution), this divides in
		 *	the frequency domain.
		 */
		Waveform& operator/= (Waveform& rhs)
		{
			this->GetFreqSpectrum() /= rhs.GetFreqSpectrum();
			return *this;
		}
		
		
	};
	
} // End of namespace PS

/*! @} End of Doxygen Groups*/

#endif
