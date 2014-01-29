Waveform
========

Waveform is a C++ header-only library which represents both the time and the frequency domains of a waveform/signal as a single object, transforming using FFTW automatically when needed.

Written by Paul Schellin at the Ohio State University, 2012 to 2013.

The Waveform class provides a safe, intuitive interface to allow a signal (or trace, or waveform) to be described in either the time domain or in the frequency domain, and manipulated in either domain without explicitly needing to convert the datatype from one domain to another.


Documentation/Reference available --  http://paulschellin.github.io/Waveform/


Notes on installation:

	
The dependencies of the Waveform class are:

	- boost	(optional in some cases)


Otherwise, the Waveform class was designed to be a header-only library, so simply including Waveform.hpp and compiling as C++11 should be sufficient.

Documentation for Waveform is of the form used by Doxygen (and incidentally some Java-style documentation standards), and there is a Doxyfile located in doc/. Running "doxygen doc/Doxyfile" will generate html documentation which is easily viewed by opening doc/html/index.html in a web browser.


The WaveformTest.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The file test_of_waveform.cpp is the file currently compiled by the makefile, and it contains various runtime tests.
