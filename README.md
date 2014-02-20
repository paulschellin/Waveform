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

The test_Waveform.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The actual FFTW wrapper can be found in FftwTransform.hpp, which can be tested using the source file test_FftwTransform.cpp and the makefile Makefile.FftwTransform.

The tests can be run (assuming you have the Google Test framework installed) by typing:
	make -f Makefile.Waveform clean test
	make -f Makefile.FftwTransform clean test
	
