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

