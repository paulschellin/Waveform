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


//	From http://stackoverflow.com/questions/1460703/comparison-of-arrays-in-google-test
//! Using the google test framework, check all elements of two containers
#define EXPECT_ITERABLE_BASE( PREDICATE, REFTYPE, TARTYPE, ref, target) \
    { \
    const REFTYPE& ref_(ref); \
    const TARTYPE& target_(target); \
    REFTYPE::const_iterator refIter = ref_.begin(); \
    TARTYPE::const_iterator tarIter = target_.begin(); \
    unsigned int i = 0; \
    while(refIter != ref_.end()) { \
        if ( tarIter == target_.end() ) { \
            ADD_FAILURE() << #target " has a smaller length than " #ref ; \
            break; \
        } \
        PREDICATE(* refIter, * tarIter) \
            << "Containers " #ref  " (refIter) and " #target " (tarIter)" \
               " differ at index " << i; \
        ++refIter; ++tarIter; ++i; \
    } \
    EXPECT_TRUE( tarIter == target_.end() ) \
        << #ref " has a smaller length than " #target ; \
    }

//! Check that all elements of two same-type containers are equal
#define EXPECT_ITERABLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, TYPE, TYPE, ref, target )

//! Check that all elements of two different-type containers are equal
#define EXPECT_ITERABLE_EQ2( REFTYPE, TARTYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, REFTYPE, TARTYPE, ref, target )

//! Check that all elements of two same-type containers of doubles are equal
#define EXPECT_ITERABLE_DOUBLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_DOUBLE_EQ, TYPE, TYPE, ref, target )


//	Modified EXPECT_ITERABLE_BASE so that it doesn't need type specifiers
#define EXPECT_ITERABLE_BASE_NOTYPE( PREDICATE, ref, target) \
    { \
	typedef decltype(ref) REFTYPE; \
	typedef decltype(target) TARTYPE; \
    const REFTYPE& ref_(ref); \
    const TARTYPE& target_(target); \
    auto /*typename REFTYPE::const_iterator*/ refIter = ref_.begin(); \
    auto /*typename TARTYPE::const_iterator*/ tarIter = target_.begin(); \
    unsigned int i = 0; \
    while(refIter != ref_.end()) { \
        if ( tarIter == target_.end() ) { \
            ADD_FAILURE() << #target " has a smaller length than " #ref ; \
            break; \
        } \
        PREDICATE(* refIter, * tarIter) \
            << "Containers " #ref  " (refIter) and " #target " (tarIter)" \
               " differ at index " << i; \
        ++refIter; ++tarIter; ++i; \
    } \
    EXPECT_TRUE( tarIter == target_.end() ) \
        << #ref " has a smaller length than " #target ; \
    }

/*
#define EXPECT_COMPLEX_EQ(expected, actual)\
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<std::complex<double> >, \
                      expected, actual)


#define ASSERT_COMPLEX_EQ(expected, actual)\
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<std::complex<double> >, \
                      expected, actual)
*/


#define EXPECT_ITERABLE_EQ_NOTYPE( ref, target) \
	EXPECT_ITERABLE_BASE_NOTYPE(EXPECT_EQ, ref, target )

#define EXPECT_ITERABLE_FLOAT_EQ_NOTYPE( ref, target) \
	EXPECT_ITERABLE_BASE_NOTYPE(EXPECT_FLOAT_EQ, ref, target )
#define EXPECT_ITERABLE_DOUBLE_EQ_NOTYPE( ref, target) \
	EXPECT_ITERABLE_BASE_NOTYPE(EXPECT_DOUBLE_EQ, ref, target )
/*
#define EXPECT_ITERABLE_COMPLEX_EQ_NOTYPE( ref, target) \
	EXPECT_ITERABLE_BASE_NOTYPE(EXPECT_COMPLEX_EQ, ref, target )
*/

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
	

	EXPECT_ITERABLE_DOUBLE_EQ_NOTYPE(originalWfm.GetConstTimeSeries(), copiedWfm.GetConstTimeSeries());

	EXPECT_ITERABLE_EQ_NOTYPE(originalWfm.GetConstFreqSpectrum(), copiedWfm.GetConstFreqSpectrum());

	/*
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

TEST_F(WaveformTest,TimeDomainRangeConstructor)
{
	WaveformType timedomaintest (tDomainExampleData_);

	EXPECT_ITERABLE_DOUBLE_EQ_NOTYPE(tDomainExampleData_, timedomaintest.GetConstTimeSeries());
}

TEST_F(WaveformTest,FreqDomainRangeConstructor)
{
	WaveformType freqdomaintest (fDomainExampleData_);

	//
	//	Approach #1
	//
	
	EXPECT_EQ(fDomainExampleData_.size(), freqdomaintest.size()/2 + 1);
	
	for (unsigned i = 0; i < fDomainExampleData_.size(); ++i) {
		EXPECT_EQ(fDomainExampleData_[i] , freqdomaintest.GetConstFreqSpectrum().at(i));
	}


	//
	//	Approach #2
	//

	EXPECT_EQ(fDomainExampleData_, freqdomaintest.GetConstFreqSpectrum());


	//
	//	Approach #3
	//

	EXPECT_TRUE(fDomainExampleData_ == freqdomaintest.GetConstFreqSpectrum());


	//
	//	Approach #4
	//

	EXPECT_ITERABLE_EQ(decltype(fDomainExampleData_), fDomainExampleData_, freqdomaintest.GetConstFreqSpectrum());
	
	
	//
	//	Approach #5
	//

	EXPECT_ITERABLE_EQ_NOTYPE(fDomainExampleData_, freqdomaintest.GetConstFreqSpectrum());

}

TEST_F(WaveformTest,CopyAssignOperator)
{
	
	ADD_FAILURE() << "Not Yet Implemented!";
}

};	//	namespace

int
main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
