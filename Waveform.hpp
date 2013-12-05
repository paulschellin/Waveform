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
 [ ] Implement better range checking. (how do iterators impact this?)
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
 
 [x] Consider changing "fourier_" to be a shared_ptr. "reset()" can be called on
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
 
 
 [ ] To keep the header file small, it may make sense to move the definition and
		development of the monitoring/proxy/observer smart pointer template to
		its own header file, perhaps along with the iterators.

 [ ] Perhaps transition from the ...USE_SHARED_PTR macro and check if __cplusplus
 		is greater than 199711L?

 */

#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP 1

#define WAVEFORM_USE_SHARED_PTR 1
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


// FFTW header files
#include <fftw3.h>

// Other header files
#include <FftwDft1d/FftwDft1d.hpp>

#ifdef WAVEFORM_WRAP_DISCRETESERIES_ALIGNED_ALLOC
#include <DiscreteSeries/DiscreteSeries.hpp>
#endif

/*!
 *	\addtogroup PS
 *	@{
 */

//! Namespace PS is the namespace used for classes, etc. created by the author (Paul Schellin)
namespace PS {
	//using namespace std;
	using std::complex;
	using std::size_t;
	
	using namespace boost::numeric::operators;
	
#ifdef WAVEFORM_USE_STD_SHARED_PTR
	//typedef std::shared_ptr vanilla_shared_ptr;
	using std::shared_ptr;
	using std::make_shared;
#elif defined(WAVEFORM_USE_BOOST_SHARED_PTR)
	//typedef boost::shared_ptr vanilla_shared_ptr;
	using boost::shared_ptr;
	using boost::make_shared;
#endif
	
	
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
	
	
	#ifdef WAVEFORM_USE_VALIDATING_SHARED_PTR
	//!	validating_shared_ptr class
	/*!
	 *	This class wraps whatever shared_ptr class (either boost or std lib)
	 *	was chosen, and hides the original operator*() and operator->()
	 *	functions with operators which additionally validate the given domain.
	 *
	 *	The domain to validate is determined by a template specialization of
	 *	MonitoringClass::Validate<>() which validates the time domain when T is
	 *	DS_T and validates the freq domain when T is DS_C.
	 */
	template <typename T, typename MonitoringClass>
	class validating_shared_ptr : public vanilla_shared_ptr<T> {
	public:
		
		MonitoringClass* monitor_;
		
		template <typename MonitoringClass>
		validating_shared_ptr (T* p, MonitoringClass& monitor)
			: vanilla_shared_ptr<T>(p)
			, monitor_(&monitor)
		{ }
		
		T&
		operator*(void) const
		{
			monitor_->ValidatePrefix<T>();
			return vanilla_shared_ptr<T>::operator*();
		}
		
		T*
		operator->(void) const
		{
			monitor_->ValidatePrefix<T>();
			return vanilla_shared_ptr<T>::operator->();
		}
		
	};
	#endif
	
#ifdef WAVEFORM_USE_VALIDATING_WRAPPER
	//!	ValidatingWrapper class
	/*!
	 *	! Disclaimer:
	 *	!	These classes (ValidatingWrapper and Call_proxy) are not of my own
	 *	!	design but rather are slight modifications of Bjarne Stroustrup's
	 *	!	templated "Wrap" class, published in his paper "Wrapping C++ Member
	 *	!	Function Calls" <http://www.stroustrup.com/wrapper.pdf>.
	 *
	 *	This class wraps an object class or a shared_ptr class (either boost or
	 *	std lib), calling MonitoringClass::ValidatePrefix<T>() before returning
	 *	the pointer. When the temporary returned Call_proxy object goes out of
	 *	scope and is destroyed, the MonitoringClass::ValidateSuffix<T>()
	 *	function is called.
	 *
	 *	The domain to validate is determined by a template specialization of
	 *	MonitoringClass::ValidatePrefix<>() which validates the time domain when
	 *	T is DS_T and validates the freq domain when T is DS_C. ValidateSuffix<>
	 *	does the opposite.
	 */
	
	template <typename T, typename MonitoringClass> class ValidatingWrapper;
	
	template <typename T, typename MonitoringClass>
	class Call_proxy {
		T* p_;
		MonitoringClass* monitor_;
		mutable bool own;
		
		Call_proxy(T* p, MonitoringClass& monitor)
			: p_(p)
			, monitor_(&monitor)
		{ }								//	Restrict creation
		
		Call_proxy&
		operator=(const Call_proxy&);	//	Prevent assignment
	public:
		template<typename U, typename M> friend class ValidatingWrapper;
		
		Call_proxy(const Call_proxy& a)
			: p_(a.p_)
			, monitor_(a.monitor_)
			, own(true)
		{
			a.own = false;
		}
		
		~Call_proxy()
		{
			if (own) monitor_->ValidateSuffix<T>();
		}
		
		T*
		operator->() const
		{
			return p_;
		}
	}
	
	template <typename T, typename MonitoringClass>
	class ValidatingWrapper {
		T* p_;
		MonitoringClass* monitor_;
		
		
		
	public:
		
		ValidatingWrapper (T* p, MonitoringClass& monitor)
			: p_(p)
			, monitor_(&monitor)
		{ }
		
		Call_proxy<T, MonitoringClass>
		operator*(void) const
		{
			monitor_->ValidatePrefix<T>();
			return p_;
		}
		
		Call_proxy<T, MonitoringClass>
		operator->(void) const
		{
			monitor_->ValidatePrefix<T>();
			return p_;
		}
	};
#endif
	
#ifdef WAVEFORM_USE_SHARED_PTR_WRAPPING
	//!
	/*!
	 *	As great as this would be, it won't work the way it's written because
	 *	the destructor/deleter function (i.e. the suffix function)
	 *	must be an functor which is CopyConstructable, etc.
	 *
	 *	Also, it must accept the pointer as its argument, but it could just be
	 *	ignored.
	 *
	 *	Better idea: in the ValidateSuffix function, accept p_ as a parameter,
	 *	but recast it as a void pointer (because T can be multiple classes) and
	 *	then compare p_ against both of the timeSeries_ and freqSpectrum_ raw
	 *	pointers to see which domain it is, and run the suffix function
	 *	appropriately.
	 *	Or overload the ValidateSuffix() function, which also works. Either way,
	 *	get rid of the template specialization.
	 *
	 *	Boost's shared_ptr constructor documentation states that the deleter
	 *	object, d (of CopyConstructible type D) must have a non-throwing copy
	 *	constructor as well as destructor, and that d(p_) "must be well-formed,
	 *	must not invoke undefined bahavior, and must not throw exceptions."
	 *
	 *	If this shared_ptr constructor throws an exception (probably bad_alloc),
	 *	then d(p_) is called (interesting: "Notes: When the time comes to delete
	 *	the object pointed to by p, the stored copy of d is invoked with the
	 *	stored copy of p as an argument." ... because they are copies, that may
	 *	require some more thought to ensure everything is done correctly.
	 *
	 *	Boost::mem_fn is an adapter which allows member functions to be treated
	 *	as function objects accepting the same arguments as the member function 
	 *	but with an extra argument prepended to the argument list, representing
	 *	the object instance.
	 *
	 *	It might be fine to only have a prefix function (forgoing the suffix)
	 *	since const-ness doesn't prevent elements of the wrapped container class
	 *	from being modified. Calling a validating prefix function which also
	 *	sets the validDomain_ to the requested one should be sufficient, though
	 *	depending on how a user implements a Waveform object into their code
	 *	severe albiet unnecessary performance penalties may occur, particularly
	 *	if accesses of one domain are interwoven with mutations of the other
	 *	domain. An example of where this may be most annoying is with access to
	 *	members of the template parameter container class for which a forwarding
	 *	interface doesn't exist for stl-compatability reasons (e.g. the
	 *	member "DiracComb" of DiscreteSamples, which can't have an interface in
	 *	Waveform otherwise stl-compatability would be lost). A possible case
	 *	might look like:
	 *
	 *		DiscreteSamples<...> noiseSpectrum (dataFromFileFactory<...>("data.csv"));
	 *		Waveform<...> myWfm(noiseSpectrum);
	 *		
	 *		for (unsigned idx_time = 0; idx_time < myWfm.size(); ++idx_time) {
	 *			
	 *		}
	 *
	 *	Event when an accessed value is left unmodified, compilers most likely
	 *	will be unable to optimize the access through invariant expression
	 *	elimination because the access itself treats both accesses and mutations
	 *	as though the object were mutated.
	 *	 
	 */
	template<typename T, typename MonitoringClass>
	class wrapped_shared_ptr {
		T* p_;
		MonitoringClass* monitor_;
		
	public:
		
		explicit wrapped_shared_ptr(T* p, MonitoringClass& monitor)
			: p_(p)
			, monitor_(&monitor)
		{ }
		
		shared_ptr<T>
		operator->() const
		{
			monitor_->ValidatePrefix<T>();
			//return shared_ptr<T>(p_, monitor_->ValidateSuffix<T>());
			return shared_ptr<T>(p_, boost::mem_fn(&MonitoringClass::ValidateSuffix));
		}
	}
	
#endif
	
	
	
	
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
	
	typedef double					DefaultRealT;
	typedef complex<DefaultRealT>	DefaultComplexT;
	
	typedef typename FftwDft1d<>::allocator<DefaultRealT>		DefaultRealTAlloc;
	typedef typename FftwDft1d<>::allocator<DefaultComplexT>	DefaultComplexTAlloc;


	#ifdef WAVEFORM_WRAP_DISCRETESERIES_ALLIGNED_ALLOC
	typedef DiscreteSeries	< DefaultRealT
							, DefaultRealT
							, DefaultRealTAlloc
							, DefaultRealTAlloc
							> DiscreteSeriesDefaultRealT;

	typedef DiscreteSeries	< DefaultRealT
							, DefaultComplexT
							, DefaultRealTAlloc
							, DefaultComplexTAlloc
							> DiscreteSeriesDefaultComplexT;


	template< typename TimeContainer = DiscreteSeriesDefaultRealT, typename FreqContainer = DiscreteSeriesDefaultComplexT>
	#else
	template< typename TimeContainer, typename FreqContainer>
	#endif
	class Waveform : boost::arithmetic1< Waveform<TimeContainer,FreqContainer> > {
		
	public:

		typedef typename TimeContainer::value_type	TimeT;
		
		typedef typename FreqContainer::value_type	FreqT;
		
		typedef typename std::pair<TimeT,FreqT> value_type;
		
		typedef typename TimeContainer::allocator_type TimeAllocT;
		
		typedef typename FreqContainer::allocator_type	FreqAllocT;

		//typedef typename FftwDft1d<>::allocator<TimeT>	TimeAllocT;
		//typedef typename FftwDft1d<>::allocator<FreqT>	FreqAllocT;
		
		
		typedef typename TimeContainer::iterator	TimeIterator;
		typedef typename FreqContainer::iterator	FreqIterator;
		
		typedef typename TimeContainer::const_iterator	TimeConstIterator;
		typedef typename FreqContainer::const_iterator	FreqConstIterator;
		
		
		
		typedef FftwDft1d<TimeT> FftType;
		
	private:
		enum DomainSpecifier {TimeDomain, FreqDomain, EitherDomain};
		
		DomainSpecifier				validDomain_;

		shared_ptr<TimeContainer>	timeSeries_;
		shared_ptr<FreqContainer>	freqSpectrum_;
		
		shared_ptr<FftType>			fourier_;
		
		
	
		
		 
		//	Default constructor
		// Because this initializes Waveform::fourier_, which cannot be modified
		//	post-initialization as currently written, it might make sense to
		//	have the default constructor be private, and simply not used.
		// Alternately, fourier_ could be made into a shared_ptr so it is able
		//	to be deleted/reset and re-new'd (or made via factory make_shared())
		//	more safely. That would make Waveform Assignable with much less
		// manual dynamic memory handling.
	private:
		Waveform(void)
			: validDomain_(EitherDomain)
			, timeSeries_(make_shared<TimeContainer>())				// default initialize
			, freqSpectrum_(make_shared<FreqContainer>())			// default initialize
			, fourier_()
			//, fourier_(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin())
		{ }
		
		
	public:
		// Range-based constructor
		Waveform(const size_t count)
			: validDomain_(EitherDomain)
			, timeSeries_(make_shared<TimeContainer>(count))						// range-based initialization
			, freqSpectrum_(make_shared<FreqContainer>(timeSeries_->size()/2 + 1))	// range-based initialization
			, fourier_(make_shared<FftType>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		
		// Copy constructor
		explicit Waveform(const Waveform& toCopy)
			: validDomain_(toCopy.validDomain_)
			, timeSeries_(make_shared<TimeContainer>(toCopy.timeSeries_))
			, freqSpectrum_(make_shared<FreqContainer>(toCopy.freqSpectrum_))
			, fourier_(make_shared<FftType>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		
		// Time domain copy constructor
		explicit Waveform(const TimeContainer& toCopy)
			: validDomain_(TimeDomain)
			, timeSeries_(make_shared<TimeContainer>(toCopy))							// copy
			, freqSpectrum_(make_shared<FreqContainer>(timeSeries_->size()/2 + 1))		// range-based initialization
			, fourier_(make_shared<FftType>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		
		// Frequency domain copy constructor
		explicit Waveform(const FreqContainer& toCopy)
			: validDomain_(FreqDomain)
			, timeSeries_(make_shared<TimeContainer>((toCopy.size()-1)*2))			// range-based initialization
			, freqSpectrum_(make_shared<FreqContainer>(toCopy))						// copy
			, fourier_(make_shared<FftType>(timeSeries_->size(), timeSeries_->begin(), freqSpectrum_->begin()))
		{ }
		 
		 
		~Waveform (void) {}
		
		//
		// Accessor Function Declarations
		//
		
		
		
		size_t
		GetSize	(void) //const
		{ return GetConstTimeSeries().size(); }
		
		size_t
		size (void) //const
		{ return GetConstTimeSeries().size(); }
		
		//!
		/*!
		 *	Pass "EitherDomain" because domain will not be modified.
		 */
		const TimeContainer&
		GetConstTimeSeries (void)
		{ ValidateDomain(EitherDomain); return *timeSeries_; }
		
		//!
		/*!
		 *	Pass "EitherDomain" because domain will not be modified.
		 */
		const FreqContainer&
		GetConstFreqSpectrum (void)
		{ ValidateDomain(EitherDomain); return *freqSpectrum_; }
		
		TimeContainer&
		GetTimeSeries (void)
		{ ValidateDomain(TimeDomain); return *timeSeries_; }
		
		FreqContainer&
		GetFreqSpectrum (void)
		{ ValidateDomain(FreqDomain); return *freqSpectrum_; }
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeConstIterator
		beginTime (void) const
		{ return TimeConstIterator (timeSeries_->begin()); }
		
		//!	Returns read-only iterator to the first element in the freq domain
		FreqConstIterator
		beginFreq (void) const
		{ return FreqConstIterator (freqSpectrum_->begin()); }
		
		//!	Returns read-only iterator to one past the last element
		TimeConstIterator
		endTime (void) const
		{ return TimeConstIterator (timeSeries_->end()); }
		
		//!	Returns read-only iterator to one past the last element
		FreqConstIterator
		endFreq (void) const
		{ return FreqConstIterator (freqSpectrum_->end()); }
		
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeIterator
		beginTime (void)
		{
			ValidateDomain(TimeDomain);
			return TimeIterator (timeSeries_->begin());
		}
		
		//!	Returns read-only iterator to the first element in the freq domain
		FreqIterator
		beginFreq (void)
		{
			ValidateDomain(FreqDomain);
			return FreqIterator (freqSpectrum_->begin());
		}
		
		//!	Returns read-only iterator to one past the last element
		TimeIterator
		endTime (void)
		{
			ValidateDomain(TimeDomain);
			return TimeIterator (timeSeries_->end());
		}
		
		//!	Returns read-only iterator to one past the last element
		FreqIterator
		endFreq (void)
		{
			ValidateDomain(FreqDomain);
			return FreqIterator (freqSpectrum_->end());
		}
		
		
		
		//!	Returns read-only iterator to the first element in the time domain
		TimeConstIterator
		cbeginTime (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return TimeConstIterator (timeSeries_->begin());
		#else
			return TimeConstIterator (timeSeries_->cbegin());
		#endif
		}
		
		//!	Returns read-only iterator to the first element in the freq domain
		FreqConstIterator
		cbeginFreq (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return FreqConstIterator (freqSpectrum_->begin());
		#else
			return FreqConstIterator (freqSpectrum_->cbegin());
		#endif
		}
		
		//!	Returns read-only iterator to one past the last element
		TimeConstIterator
		cendTime (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return TimeConstIterator (timeSeries_->end());
		#else
			return TimeConstIterator (timeSeries_->cend());
		#endif
		}
		
		//!	Returns read-only iterator to one past the last element
		FreqConstIterator
		cendFreq (void) const
		{
		#ifndef WAVEFORM_USE_CBEGIN_CEND
			return FreqConstIterator (freqSpectrum_->end());
		#else
			return FreqConstIterator (freqSpectrum_->cend());
		#endif
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
				if (validDomain_ == FreqDomain) { fourier_->ExecuteIDFT(); }
			}
			else if (toValidate == FreqDomain) {
				if (validDomain_ == TimeDomain) { fourier_->ExecuteDTFT(); }
			}
			else if (toValidate == EitherDomain) {
				if (validDomain_ == TimeDomain) {
					fourier_->ExecuteDTFT();
				} else if (validDomain_ == FreqDomain) {
					fourier_->ExecuteIDFT();
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
		
		
		//______________________________________________________________________
		//!	Copy-assignment operator
		Waveform&
		operator= (const Waveform& rhs)
		{
			if (&rhs != this) {				
				rhs.ValidateDomain(EitherDomain);
				
				timeSeries_.reset(make_shared<TimeContainer>(*rhs.timeSeries_));
				freqSpectrum_.reset(make_shared<FreqContainer>(*rhs.freqSpectrum_));
				
				
				//fourier_.reset(make_shared<FftType>(timeSeries_->size()
				//									, timeSeries_->begin()
				//									, freqSpectrum_->begin()));
				fourier_.reset(make_shared<FftType>(timeSeries_->size()
													, beginTime()
													, beginFreq()));
				
				validDomain_ = EitherDomain;
			}
			return *this;
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
