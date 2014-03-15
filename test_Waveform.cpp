//
//
//		Set:
//
//	$ export GTEST_HOME=gtest-1.7.0/
//	$ export LD_LIBRARY_PATH=$GTEST_HOME/lib:$LD_LIBRARYPATH
//
//		Compile with:
//
//	g++ -std=c++11 -I. -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread test_Waveform.cpp -o test_Waveform
//
//
//


#include <iostream>
#include <fstream>
#include <vector>
#include <complex>

#include <Waveform.hpp>
#include <gtest/gtest.h>

#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/complex.hpp>

using namespace PS;

namespace {

typedef std::vector<double>		RealType;
typedef std::vector< std::complex<double> > ComplexType;
typedef Waveform<std::vector<double>, std::vector< std::complex<double> > > WaveformType;


class WaveformTest : public ::testing::Test {
	protected:
	
	WaveformTest()
	{

	}

	virtual
	~WaveformTest()
	{

	}

	virtual
	void
	SetUp()
	{
		/*
		std::ofstream ofst("bs_WaveformTest_tDomain_.txt");
		boost::archive::text_oarchive oat (ofst);
		oat & tDomain_;
	
		std::ofstream ofsf("bs_WaveformTest_fDomain_.txt");
		boost::archive::text_oarchive oaf (ofsf);
		oaf & fDomain_;
		*/

		std::ifstream ifs_tDomain("test_data/bs_WaveformTest_tDomain_.txt");
		boost::archive::text_iarchive ia_tDomain(ifs_tDomain);
		ia_tDomain >> tDomain_;

		std::ifstream ifs_fDomain("test_data/bs_WaveformTest_fDomain_.txt");
		boost::archive::text_iarchive ia_fDomain(ifs_fDomain);
		ia_fDomain >> fDomain_;

	}

	virtual
	void
	TearDown()
	{

	}

	
	RealType tDomain_;
		
	ComplexType fDomain_;
};



TEST_F(WaveformTest,FillConstructor)
{
	WaveformType filltest (512);

	EXPECT_EQ(512, filltest.size());


}

TEST_F(WaveformTest,CopyConstructor)
{

}

TEST_F(WaveformTest,TimeDomainRangeConstructor)
{
	WaveformType timedomaintest (tDomain_);

	EXPECT_EQ(tDomain_.size(), timedomaintest.size());

	for (unsigned i = 0; i < tDomain_.size(); ++i) {
		EXPECT_EQ(tDomain_[i] , timedomaintest.GetConstTimeSeries().at(i));
	}

}

TEST_F(WaveformTest,FreqDomainRangeConstructor)
{
	WaveformType freqdomaintest (fDomain_);

	EXPECT_EQ(fDomain_.size(), freqdomaintest.size()/2 + 1);
	
	for (unsigned i = 0; i < fDomain_.size(); ++i) {
		EXPECT_EQ(fDomain_[i] , freqdomaintest.GetConstFreqSpectrum().at(i));
	}

}

TEST_F(WaveformTest,CopyAssignOperator)
{
	

}

TEST_F(WaveformTest,AdditionAssignOperator)
{
	

}


TEST_F(WaveformTest,SubtractionAssignOperator)
{
	

}


TEST_F(WaveformTest,MultiplicationAssignOperator)
{
	

}


TEST_F(WaveformTest,DivisionAssignOperator)
{
	

}


TEST_F(WaveformTest,AdditionOperator)
{
	

}


TEST_F(WaveformTest,SubtractionOperator)
{
	

}


TEST_F(WaveformTest,MultiplicationOperator)
{
	

}


TEST_F(WaveformTest,DivisionOperator)
{
	

}


/*TEST_F(WaveformTest,Operator)
{
	g++ -std=c++11 -I. -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread test_Waveform.cpp -o test_Waveform

}


TEST_F(WaveformTest,Operator)
{
	

}*/

}	// namespace


int
main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
