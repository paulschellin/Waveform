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



#include <vector>
#include <complex>

#include <Waveform.hpp>
#include <gtest/gtest.h>


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
		
	}

	virtual
	void
	TearDown()
	{

	}

	
	const RealType tDomain_{0.000069723, 0.0000669801, 0.0000649373, 0.0000629272, 0.0000612033, 0.0000596967, 0.0000580834, 0.0000563558, 0.0000548059, 0.0000517904, 0.0000489365, 0.0000475051, 0.0000446595, 0.0000387849, 0.000029462, 0.0000310918, 0.0000429959, 0.0000559752, 0.0000422251, 2.34152e-6, -0.0000306928, -1.6542e-6, 0.0000625228, 0.000149972, 0.0000163033, -0.000151445, -0.0000617592, 0.0000658249, -0.000189109, -0.000145963, -0.000265238, -0.000189341, 0.000359469, 0.000929862, -0.000507273, -0.00266354, -0.00346909, -0.0014304, 0.000505099, 0.00565233, 0.00268749, -0.0021453, -0.00704728, -0.00798463, -0.0126706, -0.00476589, 0.00285371, 0.0111981, 0.00251049, 0.000141942, -0.0114361, -0.00713103, 0.00915937, -0.0426054, -0.245048, 0.294121, 0.573692, -0.787206, -0.706752, 0.663176, 0.550089, 0.0513639, -0.473882, -0.354662, -0.0489873, 0.240936, 0.33022, 0.0722571, -0.108936, -0.325009, -0.271801, -0.0893083, 0.186103, 0.245753, 0.176571, 0.0323298, -0.054133, -0.163042, -0.203479, -0.172819, -0.0264531, 0.0863983, 0.151017, 0.139111, 0.0907815, -0.000276616, -0.0931765, -0.104342, -0.103702, -0.0856223, -0.0119351, 0.0657995, 0.101184, 0.0989509, 0.0747166, 0.0215124, -0.0252346, -0.053809, -0.0567912, -0.0531626, -0.0479862, -0.0177793, 0.0472359, 0.0709658, 0.0525359, 0.0213136, -0.00762077, -0.0193717, -0.0282366, -0.0367907, -0.0441569, -0.0356106, -0.0124185, 0.0112791, 0.0250107, 0.0309521, 0.0276434, 0.0129403, -0.0031297, -0.0177604, -0.0294523, -0.0304732, -0.0258845, -0.0177816, -0.00204156, 0.0127938, 0.0192862, 0.0140057, 0.00837937, 0.00465914, 0.00124506, -0.00579706, -0.0134437, -0.016358, -0.0172302, -0.0134716, -0.00646195, 0.00070141, 0.00730883, 0.011202, 0.0115083, 0.00666054, -0.00231752, -0.00607003, -0.00493717, -0.00414155, -0.00527888, -0.00941737, -0.0103471, -0.00583439, 0.000863245, 0.00583372, 0.00762418, 0.00615691, 0.00239808, -0.00123251, -0.00240054, -0.00295256, -0.00410912, -0.00464638, -0.00428287, -0.00303564, -0.00136928, -0.000193944, 0.00106172, 0.0023239, 0.00290728, 0.00276749, 0.000686104, -0.00102552, -0.00188957, -0.00260681, -0.00270782, -0.00199157, -0.00122065, -0.000605637, 0.000233239, 0.00077, 0.001101, 0.00114675, 0.000999776, 0.000564214, 0.000136931, -0.000320602, -0.000602047, -0.00082134, -0.000998544, -0.00101084, -0.000774967, -0.000416091, 0.000131403, 0.000532033, 0.00071742, 0.000700766, 0.000534203, 0.000323718, 0.0000625054, -0.000183316, -0.000373147, -0.000493602, -0.000463524, -0.000275936, -0.0000191424, 0.000198164, 0.000322327, 0.000394152, 0.000416653, 0.000376521, 0.000266606, 0.000141883, 0.000047311, -0.0000168986, -0.0000590419, -0.0000625026, -0.0000111573, 0.0000462337, 0.000105006, 0.000181189, 0.000225738, 0.000231936, 0.000216246, 0.000196612, 0.000169317, 0.000131116, 0.0000859758, 0.0000508931, 0.0000497328, 0.0000626155, 0.0000738655, 0.0000851699, 0.000103081, 0.000118177, 0.00011876, 0.000121784, 0.000122693, 0.000117846, 0.00010564, 0.0000934953, 0.0000880018, 0.0000831342, 0.0000821427, 0.0000828027, 0.0000885862, 0.0000924464, 0.000097427, 0.0000986633, 0.0000990483, 0.0000995723, 0.0000972888, 0.0000944287, 0.0000933896, 0.0000932408, 0.0000926983, 0.0000909193, 0.0000882532, 0.0000860885, 0.0000826242, 0.0000789914, 0.0000757509, 0.0000726354};
		
	
	const ComplexType fDomain_{ {-0.033286476582894575,0.},{0.007233891862397428,-0.024837813474009547},
   {0.01648498122714749,-0.0013459298116170332},{0.006181886090527254,0.018460458280573817} };
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
