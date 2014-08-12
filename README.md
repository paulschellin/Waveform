# Waveform
Written by Paul Schellin at the Ohio State University, 2012 to 2014.

Waveform is a C++ header-only library which represents both the time and the frequency domains of a waveform/signal as a single object, transforming using FFTW automatically when needed.

The Waveform class provides a safe, intuitive interface to allow a signal (or trace, or waveform) to be described in either the time domain or in the frequency domain, and manipulated in either domain without explicitly needing to convert the datatype from one domain to another.

This library attempts to parallel the nature of waveforms in that the time domain, frequency domain, S domain, etc. are simply different representations of a singular physical entity.

## Installation

### Dependencies

The dependencies of the Waveform class are:

	- C++11 compatible compiler
	- boost	(optional in some cases)
	- fftw (not necessary if you can match another library's interface to the one which Waveform was designed with)

Otherwise, the Waveform class was designed to be a header-only library, so simply including the header (`#include <Waveform.hpp>`) and compiling as C++11 (adding `-std=c++11` to your makefile's compiler flags) will be sufficient, assuming FFTW3 was already being used and was properly configured. If FFTW3 was not used before, you must add `-lfftw3` to the linker flags.


### Download the Waveform library
To download this library from github, simply type `git clone https://github.com/paulschellin/Waveform.git` in a terminal to clone the library to the current directory, or click the "Download ZIP" button to the right to have your browser take care of the download, then unzip it where you need it.


### Verify "Installation"
The Waveform library is ["header-only"](http://en.wikipedia.org/wiki/Header-only), so it does not require "installation" in the typical sense. But I've included the unit tests to help verify that your development environment is configured properly to use the Waveform library in your projects.

The test_src/Waveform_test.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The actual FFTW wrapper can be found in FftwTransform.hpp, which can be tested using the source file FftwTransform_test.cpp.

You must have the [Google Test framework](https://code.google.com/p/googletest/) installed in order to run these tests.

NOTE: Google recommends just dropping the test framework directory within the directory of the project you wish to test, so in this case, you would put "gtest-1.7.0" (or similar) into the Waveform/ directory.

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


This can be reduced to this:

```C++
#include <vector>		// for our container of choice
#include <complex>		// for our complex type of choice

#include <iostream>		// for std::cout
#include <iterator>		// for std::ostream_iterator
#include <algorithm>	// for std::copy

#include <Waveform/Waveform.hpp>		// the standard Waveform header file
#include <Waveform/FftwTransform.hpp>	// the FFTW3 transform header file

using namespace std;

// If we only want to use a single kind of transform (in this case the same _dft_1d as we used above), then it's easiest to typedef the type to "WaveformType":

typedef Waveform < vector<double>				// The type of the real-valued array
				, vector< complex<double> >		// The type of the complex-valued array
				, Waveform::Transform::Fftw3_Dft_1d		// The type of transform we want to perform
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

	std::copy	( myWfm.GetConstFreqSpectrum.begin()
				, myWfm.GetConstFreqSpectrum.end()
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


## Documentation
Documentation for Waveform is of the form used by Doxygen and there is a Doxyfile located in doc/. Running "doxygen Doxyfile" will generate html documentation which is easily viewed by opening doc/html/index.html in a web browser.

The Doxygen website created leaves something to be desired, but other fixes are higher priority.

There is a (sometimes out-of-date) version of the Doxygen website available at [http://paulschellin.github.io/Waveform/](http://paulschellin.github.io/Waveform/).


## Notes on Development

The features of this library are still in flux due to the decision to generalize the transforms with which Waveform is compatible. Many of the operators currently defined, especially the compound assignment operators (such as `operator +=`, `operator -=`, `operator *=`, and `operator /=`), were written with the assumption that the transform would be linear, but that cannot be guaranteed for all transforms which I would like to support.

It is likely that the compound assignment operators and other arithmetic operators will be deprecated because they no longer have a straightforward, obvious meaning when dealing with generalized transforms.

The functionality can probably be better provided by explicit friend functions which accept one Waveform and one other container as arguments. The details of these friend functions are not trivial and will take time to complete.
	
