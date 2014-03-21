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

/*
	To Do:
	
		[ ]	Use Global Environment functionality for the test arrays loaded from files
		[ ] Use type-parameterized tests so that any container can be tested without
				needing to duplicate code
		[ ]	Use value-parameterized tests so that test arrays can be individually passed
				to the tests for verification
 */


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
		std::ofstream ofst("bs_WaveformTest_tDomainExampleData_.txt");
		boost::archive::text_oarchive oat (ofst);
		oat & tDomainExampleData_;
	
		std::ofstream ofsf("bs_WaveformTest_fDomainExampleData_.txt");
		boost::archive::text_oarchive oaf (ofsf);
		oaf & fDomainExampleData_;
		*/

		std::ifstream ifs_tDomain("test_data/bs_WaveformTest_tDomain_.txt");
		boost::archive::text_iarchive ia_tDomain(ifs_tDomain);
		ia_tDomain >> tDomainExampleData_;

		std::ifstream ifs_fDomain("test_data/bs_WaveformTest_fDomain_.txt");
		boost::archive::text_iarchive ia_fDomain(ifs_fDomain);
		ia_fDomain >> fDomainExampleData_;


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


	}

	virtual
	void
	TearDown()
	{

	}

	
	RealType tDomainExampleData_;
		
	ComplexType fDomainExampleData_;



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


	
	//RealType tDomain_;
};



TEST_F(WaveformTest,FillConstructor)
{
	WaveformType filltest (512);

	EXPECT_EQ(512, filltest.size());


}

TEST_F(WaveformTest,CopyConstructor)
{
	WaveformType originalWfm (tDomainExampleData_);

	
	WaveformType copiedWfm (originalWfm);
	
	EXPECT_EQ(copiedWfm.size(), originalWfm.size());

	for (unsigned i = 0; i < copiedWfm.size(); ++i) {
		EXPECT_EQ(copiedWfm.GetConstTimeSeries().at(i) , originalWfm.GetConstTimeSeries().at(i));
	}

	EXPECT_EQ(copiedWfm.size(), originalWfm.size());

	for (unsigned i = 0; i < copiedWfm.GetConstFreqSpectrum().size(); ++i) {
		EXPECT_EQ(copiedWfm.GetConstFreqSpectrum().at(i) , originalWfm.GetConstFreqSpectrum().at(i));
	}

}

TEST_F(WaveformTest,TimeDomainRangeConstructor)
{
	WaveformType timedomaintest (tDomainExampleData_);

	EXPECT_EQ(tDomainExampleData_.size(), timedomaintest.size());

	for (unsigned i = 0; i < tDomainExampleData_.size(); ++i) {
		EXPECT_EQ(tDomainExampleData_[i] , timedomaintest.GetConstTimeSeries().at(i));
	}

}

TEST_F(WaveformTest,FreqDomainRangeConstructor)
{
	WaveformType freqdomaintest (fDomainExampleData_);

	EXPECT_EQ(fDomainExampleData_.size(), freqdomaintest.size()/2 + 1);
	
	for (unsigned i = 0; i < fDomainExampleData_.size(); ++i) {
		EXPECT_EQ(fDomainExampleData_[i] , freqdomaintest.GetConstFreqSpectrum().at(i));
	}

}

TEST_F(WaveformTest,CopyAssignOperator)
{
	
	ADD_FAILURE() << "Not Yet Implemented!";

/*
	WaveformType originalWfm (tDomainExampleData_);

	
	WaveformType copiedWfm (originalWfm.size());
	
	//copiedWfm = originalWfm;
	
	EXPECT_EQ(copiedWfm.size(), originalWfm.size());

	for (unsigned i = 0; i < copiedWfm.size(); ++i) {
		EXPECT_EQ(copiedWfm.GetConstTimeSeries().at(i) , originalWfm.GetConstTimeSeries().at(i));
	}

	EXPECT_EQ(copiedWfm.size(), originalWfm.size());

	for (unsigned i = 0; i < copiedWfm.GetConstFreqSpectrum().size(); ++i) {
		EXPECT_EQ(copiedWfm.GetConstFreqSpectrum().at(i) , originalWfm.GetConstFreqSpectrum().at(i));
	}
*/
}

/*
TEST_F(WaveformTest,AdditionAssignOperator)
{
	//ADD_FAILURE() << "Not Yet Implemented!";
	
	WaveformType lhs (stepFn1024_real);

	WaveformType rhs (triangleFn1024_real);

	lhs += rhs;

	for (unsigned i (0); i < lhs.size(); ++i) {
		EXPECT_EQ(lhs.GetConstTimeSeries().at(i), stepFn1024_real.at(i) + triangleFn1024_real.at(i));
	}
}


TEST_F(WaveformTest,SubtractionAssignOperator)
{
	
	//ADD_FAILURE() << "Not Yet Implemented!";
	WaveformType lhs (stepFn1024_real);

	WaveformType rhs (triangleFn1024_real);

	lhs -= rhs;

	for (unsigned i (0); i < lhs.size(); ++i) {
		EXPECT_EQ(lhs.GetConstTimeSeries().at(i), stepFn1024_real.at(i) - triangleFn1024_real.at(i));
	}

}


TEST_F(WaveformTest,MultiplicationAssignOperator)
{
	
	//ADD_FAILURE() << "Not Yet Implemented!";
	WaveformType lhs (stepFn1024_real);

	WaveformType rhs (triangleFn1024_real);

	lhs *= rhs;

	for (unsigned i (0); i < lhs.size(); ++i) {
		EXPECT_EQ(lhs.GetConstTimeSeries().at(i), stepFn1024_real.at(i) * triangleFn1024_real.at(i));
	}

}


TEST_F(WaveformTest,DivisionAssignOperator)
{
	
	//ADD_FAILURE() << "Not Yet Implemented!";
	WaveformType lhs (stepFn1024_real);

	WaveformType rhs (std::vector<double>(stepFn1024_real.size(), 2.));

	lhs /= rhs;

	for (unsigned i (0); i < lhs.size(); ++i) {
		EXPECT_EQ(lhs.GetConstTimeSeries().at(i), stepFn1024_real.at(i) / 2. );
	}

}


TEST_F(WaveformTest,AdditionOperator)
{
	
	ADD_FAILURE() << "Not Yet Implemented!";

}


TEST_F(WaveformTest,SubtractionOperator)
{
	
	ADD_FAILURE() << "Not Yet Implemented!";

}


TEST_F(WaveformTest,MultiplicationOperator)
{
	
	ADD_FAILURE() << "Not Yet Implemented!";

}


TEST_F(WaveformTest,DivisionOperator)
{
	
	ADD_FAILURE() << "Not Yet Implemented!";

}
*/

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
