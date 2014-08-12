# Waveform

Waveform is a C++ header-only library which represents both the time and the frequency domains of a waveform/signal as a single object, transforming using FFTW automatically when needed.

Written by Paul Schellin at the Ohio State University, 2012 to 2014.

The Waveform class provides a safe, intuitive interface to allow a signal (or trace, or waveform) to be described in either the time domain or in the frequency domain, and manipulated in either domain without explicitly needing to convert the datatype from one domain to another.

This library attempts to parallel the nature of waveforms in that the time domain, frequency domain, S domain, etc. are simply different representations of a singular physical entity.

Documentation/Reference available --  http://paulschellin.github.io/Waveform/


## Installation

### Dependencies

The dependencies of the Waveform class are:

	- C++11 compatible compiler
	- boost	(optional in some cases)
	- fftw (not necessary if you can match another library's interface to the one which Waveform was designed with)

Otherwise, the Waveform class was designed to be a header-only library, so simply including Waveform.hpp and compiling as C++11 should be sufficient.


### Download the Waveform library
To download this library from github, simply type `git clone https://github.com/paulschellin/Waveform.git` in a terminal to clone the library to the current directory, or click the "Download ZIP" button to the right to have your browser take care of the download, then unzip it where you need it.


### Verify "Installation"
The test_src/Waveform_test.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The actual FFTW wrapper can be found in FftwTransform.hpp, which can be tested using the source file FftwTransform_test.cpp.

The tests can be run (assuming you have the [Google Test framework](https://code.google.com/p/googletest/) installed[^footnoteongoogletest]) by typing:

[^footnoteongoogletest]: Google recommends just dropping the test framework directory within the directory of the project you wish to test, so in this case, you would put "gtest-1.7.0" (or similar) into the Waveform/ directory.

```Shell
make clean Waveform
make clean IdentityTransform
make clean FftwTransform[^footnoteonfftw]
./test_bin/Waveform_test
./test_bin/IdentityTransform_test
./test_bin/FftwTransform_test[^footnoteonfftw]
```

[^footnoteonfftw]: In order to run the FFTW tests, you must have the FFTW3 library installed. I highly recommend using your package manager (such as apt-get on Debian/Ubuntu, pacman on Arch, yum on Fedora, Homebrew on Mac OS X, etc.) to install it instead of installing it manually because your package manager will set up the linking and include directories automatically and appropriately.

## Documentation
Documentation for Waveform is of the form used by Doxygen and there is a Doxyfile located in doc/. Running "doxygen Doxyfile" will generate html documentation which is easily viewed by opening doc/html/index.html in a web browser.

The Doxygen website created leaves something to be desired, but other fixes are higher priority.

There is a (sometimes out-of-date) version of the Doxygen website available at [http://paulschellin.github.io/Waveform/](http://paulschellin.github.io/Waveform/).


## Notes on Development

The features of this library are still in flux due to the decision to generalize the transforms with which Waveform is compatible. Many of the operators currently defined, especially the compound assignment operators (such as `operator +=`, `operator -=`, `operator *=`, and `operator /=`), were written with the assumption that the transform would be linear, but that cannot be guaranteed for all transforms which I would like to support.

It is likely that the compound assignment operators and other arithmetic operators will be deprecated because they no longer have a straightforward, obvious meaning when dealing with generalized transforms.

The functionality can probably be better provided by explicit friend functions which accept one Waveform and one other container as arguments. The details of these friend functions are not trivial and will take time to complete.
	
