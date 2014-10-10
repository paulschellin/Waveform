/*
 Waveform.hpp
 Waveform class provides functions frequently used in the RF simulations.
 Written by Paul Schellin at the Ohio State University, 2012-2013
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
//#include <fstream>
#include <string>
#include <cmath>
//#include <iomanip>
//#include <sstream>

#include <type_traits>


// Boost header files

#include <boost/operators.hpp>

#include <boost/range.hpp>

#include <boost/assert.hpp>


#define WAVEFORM_USE_CBEGIN_CEND 1

/*!
 *	\addtogroup PS
 *	@{
 */

//! Namespace PS is the namespace used for classes, etc. created by the author (Paul Schellin)
namespace PS {

	/*
	namespace InverseTypes {
		struct Inverse {};

		struct ScaledInverse {};

		struct Other {};
	}
	*/

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

		typedef InverseTypes::Other inverse_type;

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
			/*
			< typename T1
			, typename T2 = T1
			, template <typename ...ArgsX1> class Container1 = std::vector
			, template <typename ...ArgsX2> class Container2 = Container1
			//, typename ...Args1
			//, typename ...Args2
			>
			*/

	class Waveform {
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
		
		

		//!
		/*!
		 *
		 *
		 *
		 */
		//enum DomainSpecifier {TimeDomain, FreqDomain, EitherDomain};
		enum class Domain {Time, Freq, Either};

	  private:

		//!	Indicates the valid domain array(s)
		//DomainSpecifier				validDomain_;
		Domain			validDomain_;

		//!	Container object for the time series array
		TimeContainer	timeSeries_;

		//!	Container object for the frequency spectrum array
		FreqContainer	freqSpectrum_;

		//!	Transform class object which wraps the forward and inverse transform functions
		TransformT		transform_;
 
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
		Waveform(void) = delete;
		/*
		Waveform(void)
			: validDomain_(EitherDomain)
		{ }
		*/
		
	  public:
		
		//! Fill constructor
		Waveform(const std::size_t count)
			//: validDomain_(EitherDomain)
			: validDomain_(Domain::Either)
			, timeSeries_(count)
			, freqSpectrum_(count)
			, transform_(timeSeries_, freqSpectrum_)
		{ 
			if (timeSeries_.size()%2)
				throw std::length_error("Waveform: The array length was not a multiple of 2!");
		}


		//! Copy constructor
		explicit
		Waveform(const Waveform& toCopy)
			: validDomain_(toCopy.validDomain_)
			, timeSeries_(toCopy.timeSeries_)
			, freqSpectrum_(toCopy.freqSpectrum_)
			//, transform_(timeSeries_, freqSpectrum_)
			, transform_ (toCopy.transform_)
		{
			if (timeSeries_.size()%2)
				throw std::length_error("Waveform: The array length was not a multiple of 2!");
		}
		
		

		
		//! Time domain copy constructor
		explicit Waveform(const TimeContainer& toCopy)
			//: validDomain_(TimeDomain)
			: validDomain_(Domain::Time)
			, timeSeries_(toCopy)
			, freqSpectrum_(timeSeries_.size()/2 + 1)
			, transform_(timeSeries_, freqSpectrum_)
		{
			if (timeSeries_.size()%2)
				throw std::length_error("Waveform: The array length was not a multiple of 2!");
		}
		
		//! Frequency domain copy constructor
		explicit Waveform(const FreqContainer& toCopy)
			//: validDomain_(FreqDomain)
			: validDomain_(Domain::Freq)
			, timeSeries_((toCopy.size() - 1) * 2)
			, freqSpectrum_(toCopy)
			, transform_(timeSeries_, freqSpectrum_)
		{
			if (timeSeries_.size()%2)
				throw std::length_error("Waveform: The array length was not a multiple of 2!");
		}
		
		//!	Default destructor
		~Waveform (void) {}
		
		
		//!	Returns the size of the time domain container
		/*!
		 *	<This function seems redundant and will likely be
		 *	removed. Use Waveform::size() instead, since it's
		 *	STL-compliant (though this class is difficult to
		 *	design 100% in line with the STL idioms)>
		 */
		std::size_t
		GetSize	(void) //const
		{ return GetConstTimeSeries().size(); }
	

		//!	Returns the size of the time domain container
		std::size_t
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
		//{ ValidateDomain(EitherDomain); return timeSeries_; }
		{ ValidateDomain(Domain::Either); return timeSeries_; }
		

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
		//{ ValidateDomain(EitherDomain); return freqSpectrum_; }
		{ ValidateDomain(Domain::Either); return freqSpectrum_; }
		

		//!	Returns mutable reference to the time domain container
		/*!
		 *	If the time domain is not valid, the inverse/reverse transform
		 *	will automatically calculate the appropriate values behind the
		 *	scenes, given the valid frequency domain; returns when complete.
		 */
		TimeContainer&
		GetTimeSeries (void)
		//{ ValidateDomain(TimeDomain); return timeSeries_; }
		{ ValidateDomain(Domain::Time); return timeSeries_; }
		

		//!	Returns mutable reference to the frequency domain container
		/*!
		 *	If the frequency domain is not valid, the forward transform
		 *	will automatically calculate the appropriate values behind the
		 *	scenes, given the valid time domain; returns when complete.
		 */
		FreqContainer&
		GetFreqSpectrum (void)
		//{ ValidateDomain(FreqDomain); return freqSpectrum_; }
		{ ValidateDomain(Domain::Freq); return freqSpectrum_; }
		
		


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
		 
		 
		 	[ Iterator access functions have been removed ]
		 */

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
		 *	inverse \f$\mathcal{F}^{-1}\f$ must be unitary -- that is, they must
		 *	satisfy the composition criteria
		 *
		 *		\f$\mathcal{F}^{-1} \circ \mathcal{F} (x) = id (x)\f$ 
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
		//ValidateDomain (const DomainSpecifier toValidate)
		ValidateDomain (const Domain toValidate)
		{
			if (toValidate == validDomain_ || validDomain_ == Domain::Either) {
				//	There aren't any transforms to be performed
			}
			else if (toValidate == Domain::Time) {
				transform_.exec_inverse_transform();
			}
			else if (toValidate == Domain::Freq) {
				transform_.exec_transform();
			}
			else if (toValidate == Domain::Either) {
				if (validDomain_ == Domain::Time) {
					transform_.exec_transform();
				} else // if (validDomain_ == FreqDomain)
				{
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

		Waveform(const Waveform&& rhs)
			: Waveform()
		{
			swap(*this, rhs);
		}

		
//	};

	
	//!	Equality operator
	/*!
	 *	This checks that the valid domains of two waveforms are equal.
	 *	
	 *	If the two waveforms have some valid domain which is in common,
	 *	it will compare those common domains.
	 *
	 *	If two waveforms are of opposite domains, it will pick one to convert.
	 *	No transforms are done otherwise.
	 */
	template <typename ...Args1, typename ...Args2>
	friend
	inline bool
	operator==(const Waveform<Args1...>& lhs, const Waveform<Args2...>& rhs)
	{
		//	Could probably just see what the valid domain is and compare only that.
		//	Perhaps in a later version.

		//	At the very least it makes sense to check the valid domain first, because
		//	the valid domain doesn't require a transform. 
		//	However, that would require either this function to be a friend function or
		//	for the validDomain_ variable to be made public.
		
		if (lhs.validDomain_ == Domain::Either)
		{
			if (rhs.validDomain_ == Domain::Either)
			{
				return lhs.GetTimeSeries() == rhs.GetTimeSeries() && lhs.GetFreqSpectrum() == rhs.GetFreqSpectrum();
			}
			else
			if (rhs.validDomain_ == Domain::Time)
			{
				return (lhs.GetTimeSeries() == rhs.GetTimeSeries());
			}
			else	//	Else rhs.validDomain_ == Domain::Freq
			{
				return (lhs.GetFreqSpectrum() == rhs.GetFreqSpectrum());
			}
		}
		else
		if (lhs.validDomain_ == Domain::Time)
		{
			return (lhs.GetTimeSeries() == rhs.GetTimeSeries());
		}
		else
		if (lhs.validDomain_ == Domain::Freq)
		{
			return (lhs.GetFreqSpectrum() == rhs.GetFreqSpectrum());
		}


		//return lhs.GetTimeSeries() == rhs.GetTimeSeries() && lhs.GetFreqSpectrum() == rhs.GetFreqSpectrum();
	}

	template <typename ...Args1, typename ...Args2>
	friend
	inline bool
	operator!=(const Waveform<Args1...>& lhs, const Waveform<Args2...>& rhs)
	{
		return !(lhs == rhs);
	}




	/*

	std::enable_if<std::is_same(transform_::inverse_type, InverseTypes::Inverse)::value, Waveform&>::type
	//Waveform&
	operator*= (TimeContainer& rhs)
	{
		//if (std::is_same(transform_::inverse_type, InverseTypes::Inverse)::value)
		
		this->GetTimeSeries() *= rhs.GetTimeSeries();
		return *this;
	}
	*/




	};

	
} // End of namespace PS

/*! @} End of Doxygen Groups*/

#endif
