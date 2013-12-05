Waveform
========

Waveform is a C++ header-only library which represents both the time and the frequency domains of a waveform/signal as a single object, transforming using FFTW automatically when needed.

Written by Paul Schellin at the Ohio State University, 2012 to 2013.

The Waveform class provides a safe, intuitive interface to allow a signal (or trace, or waveform) to be described in either the time domain or in the frequency domain, and manipulated in either domain without explicitly needing to convert the datatype from one domain to another.

Notes on installation:
	This library has several options for compilation, not all of which are fully implemented. The options are as follows:

		WAVEFORM_USE_INTERP			This option enables interpolation for values in both the time and frequency domains. This uses GSL's interpolation functions as well as my psGSLInterpExtension.hpp file as a wrapper. If Waveform used as a template class, interpolation will cause Waveform to fail to link against GSL if in template <typename T> class Waveform {...}, T is not either 'float' or 'double'. 

		WAVEFORM_INTERP_STATE		This option, used only in conjunction with WAVEFORM_USE_INTERP, keeps a local state for the interpolation functions, increasing memory requirements but improving performance when many interpolations are required. This option has not been completely implemented yet.


		WAVEFORM_ALLOW_EXPOSURE		This macro enables the vectors containing the raw time and frequency domains to be returned to the user, putting the domain at risk for being modified without the Waveform class being aware. Specifically, it enables the functions Waveform::GetTimeDomain() and Waveform::GetFreqDomain(). Alternately, please use Waveform::GetTimeDomainCopy() and Waveform::GetFreqDomainCopy(). Recommend this option be disabled unless absolutely necessary.

		WAVEFORM_USE_CRC			This macro, used only in conjunction with WAVEFORM_ALLOW_EXPOSURE, uses the boost crc library to determine if either the time domain or the frequency domain has changed since it was last marked as valid. This is less resource intensive than keeping an old copy of the domain and comparing element-to-element each time. This option has not been completely implemented yet. Requires boost/crc.hpp.

	
The dependencies of the Waveform class are:
	-	fftw3
	-	boost	(optional in some cases)

	Otherwise, the Waveform class was designed to be a header-only library, so simply including Waveform.hpp and compiling as C++11 should be sufficient.

Documentation for Waveform is of the form used by Doxygen (and incidentally some Java-style documentation standards), and there is a Doxyfile located in doc/. Running "doxygen doc/Doxyfile" will generate html documentation which is easily viewed by opening doc/html/index.html in a web browser.

This class was designed to be a template class, but for speed of development and testing, it was not made a template class. By simply uncommenting the "//template <typename T>" line above the Waveform class declaration, adding a typename of WaveformT as the "Waveform <typename T>" type, replacing all scope labels with WaveformT, and commenting out the "typedef double T;" line, the class will be a template class.

The WaveformTest.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The file test_of_waveform.cpp is the file currently compiled by the makefile, and it contains various runtime tests.
