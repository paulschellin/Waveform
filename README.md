Waveform
========

Waveform is a C++ header-only library which represents both the time and the frequency domains of a waveform/signal as a single object, transforming using FFTW automatically when needed.

Written by Paul Schellin at the Ohio State University, 2012 to 2014.

The Waveform class provides a safe, intuitive interface to allow a signal (or trace, or waveform) to be described in either the time domain or in the frequency domain, and manipulated in either domain without explicitly needing to convert the datatype from one domain to another.

The idea is that each of the domains are simply a representation of the same phenomena, a feature which I tried to model in the design of this library.


Documentation/Reference available --  http://paulschellin.github.io/Waveform/


Notes on installation:

	
The dependencies of the Waveform class are:

	- C++11 compatible compiler
	- boost	(optional in some cases)
	- fftw (not necessary if you can match another library's interface to the one which Waveform was designed with)


Otherwise, the Waveform class was designed to be a header-only library, so simply including Waveform.hpp and compiling as C++11 should be sufficient.

Documentation for Waveform is of the form used by Doxygen and there is a Doxyfile located in doc/. Running "doxygen doc/Doxyfile" will generate html documentation which is easily viewed by opening doc/html/index.html in a web browser.

The Doxygen website created leaves something to be desired, but other fixes are higher priority.

The Waveform_test.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The actual FFTW wrapper can be found in FftwTransform.hpp, which can be tested using the source file FftwTransform_test.cpp.

The tests can be run (assuming you have the Google Test framework installed) by typing:
	make clean Waveform
	make clean FftwTransform
	./test_bin/Waveform_test
	./test_bin/FftwTransform_test


The features of this library are still in flux due to the decision to generalize the transforms with which Waveform is compatible. Many of the operators currently defined, especially the compound assignment operators were written with the assumption that the transform would be linear, but that cannot be guaranteed. It is likely that the compound assignment operators and other arithmetic operators will be deprecated because they no longer have obvious meaning when dealing with generalized transforms. The functionality can probably be better provided by explicit friend functions which accept one Waveform and one other container as arguments. The details of these friend functions are not trivial and will take time to complete.
	
