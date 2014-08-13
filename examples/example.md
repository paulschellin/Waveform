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

