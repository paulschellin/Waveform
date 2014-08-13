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

