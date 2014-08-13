# Waveform
Written by Paul Schellin at the Ohio State University, 2012 to 2014.

Waveform is a C++ header-only library which represents both the time and the frequency domains of a waveform/signal as a single object, transforming using FFTW automatically when needed.

The Waveform class provides a safe, intuitive interface to allow a signal to be described in either the time domain or in the frequency domain and then manipulated in any domain without explicitly needing to transform from one domain to another.

This library attempts to parallel the nature of waveforms in that the time domain, frequency domain, S domain, etc. are simply different representations of a singular physical entity.

## Installation

### Dependencies

The dependencies of the Waveform class are:

- a [C++11 compatible](http://cpprocks.com/c11-compiler-support-shootout-visual-studio-gcc-clang-intel/) compiler
- [boost](http://www.boost.org/) (optional in some cases)
- [fftw](http://www.fftw.org/) (not necessary if you can match another library's interface to the one which Waveform was designed with)

I highly recommend using your system's package manager to install your compiler, the boost libraries, and fftw. The package manager will handle all of the annoying path variables for including headers and for linking libraries. Plus, it's much easier to update packages.

Otherwise, the Waveform class was designed to be a header-only library, so simply including the header (`#include <Waveform.hpp>`) and compiling with C++11 support (you can do this by adding `-std=c++11` to your compiler's flags) will be sufficient, assuming FFTW3 was already being used and was properly configured. If FFTW3 was not used before, you must add `-lfftw3` to the linker flags.


### Download the Waveform library
To download this library from github, simply type
```Shell
git clone https://github.com/paulschellin/Waveform.git
```
in a terminal to clone the library to the current directory.

Or click the "Download ZIP" button to the right to have your browser take care of the download, then unzip it where you need it.


### Verify "Installation"
The Waveform library is ["header-only"](http://en.wikipedia.org/wiki/Header-only), so it does not require "installation" in the typical sense. But I've included the unit tests to help verify that your development environment is configured properly to use the Waveform library in your projects.

The test_src/Waveform_test.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The actual FFTW wrapper can be found in FftwTransform.hpp, which can be tested using the source file FftwTransform_test.cpp.

You must have the [Google Test framework](https://code.google.com/p/googletest/) installed in order to run these tests.

NOTE: Google recommends just placing the test framework directory within the directory of the project you wish to test -- so in this case, you would put "gtest-1.7.0" (or similar) into the Waveform/ directory.

IMPORTANT: Not all tests will pass necessarily, I've put explicit failures into the test files to remind myself and users that there are still features which I must implement.

There are a number of tests, each of which can be run by typing:

#### Test Waveform
This tests the basic functionality of the library.

```Shell
make clean Waveform
./test_bin/Waveform_test
```

#### Test IdentityTransform
Test the most basic transform -- the identity transform.
```Shell
make clean IdentityTransform
./test_bin/IdentityTransform_test
```

#### Test FftwTransform

NOTE: In order to run the FFTW tests, you must have the FFTW3 library installed. I highly recommend using your package manager (such as apt-get on Debian/Ubuntu, pacman on Arch, yum on Fedora, Homebrew on Mac OS X, etc.) to install it instead of installing it manually because your package manager will set up the linking and include directories automatically and appropriately.

```Shell
make clean FftwTransform
./test_bin/FftwTransform_test
```


## How To Use

If you have used FFTW before, you'll find using Waveform is much easier!

### Creating a Waveform instance

The Waveform library is provided as a class templates, so you'll need to know what parameters are used for instantiation:

```C++
template < typename TimeContainer
		 , typename FreqContainer
		 , typename TransformT
		 >
```

`TimeContainer` is usually `std::vector<double>`, `FreqContainer` normally `std::vector<std::complex<double>>`, and `TransformT` can be one of a number of transforms, such as `Fftw3_Dft_1d`, `Fftw3_Dft_1d_Normalized`, `IdentityTransform`, and any other transforms I add in the future -- or ones that you write yourself (for more info on how to do that, look at the `PlaceholderTransformClass` definition in `Waveform.hpp`).

So a complete instantiation would be:

```C++
template < vector<double>
		 , vector<complex<double> >
		 , Fftw3_Dft_1d_Normalized
		 > myWfm;
```

Using typedefs to shorten the instantiation is recommended (see examples for how this is done).

### Using Waveform Functions

#### Constructors

| Constructor Name | Constructor Call | Parameter Names | Description |
| ---------------: | :--------------- | --------------- | :---------- |
| Default constructor			| ~~`Waveform ()`;~~							| <none> | The default constructor is disallowed because the size of the `Waveform` is needed to construct many transform objects, such as `FftwTransform`s |
| Fill constructor				| `Waveform (size_t n)`;					| `x` is the size of the time domain array | Constructs a `Waveform` container with `n` elements. |
| Copy constructor				| `Waveform (const Waveform& x)`;			| `x` is the Waveform to copy | Constructs a `Waveform` container with a copy of each of the elements of both domains in `x`, in the same order. |
| Time domain copy constructor	| `Waveform (const TimeContainer& x)`;	| `x` is the time domain container to copy | Constructs a `Waveform` container with the time domain being a copy of each of the elements in `x`. |
| Freq domain copy constructor	| `Waveform (const FreqContainer& x)`;	| `x` is the freq domain container to copy | Constructs a `Waveform` container with the freq domain being a copy of each of the elements in `x`. |

#### Member Functions

- `GetSize()`
- `size()`
- `GetConstTimeSeries()`
- `GetConstFreqSpectrum()`
- `GetTimeSeries()`
- `GetFreqSpectrum()`
- `ValidateDomain()`


### Types of Transforms

Transform objects are what make the Waveform containers do the mathematical transforms.

#### Currently Supported
- `IdentityTransform` -- the two domains of the Waveform are always identical. Not particularly useful except for in testing
- `Fftw3_Dft_1d` -- based on fftw_plan_dft_r2c_1d and _c2r_1d
- `Fftw3_Dft_1d_Normalized` -- like Fftw3_Dft_1d but [normalized](http://www.fftw.org/doc/The-1d-Discrete-Fourier-Transform-_0028DFT_0029.html#The-1d-Discrete-Fourier-Transform-_0028DFT_0029)

#### Eventual Support

##### FFTW "Plans"

| Transform Name | Forward Plan Name | Inverse Plan Name | First Domain | Second Domain |
| -------------- | ----------------- | ----------------- | ------------ | ------------- |
| Fftw3_Dft_1d | fftw_plan_dft_1d | fftw_plan_dft_1d | complex 1D array | complex 1D array |
| Fftw3_Dft_2d | fftw_plan_dft_2d | fftw_plan_dft_2d | complex 2D array | complex 2D array |
| Fftw3_Dft_3d | fftw_plan_dft_3d | fftw_plan_dft_3d | complex 3D array | complex 3D array |
| Fftw3_Dft_r2c_2d | fftw_plan_dft_r2c_2d | fftw_plan_dft_c2r_2d | real 2D array | complex 2D array |
| Fftw3_Dft_r2c_3d | fftw_plan_dft_r2c_3d | fftw_plan_dft_c2r_3d | real 3D array | complex 3D array |
| Fftw3_Dft_r2c | fftw_plan_dft_r2c | fftw_plan_dft_c2r | real N-D array | complex N-D array |
| Fftw3_r2r_1d | fftw_plan_r2r_1d | fftw_plan_r2r_1d | Real 1D array | Real 1D array |
| Fftw3_r2r_2d | fftw_plan_r2r_2d | fftw_plan_r2r_2d | Real 2D array | Real 2D array |
| Fftw3_r2r_3d | fftw_plan_r2r_3d | fftw_plan_r2r_3d | Real 3D array | Real 3D array |
| Fftw3_r2r | fftw_plan_r2r | fftw_plan_r2r | Real N-D array | Real N-D array |

Note that fftw supports many varieties of r2r transforms, parameterized by the `fftw_r2r_kind` type.

More information here: http://www.fftw.org/doc/More-DFTs-of-Real-Data.html#More-DFTs-of-Real-Data

Useful kinds include:

| fftw_r2r_kind	name | Description |
| ------------------ | ----------- |
| FFTW_R2HC	| Real to "Halfcomplex" output -- Like dft_r2c but sometimes faster |
| FFTW_HC2R	| Halfcomplex to Real -- different output than dft_c2r |
| FFTW_DHT | Discrete Hartley transform |
| FFTW_REDFT00 | (DCT-I): even around j=0 and even around j=n-1 |
| FFTW_REDFT10 | (DCT-II, “the” DCT): even around j=-0.5 and even around j=n-0.5. |
| FFTW_REDFT01 | (DCT-III, “the” IDCT): even around j=0 and odd around j=n. |
| FFTW_REDFT11 | (DCT-IV): even around j=-0.5 and odd around j=n-0.5. |
| FFTW_RODFT00 | (DST-I): odd around j=-1 and odd around j=n. |
| FFTW_RODFT10 | (DST-II): odd around j=-0.5 and odd around j=n-0.5. |
| FFTW_RODFT01 | (DST-III): odd around j=-1 and even around j=n-1. |
| FFTW_RODFT11 | (DST-IV): odd around j=-0.5 and even around j=n-0.5. |

The r2r kinds for the various REDFT and RODFT types supported by FFTW, along with the boundary conditions at both ends of the input array (n real numbers in[j=0..n-1]), are: 

##### Other Common Transforms

- Wavelet transform
- Hilbert transform
- Laplace transform 


#### User-Defined Transforms

It's simple to define your own transform if you want/need to.

All you need is to define a class with two public constructors, a destructor, and two member functions: `void exec_transform (void)` and `void exec_inverse_transform (void)`.

Here's an example for a transform that just performs `log10()` on the elements for the forwrd transform, and `pow(10, x)` for the inverse:

```C++
namespace Waveform {
namespace Transform {

template <typename Iter>
class Log10Transform {
  private:

	typedef typename Iter::value_type T;

	Iter linear_begin_;
	Iter linear_end_;
	Iter log10_begin_;
	Iter log10_end_;

  public:
	
	template <typename Iterator1, typename Iterator2>
	Log10Transform (Iterator1 first1, Iterator1 last1, Iterator2 first2)
		: linear_begin_(first1)
		, linear_end_ (last1)
		, log10_begin_ (first2)
		, log10_end_ (log10_begin_ + std::distance(linear_begin_, linear_end_))
	{
	}

	template <typename RandomAccessRange1, typename RandomAccessRange2>
	Log10Transform (RandomAccessRange1& range1, RandomAccessRange2& range2)
		: linear_begin_( boost::begin(range1))
		, linear_end_ ( boost::end(range1))
		, log10_begin_ ( boost::begin(range2))
		, log10_end_ ( boost::end(range2))
	{
	}

	~Log10Transform (void)
	{
	}

	void exec_transform (void)
	{
		std::transform (linear_begin_, linear_end_, log10_begin_,
			[](T x){ log10(x); }
		);
	}

	void exec_inverse_transform (void)
	{
		boost::transform (log10_begin_, log10_end_, linear_begin_,
			[](T x){ pow(T(10.),x); }
		);
	}

};

}
}
```


### Examples
Some examples to help show how this library can be used.

#### Example #1 -- A Single 1D DFT ("forward" transform only)

A typical use of FFTW for signal processing might look like the following:

```C++
#include <fftw3.h>

int main ()
{
	// Create the input and output array pointers
	fftw_complex *in;
	fftw_complex *out;

	// Create the plan
	fftw_plan p;

	// Dynamically allocate the input and output arrays
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	
	// Create the plan
	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	/*
		...
		Fill the input array as desired
		...
	*/

	// Explicitly perform the transform
	fftw_execute(p); /* repeat as needed */
	
	/*
		...
		Do something with the output array as desired
		...
	*/

	// Free the memory used by the dynamically allocated arrays and the plan.
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
}
```

(the above example was taken from [FFTW's documentation website](http://www.fftw.org/doc/Complex-One_002dDimensional-DFTs.html) with some changes to format and commenting by me)


Using the Waveform library, we can write it like this:

```C++
#include <vector>		// for our container of choice
#include <complex>		// for our complex type of choice

#include <iostream>		// for std::cout
#include <iterator>		// for std::ostream_iterator
#include <algorithm>	// for std::copy

#include <Waveform/Waveform.hpp>		// the standard Waveform header file
#include <Waveform/FftwTransform.hpp>	// the FFTW3 transform header file

using namespace std;

/*
	If we only want to use a single kind of transform (in this case the
	same _dft_1d as we used above but with normalization performed (see
	http://www.fftw.org/doc/The-1d-Discrete-Fourier-Transform-_0028DFT_0029.html#The-1d-Discrete-Fourier-Transform-_0028DFT_0029),
	then it's easiest to typedef the type to "WaveformType":
 */

typedef Waveform < vector<double>				// The type of the real-valued array
				, vector< complex<double> >		// The type of the complex-valued array
				, Waveform::Transform::Fftw3_Dft_1d_Normalized		// The type of transform we want to perform
				> 			WaveformType;

int main ()
{
	// Create the Waveform instance:
	WaveformType myWfm (N);

	// Fill the time domain:
	for (size_t i = 0; i < myWfm.GetConstTimeSeries().size(); ++i)
	{
		// Waveform::GetTimeSeries() returns the time domain container
		// (in this case a vector)
		myWfm.GetTimeSeries().at(i) = someFillingFunction(i);
	}


	/*
		Now that we have filled the time domain array, we want to see the freq
		domain information.
		
		We don't have to explicitly tell Waveform to do the transform, it just
		does it automatically and only if needed.

		Here, we'll show it printing out the values of the freq domain:
	*/
	
	ostream_iterator< complex<double> > cout_iterator (std::cout, ", ");

	// Here's the magic! When GetConstFreqSpectrum() is called the first time, the transform is performed and won't need to be performed again until the time domain is modified!
	std::copy	( myWfm.GetConstFreqSpectrum().begin()
				, myWfm.GetConstFreqSpectrum().end()
				, cout_iterator
				);

	/*
		And we're done!

		There is nothing which needs to be freed, as everything is handled by
		the destructor of the Waveform class when the Waveform object goes out
		of scope at the end of the function!
	*/
}
```

In this case, you might be thinking that the FFTW implementation seems simpler (or at the very least, shorter), but the complexity of an FFTW implementation increases greatly as you need more complicated signal processing, while the Waveform implementation stays the exact same! You can observe this in the following examples.

#### Example #2 -- Applying a Frequency-Domain Filter to a Signal

There's a lot of code to this, so the FFTW and Waveform implementations will share some code:
```C++
#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <complex>

using namespace std;

template <typename T>
vector<T>
parse_dat_file (const string fileName)
{
	ifstream ifs (fileName.c_str());
	assert(ifs.good());

	vector<T> result { istream_iterator<T>(ifs)
						, istream_iterator<T>() };

	return result;
}
```

FFTW Implementation:

```C++
#include <fftw3.h>
#include <vector>
#include <complex>

int main ()
{
	// Create the input and output array pointers
	fftw_complex *in;
	fftw_complex *out;

	// Create the plan
	fftw_plan p;

	vector<double> mySignal = parse_dat_file("signal_file.dat");

	vector<complex<double> > filter0 = parse_dat_file< complex<double> >("filter0_file.dat");
	vector<complex<double> > filter1 = parse_dat_file< complex<double> >("filter1_file.dat");

	// Get the size of the signal to dynamically allocate the arrays
	size_t N = mySignal.size();

	// Dynamically allocate the input and output arrays
	in = (double*) fftw_malloc(sizeof(double) * N);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	

	// Create the plan
	fftw_plan forward_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
	fftw_plan inverse_plan = fftw_plan_dft_c2r_1d(N, out, in, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);


	// Fill input array
	for (size_t i = 0; i < mySignal.size(); ++i)
		in[i] = mySignal.at(i);


	std::cout << "The original signal waveform:" << std::endl;
	std::copy ( in
			  , in + N
		      , std::ostream_iterator< complex<double> >(std::cout, ", ");


	// Explicitly perform the transform
	fftw_execute(forward_plan);

	// Apply the first filter
	for (size_t i = 0; i < filter0.size(); ++i)
		out[i] *= filter0.at(i);

	// Explicitly perform the transform
	fftw_execute(inverse_plan);

	std::cout << "After applying filter0:" << std::endl;
	std::copy ( in
			  , in + N
		      , std::ostream_iterator< complex<double> >(std::cout, ", ");

	// Explicitly perform the transform
	fftw_execute(forward_plan);

	// Apply the second filter
	for (size_t i = 0; i < filter1.size(); ++i)
		out[i] *= filter1.at(i);


	// Explicitly perform the transform
	fftw_execute(inverse_plan);

	std::cout << "After applying filter1:" << std::endl;
	std::copy ( in
			  , in + N
		      , std::ostream_iterator< complex<double> >(std::cout, ", ");
			  

	// Free the memory used by the dynamically allocated arrays and the plan.
	fftw_destroy_plan(forward_plan);
	fftw_destroy_plan(inverse_plan);
	fftw_free(in);
	fftw_free(out);
}

```

Waveform Implementation:

```C++
#include <vector>		// for our container of choice
#include <complex>		// for our complex type of choice
#include <string>
#include <iostream>		// for std::cout
#include <iterator>		// for std::ostream_iterator
#include <algorithm>	// for std::copy

#include <Waveform/Waveform.hpp>		// the standard Waveform header file
#include <Waveform/FftwTransform.hpp>	// the FFTW3 transform header file

typedef Waveform < vector<double>				// The type of the real-valued array
				, vector< complex<double> >		// The type of the complex-valued array
				, Waveform::Transform::Fftw3_Dft_1d_Normalized		// The type of transform we want to perform
				> 			WaveformType;


int main ()
{
	// Construct a Waveform instance from a vector filled by a file
	WaveformType mySignal ( parse_dat_file<double>("signal_file.dat") );

	// Create two filters, read in from files
	auto filter0 = parse_dat_file< complex<double> >("filter0_file.dat");
	auto filter1 = parse_dat_file< complex<double> >("filter1_file.dat");

	std::cout << "The original signal waveform:" << std::endl;
	std::copy ( mySignal.GetConstTimeSeries().begin()
				, mySignal.GetConstTimeSeries().end()
				, std::ostream_iterator< complex<double> >(std::cout, ", ");

	
	// Apply the first filter
	for (size_t i = 0; i < mySignal.GetConstFreqSpectrum().size(); ++i)
		mySignal.GetFreqSpectrum().at(i) *= filter0.at(i);

	// Print out the resulting signal waveform in the time domain
	std::cout << "After applying filter0:" << std::endl;
	std::copy ( mySignal.GetConstTimeSeries().begin()
				, mySignal.GetConstTimeSeries().end()
				, std::ostream_iterator< complex<double> >(std::cout, ", ");


	// Apply the second filter
	for (size_t i = 0; i < mySignal.GetConstFreqSpectrum().size(); ++i)
		mySignal.GetFreqSpectrum().at(i) *= filter1.at(i);


	// Print out the resulting signal waveform in the time domain
	std::cout << "After applying filter1:" << std::endl;
	std::copy ( mySignal.GetConstTimeSeries().begin()
				, mySignal.GetConstTimeSeries().end()
				, std::ostream_iterator< complex<double> >(std::cout, ", ");
}
```

## Documentation
Documentation for Waveform is of the form used by Doxygen and there is a Doxyfile located in doc/. Running "doxygen Doxyfile" will generate html documentation which is easily viewed by opening doc/html/index.html in a web browser.

The Doxygen website created leaves something to be desired, but other fixes are higher priority.

There is a (sometimes out-of-date) version of the Doxygen website available at [http://paulschellin.github.io/Waveform/](http://paulschellin.github.io/Waveform/).


## Why Use Waveform?

There are other libraries which simplify the use of transforms in signal processing, such as:

- [FFTW++](http://fftwpp.sourceforge.net/) has the goal of wrapping FFTW3 into a modern C++ interface
- [tspl](https://code.google.com/p/tspl/) is a set of header-only libraries for signal processing and numerical computation
- The Pierre Auger ["Offline Software Framework"](http://arxiv.org/abs/0707.1652) is the offline data analysis framework for the Pierre Auger Observatory, a high-energy cosmic ray detector
- [IT++](http://itpp.sourceforge.net/) a library of math, signal processing, and communication classes and functions
	
Of these libraries, only Auger's Offline software framework created an interface which made transforms automatic between domains, but it does so in a way that is difficult to separate from the rest of the framework. The interface it provides is not at all like any STL containers, which is something I tried to keep in mind while writing this library.

## Notes on Development

The features of this library are still in flux due to the decision to generalize the transforms with which Waveform is compatible. Many of the operators currently defined, especially the compound assignment operators (such as `operator +=`, `operator -=`, `operator *=`, and `operator /=`), were written with the assumption that the transform would be linear, but that cannot be guaranteed for all transforms which I would like to support.

It is likely that the compound assignment operators and other arithmetic operators will be deprecated because they no longer have a straightforward, obvious meaning when dealing with generalized transforms.

The functionality can probably be better provided by explicit friend functions which accept one Waveform and one other container as arguments. The details of these friend functions are not trivial and will take time to complete.

### Speak Up!
If you have any ideas, comments, criticism, or feature requests, please contact me via [email](paulschellin@gmail.com) or create an ["Issue"](https://github.com/paulschellin/Waveform/issues) on github and I'll work with you to improve this library.

### Considerations for Future Features

#### Lazy Evaluation and Optimization Through [Expression Templates](http://en.wikipedia.org/wiki/Expression_templates)

Make use of expression templates similar to the [Blitz++ library](http://blitz.sourceforge.net/) and [Armadillo](http://arma.sourceforge.net/).

This could be used to minimize the number of transforms that occur when manipulating both domains in a loop, for example.

#### Better Guarded Iterator Support

This was a feature at first, but earlier rapid development of the rest of the library made it difficult to maintain, so it was removed.

The iterator class needs to be able to notify the Waveform class when it has been modified, and this can be done by having separate Input and Output iterator types which are chosen at compile time using argument-dependent name lookup.

#### Compound Assignment Operators

These were also removed due to uncertainty as to how to indicate whether or not an operation would be a linear transform or not.

Perhaps if Transform classes have transform_type typedefs added to them it might be possible to implement the compound assignment operators in a sane fashion.
