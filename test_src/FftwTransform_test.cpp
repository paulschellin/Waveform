#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <iterator>
#include <algorithm>
#include <functional>


#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

#include <FftwTransform.hpp>
#include <Waveform.hpp>

#include <gtest/gtest.h>


#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/complex.hpp>


/*
	To Do:
	
		[ ]	Use Global Environment functionality for the test arrays loaded from files
		[ ] Use type-parameterized tests so that any container can be tested without
				needing to duplicate code
		[ ]	Use value-parameterized tests so that test arrays can be individually passed
				to the tests for verification
 */



namespace {

template <typename T>
std::vector<T>
parse_dat_file (std::string fileName)
{
	std::ifstream ifs(fileName.c_str());
	assert(ifs.good());


	std::vector<T> result { std::istream_iterator<T>(ifs)
						  , std::istream_iterator<T>() };

	return result;
}


class FftwTransformTest : public ::testing::Test {
	protected:

	FftwTransformTest()
	{

	}

	virtual
	~FftwTransformTest()
	{

	}

	virtual
	void
	SetUp()
	{
		/*
		fDomain_.resize(tDomain_.size());
	

		std::ofstream ofst("bs_FftwTransformTest_tDomain_.txt");
		boost::archive::text_oarchive oat (ofst);
		oat & tDomain_;
	
		std::ofstream ofsf("bs_FftwTransformTest_fDomain_.txt");
		boost::archive::text_oarchive oaf (ofsf);
		oaf & fDomain_;
		*/

		std::ifstream ifst("test_data/bs_FftwTransformTest_tDomain_.txt");
		boost::archive::text_iarchive iat (ifst);
		iat & tDomain_;
	
		std::ifstream ifsf("test_data/bs_FftwTransformTest_fDomain_.txt");
		boost::archive::text_iarchive iaf (ifsf);
		iaf & fDomain_;


		stepFn1024_real = parse_dat_file<double>("test_data/stepFn1024_real.dat");
		stepFn1024_complex = parse_dat_file<std::complex<double> >("test_data/stepFn1024_complex.dat");

		diracFn1024_real = parse_dat_file<double>("test_data/diracFn1024_real.dat");
		diracFn1024_complex = parse_dat_file<std::complex<double> >("test_data/diracFn1024_complex.dat");

		triangleFn1024_real = parse_dat_file<double>("test_data/triangleFn1024_real.dat");
		triangleFn1024_complex = parse_dat_file<std::complex<double> >("test_data/triangleFn1024_complex.dat");

		squareFn1024_real = parse_dat_file<double>("test_data/squareFn1024_real.dat");
		squareFn1024_complex = parse_dat_file<std::complex<double> >("test_data/squareFn1024_complex.dat");


		testArrays_real.push_back(stepFn1024_real);
		testArrays_real.push_back(diracFn1024_real);
		testArrays_real.push_back(triangleFn1024_real);
		testArrays_real.push_back(squareFn1024_real);

		testArrays_complex.push_back(stepFn1024_complex);
		testArrays_complex.push_back(diracFn1024_complex);
		testArrays_complex.push_back(triangleFn1024_complex);
		testArrays_complex.push_back(squareFn1024_complex);


		/*
		for (auto ar : testArrays_real)
			std::cout << ar.size() << std::endl;

		for (auto ar : testArrays_complex)
			std::cout << ar.size() << std::endl;
		*/

	}

	virtual
	void
	TearDown()
	{

	}

	//blah
	
	double nearVal = 0.00001;

	std::vector<double> tDomain_;


	std::vector< std::complex<double> > fDomain_;



	std::vector<double> 				stepFn1024_real;
	std::vector< std::complex<double> > stepFn1024_complex;

	std::vector<double> 				diracFn1024_real;
	std::vector< std::complex<double> > diracFn1024_complex;

	std::vector<double> 				triangleFn1024_real;
	std::vector< std::complex<double> > triangleFn1024_complex;
	
	std::vector<double> 				squareFn1024_real;
	std::vector< std::complex<double> > squareFn1024_complex;

	std::vector< std::vector<double> >					testArrays_real;
	std::vector< std::vector<std::complex<double> > > 	testArrays_complex;

};



/*
typedef testing::Types<Fftw3_Dft_1d, Fftw3_Dft_1d_Normalized> Implementations;
TYPED_TEST_CASE(FftwTransformTest, Implementations);
*/

TEST_F(FftwTransformTest, CTor1)
{
	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;

	std::vector< std::complex<double> > fresult (tDomain_.size() / 2 + 1);
	FftwTransform myFT = FftwTransform(tDomain_.begin(), tDomain_.end(), fresult.begin());

	myFT.exec_transform();


	for (unsigned iter (0); iter < fresult.size(); ++iter)
	{
		/*
		EXPECT_DOUBLE_EQ( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)) ) << "\t@\t" << iter;
		EXPECT_DOUBLE_EQ( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)) ) << "\t@\t" << iter;
		*/
		
		EXPECT_NEAR( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		EXPECT_NEAR( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		
	}
}


TEST_F(FftwTransformTest, CTor2)
{

	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;
	
	std::vector< std::complex<double> > fresult (tDomain_.size() / 2 + 1);
	FftwTransform myFT = FftwTransform(tDomain_, fresult);


	myFT.exec_transform();


	for (unsigned iter (0); iter < fresult.size(); ++iter)
	{
		/*
		EXPECT_DOUBLE_EQ( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)) ) << "\t@\t" << iter;
		EXPECT_DOUBLE_EQ( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)) ) << "\t@\t" << iter;
		*/
		
		EXPECT_NEAR( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		EXPECT_NEAR( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		
	}


}



TEST_F(FftwTransformTest, FwTrans)
{
	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;
	
	std::vector< std::complex<double> > fresult (tDomain_.size() / 2 + 1);
	FftwTransform myFT = FftwTransform(tDomain_.begin(), tDomain_.end(), fresult.begin());

	myFT.exec_transform();


	for (unsigned iter (0); iter < fresult.size(); ++iter)
	{
		/*
		EXPECT_DOUBLE_EQ( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)) ) << "\t@\t" << iter;
		EXPECT_DOUBLE_EQ( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)) ) << "\t@\t" << iter;
		*/
		
		EXPECT_NEAR( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		EXPECT_NEAR( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		
	}


/*
	std::cout << "Time Domain:" << std::endl;
	boost::copy(tDomain_, std::ostream_iterator< double >(std::cout, "\n"));
	
	std::cout << std::endl << std::endl
		<< "Freq Domain:" << std::endl;

	boost::copy(fDomain_, std::ostream_iterator< std::complex<double> >(std::cout, "\n"));
	*/


}


TEST_F(FftwTransformTest, FwTransTestArrays)
{
	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;

	//double new_nearVal = nearVal * 1.;

	for (std::size_t iter_array (0); iter_array < testArrays_real.size(); ++iter_array) {
		std::vector< std::complex<double> > fresult (testArrays_real.at(iter_array).size() / 2 + 1);

		FftwTransform myFT = FftwTransform(testArrays_real.at(iter_array).begin(), testArrays_real.at(iter_array).end(), fresult.begin());

		myFT.exec_transform();

		EXPECT_EQ (fresult.size(), testArrays_complex.at(iter_array).size()) << iter_array;

		for (unsigned iter (0); iter < fresult.size(); ++iter)
		{
		/*
		EXPECT_DOUBLE_EQ( std::real(fresult.at(iter)), std::real(fDomain_.at(iter)) ) << "\t@\t" << iter;
		EXPECT_DOUBLE_EQ( std::imag(fresult.at(iter)), std::imag(fDomain_.at(iter)) ) << "\t@\t" << iter;
		*/
		
		EXPECT_NEAR( std::real(fresult.at(iter)), std::real(testArrays_complex.at(iter_array).at(iter)), nearVal ) << "\t@\t" << iter;
		EXPECT_NEAR( std::imag(fresult.at(iter)), std::imag(testArrays_complex.at(iter_array).at(iter)), nearVal ) << "\t@\t" << iter;
		
		}
	}
}



TEST_F(FftwTransformTest, InvTrans)
{

	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;
	
	//std::vector<double> tresult ( 2 * (fDomain_.size() - 1) );
	std::vector<double> tresult (tDomain_.size());
	FftwTransform myFT = FftwTransform(tresult.begin(), tresult.end(), fDomain_.begin());

	myFT.exec_inverse_transform();

	ASSERT_EQ( tresult.size(), tDomain_.size()) << "\tfDomain size:\t" << fDomain_.size();

	
	for (auto& val : tresult)
	{
		val /= double(tresult.size());
	}
	
	for (unsigned iter (0); iter < tresult.size(); ++iter)
	{
		EXPECT_NEAR( tresult.at(iter), tDomain_.at(iter), nearVal ) << "\t@\t" << iter << "\tFactor:\t" << tresult.at(iter) / tDomain_.at(iter);
	}

}



TEST_F(FftwTransformTest, ParameterForWaveform)
{
	//std::vector< std::complex<double> > fresult (tDomain_.size() / 2 + 1);

	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;
	
	PS::Waveform< std::vector<double>
				, std::vector< std::complex<double> >
				, FftwTransform
				> myWfm (tDomain_);

	EXPECT_EQ(tDomain_.size(), myWfm.size());


	for (unsigned iter (0); iter < myWfm.size(); ++iter)
	{
		EXPECT_EQ(tDomain_.at(iter), myWfm.GetConstTimeSeries().at(iter));
	}
}



TEST_F(FftwTransformTest, FwdTransformInWaveform)
{
	
	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;

	//std::vector< std::complex<double> > fresult (tDomain_.size() / 2 + 1);

	PS::Waveform< std::vector<double>
				, std::vector< std::complex<double> >
				, FftwTransform
				> myWfm (tDomain_);

	myWfm.GetConstFreqSpectrum();

	// Because fDomain comes from Mathematica, there are around twice the number of bins as in FFTW
	EXPECT_EQ(fDomain_.size() / 2 + 1, myWfm.GetConstFreqSpectrum().size());

	for (unsigned iter (0); iter < myWfm.GetConstFreqSpectrum().size(); ++iter)
	{

		EXPECT_NEAR( std::real(myWfm.GetConstFreqSpectrum().at(iter)), std::real(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		EXPECT_NEAR( std::imag(myWfm.GetConstFreqSpectrum().at(iter)), std::imag(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;

	}
}

TEST_F(FftwTransformTest, RvsTransformInWaveform)
{
	
	typedef Waveform::Transform::Fftw3_Dft_1d FftwTransform;
	
	std::vector<std::complex<double> > fDomainProper (fDomain_.begin(), fDomain_.begin() + 131);

	PS::Waveform< std::vector<double>
				, std::vector< std::complex<double> >
				, FftwTransform
				> myWfm (fDomainProper);

	myWfm.GetConstTimeSeries();

	/*
	for (unsigned iter (0); iter < myWfm.GetConstFreqSpectrum().size(); ++iter)
	{
		EXPECT_NEAR( std::real(myWfm.GetConstFreqSpectrum().at(iter)), std::real(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
		EXPECT_NEAR( std::imag(myWfm.GetConstFreqSpectrum().at(iter)), std::imag(fDomain_.at(iter)), nearVal ) << "\t@\t" << iter;
	}
	*/


	//std::transform(myWfm.beginTime(), myWfm.endTime(), myWfm.beginTime(), [](auto x){return x/myWfm.size()});

	for (auto& val : myWfm.GetTimeSeries())
	{
		val /= myWfm.size();
	}

	EXPECT_EQ(tDomain_.size(), myWfm.GetConstTimeSeries().size());

	for (unsigned iter (0); iter < myWfm.GetConstTimeSeries().size(); ++iter)
	{
		EXPECT_NEAR(myWfm.GetTimeSeries().at(iter), tDomain_.at(iter), nearVal) << "\t@\t" << iter;
	}
}


}	// namespace

int
main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


