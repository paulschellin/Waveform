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



namespace {

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
};



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


