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

